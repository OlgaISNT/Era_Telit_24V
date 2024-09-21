

#include "../hdr/readDataByIdentifier_22.h"
#include "../hdr/uds.h"
#include "log.h"
#include "argz.h"
#include "m2mb_os_api.h"
#include "m2mb_types.h"
#include "gpio_ev.h"
#include "params.h"
#include "gnss_rec.h"
#include "app_cfg.h"
#include "sys_param.h"
#include "era_helper.h"
#include "sim.h"
#include "util.h"
#include "m2mb_info.h"
#include "factory_param.h"
#include "msd_storage.h"
#include "charger.h"
#include "failures.h"
#include "indication.h"
#include "testing.h"
#include "../../accelerometer/hdr/accel_driver.h"
#include "u_time.h"
#include "../hdr/securityAccess_27.h"
#include "codec.h"
#include "../hdr/uds_service31.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

static UINT16 pos_resp_22(UINT16 data_id, char *buf, UINT16 buf_size);
static void clearContent(void);
static BOOLEAN float_to_char(float f, char *buff, UINT32 buff_size);
static void outputStatesRecord(char *out);
static void inputStatesRecord(char *res);
static void systemState(char *res);
static void accelerometerRawData(char *res);
static UINT16 unsupportedId(UINT16 id, char *buf);
static UINT16 checkDIOState(char *content);
static BOOLEAN int_to_byte_array(UINT32 value, char *buffer, UINT32 buffer_size);
// Неподдерживаемые идентификаторы
static UINT16 unsupportedId(UINT16 id, char *buf){
    switch (id) {
        case 0x040C:
        case 0x0410:
        case 0x0420:
        case 0xF189:
        case 0xF18B:
        case 0xF192:
        case 0xF193:
        case 0xF198:
        case 0xF199:
        case 0xF19D:
        case 0xF1A1:
        case 0x02FF: // +SentMSDAmount (1 byte)
        case 0x0004: // + reserveBatteryCharge (1 byte)
        case 0x0404: // +AccelCalibrationX (2 byte)
        case 0x0405: // +AccelCalibrationY (2 byte)
        case 0x0406: // +AccelCalibrationZ (2 byte)
        case 0x040F: // +DirectionAngle (1 byte)
        case 0x0412: // +Application CheckSum (2 byte)
        case 0x0413:
        case 0x0421:
        case 0x040A: // +Vehicle_SW_Version (3 byte)
        case 0x040E: // +SertState (1 byte)
        case 0x040B: // +Midlet_SW_Version (3 byte)
        case 0xF194: // +systemSupplierECUSoftwareNumber (2 byte)
        case 0xF195: // +systemSupplierECUSoftwareVersionNumber (2 byte)
            buf[0] = SIDRSIDNRQ;
            buf[1] = READ_DATA_BY_IDENTIFIER;
            buf[2] = SUBFUNCTION_NOT_SUPPORTED;
            return 3;
    }
    return 0;
}

static char content[200];
static UINT16 size = 0;

/**
 * @brief Копирование непересекающихся массивов с указанным кол-вом байт из источника, начиная с его конца
 * @param dest      указатель на массив, в который копируются данные
 * @param source    указатель на начало массива, с конца которого начнут копироваться данные
 * @param n         кол-во копируемых данных (номер байта, с которого в обратном направлении из источника начнут копироваться данные)
 */
static void memcpy_r(UINT8 *dest, UINT8 *source, UINT16 n) {
    for (UINT16 i = 0; i < n; i++) *(dest + i) = *(source + n - 1 - i);
}

static void fill_int(int v, char *dest, UINT8 size) {
	UINT8 *p;
	p = (UINT8 *) &v;
	for (int i = 0; i < size; i++) dest[size - i - 1] = *(p + i);
}

