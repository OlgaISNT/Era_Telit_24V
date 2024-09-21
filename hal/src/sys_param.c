/**
 * @file sys_param.c
 * @version 1.0.0
 * @date 03.05.2022
 * @author: DKozenkov
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_sys.h"
#include "m2mb_os_api.h"
#include "at_command.h"
#include "sys_param.h"
#include "params.h"
#include "prop_manager.h"
#include "log.h"
#include "factory_param.h"
#include "indication.h"
#include "tasks.h"
#include <m2mb_info.h>

/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static int era_restart; 		// счётчик кол-ва последовательных рестартов модема при безуспешном завершении действий
static int restart_reason = 0;  // причина перезапуска. В качестве причины подставляется ECALL_TASK_EVENT
static int activated = 0;		// флаг признака активированности БЭГа: 0 - в БЭГ не прописан VIN, тип топлива и тип авто, 1 - прописан
static int last_latitude =  UNKNOWN_COORDINATE;  // последнее известное достоверное значение широты
static int last_longitude = UNKNOWN_COORDINATE;  // последнее известное достоверное значение долготы
static int prev_model_id = 0;   // предыдущее значение MODEL_ID (для отслеживания изменения MODEL_ID)
static int airbagsCnt = 0;      // Счетчик срабатываний подушек безопасности
static int manualCnt = 0;       // Счетчик экстренных вызовов
static int rolloverCnt = 0;     // Счетчик срабатываний акселерометра
static int rolloverAngle = 60;  // Угол опрокидывания (никогда не 0)
static int rolloverTime = 100;   // Время превышения угла опрокидывания
static char *horQuat;            // Кватернион поворота в горизон
static char *GSMSoftwareVersionNumber;
static int trCnt = 0;           // Счетчик незапланированных перезагрузок
static PropManager *sys_p;

/* Local function prototypes ====================================================================*/

/* Static functions =============================================================================*/

/* Global functions =============================================================================*/
BOOLEAN init_sys_params(void) {
	sys_p = newPropManager(SYS_PARAMS_FILE_NAME);
	if (!readAll(sys_p)) {
		if (!saveAllToFileManager(sys_p)) {
			LOG_CRITICAL("Can't save %s", SYS_PARAMS_FILE_NAME);
			return FALSE;
		}
	}

	era_restart = getInt(sys_p, ERA_RESTART_NAME, 0, INT32_MAX, 0);
	restart_reason = getInt(sys_p, REASON_MODEM_RESTART_NAME, 0, INT32_MAX, 0);
	last_latitude =  getInt(sys_p, LAST_RELIABLE_LATITUDE_NAME, 0, INT32_MAX, UNKNOWN_COORDINATE_DEFAULT);
	last_longitude = getInt(sys_p, LAST_RELIABLE_LONGITUDE_NAME, 0, INT32_MAX, UNKNOWN_COORDINATE_DEFAULT);
	activated = getInt(sys_p, ACTIVATED_NAME, 0, 1, ACTIVATED_DEFAULT);
	prev_model_id = getInt(sys_p, PREV_MODEL_ID_NAME, MODEL_ID_MIN, MODEL_ID_MAX, MODEL_ID_DEFAULT);
	airbagsCnt = getInt(sys_p, AIRBAGS_CNT_NAME, 0,255,0);
    manualCnt = getInt(sys_p, MANUAL_CNT_NAME, 0,255,0);
    rolloverCnt =  getInt(sys_p, ROLLOVER_CNT_NAME, 0,255,0);
    rolloverAngle = getInt(sys_p, ROLLOVER_ANGLE_NAME, 0,180,51);
    rolloverTime = getInt(sys_p, ROLLOVER_TIME_NAME, 0,254,50);
    horQuat = getString(sys_p, HORIZON_QUAT_NAME,"qw0.0x0.0y0.0z0.0");
    trCnt = getInt(sys_p, TR_CNT_NAME, 0,254,0);
    if (isNeedSave(sys_p)) saveAllToFileManager(sys_p);


    M2MB_INFO_HANDLE h;
    m2mb_info_init(&h);
    m2mb_info_get(h, M2MB_INFO_GET_SW_VERSION, &GSMSoftwareVersionNumber);
	return TRUE;
}

