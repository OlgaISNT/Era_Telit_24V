/**
 * @file a_player.c
 * @version 1.0.1
 * @date 16.05.2022
 * @author: DKozenkov     
 * @brief Потокобезопасный проигрыватель аудио-файлов
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "sys_utils.h"
#include "atc_era.h"
#include "at_command.h"
#include "codec.h"
#include "tasks.h"
#include "a_player.h"
#include "failures.h"
#include "log.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static void dummy_result_cb (A_PLAYER_RESULT result, INT32 ctx, INT32 params) {(void) result; (void) ctx; (void) params;}
static AZX_TIMER_ID timer_id;
static M2MB_OS_MTX_ATTR_HANDLE mtx_attr_handle;
static M2MB_OS_MTX_HANDLE      mtx_handle = NULL;
static volatile BOOLEAN process = FALSE;
static a_player_result_cb callback = &dummy_result_cb;
static BOOLEAN initialized = FALSE;
static volatile INT32 context;
static volatile INT32 params;
/* Local function prototypes ====================================================================*/
void send_result(A_PLAYER_RESULT result);
static M2MB_OS_RESULT_E mut_init(void);
static void get_mutex(void);
static void put_mutex(void);
static void finished(A_PLAYER_RESULT result);
BOOLEAN is_busy(void);
/* Static functions =============================================================================*/
static void get_mutex(void) {
    if (mtx_handle == NULL) mut_init();
    M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtx_handle, M2MB_OS_WAIT_FOREVER);
    if (res != M2MB_OS_SUCCESS) LOG_ERROR("apmtx");
}
static void put_mutex(void) {
    if (mtx_handle == NULL) {
        LOG_ERROR("mtP");
        return;
    }
    M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtx_handle);
    if (res != M2MB_OS_SUCCESS) LOG_ERROR("apmtx");
}
static M2MB_OS_RESULT_E mut_init(void) {
    M2MB_OS_RESULT_E os_res = m2mb_os_mtx_setAttrItem( &mtx_attr_handle,
                                                       CMDS_ARGS(
                                                               M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
                                                               M2MB_OS_MTX_SEL_CMD_NAME, "ap1",
                                                               M2MB_OS_MTX_SEL_CMD_USRNAME, "ap1",
                                                               M2MB_OS_MTX_SEL_CMD_INHERIT, 3
                                                       )
    );
    if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("ape1");
    os_res = m2mb_os_mtx_init( &mtx_handle, &mtx_attr_handle);
    if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("ape2");
    return os_res;
}

static void timer_handler(void* ctx, AZX_TIMER_ID t_id) {
    (void)ctx;
    (void)t_id;
    LOG_DEBUG("AP th");
    finished(A_PLAYER_TIMEOUT);
}

void ap_urc_received_cb(const CHAR* msg) {
    if (msg != NULL && strstr(msg, "#APLAYEV: 0") != NULL) {
        //LOG_DEBUG("audio_p URC:%s", msg);
        //m2mb_os_taskSleep(M2MB_OS_MS2TICKS(300)); // задержка возможно нужна, чтобы последующая проверка AT#APLAY? иногда не показывала занятости аудиокодека
        finished(A_PLAYER_OK);
    } else finished(A_PLAYER_OTHER_ERR);
}

static void finished(A_PLAYER_RESULT result) {
    deinit_timer(&timer_id);
    if (!process) {
        return;
    }
    process = FALSE;
    if (result != A_PLAYER_OK) {
        LOG_ERROR("Audio player error: %i", result);
    }
    send_result(result);
}

void send_result(A_PLAYER_RESULT result) {
    if (result != 0) LOG_DEBUG("A_Player res: %i", result);
    if (callback != NULL) callback(result, context, params);
}

BOOLEAN is_busy(void) {
    char resp[200];
    INT32 ans = ati_sendCommandEx(2*1000, resp, sizeof(resp), "AT#APLAY?\r");
    //LOG_WARN("busy resp: %s, ans:%i, process:%i", resp, ans, process);
    if (process || ans == 0 || strstr(resp, "#APLAY: 1") != NULL) return TRUE;
    else return FALSE;
}

/* Global functions =============================================================================*/
void play_audio_file(char *audio_file_name, UINT32 timeout_sec, a_player_result_cb cb, INT32 ctx, INT32 p) {
    get_mutex();
    //LOG_WARN("Try play %s", audio_file_name);
    if (!initialized) {
        initialized = TRUE;
        at_era_addUrcHandler("#APLAYEV:", ap_urc_received_cb);
    }
    if (is_busy()) {
        //LOG_WARN("APlayer is busy1!");
        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(3500));
        if (is_busy()) {
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(2500));
            if (is_busy()) {
                put_mutex();
                LOG_WARN("APlayer is busy2!");
                if (cb != NULL) cb(A_PLAYER_BUSY, ctx, p);
                return;
            }
        }
    }
    process = TRUE;
    callback = cb;
    context = ctx;
    params = p;
    UINT8 r = play_audio(audio_file_name);
    if (r != ERR_CODEC_NO_ERROR) {
        LOG_ERROR("Codec err:%i", r);
        process = FALSE;
        put_mutex();
        send_result(A_PLAYER_OTHER_ERR);
        return;
    }
    deinit_timer(&timer_id);
    timer_id = init_timer("a_timer_t", timeout_sec * 1000, timer_handler);
    put_mutex();
}


