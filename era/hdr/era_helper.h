/**
 * @file era_handler.h
 * @version 1.0.0
 * @date 08.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_ERA_HELPER_H_
#define HDR_ERA_HELPER_H_

#include "msd.h"
#include "ecall.h"

typedef enum {
    CEER_UNKNOWN_STATUS,		// нераспознанное сообщение CEER
    CEER_NORMAL_CALL_CLEARING,	// оператор ГЛОНАСС положил трубку, перезванивать не требуется
    CEER_NORMAL_UNSPECIFIED,	// иногда: оператор ГЛОНАСС положил трубку, перезванивать не требуется
    CEER_MM_NO_SERVICE,			// звонок невозможен
    CEER_DISCONNECTED,			// произошло отключение вызова
    CEER_IMPLICITY_DETACHED,	// неявно отсоединён
    CEER_INTERWORKING_UNSPECIFIED,
    CEER_PHONE_IS_OFFLINE
} CEER_CALL_STATUS;

CEER_CALL_STATUS get_CEER_status(void);
void restart_from_reason(ECALL_TASK_EVENT event);
// Проверка настроек модема для ECALL и конфигурирование его при необходимости
BOOLEAN ecall_modem_init(void);

/**
 * @brief установка указанного номера аудиопрофиля
 * @param[in] numb – номер устанавливаемого аудиопрофиля
 * @return 0/err_code - успех/код ошибки
 */
UINT8 set_aud_profile(UINT8 numb);

BOOLEAN send_msd_to_modem(T_msd * msd);

char *get_ecall_event_descr(ECALL_TASK_EVENT event);

T_msd create_msd(T_MSDFlag flag, ECALL_TASK_EVENT event, UINT8 ecallType, UINT8 id, UINT8 memorized);

void answer_call(void);
BOOLEAN cancel_call(void);

void subscribe_for_urcs(void);
BOOLEAN try_registration(void);
void deregistration(void);
UINT8 get_reg_status(void);
UINT8 get_reg_status_AT(void);
BOOLEAN is_registered_AT(void);
BOOLEAN is_ecall_to_112(void);
void restart_from_reason(ECALL_TASK_EVENT event);
void generateDTMF(UINT8 number);
char* get_reg_st_descr(UINT8 reg_status);
BOOLEAN on_mute(void);
BOOLEAN off_mute(void);
BOOLEAN is_T324X_running(void);
BOOLEAN is_one_and_one(void);
#endif /* HDR_ERA_HELPER_H_ */