/**
 * @brief Обработчик 13.2.2	ReadDataByIdentifier service (ID 0x22)
 * @param data_id   dataIdentifier - This parameter identifies the server data record(s) being requested by the client
 * @param buf       буфер, в который будет сохранён ответ
 * @param buf_size  максимальный размер буфера
 * @return UINT16   кол-во байт, записанных в буфер ответа
 */
UINT16 handler_service22(UINT16 data_id, char *buf, UINT16 buf_size) {
    UINT16 len = 0;
    clearContent();
    if (buf_size < 3) return 0;
    if (unsupportedId(data_id, buf) != 0) return 3;
    get_parameter(data_id);
    if (size <= 0) { // Если параметр не обнаружен
        buf[0] = SIDRSIDNRQ;
        buf[1] = READ_DATA_BY_IDENTIFIER;
        buf[2] = REQUEST_OUT_OF_RANGE;
        return 3;
    } else {
        len = pos_resp_22(data_id, buf, buf_size);
        if (len == 0) {
            buf[0] = SIDRSIDNRQ;
            buf[1] = READ_DATA_BY_IDENTIFIER;
            buf[2] = SUBFUNCTION_NOT_SUPPORTED;
            return 3;
        }
    }
    return len;
}

static void clearContent(void){
    memset(content,0x00, 200);
}

static UINT16 pos_resp_22(UINT16 data_id, char *buf, UINT16 buf_size) {
    UINT16 len = 3 + size;
    if (len > buf_size) return 0;
    buf[0] = RDBIPR;
    buf[1] = data_id >> 8;
    buf[2] = data_id & 0xFF;

    // Переписываем содержимое Param в buf
    memcpy(&buf[3], content, size);
    return len;
}

extern void readDataById(UINT16 data_id, char *buf, UINT16 max_len){
    clearContent();
    get_parameter(data_id);
    if (size > max_len) return;
    memcpy(buf, content, size);
}

/**
 * @brief Получение указателя на параметр в зависимости от запрошенного UDS dataIdentifier
 * @param data_id    dataIdentifier - UDS - This parameter identifies the server data record that the client is requesting
 * @param for_write  TRUE - указатель запрашивается для записи, FALSE - для чтения
 * @return Param*    указатель на параметр, если он доступен, иначе NULL
 */
