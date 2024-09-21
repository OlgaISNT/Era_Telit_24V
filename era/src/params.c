/**
 * @file params.c
 * @version 1.0.0
 * @date 13.04.2022
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
#include "params.h"
#include "factory_param.h"
#include "sys_param.h"
#include "prop_manager.h"
#include "log.h"
/* Local defines ================================================================================*/

/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static M2MB_OS_MTX_ATTR_HANDLE mtx_attr_handle;
static M2MB_OS_MTX_HANDLE      mtx_handle = NULL;
static PropManager *era_p = NULL; // менеджер параметров ЭРА
static volatile BOOLEAN inited = FALSE;
// Параметры конфигурации ЭРА
static int audio_prof_speed;
static int call_auto_answer_time;
static int ccft;
static char *codec_off;
static int auto_dial_attempts;
static char *debug_number;
static int dial_duration;
static int manual_can_cancel;
static int manual_dial_attempts;
static int msd_max_transm_time;
static int nad_dereg_time;
static int real_nad_deregistration_time; // реальное время дерегистрации вычисляется с учётом callAutoAnswerTime
static int no_auto_trig;
static char *sms_fallback_number;
static char *test_number;
static char *ecall_to;
static int gnss_data_rate;
static int int_mem_transm_attempts;
static int int_mem_transm_interval;
static char *nac;
static char *qac;
static char *tac;
static int micg;
static int spkg;
static int post_test_reg_time;
static T_VehiclePropulsionStorageType t_propulsion_type;
static char *sms_center_number;
static char *sms_fallback_debug_number;
static int test_mode_end_dist;
static int test_reg_period;
static int vehicle_type;
static char *vin;
static int gnss_wdt = GNSS_WDT_TYPE_DEFAULT;
static float zyx_threshold = 0.0f;

// Системные параметры
static char imei[16] = {0};
/* Local function prototypes ====================================================================*/
static void print_err(char *str);
static M2MB_OS_RESULT_E mut_init(void);
static void get_mutex(void);
static void put_mutex(void);
static void fill_propulsion_type(UINT8 composite);
/* Static functions =============================================================================*/
static void print_err(char *str) {
	LOG_ERROR("Can't save param %s", str);
}

static void get_mutex(void) {
	if (mtx_handle == NULL) mut_init();
	M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtx_handle, M2MB_OS_WAIT_FOREVER);
	if (res != M2MB_OS_SUCCESS) LOG_ERROR("gmtx");
}

static void put_mutex(void) {
	if (mtx_handle == NULL) return;
	M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtx_handle);
	if (res != M2MB_OS_SUCCESS) LOG_ERROR("pmtx");
}

extern int get_aut_trig(void){
    return no_auto_trig;
}

static M2MB_OS_RESULT_E mut_init(void) {
	M2MB_OS_RESULT_E os_res = m2mb_os_mtx_setAttrItem( &mtx_attr_handle,
	                                      CMDS_ARGS(
	                                        M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
	                                        M2MB_OS_MTX_SEL_CMD_NAME, "ec",
	                                        M2MB_OS_MTX_SEL_CMD_USRNAME, "ate",
	                                        M2MB_OS_MTX_SEL_CMD_INHERIT, 1
	                                      )
	                                );
	if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("Pm1");
	os_res = m2mb_os_mtx_init( &mtx_handle, &mtx_attr_handle);
	if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("Pm2");
	return os_res;
}

static void fill_propulsion_type(UINT8 composite) {
	get_mutex();
	t_propulsion_type.all = composite;
	//if ( composite & 0x01 ) t_propulsion_type.type.gasoline = 1; // потому что различается порядок бит в элементах этого объединения
	//if ( composite & 0x02 ) t_propulsion_type.type.diesel = 1;
	//if ( composite & 0x04 ) t_propulsion_type.type.compressedNaturalGas = 1;
	//if ( composite & 0x08 ) t_propulsion_type.type.liquidPropaneGas = 1;
	//if ( composite & 0x10 ) t_propulsion_type.type.electricEnergy = 1;
	//if ( composite & 0x20 ) t_propulsion_type.type.hydrogen = 1;
	put_mutex();
}

