/**
 * @file sim.c
 * @version 1.0.0
 * @date 12.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

/* Include files ================================================================================*/
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "azx_timer.h"
#include "atc_era.h"
#include "sys_utils.h"
#include "at_command.h"
#include "tasks.h"
#include "sim.h"
#include "log.h"
#include "factory_param.h"
#include "failures.h"
#include <inttypes.h>

/* Local defines ================================================================================*/
#define TIMEOUT_SIM_SWITCHING  30*1000 // ожидание переключения профиля СИМ
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static AZX_TIMER_ID timer_id;
static SIM_PROFILE to_profile;
static sim_switcher_result_cb cb = NULL;
static BOOLEAN process = FALSE;
static BOOLEAN initialized = FALSE;
static char iccidg[50] = {0}; // ICCID профиля ЭРА-Глонасс
static char iccidc[50] = {0}; // ICCID коммерческого профиля СИМ
static char imsi[50] = {0};   // IMSI
static BOOLEAN iccidg_read = FALSE; // флаг успешного чтения значения ICCIDG из СИМ-чипа
static BOOLEAN iccidc_read = FALSE; // флаг успешного чтения значения ICCIDC из СИМ-чипа
static BOOLEAN read_iccid(char *resp);
static iccid_result_cb iccidg_process_result_cb;
static iccid_result_cb iccidc_process_result_cb;
/* Local function prototypes ====================================================================*/
static void switch_profile(SIM_PROFILE profile, sim_switcher_result_cb callback);
static BOOLEAN reinsert_SIM(UINT32 delay_ms);
static void send_result(void);
static void finishing(void);
static void sim_timer_handler(void* ctx, AZX_TIMER_ID t_id);
static BOOLEAN read_cimi(char *resp);

/* Static functions =============================================================================*/
static void switch_profile(SIM_PROFILE profile, sim_switcher_result_cb callback) {
    if (!initialized) {
        initialized = TRUE;
        at_era_addUrcHandler("#STN:", urc_received_cb);
    }
    process = TRUE;
    to_profile = profile;
    cb = callback;
//	if (!reinsert_SIM(1000)) {
//		LOG_ERROR("Can't reinsert SIM");
//		send_result();
//		return;
//	}
    char resp[200];
    INT32 r = at_era_sendCommand(4000, resp, sizeof(resp),
                                 "AT+CSIM=28,\"%s\"\r",
                                 (profile == ERA_PROFILE ? "80C2000009D3070202018110017E" : "80C2000009D3070202018110017F"));
    if (r == 0 || strstr((const char *)resp, "+CSIM: 4,\"9") == NULL) {
        LOG_ERROR("Can't AT+CSIM=28");
        send_result();
        return;
    }
    timer_id = init_timer("sim_timer", TIMEOUT_SIM_SWITCHING, sim_timer_handler);
}

static void finishing(void) {
    //azx_timer_deinit(timer_id); //azx_timer_stop(timer_id);
    char resp[100];
    INT32 r = at_era_sendCommand(
            4000, resp, sizeof(resp),
            "AT+CSIM=20,\"%s\"\r",
            (to_profile == ERA_PROFILE ? "80C2000005EE03EF0120" : "80C2000005EE03EF0124"));
    if (r == 0 || strstr((const char *)resp, "+CSIM: 4,\"9") == NULL) {
        LOG_ERROR("Can't AT+CSIM=20");
        send_result();
        return;
    }

    if (!reinsert_SIM(1000)) {
        LOG_ERROR("Can't reinsert SIM");
        send_result();
        return;
    }
    send_result();
}

static BOOLEAN reinsert_SIM(UINT32 delay_ms) {
    switch (getSimSlotNumber()) {
        case 0:
            at_era_sendCommandExpectOk(2000, "AT#SIMSELECT=1\r");
            break;
        case 1:
            at_era_sendCommandExpectOk(2000, "AT#SIMSELECT=2\r");
            break;
        default:
            LOG_ERROR("INCORRECT SIM SLOT NUMBER");
            break;
        }
    if (!at_era_sendCommandExpectOk(2000, "AT#SIMDET=0\r")) return FALSE;
    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(delay_ms));
    if (!at_era_sendCommandExpectOk(2000, "AT#SIMDET=1\r")) return FALSE;
    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(3000));
    return TRUE;
}

static void send_result(void) {
    deinit_timer(&timer_id);
    SIM_PROFILE profile = get_SIM_profile();
    process = FALSE;
    LOG_INFO("New SIM profile: %s", (profile == ERA_PROFILE ? "ERA" : "COMM"));
    if (cb != NULL) {
        cb(profile);
        cb = NULL;
    }
}

static void sim_timer_handler(void* ctx, AZX_TIMER_ID t_id) {
    (void)ctx;
    (void)t_id;
    LOG_DEBUG("SIM th");
    send_to_sim_task(SIM_SWITCH_TIMER, 0, 0);
}

static BOOLEAN read_cimi(char *resp){
    char cimi[100];
    if (ati_sendCommandEx(3000, cimi, sizeof(cimi), "AT#CIMI\r") == 0) {
        LOG_ERROR("Can't read_cimi");
        resp = 0;
        return FALSE;
    } else {
        char *p = strstr(cimi, "#CIMI: ");
        char *e = strstr(p, "\r\n");
        if (p != NULL && e != NULL) {
            p += 7;
            memcpy(resp, p, e - p);
            LOG_DEBUG("IMSI: %s", resp);
            return TRUE;
        } else{
            return FALSE;
        }
    }
}