extern void get_parameter(UINT16 data_id) {
    //LOG_INFO("get_param, data_id: x%X", data_id);
    switch (data_id) {
        case 0x0001: // + inputStatesRecord (2 byte)
            size = 2;
            char inputStates[2];
            inputStatesRecord(inputStates);
            content[0] = inputStates[0];
            content[1] = inputStates[1];
            break;
        case 0x0002: // + outputStatesRecord (4 byte)
            size = 4;
            char outputStates[4];
            outputStatesRecord(outputStates);
            content[0] = outputStates[0];
            content[1] = outputStates[1];
            content[2] = outputStates[2];
            content[3] = outputStates[3];
            break;
        case 0x0003: // + boardVoltage (1 byte)
            size = 1;


        /*    INT32 val = getKL30voltage() + 1000; // Очень сильная погрешность
            for (int i = 0; i < 25500; i += 100) {
                if (i >= val){
                    goto exit;
                } else {
                    content[0]++;
                }
            }
            exit:*/

            if(getKL30voltage() > (100 * 0xff))
            	  content[0] = 0xff;
            else
                content[0] = (char) ((getKL30voltage())/100);

            break;
        case 0x0005: // + reserveBatteryTemperature (1 byte)
            size = 1;
            INT32 batTemp = 0;
            if(getBattState() != CHECK_ABSENCE){
                batTemp = getCurrentTemp();
            }
            char x = 0x00;
            for (FLOAT32 i = -40.0f; i <= 127.5f; i += 0.5f){
                if (i == (FLOAT32) batTemp){
                    goto down;
                } else {
                    x++;
                }
            }
            down:
            content[0] = x;
            break;
        case 0x0006: // + SystemState (3 byte)
            size = 3;
            systemState(content);
            break;
        case 0x0007: {// + vehicleCoordinates (8 byte)
            size = 8;
        	T_GnssData gd = get_gnss_data_fast();
            INT32 lat = gd.latitude + 324000000;
            INT32 lon = gd.longitude + 648000000;
            memcpy_r((UINT8 *) content, (UINT8 *)&lat, 4);
        	memcpy_r((UINT8 *) (content + 4), (UINT8 *)&lon, 4);
            break;
        }
        case 0x0008: {// + vehicleCoordinatesReliability (1 byte)
        	T_GnssData gd = get_gnss_data_fast();
            content[0] = gd.fix == 0 ? 0 : 1; size = 1;
            break;
        }
        case 0x0009: // + MDS_FormatVersion (1 byte)
            size = 1;
        	content[0] = MSD_FORMAT_VERSION;
            break;
        case 0xFD02: // + vehicleFuelType (1 byte) (0x000A в документации)
            size = 1;
        	content[0] = get_propulsion_type();
            break;
        case 0xFD00:
            size = 4;
            content[0] = 0xE0;
            content[1] = 0x00;
            content[2] = 0x00;
            content[3] = 0x00;
            break;
        case 0x000B: {// + timeStamp (4 byte)
            size = 4;
            T_GnssData gnss_data = get_gnss_data_fast();
            if (get_rtc_timestamp() > gnss_data.utc) {
                gnss_data.utc = get_rtc_timestamp();
            }
        	UINT32 ts = gnss_data.utc;
        	memcpy_r((UINT8 *) content, (UINT8 *)&ts, 4);
            break;
        }
        case 0x000C: // + passengersNumber (1 byte)
            size = 1;
        	content[0] = 0xFF;
            break;
        case 0x000D: // +TCM working mode (1 byte)
            size = 1;
        	content[0] = is_activated() ? 0x5A : 0xA5;
            break;
        case 0x000E: // +TCM activation source (1 byte)
        	size = 1;
        	content[0] = 0x03;
            break;
        case 0x000F: // +TCM time calculation (1 byte)
        	size = 1;
        	content[0] = 0x00;
            break;
        case 0x0010: // +ECALLManualCanCancel (1 byte)
            size = 1;
        	content[0] = get_manual_can_cancel() ? 0xC7 : 0x7C;
            break;
        case 0x0011: // +GLONASS and GLONASS/GPS groups (1 byte)
        	size = 1;
        	content[0] = 0x00;
            break;
        case 0x0012: {// +GsmRegistrationState (1 byte)
            size = 1;
        	UINT8 status = get_reg_status();
            switch (status) {
                case 0: // Не зарегистрирован
                case 2:
                case 3:
                	content[0] = 0x00;
                    break;
                case 1: // Зарегистрирован
                    content[0] = 0x01;
                    break;
                case 5: // Зарегистрирован, роуминг
                    content[0] = 0x02;
                    break;
                case 4: // Не используется
                default:
                    content[0] = 0x03;
                    break;
            }
            break;
        }
        case 0x0013: // +GsmInnerError (1 byte)
            content[0] = 0x03; size = 1;
            break;
        case 0x0014: // +VehicleCoordinatesReliabilityMem (1 byte)
            content[0] = 0x03; size = 1;
            break;
        case 0x0015: // +GsmRegistartionStateMem (1 byte)
            content[0] = 0x03; size = 1;
            break;
        case 0x0016: // +GsmRegistrationError (1 byte)
            content[0] = 0x03; size = 1;
            break;
        case 0x0017: // +EcallOn (1 byte)
            content[0] = 0x3B; size = 1;
            break;
        case 0x0018: // +CrashSignalExternal (1 byte)
            content[0] = 0xD8; size = 1;
            break;
        case 0x0019: // +TestModeEndDistance (2 byte)
            size = 2;
        	fill_int(get_test_mode_end_dist(), content, size);
            break;
        case 0x001A: // +GnssPowerOffTime (2 byte)
            size = 2;
        	content[0] = 0xFF; content[1] = 0xFF;
            break;
        case 0x001B: // +AutomaticActivationProhibition (1 byte)
            content[0] = 0x01; size = 1;
            break;
        case 0xFD01: // +VehicleType (1 byte) (0x001C в документации)
            size = 1;
        	content[0] = get_vehicle_type();
            break;
        case 0xFD03: // +SimIdentifier (19 byte) (0x001D в документации)
            size = 8;
            unsigned long long val1 = get_iccidg_ull();
            content[0] = val1 >> 56;
            content[1] = val1 >> 48;
            content[2] = val1 >> 40;
            content[3] = val1 >> 32;
            content[4] = val1 >> 24;
            content[5] = val1 >> 16;
            content[6] = val1 >> 8;
            content[7] = val1;
            break;
        case 0x0301: // +CallAutoAnswerTime
            size = 2;
        	fill_int(get_call_auto_answer_time(), content, size);
            break;
        case 0x0302: // +ECALLTestNumber (20 byte)
            size = 20;
        	get_test_number(content, size);
            break;
        case 0x0303: {// +ECALLToTest (1 byte)
            size = 1;
        	UINT8 r = 0;
        	char str[50];
            get_ecall_to_fast(str, sizeof(str));
            if (strstr(str, ECALL_TO_DEBUG) != NULL) r = 2;
            content[0] = r;
            break;
        }
        case 0x0304: // +NadDeregistrationTimer (2 byte)
            size = 2;
            fill_int(get_nad_dereg_time(), content, size);
            break;
        case 0x0305: // +NadRegistrationAtStartup (1 byte)
            size = 1;
        	content[0] = 0;
            break;
        case 0x0306: // +SMSCenterNumber (20 byte)
            size = 20;
            get_sms_center_number(content, size);
            break;
        case 0x0307: // +SMSFallbackNumber (20 byte)
            size = 20;
            get_sms_fallback_number_fast(content);
            break;
        case 0x030E: // +TestRegistrationPeriod (2 byte)
            size = 2;
            fill_int(get_test_reg_period(), content, size);
            break;
        case 0x030F: // +EcallManualDialAttempts (2 byte)
            size = 2;
        	fill_int(get_manual_dial_attempts(), content, size);
            break;
        case 0x0310: // +IntMemTransmitInterval (2 byte)
            size = 2;
            fill_int(get_int_mem_transm_interval(), content, size);
            break;
        case 0x0311: // +IntMemTransmitAttempts (2 byte)
            size = 2;
            fill_int(get_int_mem_transm_attempts(), content, size);
            break;
        case 0x0312: {// +GSMSoftwareVersionNumber (20 byte)
            size = 20;
            get_GSMSoftwareVersionNumber(content, size);
            break;
        }
        case 0x0316: // +SmsFallbackTestNumber (20 byte)
            size = 20;
            get_sms_fallback_debug_number(content, size);
            break;
        case 0x0319: // +EcallAutoDialAttempts (2 byte)
            size = 2;
            fill_int(get_auto_dial_attempts(), content, size);
            break;
        case 0x031A: // +EcallDialDuration (2 byte)
            size = 2;
            fill_int(get_dial_duration(), content, size);
            break;
        case 0x031B: // +PostTestRegistrationTime (2 byte)
            size = 2;
        	fill_int(get_post_test_reg_time(), content, size);
            break;
        case 0x031C: // +SimProfileAmount (1 byte)
            size = 1;
        	content[0] = 0x01;
            break;
        case 0x031D: // +SimProfile (1 byte)
            size = 1;
        	content[0] = 0x00;
            break;
        case 0x0323: // +AudioProfile (1 byte)
            size = 1;
        	content[0] = (UINT8) get_audio_prof_pos();
            break;
        case 0x0407: // +EcallNoAutomaticTriggering (1 byte)
            size = 1;
        	content[0] = get_no_auto_trig() ? 0x92 : 0x29;
            //LOG_DEBUG("EcallNoAutomaticTriggering 0x0407: %i", content[0]);
            break;
        case 0x0401: // +AccelerometerRawData (8 byte)
            size = 8;
            accelerometerRawData(content);
            break;
        case 0x0402: // +RolloverAngle (1 byte)
            size = 1;
            content[0] = get_rollover_angle();
            break;
        case 0x0403: // +RolloverTime (1 byte)
            size = 1;
            content[0] = get_rollover_time();
            break;
        case 0x0409: // +SW_Version (27 byte)
            size = 27;
            char ver[50] = {0};
            set_build_time(ver);
            memcpy(content, ver, size);
            break;
        case 0x0408: // +MiddletState (1 byte)
            size = 1;
            content[0] = (is_activated() == TRUE) ? 0x02 : 0x00;
            break;
        case 0x0411:
            if (!isTestMode()){
                size = 20;
                memcpy(content, get_imei(), 20);
            } else {
                size = 8;
                char **ptr = 0;
                unsigned long long val = strtoumax(get_imei(),ptr, 10);
                content[0] = val >> 56;
                content[1] = val >> 48;
                content[2] = val >> 40;
                content[3] = val >> 32;
                content[4] = val >> 24;
                content[5] = val >> 16;
                content[6] = val >> 8;
                content[7] = val;
            }
            break;
        case 0x01FF: // +SavedMSDAmount (1 byte)
            size = 1;
        	content[0] = (UINT8) get_number_MSDs();
            break;
        case 0xF18A: // +systemSupplierIdentifier (3 byte)
            size = 3;
        	content[0] = 'I'; content[1] = 'T'; content[2] = 'L';
            break;
        case 0xF18C: // +ECUSerialNumber (20 byte)
            size = 20;
            if(isTestMode()){
                size = 16;
                get_serial(content, size);
            } else {
                get_serial(content, size);
                get_sw_version(&content[16], 4);
                //memcpy(&content[16], SERT_VERS, 4);
            }
            break;
        case 0xF190: // +vehicleIdentificationNumber (17 byte)
            size = 17;
        	get_vin(content, size);
            break;
        case 0x040D: // +Boot_SW_Version (27 byte)
            size = 27;
            memcpy(content, "CHRY01_", 27);
            break;
        case 0x001E: // +CommSimIdentifier (19 byte)
            size = 19;
            break;
        case 0xF187: // +vehicleManufacturerSparePartNumber (10 byte)
            size = 10;
            content[0] = 0x00;
            break;
        case 0xF188: // +ConfigurationFileReferenceLink (10 byte)
            size = 10;
            memcpy(content,SERT_VERS, size);
            break;
        case 0xF191: // +vehicleManufacturerECUHardwareNumber (10 byte)
            size = 10;
            sprintf(&content[0], "%s", HW_VERS);
            break;
        case 0xF1A0: // +VDIAG (1 byte)
            size = 1;
        	content[0] = 0xFF;
            break;
        case 0x990D:{ // ccft
            size = 4;
            int_to_byte_array(get_ccft(), content, 4);
            break;
        }
        case 0x990E:{ // max msd transmission time
            size = 4;
            int_to_byte_array(get_msd_max_transm_time(), content, 4);
            break;
        }
        case 0x9901: // Check_Who_Am_I_Accelerometer
            if(isTestMode()){
                size = 1;
                content[0] = getWhoAmI();
            } else {
                size = 0;
            }
            break;
        case 0x9902: // Check_Who_Am_I_AudioCodec
            if(isTestMode()){
                size = 1;
                content[0] = getWhoAmICodec();
            } else {
                size = 0;
            }
            break;
        case 0x9904: // Check_SIM_IMSI
            if(isTestMode()){
                size = 8;
                unsigned long long val1 = get_imsi_ull();
                content[0] = val1 >> 56;
                content[1] = val1 >> 48;
                content[2] = val1 >> 40;
                content[3] = val1 >> 32;
                content[4] = val1 >> 24;
                content[5] = val1 >> 16;
                content[6] = val1 >> 8;
                content[7] = val1;
            } else {
                size = 0;
            }
            break;
        case 0x9906: // Check_Microphone_ADC
            if (isTestMode()){
                size = 4;
                INT32 adcVal = getDiagMicVoltage();
//                LOG_DEBUG("adc val = %d", adcVal);
                content[0] = (adcVal >> 24) & 0xFF;
                content[1] = (adcVal >> 16) & 0xFF;
                content[2] = (adcVal >> 8) & 0xFF;
                content[3] = adcVal & 0xFF;
            } else {
                size = 0;
            }
            break;
        case 0x9907: // Check_KL30_ADC
            if (isTestMode()){
                size = 4;
                INT32 adcKL30Val = getKL30Adc();
//                LOG_DEBUG("adc val = %d", adcKL30Val);
                content[0] = (adcKL30Val >> 24) & 0xFF;
                content[1] = (adcKL30Val >> 16) & 0xFF;
                content[2] = (adcKL30Val >> 8) & 0xFF;
                content[3] = adcKL30Val & 0xFF;
            } else {
                size = 0;
            }
            break;
        case 0x9908: // Check_BATTERY_ADC
            if (isTestMode()){
                size = 4;
                INT32 adcBatVal = getBatAdc();
//                LOG_DEBUG("adc val = %d", adcBatVal);
                content[0] = (adcBatVal >> 24) & 0xFF;
                content[1] = (adcBatVal >> 16) & 0xFF;
                content[2] = (adcBatVal >> 8) & 0xFF;
                content[3] = adcBatVal & 0xFF;
            } else {
                size = 0;
            }
            break;
        case 0x9909: // Check_digital_IO_state
            if (isTestMode()){
                size = checkDIOState(content);
            } else {
                size = 0;
            }
            break;
        case 0x9910:
            if (isTestMode()){
                size = 1;
                content[0] = getAvailableNetworks();
            } else {
                size = 0;
            }
            break;
        case 0x9911:
            if (isTestMode()){
                size = 1;
                content[0] = get_gnss_data_fast().fix;
            } else {
                size = 0;
            }
            break;
        case 0x9912:
            if (isTestMode()){
                size = 2;
                content[0] = get_gnss_data_fast().gps_sat;
                content[1] = get_gnss_data_fast().glonass_sat;
            } else {
                size = 0;
            }
            break;
        case 0x9913:{
            UINT16 v_bat = get_v_bat();
            if (isTestMode()){
                size = 2;
                content[0] = v_bat >> 8;;
                content[1] = v_bat;
            } else {
                size = 0;
            }
            break;
        }
        case 0x9914:{
            if (isTestMode()){
                float zyx_t = get_zyx_threshold();

                if(!float_to_char(zyx_t, content, 200)) size = 0;
                else size = 4;

            } else size = 0;
            break;
        }
        default:
        	LOG_WARN("UDS: The attempt to get an unsupported parameter ID: x%4.4X", data_id);
            size = 0;
            break;
    }
}

