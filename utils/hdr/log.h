/*
 * log.h
 * Created on: 23.03.2022, Author: DKozenkov
 */
#include "azx_log.h"

#ifndef HAL_HDR_LOG_H_
#define HAL_HDR_LOG_H_

#ifndef NULL
#define NULL ((void*)0)
#endif

void log_deinit(void);
/**
 * Инициализация механизма логирования
 */
void log_init(void);

/**
 * Вывод в лог содержимого массива m в HEX-виде с указанием заголовка caption с кол-вом байт length
 */
void print_hex(char *caption, char *m, int length);
void print_date_time(void);
void get_mutex_print(void);
void save_to_file(char *str, int size); // запись строки str в лог-файл
void put_mutex_log(void);
/**
 * @brief Sets the new log level to be used.
 *
 * This function sets the new log level that will be used by the log system.
 *
 * **Example**
 *
 *     azx_log_setLevel(AZX_LOG_LEVEL_NONE);
 *
 * @param[in] level The new log level to be set
 *
 * @see azx_log_init()
 */
void set_log_level(AZX_LOG_LEVEL_E level);

void to_log_info_uart(const CHAR* data, ...);
void to_log_error_uart(const CHAR* data, ...);

void disableClearLog(void);
#define LOG_DI(expr) do {\
	if (AZX_LOG_LEVEL_INFO >= azx_log_getLevel()) {\
	get_mutex_print();\
	AZX_LOG_INFO(#expr " = %i\r\n", expr);\
	put_mutex_log();}\
} while(0)

#define LOG_DR(expr) do {\
	if (AZX_LOG_LEVEL_INFO >= azx_log_getLevel()) {\
	get_mutex_print();\
	AZX_LOG_INFO(#expr " = %g\r\n", expr);\
	put_mutex_log();}\
} while(0)

#define LOG_IHEX(expr) do {\
	get_mutex_print();\
	AZX_LOG_INFO(#expr " = x%2.2X\r\n", expr);\
	put_mutex_log();\
} while(0)

#define LOG_RAW_HEX(a) do {\
	if (AZX_LOG_LEVEL_INFO >= azx_log_getLevel()) {\
	get_mutex_print();\
	AZX_LOG_INFO("%2.2X ", a);\
	put_mutex_log();}\
} while(0)

#define LOG_RAW(a...) do {\
	if (AZX_LOG_LEVEL_INFO >= azx_log_getLevel()) {\
	get_mutex_print();\
	AZX_LOG_INFO(a);\
	put_mutex_log();}\
} while(0)

#define LOG_CRITICAL(a...) do {\
	if (AZX_LOG_LEVEL_CRITICAL >= azx_log_getLevel()) {\
	get_mutex_print();\
	AZX_LOG_CRITICAL(a);\
	put_mutex_log();}\
} while(0)

#define LOG_ERROR(a...) do {\
	if (AZX_LOG_LEVEL_ERROR >= azx_log_getLevel()) {\
	get_mutex_print();\
	AZX_LOG_ERROR(a);\
	put_mutex_log();}\
} while(0)

#define LOG_INFO(a...) do {\
	if (AZX_LOG_LEVEL_INFO >= azx_log_getLevel()) {\
	get_mutex_print();\
	AZX_LOG_INFO(a);\
	put_mutex_log();}\
} while(0)

#define LOG_WARN(a...) do {\
	if (AZX_LOG_LEVEL_WARN >= azx_log_getLevel()) {\
	get_mutex_print();\
	AZX_LOG_WARN(a);\
	put_mutex_log();}\
} while(0)

#define LOG_DEBUG(a...) do {\
	if (AZX_LOG_LEVEL_DEBUG >= azx_log_getLevel()) {\
	get_mutex_print();\
	AZX_LOG_DEBUG(a);\
	put_mutex_log();}\
} while(0)

#define LOG_TRACE(a...) do {\
	if (AZX_LOG_LEVEL_TRACE >= azx_log_getLevel()) {\
	get_mutex_print();\
	AZX_LOG_TRACE(a);\
	put_mutex_log();}\
} while(0)

#endif /* HAL_HDR_LOG_H_ */