/* Global functions =============================================================================*/

int get_int_par_fast(int *value) {
	int res = *value;
	return res;
}

int get_int_par(int *value) {
	get_mutex();
	int res = *value;
	put_mutex();
	return res;
}
BOOLEAN set_int_par(char *par_name, int *old, int new, char *err_text, PropManager *p_manag) {
	get_mutex();
	*old = new;
	BOOLEAN ok = saveIntProp(p_manag, par_name, new, TRUE);
	put_mutex();
	if (!ok) print_err(err_text);
	return ok;
}

BOOLEAN get_str_par(char *dest, UINT32 size, char *src) {
	get_mutex();
	BOOLEAN ok = size > strlen(src);
	INT32 length = ok ? strlen(src) : size;
	memcpy(dest, src, length);
	*(dest + length) = 0;
	put_mutex();
	return ok;
}
BOOLEAN set_str_par(char *par_name, char **dest, char *new, char *default_v, char *err_text, PropManager *p_manag) {
	get_mutex();
	BOOLEAN ok = setString(p_manag, par_name, new, TRUE);
	*dest = getString(p_manag, par_name, default_v);
	put_mutex();
	if (!ok) print_err(err_text);
	return ok;
}

int get_audio_prof_speed(void) {
	return get_int_par(&audio_prof_speed);
}
BOOLEAN set_audio_prof_speed(int value) {
	return set_int_par(AUDIOPROFILE_SPEED_NAME, &audio_prof_speed, value, "S_AUD_P_S", era_p);
}

int get_call_auto_answer_time(void) {
	return get_int_par(&call_auto_answer_time);
}
BOOLEAN set_call_auto_answer_time(int value) {
	return set_int_par(CALL_AUTO_ANSWER_TIME_NAME, &call_auto_answer_time, value, "S_CAT", era_p);
}

int get_ccft(void) {
	return get_int_par_fast(&ccft);
}
BOOLEAN set_ccft(int value) {
    if (value > CCFT_MAX || value < CCFT_MIN) return FALSE;
	return set_int_par(CCFT_NAME, &ccft, value, "S_CCFT", era_p);
}

BOOLEAN get_codec_off(char *dest, UINT32 size) {
	return get_str_par(dest, size, codec_off);
}
BOOLEAN set_codec_off(char *value) {
	return set_str_par(CODEC_OFF_NAME, &codec_off, value, CODEC_OFF_DEFAULT, "S_C_OFF", era_p);
}

UINT8 get_auto_dial_attempts(void) {
	return (UINT8) get_int_par(&auto_dial_attempts);
}
BOOLEAN set_auto_dial_attempts(int value) {
	return set_int_par(ECALL_AUTO_DIAL_ATTEMPTS_NAME, &auto_dial_attempts, (int)value, "S_A_D_A", era_p);
}

BOOLEAN get_debug_number(char *dest, UINT32 size) {
	return get_str_par(dest, size, debug_number);
}
BOOLEAN set_debug_number(char *value) {
	return set_str_par(ECALL_DEBUG_NUMBER_NAME, &debug_number, value, ECALL_DEBUG_NUMBER_DEFAULT, "S_DN", era_p);
}

int get_dial_duration(void) {
	return get_int_par(&dial_duration);
}
BOOLEAN set_dial_duration(int value) {
	return set_int_par(ECALL_DIAL_DURATION_NAME, &dial_duration, value, "S_DD", era_p);
}

int get_manual_can_cancel(void) {
	return get_int_par(&manual_can_cancel);
}
BOOLEAN set_manual_can_cancel(int value) {
	return set_int_par(ECALL_MANUAL_CAN_CANCEL_NAME, &manual_can_cancel, value, "S_MCC", era_p);
}