static BOOLEAN float_to_char(float f, char *buff, UINT32 buff_size) {
    if(buff == NULL || buff_size < 4) return FALSE;

    unsigned long longdata = 0;
    longdata = *(unsigned long*)&f;
    buff[0] = (longdata & 0xFF000000) >> 24;
    buff[1] = (longdata & 0x00FF0000) >> 16;
    buff[2] = (longdata & 0x0000FF00) >> 8;
    buff[3] = (longdata & 0x000000FF);
    return TRUE;
}

static UINT16 checkDIOState(char *content){
    content[0] = 0x00;
    content[0] |= (getValSosBnt() << 7);     // 7 SOS_BTN
    content[0] |= (getValSosDiag() << 6);    // 6 DIAG_SOS_IN
    content[0] |= (getEnAudio() << 5);       // 5 EN_AUDIO
    content[0] |= (getValServiceBtn() << 4); // 4 SERVICE_BTN
    content[0] |= (getServiceCtrl() << 3);   // 3 SERVICE_CTRL
    content[0] |= (getGreenModeCtrl() << 2); // 2 GREEN_MODE_CTRL
    content[0] |= (getSosCtrl() << 1);       // 1 SOS_CTRL
    content[0] |= getOnCharge();             // 0 ON_CHARGE

    content[1] = 0x00;
    content[1] |= (getOnOffMode() << 7);      // 7 ON_OFF_MODE
    content[1] |= (getSpeakerState() << 6);   // 6 DIAG_SPN
    content[1] |= (getCurrWdOutState() << 5); // 5 WD_OUT (ACC)
    content[1] |= (isIgnition() << 4);        // 4 IGNITION
    content[1] |= (getRedMode() << 3);        // 3 RED_MODE_CTRL
    return 2;
}

