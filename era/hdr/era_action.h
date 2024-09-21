/**
 * @file era_action.h
 * @version 1.0.0
 * @date 25.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_ERA_ACTION_H_
#define HDR_ERA_ACTION_H_
#include "m2mb_types.h"

typedef enum {
	ERA_A_TRANSM_T = 310,  	// сработал таймер intMemTransmitInterval
	ERA_A_START,		// старт голосового вызова
	ERA_A_FORCED_STOP,	// принудительное прекращение вызова
	ERA_A_SOS_BUTTON_PRESSED,	// кратковременно нажата кнопка SOS
	ERA_A_DIAL_DUR_T,	// истёк таймер общего времени дозвона
	ERA_A_CALL_FAILED,	// истёк таймер ожидания начала вызова
	ERA_A_DISC_CATCH_T,	// таймер обнаружения разрыва соединения (DISCONNECT)
	ERA_A_MAKE_CALL,	// запуск подготовки вызова
	ERA_A_CCFT_T,		// сработал таймер CCFT
	ERA_A_URC_RINGING,
	ERA_A_URC_RING,
	ERA_A_URC_NO_CARRIER,
	ERA_A_URC_CREG,
	ERA_A_URC_DIALING,
	ERA_A_URC_CONNECTED,
	ERA_A_URC_RELEASED,
	ERA_A_URC_DISCONNECTED,
	ERA_A_URC_BUSY,
	ERA_A_URC_ECALLEV
} ERA_A_TASK_EVENT;   // команды для сервиса ERA_ACTION

//typedef enum {
//
//} ERA_A_RESULT;

/**
 * @brief Прототип callback-функции, вызываемой сервисом вызова для возврата результата
 */
//typedef void (*era_a_result_cb) (ERA_A_RESULT result);

//char *get_era_a_result_desc(ERA_A_RESULT result);
INT32 ERA_ACTION_task(INT32 type, INT32 param1, INT32 param2);
#endif /* HDR_ERA_ACTION_H_ */