int get_manual_dial_attempts(void) {
	return get_int_par(&manual_dial_attempts);
}
BOOLEAN set_manual_dial_attempts(int value) {
	return set_int_par(ECALL_MANUAL_DIAL_ATTEMPTS_NAME, &manual_dial_attempts, value, "S_MDA", era_p);
}

int get_msd_max_transm_time(void) {
	return get_int_par_fast(&msd_max_transm_time);
}
BOOLEAN set_msd_max_transm_time(int value) {
    if(value > ECALL_MSD_MAX_TRANSMISSION_TIME_MAX || value < ECALL_MSD_MAX_TRANSMISSION_TIME_MIN) return FALSE;
	return set_int_par(ECALL_MSD_MAX_TRANSMISSION_TIME_NAME, &msd_max_transm_time, value, "S_MMTT", era_p);
}

int get_nad_dereg_time(void) {
	return get_int_par(&nad_dereg_time);
}
BOOLEAN set_nad_dereg_time(int value) {
	return set_int_par(ECALL_NAD_DEREGISTRATION_TIME_NAME, &nad_dereg_time, value, "S_NDT", era_p);
}

int get_real_nad_deregistration_time(void) {
	get_mutex();
	int r = real_nad_deregistration_time;
	put_mutex();
	return r;
}

BOOLEAN set_real_nad_deregistration_time(int value) {
	get_mutex();
	real_nad_deregistration_time = value;
	put_mutex();
	return TRUE;
}

int get_no_auto_trig(void) {
	return get_int_par(&no_auto_trig);
}
BOOLEAN set_no_auto_trig(int value) {
	return set_int_par(ECALL_NO_AUTOMATIC_TRIGGERING_NAME, &no_auto_trig, value, "S_NAT", era_p);
}

void get_sms_fallback_number_fast(char *dest){
    memcpy(dest, sms_fallback_number, strlen(sms_fallback_number));
}

BOOLEAN get_sms_fallback_number(char *dest, UINT32 size) {
	return get_str_par(dest, size, sms_fallback_number);
}
BOOLEAN set_sms_fallback_number(char *value) {
	return set_str_par(ECALL_SMS_FALLBACK_NUMBER_NAME, &sms_fallback_number, value, ECALL_SMS_FALLBACK_NUMBER_DEFAULT, "S_SFN", era_p);
}

BOOLEAN get_test_number(char *dest, UINT32 size) {
	return get_str_par(dest, size, test_number);
}
BOOLEAN set_test_number(char *value) {
	return set_str_par(ECALL_TEST_NUMBER_NAME, &test_number, value, ECALL_TEST_NUMBER_DEFAULT, "S_TN", era_p);
}

void get_ecall_to_fast(char *dest, UINT32 size){
    (void) size;
    memcpy(dest, ecall_to, strlen(ecall_to));
}

BOOLEAN get_ecall_to(char *dest, UINT32 size) {
	return get_str_par(dest, size, ecall_to);
}
BOOLEAN set_ecall_to(char *value) {
	return set_str_par(ECALL_TO_NAME, &ecall_to, value, ECALL_TO_DEFAULT, "S_ET", era_p);
}

int get_gnss_data_rate(void) {
	return get_int_par(&gnss_data_rate);
}
BOOLEAN set_gnss_data_rate(int value) {
	return set_int_par(GNSS_DATA_RATE_NAME, &gnss_data_rate, value, "S_GNSS_DR", era_p);
}

int get_int_mem_transm_attempts(void) {
	return get_int_par(&int_mem_transm_attempts);
}
BOOLEAN set_int_mem_transm_attempts(int value) {
	return set_int_par(INT_MEM_TRANSMIT_ATTEMPTS_NAME, &int_mem_transm_attempts, value, "S_IMTA", era_p);
}

