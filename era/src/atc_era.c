/**
 * @file atc_era.c
 * @version 1.0.0
 * @date 12.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "azx_ati.h"
#include "app_cfg.h"
#include "log.h"
/* Local defines ================================================================================*/
#define CTRL_Z 0x1A
#define BUF_SIZE 1000
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static M2MB_OS_MTX_ATTR_HANDLE mtx_attr_handle;
static M2MB_OS_MTX_HANDLE      mtx_handle = NULL;
/* Local function prototypes ====================================================================*/
static void print_send(char *string);
static void print_response(const char *string);
static void print_resp_ok(BOOLEAN r);
static void get_mutex(void);
static void put_mutex(void);
static M2MB_OS_RESULT_E mut_init(void);
/* Static functions =============================================================================*/
static void print_send(char *string) {
    if (strstr(string, "AT$GPSACP") == NULL) { // чтобы не перегружать лог
        LOG_DEBUG("ati send: %s", string);
    }// else LOG_DEBUG("gnss_req");
}
static void print_response(const char *string) {
    (void) string;
    //LOG_DEBUG("ati resp: <%s>", (string == NULL ? "NULL" : string));
}
static void print_resp_ok(BOOLEAN r) {
    if (!r) LOG_DEBUG("ati OK: %i", r);
}
static void get_mutex(void) {
    if (mtx_handle == NULL) mut_init();
    M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtx_handle, M2MB_OS_WAIT_FOREVER);
    if (res != M2MB_OS_SUCCESS) LOG_ERROR("gmtx");
}

static void put_mutex(void) {
    if (mtx_handle == NULL) {
        LOG_ERROR("mtP");
        return;
    }
    M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtx_handle);
    if (res != M2MB_OS_SUCCESS) LOG_ERROR("pmtx");
}

static M2MB_OS_RESULT_E mut_init(void) {
    M2MB_OS_RESULT_E os_res = m2mb_os_mtx_setAttrItem( &mtx_attr_handle,
                                                       CMDS_ARGS(
                                                               M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
                                                               M2MB_OS_MTX_SEL_CMD_NAME, "ec1",
                                                               M2MB_OS_MTX_SEL_CMD_USRNAME, "ate1",
                                                               M2MB_OS_MTX_SEL_CMD_INHERIT, 3
                                                       )
    );
    if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("ATe1");
    os_res = m2mb_os_mtx_init( &mtx_handle, &mtx_attr_handle);
    if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("ATe2");
    return os_res;
}

static BOOLEAN try_set_MSD(char *msd_string) {
    BOOLEAN r = FALSE;
    char *at = "AT#MSDSEND\r";
    //print_send(at);
    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(200));
    const char *ans = azx_ati_sendCommandEx(AT_INSTANCE_ERA, 2000, at);
    UINT32 len = strlen(msd_string);
    if (ans != NULL && strlen(ans) == 2 && strstr(ans, "\r\n") != NULL && len < 2000) {
        char msd[len + 1];
        memcpy(msd, msd_string, len);
        msd[len] = CTRL_Z;
        print_send(msd_string);
        r = azx_ati_sendCommandExpectOkEx(AT_INSTANCE_ERA, 4001, msd);
        print_resp_ok(r);
    } else r = FALSE;
    return r;
}

/* Global functions =============================================================================*/
BOOLEAN set_MSD(char *msd_string) {
    get_mutex();
    BOOLEAN r = FALSE;
    if (try_set_MSD(msd_string)) r = TRUE;
    else {
        m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1000));
        LOG_WARN("2a setMSD");
        if (try_set_MSD(msd_string)) r = TRUE;
    }
    put_mutex();
    return r;
}

INT32 at_era_sendCommand(INT32 timeout_ms, char *resp, int len, const CHAR* cmd, ...) {
    get_mutex();
    va_list arg; // @suppress("Type cannot be resolved")
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    va_start(arg, cmd);
    vsnprintf(buf, BUF_SIZE, (void *)cmd, arg);
    va_end(arg);
    print_send(buf);
    const char *r = azx_ati_sendCommandEx(AT_INSTANCE_ERA, timeout_ms, buf);
    print_response(r);
    INT32 length = 0;
    if (r != NULL) {
        length  = strlen(r);
        strncpy(resp, r, len > length ? length : len - 1);
        resp[length] = 0;
    }
    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(80));
    put_mutex();
    return length;
}

BOOLEAN at_era_sendCommandExpectOk(INT32 timeout_ms, const CHAR* cmd, ...) {
    get_mutex();
    va_list arg; // @suppress("Type cannot be resolved")
    char buf[BUF_SIZE];
    memset(buf, 0, BUF_SIZE);
    va_start(arg, cmd);
    vsnprintf(buf, BUF_SIZE, (void *)cmd, arg);
    va_end(arg);
    print_send(buf);
    BOOLEAN r = azx_ati_sendCommandExpectOkEx(AT_INSTANCE_ERA, timeout_ms, buf);
    print_resp_ok(r);
    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(80));
    put_mutex();
    return r;
}

void at_era_addUrcHandler(const CHAR* msg_header, azx_urc_received_cb cb) {
    azx_ati_addUrcHandlerEx(AT_INSTANCE_ERA, msg_header, cb);
}

