//
// Created by klokov on 31.10.2022.
//

#include "log.h"
#include "../hdr/writeDataByIdentifier_2E.h"
#include "../hdr/uds.h"
#include "../hdr/securityAccess_27.h"
#include "string.h"
#include "params.h"
#include "sys_param.h"
#include "../../accelerometer/hdr/accel_driver.h"
#include "factory_param.h"
#include "gpio_ev.h"
#include "azx_ati.h"
#include "at_command.h"
#include "sys_utils.h"

static UINT16 pos_resp(UINT16 id, char *data, char *buf, UINT16 bufSize);
static BOOLEAN setECALLManualCanCancel(char data);
static BOOLEAN setRolloverAngle(char data);
static BOOLEAN setRolloverTime(char data);
static BOOLEAN setAccelCalibrationX(char *data);
static BOOLEAN setAccelCalibrationY(char *data);
static BOOLEAN setAccelCalibrationZ(char *data);
static BOOLEAN setEcallNoAutomaticTriggering(char data);
static BOOLEAN setSertState(char data);
static BOOLEAN setDirectionAngle(char data);
static BOOLEAN setAudioHWSettings(char data);
static BOOLEAN setCCFT(char *data);
static BOOLEAN setMsdMaxTransmissionTime(char *data);
static BOOLEAN setDOStates(char *data);
static BOOLEAN switchUsbPorts(char *data);
static UINT32 byte_array_to_int(char *buffer);