BOOLEAN set_tr_cnt(UINT8 val){
    return set_int_par(TR_CNT_NAME, &trCnt, val, "TR_CN_ERR", sys_p);;
}

UINT8 get_tr_cnt(void){
    return trCnt;
}

char *get_hor_quat(void){
    return horQuat;
}

BOOLEAN set_hor_quat(char *value){
    if (setString(sys_p, HORIZON_QUAT_NAME, value, TRUE)){
        horQuat = value;
        return TRUE;
    }
    return FALSE;
}

BOOLEAN set_rollover_time(UINT8 value){
    return set_int_par(ROLLOVER_TIME_NAME, &rolloverTime, value, "RO_TM_ERR", sys_p);
}

UINT8 get_rollover_time(void){
    return rolloverTime;
}

BOOLEAN set_rollover_cnt(UINT8 value){
    return set_int_par(ROLLOVER_CNT_NAME, &rolloverCnt, value, "RO_CNT_ERR", sys_p);
}

UINT8 get_rollover_cnt(void){
    return rolloverCnt;
}

BOOLEAN set_rollover_angle(UINT8 value){
    return set_int_par(ROLLOVER_ANGLE_NAME, &rolloverAngle, value, "RO_A_ERR", sys_p);
}

UINT8 get_rollover_angle(void){
    return rolloverAngle;
}

void get_GSMSoftwareVersionNumber(char *dest, UINT16 size){
    memcpy(dest, GSMSoftwareVersionNumber, size);
}

BOOLEAN set_airbags_cnt(UINT8 value){
    return set_int_par(AIRBAGS_CNT_NAME, &airbagsCnt, value, "AIR_B_ERR", sys_p);
}

BOOLEAN set_manual_cnt(UINT8 value){
    return set_int_par(MANUAL_CNT_NAME, &manualCnt, value, "MAN_C_ERR", sys_p);
}

UINT8 get_manual_cnt(void){
    return manualCnt;
}

UINT8 get_airbags_cnt(void){
    return airbagsCnt;
}

int get_prev_model_id(void) {
	return prev_model_id;
}

BOOLEAN set_prev_model_id(int value) {
	return set_int_par(PREV_MODEL_ID_NAME, &prev_model_id, value, "S_PM", sys_p);
}

int get_era_restart(void) {
	return get_int_par(&era_restart);
}

BOOLEAN set_era_restart(int value) {
	return set_int_par(ERA_RESTART_NAME, &era_restart, value, "S_SER", sys_p);
}

int get_restart_reason(void) {
	return get_int_par(&restart_reason);
}

BOOLEAN set_restart_reason(int value) {
	return set_int_par(REASON_MODEM_RESTART_NAME, &restart_reason, value, "S_RMR", sys_p);
}

int get_last_reliable_latitude(void) {
	return get_int_par(&last_latitude);
}

BOOLEAN set_last_reliable_coord(INT32 latitude, INT32 longitude) {
	LOG_DEBUG("save last rel GNSS");
	BOOLEAN res_lat  = set_int_par(LAST_RELIABLE_LATITUDE_NAME, &last_latitude, latitude, "LRLA", sys_p);
	BOOLEAN res_long = set_int_par(LAST_RELIABLE_LONGITUDE_NAME, &last_longitude, longitude, "LRLO", sys_p);
	return res_lat & res_long;
}

int get_last_reliable_longitude(void) {
	return get_int_par(&last_longitude);
}

BOOLEAN is_activated(void) {
	return get_int_par(&activated) != ACTIVATED_DEFAULT;
}

BOOLEAN set_activation_flag(BOOLEAN state) {
	send_to_ind_task(state ? ACTIVATED : NO_ACTIVATED, 0, 0);
	return set_int_par(ACTIVATED_NAME, &activated, state ? 1 : ACTIVATED_DEFAULT, "S_ACT", sys_p);
}
