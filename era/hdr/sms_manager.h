/**
 * @file sms.h
 * @version 1.0.0
 * @date 28.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_SMS_MANAGER_H_
#define HDR_SMS_MANAGER_H_

typedef enum {
	SMS_SENDING_RESULT,			// отправка результата процесса отправки СМС
	SMS_SEND_ALL_FROM_STORAGE, 	// отправить все МНД из хранилища, имеющих признак T_MSDFlag = MSD_SMS
	SMS_TRANSM_TIMER,			// сработал transmit таймер
	SMS_STOP_SENDING			// прекращение отправки СМС
} SMS_TASK_EVENT;

typedef enum {
	//SMS_R_SUCCESS,
	SMS_R_NO_REGISTRATION,		// нет регистрации в сотовой сети
	SMS_R_ALL_SMS_SENT,			// все СМС отправлены
	SMS_R_SENDING_FAIL			// не удалось отправить все СМС
} SMS_T_RESULT;

/**
 * @brief Прототип callback-функции, вызываемой сервисом вызова для возврата результата
 */
typedef void (*sms_result_cb) (SMS_T_RESULT result);

INT32 SMS_task(INT32 type, INT32 param1, INT32 param2);

#endif /* HDR_SMS_MANAGER_H_ */
