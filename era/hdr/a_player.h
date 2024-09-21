/**
 * @file a_player.h
 * @version 1.0.0
 * @date 26.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_A_PLAYER_H_
#define HDR_A_PLAYER_H_
#include "m2mb_types.h"

typedef enum {
	A_PLAYER_OK = 0,
	A_PLAYER_TIMEOUT,
	A_PLAYER_BUSY,
	A_PLAYER_OTHER_ERR
} A_PLAYER_RESULT;

/**
 * @brief Прототип callback-функции, вызываемой проигрывателем аудиофайлов для возврата результата
 * @param[in] result:
 * 		A_PLAYER_OK - проигрывание выполнено успешно
 *		A_PLAYER_TIMEOUT - проигрывание не выполнено, истёк заданный таймаут
 *		A_PLAYER_BUSY - проигрывание не выполнено, т.к. уже проигрывается другой аудиофайл
 *		A_PLAYER_OTHER_ERR - прочие ошибки проигрывания
 * @param[in] ctx - контекст, позволяющий связать источник вызова с получателем результата
 */
typedef void (*a_player_result_cb) (A_PLAYER_RESULT result, INT32 ctx, INT32 params);

/**
 * @brief Проигрывание указанного аудиофайла с установкой максимального времени ожидания завершения проигрывания
 * @param[in] audio_file_name - имя проигрываемого аудиофайла
 * @param[in] timeout_sec - таймаут, сек, по истечении которого (если ранее не придёт URC об успешном заверешнии проигрывания) через callback будет возвращена ошибка
 * @param[in] cb - функция, которая будет вызвана для возврата результата
 * @param[in] ctx - контекст, позволяющий связать источник вызова с получателем результата в функции a_player_result_cb
 * @param[in] params - дополнительные параметры
 * @return none
 * @Example:
 *	play_audio_file("ecall.wav", 10, &ecall_player_result_cb, A_ECALL, 0);
 */
void play_audio_file(char *audio_file_name, UINT32 timeout_sec, a_player_result_cb cb, INT32 ctx, INT32 params);

void ap_urc_received_cb(const CHAR* msg);

#endif /* HDR_A_PLAYER_H_ */