static void inputStatesRecord(char *out){
    // 0 байт
    // 7,6 биты Input_SOS
    BOOLEAN sos = !getSosState();
    // 5,4 биты Input_Service (not used)
    UINT32 service = 3;
    // 3,2 биты Input_KL15
    BOOLEAN KL15 = isIgnition();
    // 1,0 биты Input_ACC
    UINT32 ACC = 3; // Терминал выводит это с инверсией
    out[0] = (sos << 6) | (service << 4) | (KL15 << 2) | (ACC);

    // 1 байт
    // 7,6 биты Input_2 (not used)
    UINT32 Input_2 = 3;
    // 5,4 биты Input_3 (not used)
    UINT32 Input_3 = 3;
    // 3,2,1,0 биты (not used)
    out[1] = (Input_2 << 6) | (Input_3 << 4) | (3 << 2) | 3;
}

static void outputStatesRecord(char *out){
    // 0 байт
    // 7,6 биты Output_LockDoor  (not used)
    UINT32 Output_LockDoor = 3;
    // 5,4 биты Output_SERVICE_LED (not used)
    UINT32 Output_SERVICE_LED = 3;
    // 3,2 биты Output_SOS_LED (not used)
    UINT32 Output_SOS_LED = 3;
    // 1,0 биты Output_KL15A (not used)
    UINT32 Output_KL15A = 3;
    out[0] = (Output_LockDoor << 6) | (Output_SERVICE_LED << 4) | (Output_SOS_LED << 2) | (Output_KL15A);

    // 1 байт
    // 7,6 биты Output_Preheater  (not used)
    UINT32 Output_Preheater = 3;
    // 5,4 биты Output_Klakson (not used)
    UINT32 Output_Klakson = 3;
    // 3,2 биты Output_AS_Button (not used)
    UINT32 Output_AS_Button = 3;
    // 1,0 биты Output_Backlight (not used)
    UINT32 Output_Backlight = 3;
    out[1] = (Output_Preheater << 6) | (Output_Klakson << 4) | (Output_AS_Button << 2) | (Output_Backlight);

    // 2 байт
    // 7,6 биты Output_GREEN_LED  (not used)
    UINT32 Output_GREEN_LED = 3;
    // 5,4 биты Output_RED_LED (not used)
    UINT32 Output_RED_LED = 3;
    // 3,2 биты Output_ON_Charge on|off
    BOOLEAN Output_ON_Charge = getOnCharge();
    // 1,0 биты Output_ON_OFF_Mode on|off
    BOOLEAN Output_ON_OFF_Mode = getOnOffMode();
    out[2] = (Output_GREEN_LED << 6) | (Output_RED_LED << 4) | (Output_ON_Charge << 2) | (Output_ON_OFF_Mode);

    // 3 байт
    // 7,6 биты Output_Mute  on|off
    BOOLEAN Output_Mute = getEnAudio();
    // 5,4,3,2,1,0 (not used)
    UINT32 one = 3;
    UINT32 two = 3;
    UINT32 three = 3;
    out[3] = (Output_Mute << 6) | (one << 4) | (two << 2) | (three);
}