UINT16 handler_service2E(char *data, char *buf, UINT16 max_len){
    if(data == NULL){
        return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    if ((data[1] != WRITE_DATA_BY_IDENTIFIER) & (data[2] != WRITE_DATA_BY_IDENTIFIER)){
        return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    char pack[50];
    memset(pack, '\0', 50);
    UINT16 id = 0x0000;
    UINT16 len = 0;

    if((data[1] == WRITE_DATA_BY_IDENTIFIER) & (data[2] != WRITE_DATA_BY_IDENTIFIER)){
        len = data[0];
        len -= 3;
        id = ((UINT8) data[2] << 8) | ((UINT8) data[3]);
        memcpy(&pack[0], &data[4], len);
    } else if((data[1] != WRITE_DATA_BY_IDENTIFIER) & (data[2] == WRITE_DATA_BY_IDENTIFIER)){
        len = (((data[0] - 0x10) << 8) | data[1]);
        len -= 3;
        id = ((UINT8) data[3] << 8) | ((UINT8) data[4]);
        memcpy(&pack[0], &data[5], len);
    } else {
        return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }
    return pos_resp(id, pack, buf, max_len);
}


typedef BOOLEAN (*set_int_p) (int value);
static BOOLEAN set_int_param(char *source, int size, char *param_name, int min, int max, set_int_p setter) {
	if (size != 1 && size != 2 && size != 4) return FALSE;
	UINT8 p[4] = {0};
	for (int i = 0; i < size; i++) p[size - i - 1] = source[i];
	INT32 v = *((INT32*) p);
	LOG_INFO("Trying to set %s to: %i", param_name, v);
    if (v >= min && v <= max) {
    	return setter(v);
    } else {
    	LOG_WARN("%s inv arg", param_name);
    	return FALSE;
    }
}

typedef BOOLEAN (*set_str_p) (char *value);
static BOOLEAN set_str_param(char *param_name, char *value, int size, set_str_p setter) {
	if (size < 1 || value == NULL) return FALSE;
	char v[size + 1];
	memset(v, 0, sizeof(v));
	memcpy(v, value, size);
	LOG_INFO("Trying to set %s to: %s", param_name, v);
    return setter(v);
}

static UINT16 pos_resp(UINT16 id, char *data, char *buf, UINT16 bufSize){
    LOG_DEBUG("x%4.4X , %02X", id, data[0]);
    (void) bufSize;
    UINT16 len = 0;
    buf[0] = 0x6E;
    buf[1] = id >> 8;
    buf[2] = id;
    len = 3;
    switch (id) {
        case 0xFD02: // +VehicleFuelType (1 byte) (Secured)
           // LOG_DEBUG("VehicleFuelType 0xFD02 %02X", data);
           // if (is_activated() | !isAccessLevel()){
           if (!isAccessLevel()){
                len = negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 1, PROPULSION_TYPE_NAME, PROPULSION_TYPE_MIN, PROPULSION_TYPE_MAX, set_propulsion_type)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x000D: // +TCM working mode (1 byte) (Secured)
           // if (is_activated() | !isAccessLevel()){
           if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else { // Р•СЃР»Рё РѕС€РёР±РєР°
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
            }
        case 0x0010: // +ECALLManualCanCancel (1 byte) (Secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (setECALLManualCanCancel(data[0]) == TRUE) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0011: // +GLONASS and GLONASS/GPS groups (1 byte) (Secured)
            return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
        case 0x0014: // +VehicleCoordinatesReliabilityMem (1 byte) (no secured)
        	LOG_DEBUG("VehicleCoordinatesReliabilityMem: %02X", data);
        	return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
        case 0x0015: // +GsmRegistrationStateMem (1 byte) (no secured)
            LOG_DEBUG("GsmRegistrationStateMem: %02X", data);
            return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
        case 0x0017: // +EcallOn (1 byte) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
        case 0x0019: // +TestModeEndDistance (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else {
            	if (set_int_param(data, 2, TEST_MODE_END_DISTANCE_NAME, TEST_MODE_END_DISTANCE_MIN, TEST_MODE_END_DISTANCE_MAX, set_test_mode_end_dist)) {
            		return len;
            	} else {
            		return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            	}
            }
        case 0x001B: // +AutomaticActivationProhibition (1 byte) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
        case 0xFD01: // +VehicleType (1 byte) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 1, VEHICLE_TYPE_NAME, VEHICLE_TYPE_MIN, VEHICLE_TYPE_MAX, set_vehicle_type)){
                return len;
            } else {
                LOG_DEBUG("FAILED");
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0301: // + CallAutoAnswerTime (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 2, CALL_AUTO_ANSWER_TIME_NAME, CALL_AUTO_ANSWER_TIME_MIN, CALL_AUTO_ANSWER_TIME_MAX, set_call_auto_answer_time)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0302: // +ECALLTestNumber (20 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_str_param(ECALL_TEST_NUMBER_NAME, data, 20, set_test_number)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0303: // +ECALLToTest (1 byte) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else {
            	if (data[0] == 0) {
                    incrementSpec();
            		if (set_str_param(ECALL_TO_NAME, ECALL_TO_112, strlen(ECALL_TO_112), set_ecall_to)) return len;
            		else return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            	} else {
            		if (set_str_param(ECALL_TO_NAME, ECALL_TO_DEBUG, strlen(ECALL_TO_DEBUG), set_ecall_to)) return len;
            		else return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            	}
            }
        case 0x0304: // +NadDeregistrationTimer (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 2, ECALL_NAD_DEREGISTRATION_TIME_NAME, ECALL_NAD_DEREGISTRATION_TIME_MIN, ECALL_NAD_DEREGISTRATION_TIME_MAX, set_nad_dereg_time)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0305: // +NadRegistrationAtStartup (1 byte) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
        case 0x0306: // +SMSCenterNumber (20 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_str_param(SMS_CENTER_NUMBER_NAME, data, 20, set_sms_center_number)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0307: // +SMSFallbackNumber (20 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_str_param(ECALL_SMS_FALLBACK_NUMBER_NAME, data, 20, set_sms_fallback_number)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x030E: // +TestRegistrationPeriod (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 2, TEST_REGISTRATION_PERIOD_NAME, TEST_REGISTRATION_PERIOD_MIN, TEST_REGISTRATION_PERIOD_MAX, set_test_reg_period)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x030F: // +EcallManualDialAttempts (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 2, ECALL_MANUAL_DIAL_ATTEMPTS_NAME, ECALL_MANUAL_DIAL_ATTEMPTS_MIN, ECALL_MANUAL_DIAL_ATTEMPTS_MAX, set_manual_dial_attempts)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0310: // +IntMemTransmitInterval (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 2, INT_MEM_TRANSMIT_INTERVAL_NAME, INT_MEM_TRANSMIT_INTERVAL_MIN, INT_MEM_TRANSMIT_INTERVAL_MAX, set_int_mem_transm_interval)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0311: // +IntMemTransmitAttempts (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 2, INT_MEM_TRANSMIT_ATTEMPTS_NAME, INT_MEM_TRANSMIT_ATTEMPTS_MIN, INT_MEM_TRANSMIT_ATTEMPTS_MAX, set_int_mem_transm_attempts)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0316: // +SmsFallbackTestNumber (20 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_str_param(SMS_FALLBACK_DEBUG_NUMBER_NAME, data, 20, set_sms_fallback_debug_number)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0319: // +EcallAutoDialAttempts (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 2, ECALL_AUTO_DIAL_ATTEMPTS_NAME, ECALL_AUTO_DIAL_ATTEMPTS_MIN, ECALL_AUTO_DIAL_ATTEMPTS_MAX, set_auto_dial_attempts)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x031A: // +EcallDialDuration (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 2, ECALL_DIAL_DURATION_NAME, ECALL_DIAL_DURATION_MIN, ECALL_DIAL_DURATION_MAX, set_dial_duration)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x031B: // +PostTestRegistrationTime (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 2, POST_TEST_REGISTRATION_TIME_NAME, POST_TEST_REGISTRATION_TIME_MIN, POST_TEST_REGISTRATION_TIME_MAX, set_post_test_reg_time)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0323: // +AudioProfile (1 byte) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (set_int_param(data, 1, AUDIOPROFILE_POSITION_NAME, AUDIOPROFILE_POSITION_MIN, AUDIOPROFILE_POSITION_MAX, set_audio_prof_pos)) {
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x0402: // RolloverAngle
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (setRolloverAngle(data[0])){
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
            }
        case 0x0403: // RolloverTime (1 byte) secured
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (setRolloverTime(data[0])){
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
            }
        case 0x0404: // AccelCalibrationX (????) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (setAccelCalibrationX(data)){
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
            }
        case 0x0405: // AccelCalibrationY (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (setAccelCalibrationY(data)){
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
            }
        case 0x0406: // AccelCalibrationZ (2 bytes) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (setAccelCalibrationZ(data)){
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
            }
        case 0x0407: // +EcallNoAutomaticTriggering (1 byte) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (setEcallNoAutomaticTriggering(data[0])){
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
            }
        case 0x040E: // SertState (1 byte) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (setSertState(data[0])){
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
            }
        case 0x040F: // DirectionAngle (1 byte) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (setDirectionAngle(data[0])){
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
            }
        case 0x0420: // Audio_HW_Settings (1 byte) (secured)
            // if (is_activated() | !isAccessLevel()){
                if (!isAccessLevel()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            } else if (setAudioHWSettings(data[0])){
                return len;
            } else {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
            }
        case 0xF190: // vehicleIdentificationNumber (17 bytes) (no secured)
            LOG_DEBUG("SET_VIN >%s<", data);
            if(get_model_id() != MODEL_ID11){
                // Для всех MODEL_ID кроме 11 только 1 раз можно переписать
                if(is_activated()) {
                    // Если блок уже активирован
                    return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
                }
            }
            // Для остальных MODEL_ID11 можно переписать VIN сколько угодно раз
            if (set_str_param(VIN_NAME, data, 17, set_vin)) {
                set_activation_flag(TRUE);
                getHorQuat();
            } else {
                set_str_param(VIN_NAME, VIN_DEFAULT, 17, set_vin);
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
            }
            return len;
        case 0x1001: // Get Q_HOR
            if(!isAccessLevel()) {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            }
            if(getHorQuat()) {
                LOG_DEBUG("GET_Q_HOR SUCCESS");
                buf[3] = 0x00;
            } else {
                LOG_DEBUG("GET_Q_HOR FAILED");
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
            }
            len++;
            return len;
        case 0x9909: // Set_digital_out_states
            if(!isTestMode()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            }
            if (setDOStates(data)){
                return len;
            }
            return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
        case 0x990A: // disable usb ports
            if(!isTestMode()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            }
            if (switchUsbPorts(data)){
                return len;
            }
            return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
        case 0x990B:
            return len;
        case 0x990D:
            if(!isAccessLevel()) {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            }
            if (setCCFT(data)) {
                return len;
            }
            return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
        case 0x990E:
            if(!isAccessLevel()) {
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            }
            if (setMsdMaxTransmissionTime(data)) {
                return len;
            }
            return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
        case 0x9914:
            if(!isTestMode()){
                return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SECURITY_ACCESS_DENIED);
            }
            if (set_zyx_threshold(data, 4)){
                return len;
            }
            return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);

        default:
            LOG_WARN("CAN WRITE_DATA_BY_IDENTIFIER: undefined id = x%4.4X", id);
            return negative_resp(buf, WRITE_DATA_BY_IDENTIFIER, SUBFUNCTION_NOT_SUPPORTED);
    }
    return len;
}

