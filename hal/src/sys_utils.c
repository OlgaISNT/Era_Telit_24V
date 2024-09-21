/**
 * @file sys_utils.c
 * @version 1.0.0
 * @date 08.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/reboot.h>
#include "m2mb_types.h"
#include "m2mb_sys.h"
#include "m2mb_os_api.h"
#include "at_command.h"
#include "app_cfg.h"
#include "prop_manager.h"
#include "sys_utils.h"
#include "azx_string_utils.h"
#include "tasks.h"
#include "gpio_ev.h"
#include "ecall.h"
#include "azx_timer.h"
#include "log.h"
#include "sys_param.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static AZX_TIMER_ID power_off_t = NO_AZX_TIMER_ID;
static volatile BOOLEAN to_restart = FALSE;
static volatile DEVICE_MODE device_mode = MODE_ERA;
static M2MB_OS_MTX_ATTR_HANDLE mtx_attr_handle;
static M2MB_OS_MTX_HANDLE      mtx_handle = NULL;
/* Local function prototypes ====================================================================*/
static void sys_timer_handler(void* ctx, AZX_TIMER_ID id);
static M2MB_OS_RESULT_E mut_init(void);
static void get_mutex(void);
static void put_mutex(void);
/* Static functions =============================================================================*/
static void sys_timer_handler(void* ctx, AZX_TIMER_ID id) {
	(void)ctx;
	if (power_off_t == id) to_sleep();
}

static void get_mutex(void) {
	if (mtx_handle == NULL) mut_init();
	M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtx_handle, M2MB_OS_WAIT_FOREVER);
	if (res != M2MB_OS_SUCCESS) LOG_ERROR("sysmtx");
}
static void put_mutex(void) {
	if (mtx_handle == NULL) {
		LOG_ERROR("mtP");
		return;
	}
	M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtx_handle);
	if (res != M2MB_OS_SUCCESS) LOG_ERROR("sysmtx");
}
static M2MB_OS_RESULT_E mut_init(void) {
	M2MB_OS_RESULT_E os_res = m2mb_os_mtx_setAttrItem( &mtx_attr_handle,
	                                      CMDS_ARGS(
	                                        M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
	                                        M2MB_OS_MTX_SEL_CMD_NAME, "sys1",
	                                        M2MB_OS_MTX_SEL_CMD_USRNAME, "sys1",
	                                        M2MB_OS_MTX_SEL_CMD_INHERIT, 3
	                                      )
	                                );
	if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("sys1");
	os_res = m2mb_os_mtx_init( &mtx_handle, &mtx_attr_handle);
	if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("sys2");
	return os_res;
}

/* Global functions =============================================================================*/
char *get_mode_descr(DEVICE_MODE mode) {
	switch (mode) {
		case MODE_ERA_OFF: 	return "ERA_OFF";
		case MODE_ERA: 		return "ERA";
		case MODE_TEST:		return "TEST";
		case MODE_ECALL:	return "ECALL";
		case MODE_TELEMATIC:return "TELEMATIC";
		default:			return "UNKNOWN";
	}
}

DEVICE_MODE get_device_mode(void) {
	return device_mode;
}

void set_device_mode(DEVICE_MODE mode) {
	device_mode = mode;
	LOG_INFO("Device mode: %s", get_mode_descr(mode));
}

void restart(UINT32 timeout_ms) {
	if (to_restart) return;
	to_restart = TRUE;
	to_log_info_uart("! Modem will be restarted in %i ms !", timeout_ms);
	m2mb_os_taskSleep( M2MB_OS_MS2TICKS(timeout_ms));
	m2mb_sys_reboot(0);
	//ati_sendCommandExpectOkEx(2000, "AT#ENHRST=1,0\r"); // 	AT+CFUN=6  AT#REBOOT // если не сработает предыдущая команда
}

