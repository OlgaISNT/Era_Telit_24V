/**

 * @file sys_param.h

 * @version 1.0.0

 * @date 03.05.2022

 * @author: DKozenkov     

 * @brief app description

 */



#ifndef HDR_SYS_PARAM_H_

#define HDR_SYS_PARAM_H_

#include "prop_manager.h"

#include "gnss_rec.h"



#define ERA_RESTART_NAME 			"ERA_RESTART"

#define REASON_MODEM_RESTART_NAME 	"REASON_MODEM_RESTART" 	// Указатель причины перезапуска модема. В качестве причины подставляется ECALL_TASK_EVENT

#define MAX_RESTART_NUMBER  		3	    				// Максимальное кол-во самоперезагрузок модема

#define LAST_RELIABLE_LATITUDE_NAME 	"LAST_REL_LAT"  // последнее известное достоверное значение широты

#define LAST_RELIABLE_LONGITUDE_NAME 	"LAST_REL_LONG" // последнее известное достоверное значение долготы

#define UNKNOWN_COORDINATE_DEFAULT	UNKNOWN_COORDINATE	// значение, подставляемое в случае, если координата неизвестна

#define ACTIVATED_NAME				"ACTIVATED"
#define ACTIVATED_DEFAULT			0		// по умолчанию БЭГ после первого включения считается не активированным

#define PREV_MODEL_ID_NAME			"PREV_MODEL_ID"
#define AIRBAGS_CNT_NAME 			"AIRBAGS_CNT"
#define MANUAL_CNT_NAME 			"MANUAL_CNT"
#define ROLLOVER_CNT_NAME           "ROLLOVER_CNT"
#define ROLLOVER_ANGLE_NAME 	    "ROLLOVER_ANGLE"
#define ROLLOVER_TIME_NAME 	        "ROLLOVER_TIME"
#define HORIZON_QUAT_NAME           "HORIZON_QUAT"
#define TR_CNT_NAME                 "TR_CNT"

BOOLEAN init_sys_params(void);
char *get_hor_quat(void);
BOOLEAN set_hor_quat(char *value);
int get_prev_model_id(void);
BOOLEAN set_prev_model_id(int value);
int get_era_restart(void);
BOOLEAN set_era_restart(int value);
int get_restart_reason(void);
BOOLEAN set_restart_reason(int value);
int get_last_reliable_latitude(void);
int get_last_reliable_longitude(void);
BOOLEAN set_last_reliable_coord(INT32 latitude, INT32 longitude);
BOOLEAN is_activated(void);
BOOLEAN set_activation_flag(BOOLEAN state);
BOOLEAN set_airbags_cnt(UINT8 value);
UINT8 get_airbags_cnt(void);
BOOLEAN set_manual_cnt(UINT8 value);
UINT8 get_manual_cnt(void);
BOOLEAN set_rollover_angle(UINT8 value);
UINT8 get_rollover_angle(void);
BOOLEAN set_rollover_cnt(UINT8 value);
UINT8 get_rollover_cnt(void);
BOOLEAN set_rollover_time(UINT8 value);
UINT8 get_rollover_time(void);
BOOLEAN set_tr_cnt(UINT8 val);
UINT8 get_tr_cnt(void);

void get_GSMSoftwareVersionNumber(char *dest, UINT16 size);
#endif /* HDR_SYS_PARAM_H_ */