static BOOLEAN switchUsbPorts(char *data){
    BOOLEAN res = TRUE;
    if (data[0] == 0x01){
        LOG_DEBUG("Enable USB ports");
        res = ati_sendCommandExpectOkEx(10000, "AT#USBCFG=0\r");
    } else if (data[0] == 0x00){
        LOG_DEBUG("Disable USB ports");
        res = ati_sendCommandExpectOkEx(10000, "AT#USBCFG=3\r");
    } else {
        return FALSE;
    }
    return res;
}

static BOOLEAN setDOStates(char *data){
//    LOG_DEBUG("setDOStates %02X %02X %02X %02X %02X %02X %02X %02X", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    // OUTPUTS
    char states = data[0];
    enAudio(((states & 0x80) == 0x80) ? TRUE : FALSE);        // 7 EN_AUDIO
    serviceControl(((states & 0x40) == 0x40) ? TRUE : FALSE); // 6 SERVICE_CTRL
    greenModeCtrl(((states & 0x20) == 0x20) ? TRUE : FALSE);  // 5 GREEN_MODE_CTRL
    sosCtrl(((states & 0x10) == 0x10) ? TRUE : FALSE);        // 4 SOS_CTRL
    onCharge(((states & 0x08) == 0x08) ? TRUE : FALSE);       // 3 ON_CHARGE
    onOffMode(((states & 0x04) == 0x04) ? TRUE : FALSE);      // 2 ON_OFF_MODE
    setWdOut(((states & 0x02) == 0x02) ? TRUE : FALSE);       // 1 WD_OUT
    redMode(((states & 0x01) == 0x01) ? TRUE : FALSE);        // 0 RED_MODE_CTRL
    return TRUE;
}