static BOOLEAN read_iccid(char *resp) {
    char iccid[100];
    if (ati_sendCommandEx(3000, iccid, sizeof(iccid), "AT#CCID\r") == 0) {
        LOG_ERROR("Can't read_iccid");
        resp = 0;
        return FALSE;
    } else {
        char *p = strstr(iccid, "#CCID: ");
        char *e = strstr(p, "\r\n");
        if (p != NULL && e != NULL) {
            p += 7;
            memcpy(resp, p, e - p);
            LOG_DEBUG("ICCID: %s", resp);
            read_cimi(imsi);
            return TRUE;
        } else{
            return FALSE;
        }
    }
}

static void iccidg_sim_switch_cb(SIM_PROFILE sim_profile) {
    BOOLEAN success;
    if (sim_profile == ERA_PROFILE) {
        if (read_iccid(iccidg)) {
            iccidg_read = TRUE;
            success = TRUE;
        } else success = FALSE;
    } else success = FALSE;
    iccidg_process_result_cb(success, iccidg);
}

static void iccidc_sim_switch_cb(SIM_PROFILE sim_profile) {
    BOOLEAN success;
    if (sim_profile == COMM_PROFILE) {
        if (read_iccid(iccidc)) {
            iccidc_read = TRUE;
            success = TRUE;
        } else success = FALSE;
    } else success = FALSE;
    iccidc_process_result_cb(success, iccidc);
}

/* Global functions =============================================================================*/
void urc_received_cb(const CHAR* msg) {
    if (msg != NULL && strstr(msg, "#STN: 01,04") != NULL) {
        send_to_sim_task(URC_SIM_WAS_RESET, 0, 0);
    }
}

SIM_PROFILE get_SIM_profile(void) {
    char resp[50];
    INT32 r = at_era_sendCommand(4000, resp, sizeof(resp), "AT+CIMI\r");
    if (r == 0 || strstr((const char *)resp, "OK") == NULL) return UNKNOWN_PROFILE;
    else {
    	if (!iccidg_read &&	read_iccid(iccidg)) iccidg_read = TRUE;
    	return ERA_PROFILE;
    }
//    if (
//        strstr((const char *)resp, "\n25077") != NULL ||
//        strstr((const char *)resp, "\n25001") != NULL ||
//        strstr((const char *)resp, "\n25099") != NULL ||
//        strstr((const char *)resp, "\n00101") != NULL) {
//        if (!iccidg_read &&	read_iccid(iccidg)) iccidg_read = TRUE;
//        return ERA_PROFILE;
//    }
//    else {
//        if (!iccidc_read &&	read_iccid(iccidc)) iccidc_read = TRUE;
//        return COMM_PROFILE;
//    }
}

/***************************************************************************************************
   SIM_task Нельзя вызывать эту функцию напрямую!
**************************************************************************************************/
INT32 SIM_task(INT32 type, INT32 param1, INT32 param2) {
    sim_switcher_result_cb callback = param1 == 0 ? NULL : (sim_switcher_result_cb) param1;
    (void) param2;
    LOG_DEBUG("SIM_task command: %i", type);
    switch(type) {
        case SIM_PROFILE_TO_ERA:
        case SIM_PROFILE_TO_COMM:
            LOG_INFO("Switching SIM profile to %s", (type == SIM_PROFILE_TO_ERA ? "ERA" : "COMM"));
            SIM_PROFILE profile = get_SIM_profile();
            //LOG_DEBUG("prof: %i", profile);
            if ((type == SIM_PROFILE_TO_ERA && profile == ERA_PROFILE) ||
                (type == SIM_PROFILE_TO_COMM && profile == COMM_PROFILE)) {
                if (callback != NULL) callback(profile);
                else LOG_DEBUG("SIM callback is NULL");
            } else {
                if (process) {
                    LOG_WARN("SIM is switching already");
                    if (callback != NULL) callback(profile);
                } else switch_profile(type == SIM_PROFILE_TO_ERA ? ERA_PROFILE : COMM_PROFILE, callback);
            }
            return M2MB_OS_SUCCESS;
        case SIM_SWITCH_TIMER:
        case URC_SIM_WAS_RESET:
            if (process) finishing();
            return M2MB_OS_SUCCESS;
        default:
            return M2MB_OS_SUCCESS;
    }
}

void read_iccidg(iccid_result_cb callback) {
    iccidg_process_result_cb = callback;
    if (iccidg_read) {
        callback(TRUE, iccidg);
        return;
    } else {
        if (get_SIM_profile() == ERA_PROFILE) {
            if (read_iccid(iccidg)) {
                iccidg_read = TRUE;
                callback(TRUE, iccidg);
            } else callback(FALSE, iccidg);
        } else send_to_sim_task(SIM_PROFILE_TO_ERA, (INT32) &iccidg_sim_switch_cb, 0);
    }
}

void read_iccidc(iccid_result_cb callback) {
    iccidc_process_result_cb = callback;
    if (iccidc_read) {
        callback(TRUE, iccidc);
        return;
    } else {
        if (get_SIM_profile() == COMM_PROFILE) {
            if (read_iccid(iccidc)) {
                iccidc_read = TRUE;
                callback(TRUE, iccidc);
            } else callback(FALSE, iccidc);
        } else send_to_sim_task(SIM_PROFILE_TO_COMM, (INT32) &iccidc_sim_switch_cb, 0);
    }
}

char *get_iccidg(void) {
    return iccidg;
}

unsigned long long get_iccidg_ull(void){
//    read_iccid(iccidg);
    char **ptr = 0;
    return strtoumax(iccidg,ptr, 10);
}

unsigned long long get_imsi_ull(void){
    char **ptr = 0;
    return strtoumax(imsi,ptr, 10);
}

extern BOOLEAN hasIccid(void){
    return iccidg_read;
}
