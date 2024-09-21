/**
 * @version 1.0.0
 * @date 31.08.2022
 * @author: DKozenkov     
 * @brief РћР±СЂР°Р±РѕС‚РєР° РєРѕРјР°РЅРґ UDS-СЃРµСЂРІРёСЃР° 13.3.1 RoutineControl service (ID 0x31)
 */

/* Include files ================================================================================*/
#include "log.h"
#include "../hdr/uds_service31.h"
#include "../hdr/uds.h"
#include "../hdr/securityAccess_27.h"
#include "msd_storage.h"
#include "sys_utils.h"
#include "testing.h"
#include "tasks.h"
#include "ecall.h"
#include "m2mb_types.h"
#include "../hdr/uds.h"
#include "../../accelerometer/hdr/accel_driver.h"
#include "../hdr/securityAccess_27.h"
#include "codec.h"
#include "../hdr/readDataByIdentifier_22.h"
#include "sim.h"
#include <string.h>
#include "log.h"
#include "tasks.h"
#include "era_helper.h"
#include "factory_param.h"
#include "sys_param.h"
#include "atc_era.h"
#include "char_service.h"
#include "sys_utils.h"
#include "at_command.h"
#include <inttypes.h>
#include <m2mb_ati.h>
#include <stdio.h>

static UINT16 start_ClearALLMSD(UINT8* buf, UINT16 buf_size);

static UINT16 start_SelfTest(UINT8 mode, UINT8* buf, UINT16 buf_size);

static UINT16 start_SOSCall(UINT8* buf, UINT16 buf_size, BOOLEAN auto_call);

static UINT16 start_Clear_EEPROM(UINT8* buf, UINT16 buf_size);

// TEST_MODE function
static UINT16 search_networks(UINT8* buf, UINT16 buf_size);

static UINT16 rec_mic(UINT8 time, UINT8* buf, UINT16 buf_size);

static UINT16 play_mic(UINT8* buf, UINT16 buf_size);

static UINT16 stop_ClearALLMSD(UINT8* buf, UINT16 buf_size);

static UINT16 stop_SelfTest(UINT8* buf, UINT16 buf_size);

static UINT16 stop_SOSCall(UINT8* buf, UINT16 buf_size);

static UINT16 stop_resp(UINT8 r_numb, UINT16 r_id, UINT8* buf, UINT16 buf_size);

static UINT16 result_resp(UINT8 r_numb, UINT16 r_id, UINT8* buf, UINT16 buf_size);

static UINT16 pos_resp_31(UINT8 subf_id, UINT16 r_id, UINT8 add, UINT8* buf, UINT16 buf_size);

static UINT16 at_execute(char* data, UINT16 data_size, char* buf, UINT16 buf_size);


static void testing_res_cb(TESTING_RESULT result);

static void ecall_res_cb(ECALL_RESULT result);

static UINT8 r_ClearALLMSD = NOT_DEFINED; // routine ClearALLMSD
static UINT8 r_DeleteFile = NOT_DEFINED; // routine DELETE_FILE
static UINT8 r_ModuleRegistration = NOT_DEFINED; // routine MODULE_REGISTRATION
static UINT8 r_SelfTest = NOT_DEFINED; // routine SELF_TEST
static UINT8 r_BootstrapTransition = NOT_DEFINED; // routine BOOTSTRAP_TRANSITION
static UINT8 r_SOSCall = NOT_DEFINED; // routine SOS_CALL

static UINT8 availableNetworks = 0;

static void getNetworks(void);

static BOOLEAN nowTest = FALSE;
static INT32 lastRecTime = 0;

#define MAX_SIZE_AT 2048
static char at_req[MAX_SIZE_AT];
static char at_resp[MAX_SIZE_AT];
static UINT32 resp_size = 0;
static INT32 at_timeout = 0;
static BOOLEAN has_resp = FALSE;

static void exec_at(void);

static UINT16 get_at_result(char* data, UINT16 data_size, char* buf, UINT16 buf_size);