//0010
static BOOLEAN setECALLManualCanCancel(char data){
    switch (data) {
        case 0xC7:
            LOG_DEBUG("ECALLManualCanCancel: active");
            set_manual_can_cancel(TRUE);
            return TRUE;
        case 0x7C:
            LOG_DEBUG("ECALLManualCanCancel: inactive");
            set_manual_can_cancel(FALSE);
            return TRUE;
        default:
            LOG_DEBUG("ECALLManualCanCancel: unknown");
            return FALSE;
    }
}

// 0402
static BOOLEAN setRolloverAngle(char data){
    LOG_DEBUG("RolloverAngle: %d", data);
    if (data >= 0xB4){
        return FALSE;
    }
    return set_rollover_angle(data);
}

// 0403
static BOOLEAN setRolloverTime(char data){
    if (data == 0xFF){
        LOG_DEBUG("RolloverTime: invalid value");
        return FALSE;
    }
    return set_rollover_time(data);;
}

// 0404
static BOOLEAN setAccelCalibrationX(char *data){
    UINT16 val = ((data[0] << 8)) | data[1];
    if (val == 0x8000){ // INVALID VALUE 0x8000
        LOG_DEBUG("AccelCalibrationX: invalid value");
        return FALSE;
    }
    LOG_DEBUG("AccelCalibrationX: %d", (INT16) val);
    return TRUE;
}

