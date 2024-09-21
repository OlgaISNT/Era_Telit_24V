/**
 * log.c
 * Created on: 23.03.2022, Author: DKozenkov
 * Обертка, использующая библиотеку AZX_LOG
 * Конфигурируется в config.h
 * Позволяет сохранять логи в два файла log1.txt и log2.txt, запись в которых производится по очереди по мере
 * их заполнения. В лог файлы сохраняется только timestamp и содержимое сообщений
 */

/* Include files ================================================================================*/
#include "m2mb_types.h"
#include "azx_log.h"
#include <stdarg.h>
#include "azx_utils.h"
#include "app_cfg.h"
#include "string.h"
#include <stdio.h>
#include <m2mb_os_api.h>
#include <m2mb_os.h>
#include "m2mb_fs_posix.h"
#include "m2mb_trace.h"
#include "log.h"
#include "uart0.h"
#include "files.h"
#include "u_time.h"
#include "util.h"
#include "azx_log.h"
/* Local defines ================================================================================*/
#define BUF_SIZE 2048
/* Local typedefs ===============================================================================*/
typedef enum {UNKNOWN = 0, LOG1 = 1, LOG2 = 2} CUR_FILE;
/* Local statics ================================================================================*/
static M2MB_OS_RESULT_E        os_res;
static M2MB_OS_MTX_ATTR_HANDLE mtxAttrHandle;
static M2MB_OS_MTX_HANDLE      mtxHandle;

static char file_numb = UNKNOWN; // номер текущего используемого лог-файла, UNKNOWN - нет данных, LOG1 - log1.txt, LOG2 - log2.txt
static char file_name_cfg[] = LOCALPATH "/log.dat"; // имя вспомогательного файла механизма логирования
static char file_name1[] = LOCALPATH_HIDE "/log1.txt";
static char file_name2[] = LOCALPATH_HIDE "/log2.txt";
static BOOLEAN isEnable = TRUE;

/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
static void get_mutex_log(void);
static void read_file_numb(void); // чтение номера используемого лог-файла
static char *replacement_log_file(char *cur_file); // замена лог-файла с удалением предыдущего
static void log_hex(char d);
static void log_raw(char *a);
/* Global functions =============================================================================*/

void log_deinit(void) {
	azx_log_deinit();
	m2mb_os_mtx_deinit(mtxHandle);
}

void read_file_numb(void) {
	INT32 fd = -1;
	fd = m2mb_fs_open(file_name_cfg, M2MB_O_RDWR | M2MB_O_CREAT, M2MB_ALLPERMS);
	if (fd == -1) {
		AZX_LOG_CRITICAL("\r\nCan't open %s\r\n", file_name_cfg);
		file_numb = LOG1;
		return;
	}
	char recv[1] = {0};
	int res = m2mb_fs_read(fd, recv, sizeof(recv));
	if (res == -1 || recv[0] != LOG1 || recv[0] != LOG2) {
		if (res == 0) AZX_LOG_INFO("\r\nFile was created: %s\r\n", file_name_cfg);
		else if (res == -1) AZX_LOG_CRITICAL("\r\nInv data in %s\r\n", file_name_cfg);
		file_numb = LOG1;
		recv[0] = file_numb;
		res = m2mb_fs_write(fd, recv, sizeof(recv));
		if (res != sizeof(recv)) {
			AZX_LOG_CRITICAL("\r\nCan't write in %s\r\n", file_name_cfg);
		}
	} else {
		file_numb = recv[0];
	}
	//AZX_LOG_INFO("\r\nFile_numb: %i\r\n", file_numb);
	m2mb_fs_close(fd); // здесь способ сохранения файла нужно заменить на вариант из msg_storage.c
}

void disableClearLog(void){
    isEnable = FALSE;
}

static BOOLEAN needSave = TRUE;
void save_to_file(char *str, int size) { // не использовать функции библиотеки azx_log.h в этой функции!
    if (str == NULL || size == 0) return;
//	if (!SAVE_LOG_TO_FILE) return;
    if (needSave){
        char *cur_file;
        cur_file = file_numb == LOG1 ? file_name1 : file_name2;
        int f_size = get_file_size(cur_file);
        if (f_size + size > LOG_FILE_SIZE) {
            if(isEnable){
                cur_file = replacement_log_file(cur_file);
            } else {
                needSave = FALSE;
            }
        }
        INT32 fd = -1;
        fd = m2mb_fs_open(cur_file, M2MB_O_WRONLY | M2MB_O_APPEND | M2MB_O_CREAT, M2MB_ALLPERMS);

        char log_time[32];
        get_time_for_logs(log_time,32);
//        LOG_DEBUG("time >%s<", log_time);

        char full_str[256];
        memset(full_str, '\0', 256);
        sprintf(full_str, "%s %s", log_time, str);
        size_t full_size = strlen(full_str);

        if (fd > -1) {
            size_t r = m2mb_fs_write(fd, full_str, full_size);
            if (r != full_size) {}
            m2mb_fs_close(fd);  // здесь способ сохранения файла нужно заменить на вариант из msg_storage.c
        }
    }
}


