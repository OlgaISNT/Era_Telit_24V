/**
 * @file testing.h
 * @version 1.0.0
 * @date 15.05.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_TESTING_H_
#define HDR_TESTING_H_

#include "m2mb_types.h"
#include "a_player.h"
#include "azx_timer.h"

typedef enum {
	START_TESTING = 500,
	START_TEST_CALL_ONLY,	// запуск только тестового вызова
	START_TEST_MD_ONLY,		// запуск только теста микрофона и динамика
	TEST_IGNITION_STATE,	// изменилось состояние выключателя зажигания
	TEST_FORCED_STOP,		// принудительный останов тестирования
	TEST_CONFIRMATION_DONE, // нажата кнопка SOS
	TEST_CONFIRM_TIMER		// сработал таймер ожидания подтверждения пользователем успешности аудиозаписи
} TESTING_TASK_EVENT;   	// команды-события для сервиса TESTING

typedef enum { // коды результата сервиса testing
	TEST_R_SUCCESS = 1000,
	TEST_R_IN_PROGRESS,			// ошибка - тестирование уже запущено
	TEST_R_CODEC_FAULT,			// ошибка инициализации аудиокодека
	TEST_R_AUD_FILES_INCOMPLETE,// не хватает аудиофайлов
	TEST_R_RECORDER_FAULT,		// ошибка рекордера аудио
	TEST_R_FORCED_STOPPED,		// тестирование было принудительно завершено
	TEST_R_CALL_FAIL,           // ошибка выполнения тестового вызова
	TEST_R_AUD_CTX_UNKNOWN,		// была попытка воспроизведения неизвестного аудиофайла
	TEST_R_REGISTERED_YET,      // еще не вышел таймаут запрета на повторную регистрацию в сети (testRegistrationPeriod)
	TEST_R_IS_WAITING_CALLBACK,	// ещё ожидается обратный вызов после экстренного звонка
	TEST_R_IS_ECALLING_NOW		// уже выполняется экстренный вызов
} TESTING_RESULT;

/**
 * @brief Прототип callback-функции, вызываемой сервисом тестирования для возврата результата
 */
typedef void (*testing_result_cb) (TESTING_RESULT result);

INT32 TESTING_task(INT32 event, INT32 param1, INT32 param2);

char *get_testing_event_descr(TESTING_TASK_EVENT event);
char *get_testing_res_descr(TESTING_RESULT result);
void finish_testing(TESTING_RESULT res, BOOLEAN audio_skip);
BOOLEAN is_testing_working(void);

#endif /* HDR_TESTING_H_ */
