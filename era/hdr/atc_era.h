/**
 * @file atc_era.h
 * @version 1.0.0
 * @date 12.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_ATC_ERA_H_
#define HDR_ATC_ERA_H_
#include "m2mb_os_types.h"
#include "azx_ati.h"

/**
 * @brief Запись в модем МНД
 *
 * @param[in] msd_string строка, содержащая МНД в виде HEX-символов
 *
 * @return TRUE / FALSE - результат команды записи
 */
BOOLEAN set_MSD(char *msd_string);

/**
 * @brief !Только для команд ERA!
 *
 * @param[in] timeout_ms How long to wait for a response
 * @param[in] cmd The command format followed by any parameters to resolve it.
 *
 * @return One of
 *     True if OK was received,
 *     False if a timeout, or error occurred.
 */
BOOLEAN at_era_sendCommandExpectOk(INT32 timeout_ms, const CHAR* cmd, ...);

/**
 * @brief !Только для команд ERA!
 *
 * @param[in] timeout_ms How long to wait for a response
 * @param[in] resp указатель на строку, в которую будет записан ответ на АТ-команду
 * @param[in] len максимальная длина ответа, который может быть записан в resp
 * @param[in] cmd The command format followed by any parameters to resolve it.
 *
 * @return реальное кол-во символов в ответе на АТ-команду, 0 - если ответа нет или команда завершена по таймауту
 */
INT32 at_era_sendCommand(INT32 timeout_ms, char *resp, int len, const CHAR* cmd, ...);

/**
 * @brief !Только для команд ERA!
 * Добавление слушателя URC (максимум можно добавить 20 слушателей)
 * Example:
 *
 *     // Get notified of SIM events
 *     void sim_event_cb(const CHAR* msg)
 *     {
 *       // Parse the "#QSS: %d" string
 *     }
 *
 *     azx_ati_addUrcHandlerEx(0, "#QSS:", &sim_event_cb);
 *
 * @param[in] msg_header How the URC message is expected to look
 * @param[in] cb The function to handle that message
 *
 * @return
 * В проекте выполняется подписка на такие URC:
 * 	#APLAYEV
 *  RING
 *  NO CARRIER
 *  +CREG:
 *  #STN:
 */
void at_era_addUrcHandler(const CHAR* msg_header, azx_urc_received_cb cb);

#endif /* HDR_ATC_ERA_H_ */
