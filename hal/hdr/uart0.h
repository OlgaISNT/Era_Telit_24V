/**
 * @file uart.h
 * @version 1.0.0
 * @date 11.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_UART0_H_
#define HDR_UART0_H_
#include "m2mb_os_api.h"

/**
 * Перечень команд для UART0
 */
typedef enum {
	//SEND_DATA_UART0,/**< SEND_DATA_UART0 */
	UART0_START_LISTENING,	// запуск приёма данных по UART
	UART0_NEW_DATA, 		// получены новые данные по UART
	CLOSE_UART0,     		// закрыть UART0
	CLEAN_REC_BUF			// очистить буфер приёма
} UART0_COMMAND;

/*!
  @brief
    Обработчик команд взаимодействия с UART0
  @details
    Обработчик команд, перечисленных в UART0_COMMAND.
    Используется через функцию send_to_uart0_task(), см. tasks.h
  @param[in] type тип команды для UART0, см. enum UART0_COMMAND
  @param[in] param1 параметр 1 команды, интерпретация зависит от типа команды
  @param[in] param2 параметр 2 команды, интерпретация зависит от типа команды
  @return M2MB_OS_SUCCESS в случае успешного выполнения, иначе код ошибки

  @note
    Примеры
  @code
	Открытие порта:
  	  if (M2MB_OS_SUCCESS != send_to_uart0_task(OPEN_UART0, UART0_BAUD_RATE, UART0_TX_RX_TIMEOUT_MS)) {
	  	  LOG_CRITICAL("Init uart0 error");
  	  }

	Закрытие порта:
		send_to_uart0_task(CLOSE_UART0, 0, 0);

  @endcode
 */
M2MB_OS_RESULT_E UART0_task(INT32 type, INT32 param1, INT32 param2);
/*
 * 	Отправка данных в UART0:
		char *str = "Hello!";
		if (send_to_UART0(str, strlen(str)) < 0) LOG_ERROR("UART0 sending error");
 */
//INT32 send_to_UART0(const void* buffer, SIZE_T nbyte);

/*
 * 	Отправка данных в UART0:
		char *str = "Hello!";
		if (!send_str_to_UART0(str)) LOG_ERROR("UART0 sending error");
 */
BOOLEAN send_str_to_UART0(const char *str);

void uart_data_available_cb(UINT16 nbyte, void* ctx);

BOOLEAN open_uart0(UINT32 baud_rate, UINT16 timeout_ms);
INT32 get_uart0_fd(void);

void close_uart(void);

#endif /* HDR_UART0_H_ */