INT32 TEST_MODE_task(INT32 type, INT32 param1, INT32 param2) {
    (void) param2;
    switch (type) {
        case EXECUTE_AT:
            LOG_DEBUG("EXECUTE_AT");
            exec_at();
            return M2MB_OS_SUCCESS;
        case CHECK_AVAILABLE_NETWORK:
            LOG_DEBUG("CHECK_AVAILABLE_NETWORK");
            getNetworks();
            nowTest = FALSE;
            return M2MB_OS_SUCCESS;
        case START_MIC_REC:
            LOG_DEBUG("START_MIC_REC %d", param1);
            azx_sleep_ms(100);
            on_mute();
            azx_sleep_ms(100);
            record_audio("test_aud.wav", param1);
        //            azx_sleep_ms(param1);
            off_mute();
            nowTest = FALSE;
            lastRecTime = param1;
            LOG_DEBUG("END_MIC_REC");
            return M2MB_OS_SUCCESS;
        case PLAY_LAST_MIC_REC:
            LOG_DEBUG("PLAY_LAST_MIC_REC");
            azx_sleep_ms(100);
            on_mute();
            azx_sleep_ms(100);
            play_audio("test_aud.wav");
            azx_sleep_ms(lastRecTime);
            off_mute();
            nowTest = FALSE;
            LOG_DEBUG("END PLAY_LAST_MIC_REC");
            return M2MB_OS_SUCCESS;
    }
    return M2MB_OS_SUCCESS;
}

static void exec_at(void) {
    LOG_DEBUG("Start execute >%s<, %i timeout", at_req, at_timeout);
    memset(&at_resp[0], 0x00, MAX_SIZE_AT);
    INT32 res = at_era_sendCommand(at_timeout, at_resp, MAX_SIZE_AT, at_req);
    if (res == 0) sprintf(&at_resp[0], "%s", "ERROR");
    resp_size = strlen(at_resp);
    LOG_DEBUG("RESULT >%s<. Size = %d", at_resp, resp_size);
    nowTest = FALSE;
    has_resp = TRUE;
}

static UINT16 get_at_result(char* data, UINT16 data_size, char* buf, UINT16 buf_size) {
    (void) data;
    (void) data_size;
    (void) buf;
    (void) buf_size;
    if (!has_resp || resp_size == 0 || buf_size < resp_size) return negative_resp(
        buf, ROUTINE_CONTROL, CONDITIONS_NOT_CORRECT);
    memset(&buf[0], 0x00, buf_size);

    buf[0] = ROUTINE_CONTROL | 0x40;
    buf[1] = REQUEST_ROUTINE_RESULTS;
    buf[2] = 0x00;
    buf[3] = 0x07;
    buf[4] = resp_size >> 8;
    buf[5] = resp_size;
    memcpy(&buf[6], &at_resp[0], resp_size);
    return 6 + resp_size;
}