static void systemState(char *res) {
    // 1й байт
    // 7,6,5 биты TcmState
    UINT8 tcmState = (is_testing_working() == TRUE) ? 5 : 0; // testing/work;

    // 4,3,2 биты Airbag_State
    UINT8 airbagState = (get_airbags_failure() == F_ACTIVE) ? 4 : 0;
    // 1,0 биты Comfort_State
    UINT8 comfortState = 2; // not used;
    res[0] = (tcmState << 5) | (airbagState << 2) | (comfortState);

    // 2й байт
    // 7,6 биты Motion_State
    UINT8 motionState = (isIgnition() == TRUE) ? 1 : 0;
    // 5,4,3 биты Battery_State
    UINT8 batteryState = 0;
    switch (getBattState()) {
        case NO_CHARGING:
            batteryState = 0;
            break;
        case TRICKLE_CHARGING:
            batteryState = 1;
            break;
        case FAST_CHARGING:
            batteryState = 2;
            break;
        case CHECK_OLD:
            batteryState = 4;
            break;
        case CHECK_ABSENCE:
            batteryState = 5;
            break;
        case STABILIZATION:
            batteryState = 6;
            break;
        case CARE_CHARGE:
            batteryState = 0x59;
            break;
        case UNDEFINED:
        default:
            batteryState = 7;
            break;
    }
    // 2,1,0 биты не используется
    res[1] = (motionState << 6) | (batteryState << 3);

    // 3й байт
    // 7,6,5,4,3 биты GSM_State
    UINT8 gsmState = 0;
    IND_TASK_COMMAND event = getCurrenTask();
    switch (event) {
        case SHOW_TEST_STARTED:
            gsmState = 7;
            break;
        case SHOW_ECALL:            // 11 – call_preparation;
            gsmState = 11;
            break;
        case SHOW_ECALL_DIALING:    // 12 – dial;
            gsmState = 12;
            break;
        case SHOW_SENDING_MSD:      // MSD_sends;
            gsmState = 15;
            break;
        default:
           // LOG_DEBUG(">>>>>>>>>>>>> status %d", get_reg_status());
//            gsmState = get_reg_status() != 2 ? 1 : 17;
            gsmState = 1;
            break;
    }

    // 2 бит Rollover angle status
    // 1,0 биты не используются
    UINT8 rolloverAngleStatus = 0;
    res[2] = (gsmState << 3) | (rolloverAngleStatus << 2);
}

static void accelerometerRawData(char *res){
    RAW raw = getRawData();
    // 0,1 байты
    res[0] = raw.xL;
    res[1] = raw.xH;
    // 2,3 байты
    res[2] = raw.yL;
    res[3] = raw.yH;
    // 4,5 байты
    res[4] = raw.zL;
    res[5] = raw.zH;
    // 6,7 байты
    INT16 tiltHorAng = getCurrentRollAng();
    res[6] = 0x00;
    res[7] = tiltHorAng;
//    LOG_DEBUG("ang %d %02X %02X", tiltHorAng ,res[6], res[7]);
}

static BOOLEAN int_to_byte_array(UINT32 value, char *buffer, UINT32 buffer_size) {
    if (buffer == NULL || buffer_size < 4) return FALSE;
    memset(&buffer[0], 0x00, buffer_size);
    buffer[0] = (char) ((value & 0xFF000000) >> 24);
    buffer[1] = (char) ((value & 0x00FF0000) >> 16);
    buffer[2] = (char) ((value & 0x0000FF00) >> 8);
    buffer[3] = (char) ((value & 0x000000FF) >> 0);
    return TRUE;
}
