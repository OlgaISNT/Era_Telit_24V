/**
 * @file at_interface.h
 * @version 1.0.0
 * @date 07.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_AT_COMMAND_H_
#define HDR_AT_COMMAND_H_

#include "m2mb_os_types.h"
#include "azx_ati.h"

/**
 * @brief Инициализация интерфейсов АТ-команд
 *
 * @return M2MB_OS_SUCCESS - успех, иначе код ошибки
 */
M2MB_OS_RESULT_E ati_init(void);

/**
 * @brief Деинициализация интерфейсов АТ-команд
 *
 * @return
 */
void ati_deinit(void);

/**
 * @brief Sends an AT command and returns its response.
 *
 * @param[in] timeout_ms How long to wait for a response
 * @param[in] resp указатель на строку, в которую будет записан ответ на АТ-команду
 * @param[in] len максимальная длина ответа, который может быть записан в resp
 * @param[in] cmd The command format followed by any parameters to resolve it.
 *
 * @return реальное кол-во символов в ответе на АТ-команду, 0 - если ответа нет или команда завершена по таймауту
 */
INT32 ati_sendCommandEx(INT32 timeout_ms, char *resp, int len, const CHAR* cmd, ...);

/**
 * @brief Sends an AT command and checks that OK is received.
 *
 * Example:
 *
 *     // Sets a certain GPIO pin to HIGH
 *     if( ! ati_sendCommandExpectOkEx(0, AZX_ATI_DEFAULT_TIMEOUT, "AT#GPIO=%d,1,1", pin) )
 *     {
 *       // Log failure
 *     }
 *
 * @param[in] timeout_ms How long to wait for a response
 * @param[in] cmd The command format followed by any parameters to resolve it.
 *
 * @return One of
 *     True if OK was received,
 *     False if a timeout, or error occurred.
 */
BOOLEAN ati_sendCommandExpectOkEx(INT32 timeout_ms, const CHAR* cmd, ...);
INT32 ATC_TT_task(INT32 type, INT32 param1, INT32 param2);

//void ati_addUrcHandler(const CHAR* msg_header, azx_urc_received_cb cb);

#endif /* HDR_AT_COMMAND_H_ */
