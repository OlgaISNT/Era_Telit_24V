/**
 * @file commands.h
 * @version 1.0.0
 * @date 11.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_COMMANDS_H_
#define HDR_COMMANDS_H_
#include "m2mb_types.h"

/**----- UART0 --------------------- */
#define UART0_BAUD_RATE 9600
#define UART0_TX_RX_TIMEOUT_MS 3000
#define UART0_COMMAND_BUF_SIZE 300   // кол-во байт буфера приёма команд
// отключение обработки отправляемых и сразу же принимаемых модемом символов из-за аппаратного "эха"
// это вариант для версии платы без микроконтроллера:
#define UART0_ECHO_OFF

typedef BOOLEAN (*str_param_setter) (char *value);
typedef BOOLEAN (*int_param_setter) (int);

void command_handler(UINT8 *data, UINT32 size);

void set_service_level(UINT8 level);

#endif /* HDR_COMMANDS_H_ */