int get_int_mem_transm_interval(void) {
	return get_int_par(&int_mem_transm_interval);
}
BOOLEAN set_int_mem_transm_interval(int value) {
	return set_int_par(INT_MEM_TRANSMIT_INTERVAL_NAME, &int_mem_transm_interval, value, "S_IMTI", era_p);
}

BOOLEAN get_nac(char *dest, UINT32 size) {
	return get_str_par(dest, size, nac);
}
BOOLEAN set_nac(char *value) {
	return set_str_par(NAC_NAME, &nac, value, NAC_DEFAULT, "S_NAC", era_p);
}

BOOLEAN get_qac(char *dest, UINT32 size) {
	return get_str_par(dest, size, qac);
}
BOOLEAN set_qac(char *value) {
	return set_str_par(QAC_NAME, &qac, value, QAC_DEFAULT, "S_QAC", era_p);
}

BOOLEAN get_tac(char *dest, UINT32 size) {
	return get_str_par(dest, size, tac);
}
BOOLEAN set_tac(char *value) {
	return set_str_par(TAC_NAME, &tac, value, TAC_DEFAULT, "S_TAC", era_p);
}

int get_micg(void) {
	return get_int_par(&micg);
}
BOOLEAN set_micg(int value) {
	return set_int_par(MICG_NAME, &micg, value, "S_MICG", era_p);
}

int get_spkg(void) {
	return get_int_par(&spkg);
}
BOOLEAN set_spkg(int value) {
	return set_int_par(SPKG_NAME, &spkg, value, "S_SPKG", era_p);
}

int get_post_test_reg_time(void) {
	return get_int_par(&post_test_reg_time);
}
BOOLEAN set_post_test_reg_time(int value) {
	return set_int_par(POST_TEST_REGISTRATION_TIME_NAME, &post_test_reg_time, value, "S_PTRT", era_p);
}

UINT8 get_propulsion_type(void) {
	get_mutex();
	UINT8 v = t_propulsion_type.all;
	put_mutex();
	return v;
}

BOOLEAN set_propulsion_type(int value) {
	get_mutex();
	fill_propulsion_type((UINT8)value);
	BOOLEAN ok = saveIntProp(era_p, PROPULSION_TYPE_NAME, value, TRUE);
	put_mutex();
	if (!ok) print_err("S_PT");
	return ok;
}

BOOLEAN get_sms_center_number(char *dest, UINT32 size) {
	return get_str_par(dest, size, sms_center_number);
}
BOOLEAN set_sms_center_number(char *value) {
	return set_str_par(SMS_CENTER_NUMBER_NAME, &sms_center_number, value, SMS_CENTER_NUMBER_DEFAULT, "S_SCN", era_p);
}

BOOLEAN get_sms_fallback_debug_number(char *dest, UINT32 size) {
	return get_str_par(dest, size, sms_fallback_debug_number);
}
BOOLEAN set_sms_fallback_debug_number(char *value) {
	return set_str_par(SMS_FALLBACK_DEBUG_NUMBER_NAME, &sms_fallback_debug_number, value, SMS_FALLBACK_DEBUG_NUMBER_DEFAULT, "S_FDN", era_p);
}

int get_test_mode_end_dist(void) {
	return get_int_par(&test_mode_end_dist);
}
BOOLEAN set_test_mode_end_dist(int value) {
    if (value > TEST_MODE_END_DISTANCE_MAX || value < TEST_MODE_END_DISTANCE_MIN) return FALSE;
	return set_int_par(TEST_MODE_END_DISTANCE_NAME, &test_mode_end_dist, value, "S_TMED", era_p);
}

int get_test_reg_period(void) {
	return get_int_par(&test_reg_period);
}
BOOLEAN set_test_reg_period(int value) {
	return set_int_par(TEST_REGISTRATION_PERIOD_NAME, &test_reg_period, value, "S_TRP", era_p);
}