char *replacement_log_file(char *cur_file) {
	file_numb = file_numb == LOG1 ? LOG2 : LOG1;
	cur_file = file_numb == LOG1 ? file_name1 : file_name2;
	m2mb_fs_unlink(cur_file);// != 0) AZX_LOG_CRITICAL("\r\nCan't delete %s\r\n", cur_file);  // не использовать azx библиотеку в этой функции!
	return cur_file;
}

void log_init(void) {
#ifndef RELEASE
	AZX_LOG_INIT();
#endif
	char *err_msg = "\r\nCan't log init\r\n";
	os_res = m2mb_os_mtx_setAttrItem( &mtxAttrHandle,
	                                      CMDS_ARGS(
	                                        M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
	                                        M2MB_OS_MTX_SEL_CMD_NAME, "logMtx1",
	                                        M2MB_OS_MTX_SEL_CMD_USRNAME, "log",
	                                        M2MB_OS_MTX_SEL_CMD_INHERIT, 1
	                                      )
	                                );
	if (os_res != M2MB_OS_SUCCESS) {
		AZX_LOG_CRITICAL(err_msg);
		return;
	}
	os_res = m2mb_os_mtx_init( &mtxHandle, &mtxAttrHandle);
	if (os_res != M2MB_OS_SUCCESS) {
		AZX_LOG_CRITICAL(err_msg);
		return;
	}
	if (SAVE_LOG_TO_FILE) {
		read_file_numb();
		LOG_INFO("!! - Logging to file enabled - !!");
	}
}

void get_mutex_print(void) {
	get_mutex_log();
#ifdef RELEASE
	m2mb_os_taskSleep( M2MB_OS_MS2TICKS(60));
#endif
	print_date_time();
}

static void get_mutex_log(void) {
	//M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtxHandle, M2MB_OS_MS2TICKS(5000));// M2MB_OS_WAIT_FOREVER
	//if (res != M2MB_OS_SUCCESS) AZX_LOG_CRITICAL("\r\nCan't get lmtx:%i\r\n", res);
}

void put_mutex_log(void) {
	AZX_LOG_INFO("\r\n");
	//M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtxHandle);
	//if (res != M2MB_OS_SUCCESS) AZX_LOG_CRITICAL("\r\nCan't put lmtx:%i\r\n", res);
}

/** Вывод в лог шестнадцатиричного числа "как есть", без добавления даты/времени и "\r\n" в конце */
void log_hex(char d) {
	AZX_LOG_INFO("%2.2X ", d);
}

/** Вывод в лог "как есть", без добавления даты/времени и "\r\n" в конце */
void log_raw(char *a) {
	AZX_LOG_INFO(a);
}

/**
 * Вывод в лог содержимого массива m в HEX-виде с указанием заголовка caption с кол-вом байт length
 */
void print_hex(char *caption, char *m, int length) {
	get_mutex_log();
	print_date_time();
	if (caption != NULL) {
		log_raw(caption);
		AZX_LOG_INFO(" [%i]: ", length);
	}
	if (m == NULL) {
		log_raw("Massive is empty!");
	} else {
		for (int i = 0; i < length; i++) {
			log_hex(m[i]);
		}
	}
	//log_raw("\r\n");
	put_mutex_log();
}
/**
 * Вывод в лог текущей даты/времени. Полный состав выводимой строки определяется в файле config.h с помощью SHOW_LOG_DATE и SHOW_LOG_TIME
 */
void print_date_time() {
#if (SHOW_LOG_DATE || SHOW_LOG_TIME )
	char *time = (char*) m2mb_os_malloc(STR_SIZE);
	if (time == NULL) return;
	#if (SHOW_LOG_DATE && SHOW_LOG_TIME)
		get_sys_date_time(time);
	#elif (SHOW_LOG_DATE)
		get_sys_date(time);
	#else
		get_sys_time(time);
	#endif
	AZX_LOG_INFO(time);
	m2mb_os_free(time);
#endif
}

void set_log_level(AZX_LOG_LEVEL_E level) {
	LOG_INFO("LOG_LEVEL SETTED TO: %i", level);
	azx_log_setLevel(level);
}

void to_log_info_uart(const CHAR* data, ...) {
	va_list arg; // @suppress("Type cannot be resolved")
	UINT32 b_size = 1000;
	char buf[b_size];
	memset(buf, 0, b_size);
    va_start(arg, data);
 	vsnprintf(buf, b_size, (void *)data, arg);
    va_end(arg);
    LOG_INFO(buf);
#ifdef LOG_TO_UART0
    char ans[1100];
	concat3("I: ", buf, "\r\n", ans, sizeof(ans));
    send_str_to_UART0(ans);
#endif
}

void to_log_error_uart(const CHAR* data, ...) {
	va_list arg; // @suppress("Type cannot be resolved")
	UINT32 b_size = 1000;
	char buf[b_size];
	memset(buf, 0, b_size);
    va_start(arg, data);
 	vsnprintf(buf, b_size, (void *)data, arg);
    va_end(arg);
    LOG_ERROR(buf);
#ifdef LOG_TO_UART0
	char ans[1100];
    concat3("E: ", buf, "\r\n", ans, sizeof(ans));
    send_str_to_UART0(ans);
#endif
}
