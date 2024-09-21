/**
 * @file calling.h
 * @version 1.0.0
 * @date 18.04.2022
 * @author: DKozenkov
 * @brief app description
 */

#ifndef HDR_ECALL_H_
#define HDR_ECALL_H_

#include "m2mb_types.h"
#include "msd.h"
#include "a_player.h"
#include "era_action.h"
#include "sim.h"
#include "azx_timer.h"

typedef enum {
	ECALL_DEFAULT = 0,			// dummy
	ECALL_EVENT_SOS_PRESSED = 1200, // нажата кнопка SOS, но ручной вызов не инициирован
	ECALL_MANUAL_SOS,  		 // ручной вызов нажатием кнопки SOS
	ECALL_SMS_REQUEST,		 // вызов по команде из SMS
	ECALL_SMS_SENT,			 // извещение о завершении попытки отправки СМС
    ECALL_AIRBAGS,           // экстренный вызов при срабатывании подушек безопасности
	ECALL_TEST,
	ECALL_FRONT,
	ECALL_LEFT,
	ECALL_RIGHT,
	ECALL_REAR,
	ECALL_ROLLOVER,
	ECALL_SIDE,
	ECALL_FRONT_OR_SIDE,
	ECALL_ANOTHER,
	ECALL_SIM_PREPARED,
	ECALL_DEREGISTRATION,		// команда дерегистрации в сети
	ECALL_FORCED_STOP,			// команда безусловного прекращения вызова
	ECALL_ERA_A_CALLBACK,		// возврат результата сервиса осуществления вызова
	ECALL_URC_RING,
	ECALL_URC_NO_CARRIER,
	ECALL_URC_CREG,
	ECALL_URC_CONNECTED,
	ECALL_URC_DISCONNECTED
} ECALL_TASK_EVENT;   	// команды-события для сервиса ECALL

typedef enum {
	// коды результата сервиса ecall
	ECALL_R_SUCCESS = 1400,		// процедура вызова штатно завершена, однако МНД может быть не отправлен, поэтому требуется проверка и отправка при необходимости через СМС
	ECALL_R_IN_PROGRESS,		// ECALL уже выполняется
	ECALL_R_AUDIO_CTX_UNKNOWN,	// сервис получил контекст неизвестного аудиофайла
	ECALL_R_EVENT_UNKNOWN,		// сервис получил неизвестное событие-команду
	ECALL_R_MANUAL_CANCEL,		// вызов завершён из-за своевременного нажатия кнопки SOS
	ECALL_R_FAULT_CALLING,		// сбой звонка
	ECALL_R_FORCED_STOP,		// вызов был принудительно прекращён

	// коды результата сервиса era_action
	ERA_R_MANUAL_CANCEL,		// вызов отменён вручную нажатием кнопки SOS
	ERA_R_ATTEMPTS_ENDED,		// вызов отменён из-за исчерпания кол-ва возможных попыток вызова
	ERA_R_DIAL_DURATION_TIMER,	// вызов отменён из-за срабатывания таймера продолжительности дозвона
	ERA_R_TRANSM_INT,			// истёк интервал ожидания отправки МНД согласно пп. 8.10.5 и 8.10.6 ГОСТ33464
	ERA_R_NO_SERVICE,			// ошибка сотовой сети "MM no service"
	ERA_R_CCFT_TIMER,			// сработал таймер CCFT
	ERA_R_FORCED_STOP			// вызов принудительно прекращён
} ECALL_RESULT;

typedef enum {
    _112 = 1,
    _TO_TEST = 2,
    _TO_DEBUG = 3,
} ECALL_TO_STATES; // Ecall_to available states

typedef enum {
	ECALL_NO_REPEAT_REASON = 0,		// нет необходимости в повторной отправки, должен быть отправлен новый МНД
	ECALL_MODEM_RESTARTED,		// модем был перезапущен из-за невозможности совершения экстренного вызова
	ECALL_SMS_REQUEST_FOR_MSD	// был получен в СМС запрос на повторную отправку МНД
} ECALL_REPEAT_REASON;			// код причины повторной отправки МНД

/**
 * @brief Прототип callback-функции, вызываемой сервисом ЭРА для возврата результата
 */
typedef void (*ecall_result_cb) (ECALL_RESULT result);

void ecall_sim_result_cb(SIM_PROFILE sim_profile);
//void ecall_era_a_result_cb (ERA_A_RESULT result);
char *get_ecall_result_desc(ECALL_RESULT result);
T_msd get_current_msd(void);
BOOLEAN is_incoming_call(void);
void increment_msd_id(BOOLEAN from_inband);
BOOLEAN is_ecall_in_progress(void);
INT32 ECALL_task(INT32 event, INT32 param1, INT32 param2);

BOOLEAN is_waiting_call(void);
#endif /* HDR_ECALL_H_ */