int get_vehicle_type(void) {
	return get_int_par(&vehicle_type);
}
BOOLEAN set_vehicle_type(int value) {
	return set_int_par(VEHICLE_TYPE_NAME, &vehicle_type, value, "S_VT", era_p);
}

BOOLEAN get_vin(char *dest, UINT32 size) {
	return get_str_par(dest, size, vin);
}
BOOLEAN set_vin(char *value) {
	return set_str_par(VIN_NAME, &vin, value, VIN_DEFAULT, "S_VIN", era_p);
}

char *get_imei(void) {
	return imei;
}

void set_imei(char *v) {
	memcpy(imei, v, 15);
	//LOG_DEBUG("IMEI: %s", imei);
}

PropManager *get_era_p(void) {
	return era_p;
}

BOOLEAN init_parameters(void) {
	if (inited) return TRUE;
	inited = TRUE;
	init_factory_params();
	init_sys_params();
	BOOLEAN model_id_changed = FALSE;
	if (get_model_id() != get_prev_model_id()) {
		model_id_changed = TRUE;
		set_prev_model_id(get_model_id());
		LOG_WARN("MODEL_ID was changed to %i", get_model_id());
	}
	mut_init();
	get_mutex();
	era_p = newPropManager(ERA_PARAMS_FILE_NAME);
	if (!readAll(era_p)) {
		if (!saveAllToFileManager(era_p)) {
			LOG_CRITICAL("Can't save %s", ERA_PARAMS_FILE_NAME);
			put_mutex();
			return FALSE;
		}
	}

	audio_prof_speed = getInt(era_p, AUDIOPROFILE_SPEED_NAME, AUDIOPROFILE_SPEED_MIN, AUDIOPROFILE_SPEED_MAX, AUDIOPROFILE_SPEED_DEFAULT);
	call_auto_answer_time = getInt(era_p, CALL_AUTO_ANSWER_TIME_NAME, CALL_AUTO_ANSWER_TIME_MIN, CALL_AUTO_ANSWER_TIME_MAX, CALL_AUTO_ANSWER_TIME_DEFAULT);
	ccft = getInt(era_p, CCFT_NAME, CCFT_MIN, CCFT_MAX, CCFT_DEFAULT);
	codec_off = getString(era_p, CODEC_OFF_NAME, CODEC_OFF_DEFAULT);
	auto_dial_attempts = getInt(era_p, ECALL_AUTO_DIAL_ATTEMPTS_NAME, ECALL_AUTO_DIAL_ATTEMPTS_MIN, ECALL_AUTO_DIAL_ATTEMPTS_MAX, ECALL_AUTO_DIAL_ATTEMPTS_DEFAULT);
	debug_number = getString(era_p, ECALL_DEBUG_NUMBER_NAME, ECALL_DEBUG_NUMBER_DEFAULT);
	dial_duration = getInt(era_p, ECALL_DIAL_DURATION_NAME, ECALL_DIAL_DURATION_MIN, ECALL_DIAL_DURATION_MAX, ECALL_DIAL_DURATION_DEFAULT);
	manual_can_cancel = getInt(era_p, ECALL_MANUAL_CAN_CANCEL_NAME, ECALL_MANUAL_CAN_CANCEL_MIN, ECALL_MANUAL_CAN_CANCEL_MAX, ECALL_MANUAL_CAN_CANCEL_DEFAULT);
	manual_dial_attempts = getInt(era_p, ECALL_MANUAL_DIAL_ATTEMPTS_NAME, ECALL_MANUAL_DIAL_ATTEMPTS_MIN, ECALL_MANUAL_DIAL_ATTEMPTS_MAX, ECALL_MANUAL_DIAL_ATTEMPTS_DEFAULT);
	msd_max_transm_time = getInt(era_p, ECALL_MSD_MAX_TRANSMISSION_TIME_NAME, ECALL_MSD_MAX_TRANSMISSION_TIME_MIN, ECALL_MSD_MAX_TRANSMISSION_TIME_MAX, ECALL_MSD_MAX_TRANSMISSION_TIME_DEFAULT);
	nad_dereg_time = getInt(era_p, ECALL_NAD_DEREGISTRATION_TIME_NAME, ECALL_NAD_DEREGISTRATION_TIME_MIN, ECALL_NAD_DEREGISTRATION_TIME_MAX, ECALL_NAD_DEREGISTRATION_TIME_DEFAULT);
	real_nad_deregistration_time = ECALL_NAD_DEREGISTRATION_TIME_DEFAULT;
	no_auto_trig = getInt(era_p, ECALL_NO_AUTOMATIC_TRIGGERING_NAME, ECALL_NO_AUTOMATIC_TRIGGERING_MIN, ECALL_NO_AUTOMATIC_TRIGGERING_MAX, ECALL_NO_AUTOMATIC_TRIGGERING_DEFAULT);
	sms_fallback_number = getString(era_p, ECALL_SMS_FALLBACK_NUMBER_NAME, ECALL_SMS_FALLBACK_NUMBER_DEFAULT);
	test_number = getString(era_p, ECALL_TEST_NUMBER_NAME, ECALL_TEST_NUMBER_DEFAULT);
	ecall_to = getString(era_p, ECALL_TO_NAME, ECALL_TO_DEFAULT);
	gnss_data_rate = getInt(era_p, GNSS_DATA_RATE_NAME, GNSS_DATA_RATE_MIN, GNSS_DATA_RATE_MAX, GNSS_DATA_RATE_DEFAULT);
	int_mem_transm_attempts = getInt(era_p, INT_MEM_TRANSMIT_ATTEMPTS_NAME, INT_MEM_TRANSMIT_ATTEMPTS_MIN, INT_MEM_TRANSMIT_ATTEMPTS_MAX, INT_MEM_TRANSMIT_ATTEMPTS_DEFAULT);
	int_mem_transm_interval = getInt(era_p, INT_MEM_TRANSMIT_INTERVAL_NAME, INT_MEM_TRANSMIT_INTERVAL_MIN, INT_MEM_TRANSMIT_INTERVAL_MAX, INT_MEM_TRANSMIT_INTERVAL_DEFAULT);
	nac = getString(era_p, NAC_NAME, is20dB() ? NAC_DEFAULT_20dB : NAC_DEFAULT);
	LOG_INFO("is20dB:%i", is20dB());
	LOG_INFO("nac:%s", nac);
	LOG_INFO("modelId:%i", get_model_id());
	qac = getString(era_p, QAC_NAME, is20dB() ? QAC_DEFAULT_20dB : QAC_DEFAULT);
	tac = getString(era_p, TAC_NAME, is20dB() ? TAC_DEFAULT_20dB : TAC_DEFAULT);
	micg = getInt(era_p, MICG_NAME, MICG_MIN, MICG_MAX, MICG_DEFAULT);
	spkg = getInt(era_p, SPKG_NAME, SPKG_MIN, SPKG_MAX, SPKG_DEFAULT);
	post_test_reg_time = getInt(era_p, POST_TEST_REGISTRATION_TIME_NAME, POST_TEST_REGISTRATION_TIME_MIN, POST_TEST_REGISTRATION_TIME_MAX, POST_TEST_REGISTRATION_TIME_DEFAULT);
	fill_propulsion_type((UINT8) getInt(era_p, PROPULSION_TYPE_NAME, PROPULSION_TYPE_MIN, PROPULSION_TYPE_MAX, PROPULSION_TYPE_DEFAULT));
	sms_center_number = getString(era_p, SMS_CENTER_NUMBER_NAME, SMS_CENTER_NUMBER_DEFAULT);
	sms_fallback_debug_number = getString(era_p, SMS_FALLBACK_DEBUG_NUMBER_NAME, SMS_FALLBACK_DEBUG_NUMBER_DEFAULT);
	test_mode_end_dist = getInt(era_p, TEST_MODE_END_DISTANCE_NAME, TEST_MODE_END_DISTANCE_MIN, TEST_MODE_END_DISTANCE_MAX, TEST_MODE_END_DISTANCE_DEFAULT);
	test_reg_period = getInt(era_p, TEST_REGISTRATION_PERIOD_NAME, TEST_REGISTRATION_PERIOD_MIN, TEST_REGISTRATION_PERIOD_MAX, TEST_REGISTRATION_PERIOD_DEFAULT);
	vehicle_type = getInt(era_p, VEHICLE_TYPE_NAME, VEHICLE_TYPE_MIN, VEHICLE_TYPE_MAX, VEHICLE_TYPE_DEFAULT);
	vin = getString(era_p, VIN_NAME, VIN_DEFAULT);
	gnss_wdt = getInt(era_p, GNSS_WDT_TYPE_NAME, GNSS_WDT_TYPE_MIN, GNSS_WDT_TYPE_MAX, GNSS_WDT_TYPE_DEFAULT);
    zyx_threshold = getFloat(era_p, ZYX_THRESHOLD_TYPE_NAME, ZYX_THRESHOLD_MIN, ZYX_THRESHOLD_MAX, ZYX_THRESHOLD_DEFAULT);
    LOG_DEBUG("ZYX_THRESHOLD: %.3f", zyx_threshold);
	LOG_DEBUG("GNSS WDT %d", gnss_wdt);
	if (isNeedSave(era_p)) saveAllToFileManager(era_p);
	if (model_id_changed) {
		setString(era_p, NAC_NAME, is20dB() ? NAC_DEFAULT_20dB : NAC_DEFAULT, FALSE);
		setString(era_p, QAC_NAME, is20dB() ? QAC_DEFAULT_20dB : QAC_DEFAULT, FALSE);
		setString(era_p, TAC_NAME, is20dB() ? TAC_DEFAULT_20dB : TAC_DEFAULT, TRUE);
	}
	put_mutex();
	return TRUE;
}

