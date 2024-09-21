//
// Created by klokov on 17.10.2022.
//


#include "log.h"
#include "../hdr/readDataByLocalIdentifier_21.h"
#include "../hdr/uds.h"
#include "../hdr/readDataByIdentifier_22.h"
#include "argz.h"
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "azx_log.h"
#include "u_time.h"
#include "gpio_ev.h"
#include "charger.h"
#include "azx_ati.h"
#include "gnss_rec.h"
#include "era_helper.h"
#include "sys_utils.h"
#include "airbags.h"
#include "sys_param.h"
#include "indication.h"
#include "testing.h"
#include "../../accelerometer/hdr/accel_driver.h"
#include <time.h>

static char res2101[23] = {0};
static char res2102[57] = {0};
static char res2103[94] = {0};
static char res2180[26] = {0};
static char res2181[21] = {0};
static char res2184[22] = {0};

static UINT16 readData(char data_id, char *buf, UINT16 buf_size);
static UINT16 pos_resp_21(char data_id, char *buf, UINT16 buf_size);
static char getSystemState(void);
static char getReserveBatteryState(void);
static char getGSMState(void);
static char getStatus_01(void);
static void getUTCTime(char *buf);
static void getDeveloperInformation(char *buf);
static void getReserveBatteryVoltage(char *buf);
static char getAngleForRolloverDetection(void);
static char crashManual(void);
static char crashAirbag(void);
static char crashRollover(void);
static void readSparePartNumber(char *buf);
static char readDiagnosticIdCode(void);
static void readSupplierNumber(char *buf);
static void readHardwareNumber(char *buf);
static void readSoftwareNumber(char *buf);
static void readEditionNumber(char *buf);
static void readCalibrationNumber(char *buf);
static void readOtherSystemFeatures(char *buf);
static char readManufacturerIdentificationCode(void);