AZX_TIMER_ID init_timer(char *timer_name, UINT32 timeout_ms, azx_expiration_cb cb) {
	get_mutex();
	AZX_TIMER_ID id = NO_AZX_TIMER_ID;
	id = azx_timer_initWithCb(cb, NULL, 0);
	if (id == NO_AZX_TIMER_ID) LOG_ERROR("%s creation failed!", timer_name);
	else {
		LOG_DEBUG("%s id%i started for: %i mS", timer_name, id, timeout_ms);
		azx_timer_start(id, timeout_ms, FALSE);
	}
	put_mutex();
	return id;
}

BOOLEAN deinit_timer(AZX_TIMER_ID *id) {
	get_mutex();
	BOOLEAN r;
	if (*id != NO_AZX_TIMER_ID) {
		r = azx_timer_deinit(*id);
		LOG_DEBUG("timer id%i was deinit", *id);
		*id = NO_AZX_TIMER_ID;
	}
	put_mutex();
	return r;
}

INT32 get_adc(UINT8 adc_number) {
	if (adc_number < 1 || adc_number > 3) return -1;
    FILE *pf;
    char command[100];
    char data[200];
    sprintf(command, "cat /sys/devices/qpnp-vadc-8/adc%i", adc_number);
    pf = popen(command, "r");
    // Get the data from the process execution
    fgets(data, sizeof(data) , pf);
 //   LOG_DEBUG("ADC Voltage:%s", data);
    if (pclose(pf) != 0) LOG_ERROR("Failed to close command stream");
    if (strlen(data) > 20) {
    	char *s = strstr(data, "Result:");
    	if (s == NULL) return -1;
    	s += 7;
    	char *e = strstr(data, " Raw:");
    	if (e == NULL || (e - s >= 30)) return -1;
    	char res[30] = {0};
    	memcpy(res, s, e - s);
    	//LOG_DEBUG("res:%s", res);
    	UINT32 value;
    	if (azx_str_to_ul(res, &value) == 0)
    		return value;
    	else return -1;
    } else return -1;
}

INT32 get_temp(void) {
    FILE *pf;
    char *command = "cat /sys/class/thermal/thermal_zone5/temp";
    char data[200];
    pf = popen(command, "r");
    fgets(data, sizeof(data) , pf);
    if (pclose(pf) != 0){
        LOG_ERROR("Failed to close command stream");
    }
    return atoi(data);
}

void manage_sleep(BOOLEAN ign_state, BOOLEAN t30_state) {
	deinit_timer(&power_off_t);
	//LOG_DEBUG("ign:%i, T30_state:%i", ign_state, t30_state);
	if (ign_state) {
		onOffMode(TRUE);
	} else {
		power_off_t = init_timer("power_off_t", t30_state ? 30 * 1000 :  300 * 1000, sys_timer_handler);
	}
}

void to_sleep(void) {
	LOG_INFO("Device mode:%s, %s callback waiting", get_mode_descr(get_device_mode()), (is_waiting_call() ? "" : "no"));
	if (!isIgnition() && !is_waiting_call() && (get_device_mode() == MODE_ERA_OFF || get_device_mode() == MODE_ERA)) {
		LOG_INFO("Ign off -> power off");
		onOffMode(FALSE);
		m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1000));
		reboot(RB_POWER_OFF);
		ati_sendCommandExpectOkEx(2000, "AT#SHDN\r");
	} else {
		LOG_INFO("No sleep, because: ignition:%i, callback waiting:%i, dev_mode:%s", isIgnition(), is_waiting_call(), get_mode_descr(get_device_mode()));
	}
}

UINT32 get_free_RAM(void) {
	UINT32 bytes_available = 0;
	M2MB_OS_RESULT_E res = m2mb_os_memInfo( M2MB_OS_MEMINFO_BYTES_AVAILABLE, &bytes_available );
	if ( res != M2MB_OS_SUCCESS ) LOG_ERROR("Unable to get mem info");
	return bytes_available;
}
