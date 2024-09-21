/**
 * @file sms.c
 * @version 1.0.0
 * @date 28.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "params.h"
#include "msd.h"
#include "sms.h"
#include "egts.h"
#include "msd_storage.h"
#include "era_helper.h"
#include "sms_manager.h"
#include "azx_timer.h"
#include "atc_era.h"
#include "sys_utils.h"
#include "tasks.h"
#include "log.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static sms_result_cb result_cb;
static AZX_TIMER_ID transm_t = NO_AZX_TIMER_ID;
static T_msd msd; // отправляемая МНД
static volatile INT32 msd_n = -1; // номер в хранилище отправляемой МНД
static volatile UINT8 attempts;
static volatile BOOLEAN stopped = FALSE;
/* Local function prototypes ====================================================================*/
static BOOLEAN get_SMSCA(char *number);
static void send_SMSs(void);
static BOOLEAN set_SMSCA(char *number);
static BOOLEAN check_SMSCA(void);
static void check(void);
static void check_reg(void);
static void prepare(void);
static void finish(SMS_T_RESULT result);
static void sms_sending_handler(BOOLEAN result);
static void sms_timer_handler(void* ctx, AZX_TIMER_ID id);
/* Static functions =============================================================================*/
static BOOLEAN get_SMSCA(char *number) {
    char resp[150] = {0};
    char *atc = "AT+CSCA?\r";
    int length = at_era_sendCommand(2000, resp, sizeof(resp), atc);
    char *plus = strstr(resp, "+CSCA: \"+");
    if (length == 0 || plus == NULL) {
        LOG_ERROR("Can't execute: %s", atc);
        return FALSE;
    } else {
        memcpy(number, plus + 8, 12);
        *(number + 12) = 0;
        return TRUE;
    }
}

static BOOLEAN set_SMSCA(char *number) {
    return at_era_sendCommandExpectOk(2000, "AT+CSCA=%s\r", number);
}

static BOOLEAN check_SMSCA(void) {
    char smsca[50] = {0};
    get_SMSCA(smsca);
    char smsca_p[50];
    get_sms_center_number(smsca_p, sizeof(smsca_p));
    if (strcmp(smsca, smsca_p) != 0) {
        if (!set_SMSCA(smsca_p)) LOG_ERROR("Can't set SMS center");
        else if (!at_era_sendCommandExpectOk(2000, "AT+CSAS=0\r")) LOG_ERROR("Can't +CSAS");
        return FALSE;
    }
    return TRUE;
}

static void send_SMSs(void) {
    stopped = FALSE;
    msd_n = -1;
    sms_init_variables();
    check_SMSCA();
    check();
}

static void check(void) {
    if (stopped) return;
    msd_n = find_msd_n(&msd, MSD_SMS);
    if (msd_n != -1) {
        check_reg();
    } else {
        to_log_info_uart("No SMSs for sending");
        if (!is_ecall_in_progress()) send_to_ind_task(CANCEL_FLASHING, 0, 0); // это на случай отправки СМС после регистрации через "пустой" тестовый ECALL
        finish(SMS_R_ALL_SMS_SENT);
    }
}

static void check_reg(void) {
    if (stopped) return;
    //if (!is_registered()) try_registration(); //finish(SMS_R_NO_REGISTRATION);
    attempts = 0;
    prepare();
}

static void prepare(void) {
    deinit_timer(&transm_t);
    transm_t = init_timer("transm_t", get_int_mem_transm_interval() * 60 * 1000, sms_timer_handler);
    if (egts_config_msd(MSD_SMS)) {
        LOG_DEBUG("EGTS outgoing was prepared");
        sms_send(TRUE);
    } else {
        LOG_ERROR("Can't prepare egts");
        delete_MSD(msd_n);
        check();
    }
}

static void sms_sending_handler(BOOLEAN result_ok) {
    if (result_ok) {
        BOOLEAN del_ok = delete_MSD(msd_n);
        print_msd_storage_status();
        if (!del_ok) {
            to_log_error_uart("Invalid msd_n:%i", msd_n);
            finish(SMS_R_SENDING_FAIL);
        } else check();
    } else {
        attempts++;
        to_log_info_uart("SMS didn't send, attempt: %i", attempts);
        if (attempts >= get_int_mem_transm_attempts()) {
            if (!move_sms_to_saved()) LOG_ERROR("Can't move MSD to 'SAVED'");
            msd_n = find_msd_n(&msd, MSD_SMS);
            if (msd_n != -1) {
                finish(SMS_R_SENDING_FAIL);
            } else check_reg();
        }
    }
}

static void finish(SMS_T_RESULT result) {
    stopped = TRUE;
    deinit_timer(&transm_t);
    if (result_cb != NULL) result_cb(result);
}

static void sms_timer_handler(void* ctx, AZX_TIMER_ID id) {
    (void) ctx;
    if (transm_t == id) {
        LOG_DEBUG("transm_t expired");
        send_to_sms_task(SMS_TRANSM_TIMER, 0, 0);
    }
}
/* Global functions =============================================================================*/
/***************************************************************************************************
   SMS_task Нельзя вызывать эту функцию напрямую!
**************************************************************************************************/
INT32 SMS_task(INT32 type, INT32 param1, INT32 param2) {
    (void) param1;
    (void) param2;
    LOG_DEBUG("SMS_task command: %i", type);
    switch(type) {
        case SMS_SENDING_RESULT:
            sms_sending_handler(param1 == 0 ? TRUE : FALSE);
            break;
        case SMS_SEND_ALL_FROM_STORAGE:
            result_cb = param2 == 0 ? NULL : (sms_result_cb) param2;
            send_SMSs();
            break;
        case SMS_TRANSM_TIMER:
            prepare();
            break;
        case SMS_STOP_SENDING:
            if (!stopped) {
                LOG_INFO("SMS service was stopped");
                stopped = TRUE;
                deinit_timer(&transm_t);
            }
            break;
        default:
            break;
    }
    return M2MB_OS_SUCCESS;
}