extern UINT16 handler_service21(char *data, char *buf, UINT16 buf_size){
    // Проверка фрейма
    if (data == NULL || data[1] != READ_DATA_BY_LOCAL_IDENTIFIER){
        return negative_resp(buf, READ_DATA_BY_LOCAL_IDENTIFIER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Проверка длины фрейма (фрейм с запросом всегда имеет длину 0х02)
    if (data[0] != 0x02){
        return negative_resp(buf, READ_DATA_BY_LOCAL_IDENTIFIER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Проверка пустых байтов фрейма
    for (UINT8 i = 3; i < 8; i++){
        if (data[i] != 0xAA){
            return negative_resp(buf, READ_DATA_BY_LOCAL_IDENTIFIER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
        }
    }

    // Собираем ответ
    return pos_resp_21(data[2], buf, buf_size);
}

static UINT16 pos_resp_21(char data_id, char *buf, UINT16 buf_size) {
    return readData(data_id, buf, buf_size);
}

static UINT16 readData(char data_id, char *buf, UINT16 buf_size){
    (void) buf_size;
    switch (data_id) {
        case TCM_STATUS:                // TCM Status / no secured / 22 bytes
            memcpy(buf, res2101, 23);
            return 23;
        case GSM_STATUS:               // GSM Status / no secured / 0 - 100 bytes
            memcpy(buf, res2102, 57);
            return 57;
        case ACCEL_CALIB_STATUS:       // AccelCalibStatus / no secured / 0 - 100 bytes
            memcpy(buf, res2103, 94);
            return 94;
        case SYSTEM_IDENTIFICATION:    // Renault’s system identification / no secured / 24 bytes
            memcpy(buf, res2180, 26);
            return 26;
        case VIN:                      // VIN / no secured / 19 bytes
            memcpy(buf, res2181, 21);
            return 21;
//            return readVin(buf);
        case TRACEABILITY_INFORMATION: // Traceability information / no secured / 20 bytes
            res2184[0] = 0x61;
            res2184[1] = TRACEABILITY_INFORMATION;
            memcpy(buf, res2184, 22);
            return 22;
        //            return readTraceabilityInformation(buf);
        default:
            return negative_resp(buf, READ_DATA_BY_LOCAL_IDENTIFIER, REQUEST_OUT_OF_RANGE);
    }
}

extern void update2101(void){
//    memset(res2101, 0x00, 23);
    char temp[50];
    res2101[0] = 0x61;
    res2101[1] = TCM_STATUS;

    // InputStatesRecord (2 bytes)
    readDataById(0x0001, temp, 50);
    memcpy(&res2101[2], temp, 2);

    // OutputStatesRecord (4 bytes)
    readDataById(0x0002, temp, 50);
    memcpy(&res2101[4], temp, 4);

    // BoardVoltage (1 byte)
    readDataById(0x0003, temp, 50);
    memcpy(&res2101[8], temp, 1);

    // ReserveBatteryTemperature (1 byte)
    INT32 batTemp = 0;
    if(getBattState() != CHECK_ABSENCE){
        batTemp = getCurrentTemp();
    }
    res2101[9] = batTemp;

    // System state (1 byte)
    res2101[10] = getSystemState();

    // ReserveBatteryState (1 byte)
    res2101[11] = getReserveBatteryState();

    // GSMState (1 byte)
    res2101[12] = getGSMState();

    // Status (1 byte)
    res2101[13] = getStatus_01();

    // UTC time (4 byte)
    getUTCTime(temp);
    memcpy(&res2101[14], temp, 4);

    // Developer information (2 byte)
    getDeveloperInformation(temp);
    memcpy(&res2101[18], temp, 2);

    // ReserveBatteryVoltage (2 byte)
    getReserveBatteryVoltage(temp);
    memcpy(&res2101[20], temp, 2);

    // Current Angle for rollover detection (1 byte)
    res2101[22] = getAngleForRolloverDetection();
}

extern void update2102(void){
//    memset(res2102, 0x00, 57);
    char temp[50];
    res2102[0] = 0x61;
    res2102[1] = GSM_STATUS;
    res2102[32] = MSD_FORMAT_VERSION;          // U08 msd_format_version;
    readDataById(0x0012,temp,50); // U08 status;
    res2102[33] = temp[0];
    res2102[34] = crashManual();                  // U08 crash_manual;
    res2102[35] = crashAirbag();                  // U08 crash_airbag;
    res2102[36] = crashRollover();                // U08 crash_rollover;
}

extern void update2103(void){
    res2103[0] = 0x61;
    res2103[1] = ACCEL_CALIB_STATUS;

    // 1 точка
    // S16 x1;
    res2103[2] = 0x00;
    res2103[3] = 0x00;
    // S16 y1;
    res2103[4] = 0x00;
    res2103[5] = 0x00;
    // S16 z1;
    res2103[6] = 0x00;
    res2103[7] = 0x00;

    // 2 точка
    // S16 x2;
    res2103[8] = 0x00;
    res2103[9] = 0x00;
    // S16 y2;
    res2103[10] = 0x00;
    res2103[11] = 0x00;
    // S16 z2;
    res2103[12] = 0x00;
    res2103[13] = 0x00;

    // 3 точка
    // S16 x3;
    res2103[14] = 0x00;
    res2103[15] = 0x00;
    // S16 y3;
    res2103[16] = 0x00;
    res2103[17] = 0x00;
    // S16 z3;
    res2103[18] = 0x00;
    res2103[19] = 0x00;

    // 4 точка
    // S16 x4;
    res2103[20] = 0x00;
    res2103[21] = 0x00;
    // S16 y4;
    res2103[22] = 0x00;
    res2103[23] = 0x00;
    // S16 z4;
    res2103[24] = 0x00;
    res2103[25] = 0x00;

    // 5 точка
    // S16 x5;
    res2103[26] = 0x00;
    res2103[27] = 0x00;
    // S16 y5;
    res2103[28] = 0x00;
    res2103[29] = 0x00;
    // S16 z5;
    res2103[30] = 0x00;
    res2103[31] = 0x00;

    // 6 точка
    // S16 x6;
    res2103[32] = 0x00;
    res2103[33] = 0x00;
    // S16 y6;
    res2103[34] = 0x00;
    res2103[35] = 0x00;
    // S16 z6;
    res2103[36] = 0x00;
    res2103[37] = 0x00;

    // счетчики калибровок
    res2103[38] = 0x00; // U08 cnt1;
    res2103[39] = 0x00; // U08 cnt2;
    res2103[40] = 0x00; // U08 cnt3;
    res2103[41] = 0x00; // U08 cnt4;
    res2103[42] = 0x00; // U08 cnt5;
    res2103[43] = 0x00; // U08 cnt6;

    // сами калибровки
    // S16 Kx;
    res2103[44] = 0x00;
    res2103[45] = 0x00;
    // S16 Ky;
    res2103[46] = 0x00;
    res2103[47] = 0x00;
    // S16 Kz;
    res2103[48] = 0x00;
    res2103[49] = 0x00;

    // S16 Xofs;
    res2103[50] = 0x00;
    res2103[51] = 0x00;
    //  S16 Yofs;
    res2103[52] = 0x00;
    res2103[53] = 0x00;
    // S16 Zofs;
    res2103[54] = 0x00;
    res2103[55] = 0x00;

    // сырые данные акселерометра
    // S16 Xraw;
    res2103[56] = 0x00;
    res2103[57] = 0x00;
    // S16 Yraw;
    res2103[58] = 0x00;
    res2103[59] = 0x00;
    // S16 Zraw;
    res2103[60] = 0x00;
    res2103[61] = 0x00;

    // Обработанные данные акселерометра (после калибровки)
    // S16 XX;
    res2103[62] = 0x00;
    res2103[63] = 0x00;
    // S16 YY;
    res2103[64] = 0x00;
    res2103[65] = 0x00;
    // S16 ZZ;
    res2103[66] = 0x00;
    res2103[67] = 0x00;
    // S16 ugol;
    res2103[68] = 0x00;
    res2103[69] = 0x00;

    // Калибровки на переворот
    // S16 AccelCalibratiobX;
    res2103[70] = 0x00;
    res2103[71] = 0x00;
    // S16 AccelCalibratiobY;
    res2103[72] = 0x00;
    res2103[73] = 0x00;
    // S16 AccelCalibratiobZ;
    res2103[74] = 0x00;
    res2103[75] = 0x00;

    res2103[76] = 0x00; // U08 RolloverAngle;
    res2103[77] = 0x00; // U08 RolloverTime;

    // S16 DirectionAngle;
    res2103[78] = 0x00;
    res2103[79] = 0x00;

    // 2 point calibration
    // S16 X1;
    res2103[80] = 0x00;
    res2103[81] = 0x00;
    // S16 Y1;
    res2103[82] = 0x00;
    res2103[83] = 0x00;
    // S16 Z1;
    res2103[84] = 0x00;
    res2103[85] = 0x00;
    // S16 X2;
    res2103[86] = 0x00;
    res2103[87] = 0x00;
    // S16 Y2;
    res2103[88] = 0x00;
    res2103[89] = 0x00;
    // S16 Z2;
    res2103[90] = 0x00;
    res2103[91] = 0x00;

    // счетчики
    res2103[92] = 0x00; // U08 count1;
    res2103[93] = 0x00; // U08 count2;
}

extern void update2180(void){
    char temp[50];
    res2180[0] = 0x61;
    res2180[1] = SYSTEM_IDENTIFICATION;
    // Spare part number (5 bytes)
    readSparePartNumber(temp);
    memcpy(&res2180[6], temp, 5);

    // Diagnostic ID code (1 byte)
    res2180[7] = readDiagnosticIdCode();

    // Supplier number (3 byte)
    readSupplierNumber(temp);
    memcpy(&res2180[8], temp, 3);

    // Hardware number (5 bytes)
    readHardwareNumber(temp);
    memcpy(&res2180[11], temp, 5);

    // Software number (2 bytes)
    readSoftwareNumber(temp);
    memcpy(&res2180[16], temp, 2);

    // Edition number (2 byte)
    readEditionNumber(temp);
    memcpy(&res2180[18], temp, 2);

    // Calibration number (2 byte)
    readCalibrationNumber(temp);
    memcpy(&res2180[20], temp, 2);

    // Other system features (3 byte)
    readOtherSystemFeatures(temp);
    memcpy(&res2180[22], temp, 3);

    // Manufacturer identification code (1 byte)
    res2180[25] = readManufacturerIdentificationCode();
}

extern void update2181(void){
    res2181[0] = 0x61;
    res2181[1] = VIN;

    char temp[50];
    readDataById(0xF190, temp, 50);
    memcpy(&res2181[2], temp, 17);

    res2181[19] = 0x00;
    res2181[20] = 0x00;
}

extern void update2184(void){
    res2184[0] = 0x61;
    res2184[1] = TRACEABILITY_INFORMATION;

    for (UINT8 i = 2; i < 22; i ++){
        res2184[i] = 0x00;
    }
}

//  This byte shall be filled with 88
static char readManufacturerIdentificationCode(void){ // (!!!!!!!!!ЗАГЛУШКА!!!!!!!!!!!!)
    char manufacturerIdentificationCode = 0x00; // HEX
    return manufacturerIdentificationCode;
}

// These 3 bytes shall be filled with 1
static void readOtherSystemFeatures(char *buf){ // (!!!!!!!!!ЗАГЛУШКА!!!!!!!!!!!!)
    buf[0] = 0x01;
    buf[1] = 0x01;
    buf[2] = 0x01;
}

// The version of the current TCM calibration codes
static void readCalibrationNumber(char *buf){ // (!!!!!!!!!ЗАГЛУШКА!!!!!!!!!!!!)
    buf[0] = 0x00;
    buf[1] = 0x00;
}

// It is the same as systemSupplierECUSoftwareVersionNumber in the RDBI service
static void readEditionNumber(char *buf){ // (!!!!!!!!!ЗАГЛУШКА!!!!!!!!!!!!)
    buf[0] = 0x00;
    buf[1] = 0x00;
}

// It is the same as systemSupplierECUSoftwareNumber in the RDBI service
static void readSoftwareNumber(char *buf){ // (!!!!!!!!!ЗАГЛУШКА!!!!!!!!!!!!)
    buf[0] = 0x00;
    buf[1] = 0x00;
}

// It is the same as vehicleManufacturerECUHardwareNumber in the RDBI service
static void readHardwareNumber(char *buf){  // (!!!!!!!!!ЗАГЛУШКА!!!!!!!!!!!!)
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
}

// It is the same as systemSupplierIdentifier in the RDBI service
static void readSupplierNumber(char *buf){ // (!!!!!!!!!ЗАГЛУШКА!!!!!!!!!!!!)
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0x00;
}

//  It is the same as VDIAG in the RDBI service
static char readDiagnosticIdCode(void){ // (!!!!!!!!!ЗАГЛУШКА!!!!!!!!!!!!)
    char diagnosticIdCode = 0x00;
    return diagnosticIdCode;
}

//  It is the same as vehicleManufacturerSparePartNumber in the RDBI service
static void readSparePartNumber(char *buf){ // (!!!!!!!!!ЗАГЛУШКА!!!!!!!!!!!!)
    buf[0] = 0x00;
    buf[1] = 0x00;
    buf[2] = 0x00;
    buf[3] = 0x00;
    buf[4] = 0x00;
}

static char getSystemState(void){
    return ((get_device_mode() == MODE_ECALL) | is_waiting_call()) ? FALSE : TRUE;
}

static char getReserveBatteryState(void){
    switch (getBattState()) {
        case NO_CHARGING:
            return 0x00; // 0000 0000
        case TRICKLE_CHARGING:
            return 0x0A; // 0000 1010
        case FAST_CHARGING:
            return 0x51; // 0101 0001
        case CHECK_OLD:
            return 0x61; // 0110 0001
        case CHECK_ABSENCE:
            return 0xA9; // 1010 1001
        case STABILIZATION:
            return 0xF1; // 1111 0001
        case CARE_CHARGE:
            return 0x59; // 0101 1001
        case UNDEFINED:
        default:
            return 0x7A; // 0111 1010
    }
}
//
// 01 - инициализация
// 81 - входящий вызов
// 8B - not initial
// 0A - idle
// 13 - sleep
// 3b - testing sound
// 46 - sos
// 7F - msd sends
// 55 - registration
// 5D - call prepare
// 66 - dial
static char getGSMState(void){
    IND_TASK_COMMAND event = getCurrenTask();
    switch (event) {
        case SHOW_TEST_STARTED:
            return 0x3B;
        case SHOW_ECALL:            // 11 – call_preparation;
            return 0x5D;
        case SHOW_ECALL_DIALING:    // 12 – dial;
            return 0x66;
        case SHOW_SENDING_MSD:      // MSD_sends;
            return 0x7F;
        default:
//            return get_reg_status() != 2 ? 0x0A : 0x8B;
            return 0x0A;
    }
}

static char getStatus_01(void){
    char resp = 0x00;
    // 0 bit: Current value of vehicle coordinates reliability (1 – true).
    // 1 bit: Value of vehicle memory coordinates reliability i (1 – true).
    // 2 bit: ECallOn status (1 – prohibition).
    // 3-4 bits: ECallTo status (0 – to 112, 1 – to test server, 2 – to debug server, 3 – undef).
    // 5 bit: NoAutomaticTriggering status (1 – prohibition).
    // 6 bit: Start test allow. ( 1 –allow).
    // 7 bit: ENS Prohibition status (1 – prohibition).

    BOOLEAN coordinatesReliability = (get_gnss_data_fast().fix == 0) ? FALSE : TRUE;
    BOOLEAN memoryCoordinatesReliability = (get_gnss_data_fast().fix == 0) ? FALSE : TRUE;
    BOOLEAN eCallStatus = TRUE;
    UINT8 ecallTo;
    char str[50];
    get_ecall_to_fast(str, sizeof(str));
    if (strstr(str, ECALL_TO_DEBUG) == 0) {
        ecallTo = 0; // DEBUG
    } else if (strstr(str, ECALL_TO_112) == 0){
        ecallTo = 2; // 112
    } else {
        ecallTo = 3; // UNDEFINED
    }
    BOOLEAN noAutomaticTriggering = get_aut_trig();
    BOOLEAN startTestAllow = ((get_device_mode() == MODE_ECALL) | is_waiting_call()) ? FALSE : TRUE;
    BOOLEAN ensStatus = FALSE;
    resp = coordinatesReliability | (memoryCoordinatesReliability << 1) | (eCallStatus << 2) | (ecallTo << 3) |
            (noAutomaticTriggering << 5) | (startTestAllow << 6) | (ensStatus);
    return resp;
}

static void getUTCTime(char *buf){
    T_GnssData gnss_data = get_gnss_data();
    if (get_rtc_timestamp() > gnss_data.utc) {
        gnss_data.utc = get_rtc_timestamp();
    }
    UINT32 timeStamp = gnss_data.utc;
    for(int i = 0; i < 4; i++){
        buf[i] = timeStamp >> (i*8);
    }
}

static void getDeveloperInformation(char *buf){//(!!!!!!!ЗАГЛУШКА!!!!!)
    buf[0] = 0x00;
    buf[1] = 0x01;
}

static void getReserveBatteryVoltage(char *buf){
    INT32 volt = getBatteryVolatage() / 4.3;
    buf[0] = volt;
    buf[1] = volt >> 8;
}

static char getAngleForRolloverDetection(void){
    return getCurrentRollAng();
}

// [0,255]
static char crashManual(void){
    return get_manual_cnt();
}

// [0,255]
static char crashAirbag(void){
    return  get_airbags_cnt();
}

// [0,255]
// Какое число отправляешь, то высвечивается в терминале
static char crashRollover(void){
    return  get_rollover_cnt();
}