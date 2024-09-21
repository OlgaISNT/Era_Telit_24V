/**
 * @file sim.h
 * @version 1.0.0
 * @date 12.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_SIM_H_
#define HDR_SIM_H_

#include "azx_timer.h"

typedef enum {
	UNKNOWN_PROFILE, // профиль неизвестен
	ERA_PROFILE,	 // профиль ЭРА
	COMM_PROFILE,	 // коммерческий профиль
} SIM_PROFILE;

typedef enum {
	SIM_PROFILE_TO_ERA,  	// переключение СИМ профиля на ЭРА
	SIM_PROFILE_TO_COMM,	// переключение СИМ профиля на коммерческий
	SIM_SWITCH_TIMER,		// сработал таймер ожидания URC о сбросе СИМ
	URC_SIM_WAS_RESET		// получен URC о сбросе SIM
} SIM_TASK_COMMAND;

/**
 * @brief Прототип callback-функции, вызываемой переключателем профилей СИМ для возврата результата
 *
 */
typedef void (*sim_switcher_result_cb) (SIM_PROFILE sim_profile);

/**
 * @brief Функция определения текущего профиля СИМ-чипа
 *
 * @return SIM_PROFILE текущий профиль СИМ-чипа
 */
SIM_PROFILE get_SIM_profile(void);

/**
 * @brief ! Не вызывать эту функцию напрямую, она запускается в отдельном потоке
 */
INT32 SIM_task(INT32 type, INT32 param1, INT32 param2);
void urc_received_cb(const CHAR* msg);

/**
 * @brief Прототип callback-функции, вызываемой после выполнения действия
 */
typedef void (*iccid_result_cb) (BOOLEAN success, char *iccid);

/**
 * @brief Чтение ICCID профиля ЭРА Глонасс и возврат результата в callback-функцию
 */
void read_iccidg(iccid_result_cb callback);
/**
 * @brief Чтение ICCID коммерческого профиля возврат результата в callback-функцию
 */
void read_iccidc(iccid_result_cb callback);

char *get_iccidg(void);

unsigned long long get_iccidg_ull(void);
unsigned long long get_imsi_ull(void);
extern BOOLEAN hasIccid(void);

#endif /* HDR_SIM_H_ */