// 0405
static BOOLEAN setAccelCalibrationY(char *data){
    UINT16 val = ((data[0] << 8)) | data[1];
    if (val == 0x8000){ // INVALID VALUE 0x8000
        LOG_DEBUG("AccelCalibrationY: invalid value");
        return FALSE;
    }
    LOG_DEBUG("AccelCalibrationY: %d", (INT16) val);
    return TRUE;
}

// 0406
static BOOLEAN setAccelCalibrationZ(char *data){
    UINT16 val = ((data[0] << 8)) | data[1];
    if (val == 0x8000){ // INVALID VALUE 0x8000
        LOG_DEBUG("AccelCalibrationZ: invalid value");
        return FALSE;
    }
    LOG_DEBUG("AccelCalibrationZ: %d", (INT16) val);
    return TRUE;
}

// 0407
static BOOLEAN setEcallNoAutomaticTriggering(char data){
    switch (data) {
        case 0x29:
        case 0x92:
            LOG_DEBUG("EcallNoAutomaticTriggering: %s", data == 0x29 ? "prohibited" : "allowed");
            char v[1];
            v[0] = data == 0x29 ? 0 : 1;
            return set_int_param(v, 1, ECALL_NO_AUTOMATIC_TRIGGERING_NAME, ECALL_NO_AUTOMATIC_TRIGGERING_MIN, ECALL_NO_AUTOMATIC_TRIGGERING_MAX, set_no_auto_trig);
        default:
            LOG_DEBUG("EcallNoAutomaticTriggering: undefined");
            return FALSE;
    }
}

static BOOLEAN setSertState(char data){
    data = data & 0x07;
    switch (data) {
        case 0x01:
            LOG_DEBUG("SertState: sert_555533 ");
            return TRUE;
        case 0x02:
            LOG_DEBUG("SertState: sert_no_beeline");
            return TRUE;
        case 0x04:
            LOG_DEBUG("SertState: sert_no_dtc");
            return TRUE;
        default:
            LOG_DEBUG("SertState: undefined");
            return FALSE;
    }
}

// 040F
static BOOLEAN setDirectionAngle(char data){
    LOG_DEBUG("DirectionAngle: %d", data);
    if (data >= 0xB4){
        return FALSE;
    }
    return TRUE;
}

// 0420
static BOOLEAN setAudioHWSettings(char data){
    LOG_DEBUG("Audio_HW_Settings: %d", data);
    if (data == 0xFF){
        return FALSE;
    }
    return TRUE;
}

static BOOLEAN setCCFT(char *data){
    if (data == NULL){
        return FALSE;
    }
    LOG_DEBUG("setCCFT %02X %02X %02X %02X", data[0], data[1], data[2], data[3]);
    INT32 val = byte_array_to_int(data);
    LOG_DEBUG("setCCFT val = %d", val);
    return set_ccft(val);
}

static BOOLEAN setMsdMaxTransmissionTime(char *data){
    if (data == NULL){
        return FALSE;
    }
    LOG_DEBUG("setMsdMaxTransmissionTime %02X %02X %02X %02X", data[0], data[1], data[2], data[3]);
    INT32 val = byte_array_to_int(data);
    LOG_DEBUG("setMsdMaxTransmissionTime val = %d", val);
    return set_msd_max_transm_time(val);
}

static UINT32 byte_array_to_int(char *buffer) {
    UINT32 value = 0;
    for (int i = 0; i < 4; i++) {
        int shift = (4 - 1 - i) * 8;
        value += (buffer[i] & 0x000000FF) << shift;
    }
    return value;
}
