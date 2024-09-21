/**
 * @file at_interface.c
 * @version 1.0.0
 * @date 07.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "softdog.h"
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "azx_ati.h"
#include "atc_era.h"
#include "app_cfg.h"
#include "log.h"
/* Local defines ================================================================================*/
#define CTRL_Z 0x1A
#define BUF_SIZE 1000
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static BOOLEAN wdt_started = FALSE;
//static char buf[BUF_SIZE];
static M2MB_OS_MTX_ATTR_HANDLE mtx_attr_handle;
static M2MB_OS_MTX_HANDLE      mtx_handle = NULL;
/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
static void print_send(char *string) {
	LOG_TRACE("ati send: %s", string);
	(void) string;
}
static void print_response(const char *string) {
	LOG_TRACE("ati resp: <%s>", (string == NULL ? "NULL" : string));
	(void) string;
}
static void print_resp_ok(BOOLEAN r) {
	LOG_TRACE("ati OK: %i", r);
	(void) r;
}
static void get_mutex(void) {
	if (mtx_handle == NULL) {
		LOG_ERROR("mtxA");
		return;
	}
	M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtx_handle, M2MB_OS_WAIT_FOREVER);
	if (res != M2MB_OS_SUCCESS) LOG_ERROR("Can't get mtx:%i", res);
}

static void put_mutex(void) {
	if (mtx_handle == NULL) {
		LOG_ERROR("mtxA");
		return;
	}
	M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtx_handle);
	if (res != M2MB_OS_SUCCESS) LOG_ERROR("Can't put mtx:%i", res);
}

/* Global functions =============================================================================*/

M2MB_OS_RESULT_E ati_init(void) {
	M2MB_OS_RESULT_E os_res = m2mb_os_mtx_setAttrItem( &mtx_attr_handle,
	                                      CMDS_ARGS(
	                                        M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
	                                        M2MB_OS_MTX_SEL_CMD_NAME, "atс1",
	                                        M2MB_OS_MTX_SEL_CMD_USRNAME, "atс1",
	                                        M2MB_OS_MTX_SEL_CMD_INHERIT, 1
	                                      )
	                                );
	if (os_res != M2MB_OS_SUCCESS) {
		LOG_ERROR("ATMsF");
	}
	os_res = m2mb_os_mtx_init( &mtx_handle, &mtx_attr_handle);
	if (os_res != M2MB_OS_SUCCESS) {
		LOG_ERROR("ATMiF");
	}
	//init_azx_ati();
	azx_ati_disable_logs();
	return os_res;
}

void ati_deinit(void) {
	azx_ati_deinitEx(AT_INSTANCE_GENERAL);
	azx_ati_deinitEx(AT_INSTANCE_ERA);
#ifdef RELEASE
	if (wdt_started) resetSoftdog();
#endif
	//LOG_WARN("________ATI deinit OK!");
}

INT32 ati_sendCommandEx(INT32 timeout_ms, char *resp, int len, const CHAR* cmd, ...) {
	get_mutex();
	va_list arg; // @suppress("Type cannot be resolved")
	char buf[BUF_SIZE];
	memset(buf, 0, 1000);
    va_start(arg, cmd);
 	vsnprintf(buf, BUF_SIZE, (void *)cmd, arg);
    va_end(arg);
    print_send(buf);
    const char *r = azx_ati_sendCommandEx(AT_INSTANCE_GENERAL, timeout_ms, buf);
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

BOOLEAN ati_sendCommandExpectOkEx(INT32 timeout_ms, const CHAR* cmd, ...) {
	get_mutex();
	va_list arg; // @suppress("Type cannot be resolved")
	char buf[BUF_SIZE];
	memset(buf, 0, BUF_SIZE);
    va_start(arg, cmd);
 	vsnprintf(buf, BUF_SIZE, (void *)cmd, arg);
    va_end(arg);
    print_send(buf);
    BOOLEAN r = azx_ati_sendCommandExpectOkEx(AT_INSTANCE_GENERAL, timeout_ms, buf);
    print_resp_ok(r);
    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(80));
    put_mutex();
    return r;
}

/** Нельзя использовать эту функцию во избежание неправильной работы функций ati_sendCommandEx и ati_sendCommandExpectOkEx
 * Проблема связана с тем, что если подписаться на URC, содержимое которого будет частично совпадать с содержимым ответа
 * указанных функций, то они не вернут правильный результат. Это ошибка в используемой библиотеке azx_ati.
 * Поэтому подписки через at_era_addUrcHandler на все необходимые URCs нужно делать только через atc_era и следить,
 * чтобы функции отправки AT-команд в atc_era не использовали at-команды с ответами, частично идентичными содержимому в подписанных URC.
 * Для примера см. в коде использование "+CREG"
 */
void ati_addUrcHandler(const CHAR* msg_header, azx_urc_received_cb cb) {
	azx_ati_addUrcHandlerEx(AT_INSTANCE_GENERAL, msg_header, cb);
}

INT32 ATC_TT_task(INT32 type, INT32 param1, INT32 param2) {
	(void) type;
	(void) param1;
    (void) param2;
    if (initSoftdog(45*1000) != 0) LOG_ERROR("Can't start softdog");
    else {
    	wdt_started = TRUE;
    	LOG_DEBUG("ATC WDT was started");
    	const char *tc = "AT+CREG=1\r";
    	while (1) {
    		m2mb_os_taskSleep( M2MB_OS_MS2TICKS(23 * 1000));
    		if (!at_era_sendCommandExpectOk(3*1000, tc)) { // проверку нужно выполнять именно в этом канале, т.к. в нём оформляется подписка на URC
    			m2mb_os_taskSleep( M2MB_OS_MS2TICKS(4 * 1000));
    			if (!at_era_sendCommandExpectOk(3*1000, tc)) {
    				LOG_CRITICAL("AT fault");
    				break;
    			}
    		}
    		resetSoftdog();
    	}
    }
    return M2MB_OS_SUCCESS;
}
