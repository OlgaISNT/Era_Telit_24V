/**
 * @file uart0.c
 * @version 1.0.0
 * @date 11.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "tasks.h"
#include "app_cfg.h"
#include "uart0.h"
#include "commands.h"
#include "azx_uart.h"
#include "log.h"
#include "sys_param.h"
#include "util.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static M2MB_OS_RESULT_E get_data(INT32 size);
static UINT8 buf_out[1000]; // исходящий буфер
static volatile UINT32 od_size = 0; // кол-во отправляемых байт
static volatile UINT32 out_pos = 0; // текущая позиция сравнения байт во входящем и исходящем буферах
static volatile UINT32 in_pos = 0;
static volatile INT32 fd = -1;  // дескриптор uart0
static volatile BOOLEAN opened = FALSE;
static volatile BOOLEAN permission = FALSE;	// разрешение парсинга данных, принятых через UART
static AZX_TIMER_ID cleaner_t = NO_AZX_TIMER_ID;
/* Local function prototypes ====================================================================*/
//static void uart_data_available_cb(UINT16 nbyte, void* ctx);
static INT32 send_to_UART0(const void* buffer, SIZE_T nbyte);
static void u_timer_handler(void* ctx, AZX_TIMER_ID id);
/* Static functions =============================================================================*/
static M2MB_OS_RESULT_E get_data(INT32 size) {
	if (cleaner_t != NO_AZX_TIMER_ID) {
		azx_timer_deinit(cleaner_t);
		cleaner_t = NO_AZX_TIMER_ID;
	}
	UINT8 buffer[size];
	INT32 read = azx_uart_read(0, buffer, size);
	//print_hex("Rec UART0:", (char*)buffer, read);
	//m2mb_os_taskSleep( M2MB_OS_MS2TICKS(100));
//#ifdef UART0_ECHO
//	int wrote = azx_uart_write(0, (const char *) buffer, read);
//	if (wrote == 0) LOG_WARN("UART0 write err");
//#endif
#ifdef UART0_ECHO_OFF

//	if (out_pos < od_size) {
//		out_pos += size;
//	} else {
//		out_pos = 0;
//		od_size = 0;
//		command_handler(buffer, size);
//	}
	//m2mb_os_taskSleep( M2MB_OS_MS2TICKS(100)); // необходимо, чтобы обеспечить успешность приёма "эха" отправленных символов и эквивалентность in_pos и out_pos
	if (in_pos < out_pos) {
		in_pos += size;
		//LOG_DEBUG("in_pos: %i, out_pos: %i", in_pos, out_pos);
	} else {
		out_pos = 0;
		od_size = 0;
		in_pos = 0;
		if (!permission) return M2MB_OS_FEATURE_NOT_SUPPORTED;
		command_handler(buffer, size);
	}

#else
	command_handler(buffer, size);
#endif

	cleaner_t = NO_AZX_TIMER_ID;
	cleaner_t = azx_timer_initWithCb(u_timer_handler, NULL, 0);
	if (cleaner_t == NO_AZX_TIMER_ID) LOG_ERROR("uart_t creation failed!");
	else {
		azx_timer_start(cleaner_t, 500, FALSE);
	}

	return read >= 0 ? M2MB_OS_SUCCESS : M2MB_OS_NOT_AVAILABLE;
}

static void u_timer_handler(void* ctx, AZX_TIMER_ID id) {
	(void) ctx;
	if (cleaner_t == id) send_to_uart0_task(CLEAN_REC_BUF, 0, 0);
}

void uart_data_available_cb(UINT16 nbyte, void* ctx) {
	send_to_uart0_task(UART0_NEW_DATA, (INT32) nbyte, (INT32)ctx);
}

/* Global functions =============================================================================*/
INT32 get_uart0_fd(void) {return fd;}

// 115200 или 9600, 1000
BOOLEAN open_uart0(UINT32 baud_rate, UINT16 timeout_ms) {
	//static char* aa = "___Era app started\r\n";
	if ( FALSE == azx_uart_open(0 /*Main UART*/, baud_rate, timeout_ms, uart_data_available_cb, 0, (INT32 *)&fd)) {
		LOG_ERROR("Can't open UART0");
		return FALSE;
	} else {
		LOG_DEBUG("UART0 was opened with %i", baud_rate);
		opened = TRUE;
		permission = TRUE;
		//send_str_to_UART0(aa);
		set_service_level(is_activated() ? 0 : 1);
		return TRUE;
	}
}

BOOLEAN send_str_to_UART0(const char *str) {
	if (!opened) return FALSE;
	//LOG_INFO("> %s", str);
	return send_to_UART0(str, strlen(str)) >= 0 ? TRUE : FALSE;
}

static INT32 send_to_UART0(const void* buffer, SIZE_T nbytes) {
	od_size = nbytes;
	out_pos += od_size;
	memcpy(buf_out, buffer, od_size);
	//print_hex("To UART0:", (char *) buf_out, od_size);
	INT32 res = azx_uart_write(0, (const char *) buf_out, od_size);
	//m2mb_os_taskSleep( M2MB_OS_MS2TICKS(50)); // необходимо, чтобы обеспечить успешность приёма "эха" отправленных символов и эквивалентность in_pos и out_pos
	return res;
}

void close_uart(void) {
	azx_uart_close(0);
	//LOG_DEBUG("UART0 was closed");
}

/***************************************************************************************************
   UART0_task handles UART0: OPEN, CLOSE, NEW DATA
 **************************************************************************************************/
M2MB_OS_RESULT_E UART0_task(INT32 type, INT32 param1, INT32 param2) {
	(void)param2;
	//LOG_DEBUG("UART0 command: %i", type);
	switch(type) {
		case UART0_START_LISTENING:
			permission = TRUE;
			m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1000));
			char dt[50] = {0};
			set_app_version(dt);
			to_log_info_uart("___Era app prepared. This is v%s", dt);
			break;
		case UART0_NEW_DATA:
			get_data(param1);
			break;
		case CLOSE_UART0:
			close_uart();
			break;
		case CLEAN_REC_BUF:
			out_pos = 0;
			od_size = 0;
			in_pos = 0;
			break;
		default:
			return M2MB_OS_NOT_AVAILABLE;
	}
	return M2MB_OS_SUCCESS;
}