float get_zyx_threshold(void){
    return zyx_threshold;
}

BOOLEAN set_zyx_threshold(char *buff, UINT32 buff_size){
    if (buff == NULL || buff_size != 4) return FALSE;

    float val=0;
    unsigned long longdata = (*buff<< 24) + (*(buff+1) << 16) + (*(buff + 2) << 8) + (*(buff + 3) << 0);
    val = *(float*) &longdata;

    if(isOutRangeFloat(val, ZYX_THRESHOLD_MIN, ZYX_THRESHOLD_MAX)) return FALSE;
    if(!saveFloatProp(era_p, ZYX_THRESHOLD_TYPE_NAME, val, TRUE)) return FALSE;

    zyx_threshold = getFloat(era_p, ZYX_THRESHOLD_TYPE_NAME, ZYX_THRESHOLD_MIN, ZYX_THRESHOLD_MAX, ZYX_THRESHOLD_DEFAULT);
    LOG_DEBUG("After set ZYX_T = %.5f", zyx_threshold);
    return TRUE;
}

int get_gnss_wdt(void){
   return gnss_wdt;
}

BOOLEAN set_gnss_wdt(int value){
	return set_int_par(GNSS_WDT_TYPE_NAME, &gnss_wdt, value, "GNSS_WDT", era_p);
}

BOOLEAN is20dB(void) {
	return (get_model_id() == MODEL_ID4 || get_model_id() == MODEL_ID10 || with_can() || (get_model_id() == MODEL_ID7 && get_audio_prof_pos() == 1)) ?  TRUE : FALSE;
}

void get_all_params(char *buf, UINT32 buf_size) {
	char *str = propToString(era_p->prop);
	memset(buf, 0, buf_size);
	if (str == NULL || strlen(str) >= buf_size) {
		LOG_ERROR("Can't gap");
	} else {
		strcpy(buf, str);
	}
	free(str);
}