static UINT16 at_execute(char* data, UINT16 data_size, char* buf, UINT16 buf_size) {
    (void) buf;
    (void) buf_size;
    (void) data_size;
    if (data_size <= 8) return negative_resp(buf, ROUTINE_CONTROL, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    if (nowTest) return negative_resp(buf, ROUTINE_CONTROL, BUSY_REPEAT_REQUEST);
    nowTest = TRUE;

    has_resp = FALSE;
    memset(&at_req[0], 0x00, MAX_SIZE_AT);
    memset(&at_resp[0], 0x00, MAX_SIZE_AT);
    resp_size = 0;
    at_timeout = 0;

    UINT16 cmd_size = (data[4] << 8) | data[5];

    memcpy(&at_req[0], &data[6], cmd_size);
    at_req[cmd_size] = '\r';


    UINT8 tm = data[6 + cmd_size];
    if (tm < 0x01 || tm > 0x3C) return negative_resp(buf, ROUTINE_CONTROL, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    at_timeout = data[6 + cmd_size] * 1000;

    char cmd[cmd_size + 1];
    memset(&cmd[0], 0x00, cmd_size + 1);
    memcpy(&cmd[0], &data[6], cmd_size);
    cmd[cmd_size] = '\r';

    send_to_test_mode_task(EXECUTE_AT, 0, 0);

    buf[0] = ROUTINE_CONTROL | 0x40;
    buf[1] = START_ROUTINE;
    buf[2] = 0x00;
    buf[3] = 0x07;
    buf[4] = 0x00;
    return 5;
}


UINT16 handler_service31(UINT8* data, UINT16 data_size, UINT8* buf, UINT16 buf_size) {
    UINT16 len = 0;
    if (buf_size < 3 || data_size < 4 || data[0] != ROUTINE_CONTROL) return 0;

    print_hex("content: ", (char *) data, data_size);
    UINT8 subf_id = data[1];
    UINT16 r_id = (data[2] << 8) | data[3];

    if (isTestMode()) {
        switch (subf_id) {
            case START_ROUTINE:
                switch (r_id) {
                    case AT_EXECUTE:
                        return at_execute((char *) data, data_size, (char *) buf, buf_size);
                    case MODULE_REGISTRATION:
                        return search_networks(buf, buf_size);
                    case SELF_TEST:
                        switch (data[4]) {
                            case MICROPHONE_REC:
                                return rec_mic((UINT8) data[5], buf, buf_size);
                            case MICROPHONE_PLAY:
                                return play_mic(buf, buf_size);
                        }
                }
            case REQUEST_ROUTINE_RESULTS:
                return get_at_result((char *) data, data_size, (char *) buf, buf_size);
        }
        return negative_resp((char *) buf, ROUTINE_CONTROL, SECURITY_ACCESS_DENIED);
    }


    if ((r_id == CLEAR_ALL_MSD || r_id == DELETE_FILE || r_id == BOOTSTRAP_TRANSITION || r_id == SOS_CALL || r_id ==
         CLEAR_EEPROM) && !isAccessLevel()) {
        len = negative_resp((char *) buf, ROUTINE_CONTROL, SECURITY_ACCESS_DENIED);
    } else
        switch (subf_id) {
            case START_ROUTINE:
                switch (r_id) {
                    case CLEAR_ALL_MSD: len = start_ClearALLMSD(buf, buf_size);
                        break;
                    case SOS_CALL:
                        len = start_SOSCall(buf, buf_size, TRUE);
                        break;
                    case SOS_CALL_MANUAL:
                        len = start_SOSCall(buf, buf_size, FALSE);
                        break;
                    case SELF_TEST: {
                        UINT8 opt = 0;
                        if (data_size == 5) opt = data[4];
                        len = start_SelfTest(opt, buf, buf_size);
                        break;
                    }
                    case CLEAR_EEPROM:
                        len = start_Clear_EEPROM(buf, buf_size);
                        break;
                    default:
                    case MODULE_REGISTRATION:
                    case DELETE_FILE:
                    case BOOTSTRAP_TRANSITION:
                        len = pos_resp_31(subf_id, r_id, ROUTINE_START_FAILURE, buf, buf_size);
                        break;
                }
                break;
            case STOP_ROUTINE:
                switch (r_id) {
                    case CLEAR_ALL_MSD: len = stop_resp(r_ClearALLMSD, r_id, buf, buf_size);
                        break;
                    case SOS_CALL: len = stop_resp(r_SOSCall, r_id, buf, buf_size);
                        break;
                    case SELF_TEST: len = stop_resp(r_SelfTest, r_id, buf, buf_size);
                        break;
                    default:
                    case DELETE_FILE:
                    case MODULE_REGISTRATION:
                    case BOOTSTRAP_TRANSITION: len = negative_resp((char *) buf, ROUTINE_CONTROL, requestSequenceError);
                        break;
                }
                break;
            case REQUEST_ROUTINE_RESULTS:
                switch (r_id) {
                    case CLEAR_ALL_MSD: len = result_resp(r_ClearALLMSD, r_id, buf, buf_size);
                        break;
                    case SOS_CALL: len = result_resp(r_SOSCall, r_id, buf, buf_size);
                        break;
                    case SELF_TEST: len = result_resp(r_SelfTest, r_id, buf, buf_size);
                        break;
                    case DELETE_FILE: len = result_resp(r_DeleteFile, r_id, buf, buf_size);
                        break;
                    case MODULE_REGISTRATION: len = result_resp(r_ModuleRegistration, r_id, buf, buf_size);
                        break;
                    case BOOTSTRAP_TRANSITION: len = result_resp(r_BootstrapTransition, r_id, buf, buf_size);
                        break;
                    default: len = negative_resp((char *) buf, ROUTINE_CONTROL, requestSequenceError);
                        break;
                        break;
                }
                break;
            default: len = negative_resp((char *) buf, ROUTINE_CONTROL, REQUEST_OUT_OF_RANGE);
                break;
        }
    return len;
}

static UINT16 rec_mic(UINT8 time, UINT8* buf, UINT16 buf_size) {
    if ((nowTest == TRUE) | (time < 1) | (time > 30)) {
        LOG_DEBUG("Some test is already started");
        return pos_resp_31(START_ROUTINE, SELF_TEST, 0xFF, buf, buf_size);
    } else {
        LOG_DEBUG("Mic record was started");
        nowTest = TRUE;
        send_to_test_mode_task(START_MIC_REC, time * 1000, 0);
        return pos_resp_31(START_ROUTINE, SELF_TEST, 0x00, buf, buf_size);
    }
}

static UINT16 play_mic(UINT8* buf, UINT16 buf_size) {
    if ((nowTest == TRUE) | (lastRecTime == 0)) {
        LOG_DEBUG("Some test is already started");
        return pos_resp_31(START_ROUTINE, SELF_TEST, 0xFF, buf, buf_size);
    } else {
        LOG_DEBUG("Play last mic record was started");
        nowTest = TRUE;
        send_to_test_mode_task(PLAY_LAST_MIC_REC, 0, 0);
        return pos_resp_31(START_ROUTINE, SELF_TEST, 0x00, buf, buf_size);
    }
}

static UINT16 search_networks(UINT8* buf, UINT16 buf_size) {
    if (nowTest) {
        LOG_DEBUG("Some test is already started");
        return pos_resp_31(START_ROUTINE, MODULE_REGISTRATION, 0xFF, buf, buf_size);
    } else {
        LOG_DEBUG("Search networks was started");
        availableNetworks = 0;
        nowTest = TRUE;
        send_to_test_mode_task(CHECK_AVAILABLE_NETWORK, 0, 0);
        return pos_resp_31(START_ROUTINE, MODULE_REGISTRATION, 0x00, buf, buf_size);
    }
}

static UINT16 stop_resp(UINT8 r_numb, UINT16 r_id, UINT8* buf, UINT16 buf_size) {
    switch (r_numb) {
        case NOT_DEFINED: return negative_resp((char *) buf, ROUTINE_CONTROL, requestSequenceError);
        case ROUTINE_STOP: return pos_resp_31(STOP_ROUTINE, r_id, ROUTINE_STOP_FAILURE, buf, buf_size);
        case ROUTINE_RUN:
            switch (r_id) {
                case CLEAR_ALL_MSD: return stop_ClearALLMSD(buf, buf_size);
                case SOS_CALL: return stop_SOSCall(buf, buf_size);
                case SELF_TEST: return stop_SelfTest(buf, buf_size);
                default: return negative_resp((char *) buf, ROUTINE_CONTROL, requestSequenceError);
            }
        default: return negative_resp((char *) buf, ROUTINE_CONTROL, requestSequenceError);
    }
}

static UINT16 result_resp(UINT8 r_numb, UINT16 r_id, UINT8* buf, UINT16 buf_size) {
    if (r_numb == NOT_DEFINED) return negative_resp((char *) buf, ROUTINE_CONTROL, requestSequenceError);
    else return pos_resp_31(REQUEST_ROUTINE_RESULTS, r_id, r_SelfTest, buf, buf_size);
}


static UINT16 pos_resp_31(UINT8 subf_id, UINT16 r_id, UINT8 add, UINT8* buf, UINT16 buf_size) {
    UINT16 len = 5;
    if (len > buf_size) return 0;
    buf[0] = RCPR;
    buf[1] = subf_id;
    buf[2] = r_id >> 8;
    buf[3] = r_id & 0xFF;
    buf[4] = add;
    return len;
}

static UINT16 start_ClearALLMSD(UINT8* buf, UINT16 buf_size) {
    UINT8 res = ROUTINE_START_FAILURE;
    if (r_ClearALLMSD != ROUTINE_RUN) {
        res = ROUTINE_START_SUCCESS;
        r_ClearALLMSD = ROUTINE_RUN;
        r_ClearALLMSD = delete_all_MSDs() ? ROUTINE_FINISH_SUCCESS : ROUTINE_FINISH_ERROR;
        if (r_ClearALLMSD == ROUTINE_FINISH_SUCCESS) set_restart_reason(0);
    }
    return pos_resp_31(START_ROUTINE, CLEAR_ALL_MSD, res, buf, buf_size);
}

static UINT16 stop_ClearALLMSD(UINT8* buf, UINT16 buf_size) {
    r_ClearALLMSD = ROUTINE_STOP;
    return pos_resp_31(STOP_ROUTINE, CLEAR_ALL_MSD, ROUTINE_STOP_SUCCESS, buf, buf_size);
}

static UINT16 start_SelfTest(UINT8 mode, UINT8* buf, UINT16 buf_size) {
    UINT8 res = ROUTINE_START_FAILURE;
    if (r_SelfTest != ROUTINE_RUN) {
        if (is_testing_working() || get_device_mode() == MODE_TEST || get_device_mode() == MODE_ECALL)
            res = ROUTINE_START_FAILURE;
        else {
            res = ROUTINE_START_SUCCESS;
            r_SelfTest = ROUTINE_RUN;
            UINT32 comm = START_TESTING;
            switch (mode) {
                case MICROPHONE_DYNAMIC: comm = START_TEST_MD_ONLY;
                    break;
                case SOS_TEST_CALL: comm = START_TEST_CALL_ONLY;
                    break;
                default: comm = START_TESTING;
                    break;
            }
            send_to_testing_task(comm, 0, (INT32) &testing_res_cb);
        }
    }
    return pos_resp_31(START_ROUTINE, SELF_TEST, res, buf, buf_size);
}

static UINT16 stop_SelfTest(UINT8* buf, UINT16 buf_size) {
    UINT8 res;
    if (is_testing_working()) {
        r_SelfTest = ROUTINE_STOP;
        send_to_testing_task(TEST_FORCED_STOP, FALSE, 0);
        res = ROUTINE_STOP_SUCCESS;
    } else res = ROUTINE_STOP_FAILURE;
    return pos_resp_31(STOP_ROUTINE, SELF_TEST, res, buf, buf_size);
}

static void testing_res_cb(TESTING_RESULT result) {
    LOG_INFO("uds31 tr:%s", get_testing_res_descr(result));
    r_SelfTest = result == TEST_R_SUCCESS
                     ? ROUTINE_FINISH_SUCCESS
                     : (result == TEST_R_FORCED_STOPPED ? ROUTINE_STOP : ROUTINE_FINISH_ERROR);
}

static UINT16 start_SOSCall(UINT8* buf, UINT16 buf_size, BOOLEAN auto_call) {
    UINT8 res = ROUTINE_START_FAILURE;
    if (r_SOSCall != ROUTINE_RUN && get_device_mode() != MODE_ECALL) {
        res = ROUTINE_START_SUCCESS;
        r_SOSCall = ROUTINE_RUN;
        if (auto_call) {
            send_to_ecall_task(ECALL_AIRBAGS, ECALL_NO_REPEAT_REASON, (INT32) &ecall_res_cb);
        } else {
            send_to_ecall_task(ECALL_MANUAL_SOS, ECALL_NO_REPEAT_REASON, (INT32) &ecall_res_cb);
        }
    }
    return pos_resp_31(START_ROUTINE, SOS_CALL, res, buf, buf_size);
}

static UINT16 stop_SOSCall(UINT8* buf, UINT16 buf_size) {
    UINT8 res;
    if (is_ecall_in_progress()) {
        r_SOSCall = ROUTINE_STOP;
        send_to_ecall_task(ECALL_FORCED_STOP, 0, 0);
        res = ROUTINE_STOP_SUCCESS;
    } else res = ROUTINE_STOP_FAILURE;
    return pos_resp_31(STOP_ROUTINE, SOS_CALL, res, buf, buf_size);
}

static void ecall_res_cb(ECALL_RESULT result) {
    LOG_INFO("uds31 er:%s", get_ecall_result_desc(result));
    r_SOSCall = result == ECALL_R_SUCCESS
                    ? ROUTINE_FINISH_SUCCESS
                    : ((result == ECALL_R_FORCED_STOP || result == ERA_R_FORCED_STOP)
                           ? ROUTINE_STOP
                           : ROUTINE_FINISH_ERROR);
}

static void getNetworks(void) {
    char resp[500] = {0};
    at_era_sendCommand(60000, resp, sizeof(resp), "AT+COPS=?\r");
    availableNetworks = 0;

    for (unsigned int j = 0; j < strlen(resp); j++) {
        if (resp[j] == '\n' || resp[j] == '\r' || resp[j] == ' ' || resp[j] == '\t') {
            deleteChar(resp, j);
            j--;
        }
    }

    for (UINT16 i = 6; i < strlen(resp); i++) {
        resp[i - 6] = resp[i];
    }
    resp[strlen(resp) - 6] = '\0';

    resp[strlen(resp) - 3] = '\0';

    for (UINT16 i = 0; i < strlen(resp) - 1; i++) {
        if ((resp[i] == ',') & (resp[i + 1] == ',')) {
            resp[i] = '\0';
        }
    }

    for (UINT16 i = 0; i < strlen(resp) - 1; i++) {
        if ((resp[i] == ',') & (resp[i + 1] == '(')) {
            availableNetworks++;
        }
    }
    if (availableNetworks != 0) availableNetworks++;
    LOG_DEBUG("GET_NETWORKS: >%s< \r\navailable networks %d", resp, availableNetworks);
}

extern UINT8 getAvailableNetworks(void) {
    return availableNetworks;
}

static UINT16 start_Clear_EEPROM(UINT8* buf, UINT16 buf_size) {
    resetAccelPref();
    set_vin(VIN_DEFAULT);
    set_activation_flag(FALSE);
    set_airbags_cnt(0);
    set_manual_cnt(0);
    set_rollover_cnt(0);
    delete_all_MSDs();
    set_restart_reason(0);
    send_to_ind_task(NO_ACTIVATED, 0, 0);
    return pos_resp_31(START_ROUTINE, CLEAR_EEPROM, ROUTINE_START_SUCCESS, buf, buf_size);
}
