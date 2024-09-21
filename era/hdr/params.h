/**
 * @file params.h
 * @version 1.0.0
 * @date 13.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_PARAMS_H_
#define HDR_PARAMS_H_
#include "prop_manager.h"
#include "app_cfg.h"

/**-- Конфигурационные параметры --- */

// Speed limit to change quiet/noisy environment
#define AUDIOPROFILE_SPEED_NAME 			"AUDIOPROFILE_SPEED"
#define AUDIOPROFILE_SPEED_DEFAULT 			0
#define AUDIOPROFILE_SPEED_MIN 				0
#define AUDIOPROFILE_SPEED_MAX 				1000

//Промежуток времени после завершения экстренного вызова, в течение которого УСВ автоматически
// отвечает на входящие звонки, минуты (см. ещё ECALL_NAD_DEREGISTRATION_TIME)
//Он должен быть не менее 20 мин (п.7.5.3.22)
#define CALL_AUTO_ANSWER_TIME_NAME 			"CALL_AUTO_ANSWER_TIME"
#define CALL_AUTO_ANSWER_TIME_DEFAULT		20
#define CALL_AUTO_ANSWER_TIME_MIN			5
#define CALL_AUTO_ANSWER_TIME_MAX			2184

// Длительность счетчика автоматического прекращения звонка в режиме разговора, минуты
#define CCFT_NAME							"CCFT"
#define CCFT_DEFAULT						60
#define CCFT_MIN							0
#define CCFT_MAX						    65535

#define CODEC_OFF_NAME						"CODEC_OFF"
#define CODEC_OFF_DEFAULT					"1204500"

// Только транспортные средства категорий M1 и N1 — число попыток дозвона при автоматически инициированном экстренном вызове.
// Не может быть установлено в «0»
#define ECALL_AUTO_DIAL_ATTEMPTS_NAME		"ECALL_AUTO_DIAL_ATTEMPTS"
#define ECALL_AUTO_DIAL_ATTEMPTS_DEFAULT 	10
#define ECALL_AUTO_DIAL_ATTEMPTS_MIN 		1
#define ECALL_AUTO_DIAL_ATTEMPTS_MAX 		35790

// Число попыток дозвона при экстренном вызове, инициированном вручную. Значение не может устанавливаться в «0»
#define ECALL_MANUAL_DIAL_ATTEMPTS_NAME		"ECALL_MANUAL_DIAL_ATTEMPTS"
#define ECALL_MANUAL_DIAL_ATTEMPTS_DEFAULT	10
#define ECALL_MANUAL_DIAL_ATTEMPTS_MIN		1
#define ECALL_MANUAL_DIAL_ATTEMPTS_MAX		35790

// Общая продолжительность дозвона при инициации экстренного вызова, мин
#define ECALL_DIAL_DURATION_NAME			"ECALL_DIAL_DURATION"
#define ECALL_DIAL_DURATION_DEFAULT			5
#define ECALL_DIAL_DURATION_MIN				1
#define ECALL_DIAL_DURATION_MAX				2184

// 1 — экстренный вызов, инициированный вручную, может быть прекращен со стороны пользователя
#define ECALL_MANUAL_CAN_CANCEL_NAME		"ECALL_MANUAL_CAN_CANCEL"
#define ECALL_MANUAL_CAN_CANCEL_DEFAULT		1
#define ECALL_MANUAL_CAN_CANCEL_MIN			0
#define ECALL_MANUAL_CAN_CANCEL_MAX			1

 // Максимальная длительность передачи МНД, сек.
#define ECALL_MSD_MAX_TRANSMISSION_TIME_NAME "ECALL_MSD_MAX_TRANSMISSION_TIME"
#define ECALL_MSD_MAX_TRANSMISSION_TIME_DEFAULT 20
#define ECALL_MSD_MAX_TRANSMISSION_TIME_MIN 	0
#define ECALL_MSD_MAX_TRANSMISSION_TIME_MAX 	300

//Время, после которого коммуникационный модуль прекращает регистрацию в сети, минуты
//(фактически, определяет значение параметра Call/eCall/CallbackTimeout)
//реальное время дерегистрации вычисляется с учётом CALL_AUTO_ANSWER_TIME, см.алгоритм в era_helper.c
#define ECALL_NAD_DEREGISTRATION_TIME_NAME	"ECALL_NAD_DEREGISTRATION_TIME"
#define ECALL_NAD_DEREGISTRATION_TIME_DEFAULT	120
#define ECALL_NAD_DEREGISTRATION_TIME_MIN		5
#define ECALL_NAD_DEREGISTRATION_TIME_MAX		2184

//Промежуток времени, в течение которого УСВ остается зарегистрированной
//в сети после передачи результатов тестирования оператору системы, сек
#define POST_TEST_REGISTRATION_TIME_NAME	"POST_TEST_REGISTRATION_TIME"
#define POST_TEST_REGISTRATION_TIME_DEFAULT	120
#define POST_TEST_REGISTRATION_TIME_MIN		60
#define POST_TEST_REGISTRATION_TIME_MAX		INT32_MAX

    /** 6.7 Для СВ, установленных на транспортных средствах категорий M1 и N1, должна быть реализова-
        на возможность отключения процедуры инициализации режима «Экстренный вызов» в автоматическом
        режиме посредством использования параметра настройки ECALL_NO_AUTOMATIC_TRIGGERING*/
#define ECALL_NO_AUTOMATIC_TRIGGERING_NAME	"ECALL_NO_AUTOMATIC_TRIGGERING"
#define ECALL_NO_AUTOMATIC_TRIGGERING_DEFAULT	1
#define ECALL_NO_AUTOMATIC_TRIGGERING_MIN		0
#define ECALL_NO_AUTOMATIC_TRIGGERING_MAX		1

// Темп выдачи данных приемником ГНСС, Гц
#define GNSS_DATA_RATE_NAME					"GNSS_DATA_RATE"
#define GNSS_DATA_RATE_DEFAULT				1
#define GNSS_DATA_RATE_MIN					1
#define GNSS_DATA_RATE_MAX					10

//Число повторных попыток передачи сообщения,содержащегося во внутренней памяти УСВ.
//Значение, установленное в «0», означает, что повторных попыток передачи сообщения не производится
#define INT_MEM_TRANSMIT_ATTEMPTS_NAME		"INT_MEM_TRANSMIT_ATTEMPTS"
#define INT_MEM_TRANSMIT_ATTEMPTS_DEFAULT	10
#define INT_MEM_TRANSMIT_ATTEMPTS_MIN		0
#define INT_MEM_TRANSMIT_ATTEMPTS_MAX		35790

//Промежуток времени между попытками передачи сообщения по СМС,
//содержащегося во внутренней памяти УСВ, мин. Значение не может быть установлено в «0»
#define INT_MEM_TRANSMIT_INTERVAL_NAME		"INT_MEM_TRANSMIT_INTERVAL"
#define INT_MEM_TRANSMIT_INTERVAL_DEFAULT	60
#define INT_MEM_TRANSMIT_INTERVAL_MIN		1
#define INT_MEM_TRANSMIT_INTERVAL_MAX		35790

#define PROPULSION_TYPE_NAME				"PROPULSION_TYPE"
#define PROPULSION_TYPE_DEFAULT				1
#define PROPULSION_TYPE_MIN					0
#define PROPULSION_TYPE_MAX					0x7F

// Audiocodec set string for quiet environment
#define QAC_NAME							"QAC"
#define QAC_DEFAULT							"0220101000242000003300340000008b"
#define QAC_DEFAULT_20dB					"0220101000242000003300540000008b"
// Audiocodec set string for noisy environment
#define NAC_NAME							"NAC"
#define NAC_DEFAULT							"0220101000242000003300340000008b"
#define NAC_DEFAULT_20dB					"0220101000242000003300540000008b"
// Audiocodec set string for testing
#define TAC_NAME							"TAC"
#define TAC_DEFAULT							"0220101000242000003300340000008b"
#define TAC_DEFAULT_20dB					"0220101000242000003300540000008b"
// Microphone gain setting AT#HFMICG
#define MICG_NAME							"MICG"
#define MICG_DEFAULT						7
#define MICG_MIN							0
#define MICG_MAX							7

// Громкость воспроизведения аудиофайлов (playback volume, one dB per step)
#define SPKG_NAME							"SPKG"
#define SPKG_DEFAULT						69
#define SPKG_MIN							0
#define SPKG_MAX							69

// Номер SMS-центра («+79418100013», для ЭВАК «+77400001002»)
#define SMS_CENTER_NUMBER_NAME				"SMS_CENTER_NUMBER"
#define SMS_CENTER_NUMBER_DEFAULT			"+79418100013"

// Номер, на который будет отправляться SMS («+79418100090», для ЭВАК «+77400002001») в режиме 112
#define ECALL_SMS_FALLBACK_NUMBER_NAME		"ECALL_SMS_FALLBACK_NUMBER"
#define ECALL_SMS_FALLBACK_NUMBER_DEFAULT	"+79418100090"

// Отладочный номер, на который будет отправляться SMS («2233», "+79411111097", для ЭВАК «+77408100029»)
// в режиме DEBUG.
// "2233";//​Ответ телекома насчет коротких номеров "На Билайне стоит файрвол на короткие номера.
// Поэтому лучше правильнее будет отправлять на номер +79411111097."
#define SMS_FALLBACK_DEBUG_NUMBER_NAME		"SMS_FALLBACK_DEBUG_NUMBER"
#define SMS_FALLBACK_DEBUG_NUMBER_DEFAULT	"+79411111097"

// Тестовый номер звонка («+79418100029», для ЭВАК «+77408100029»)
#define ECALL_TEST_NUMBER_NAME				"ECALL_TEST_NUMBER"
#define ECALL_TEST_NUMBER_DEFAULT			"+79418100029"

// Отладочный номер звонка («+79418100025», для ЭВАК «+77408100025»)
#define ECALL_DEBUG_NUMBER_NAME				"ECALL_DEBUG_NUMBER"
#define ECALL_DEBUG_NUMBER_DEFAULT			"+79418100029"

//Дистанция, на которой режим тестирования выключается автоматически, метры
#define TEST_MODE_END_DISTANCE_NAME			"TEST_MODE_END_DISTANCE"
#define TEST_MODE_END_DISTANCE_DEFAULT		300
#define TEST_MODE_END_DISTANCE_MIN			0
#define TEST_MODE_END_DISTANCE_MAX			INT32_MAX

// номер, на который будет осуществлен звонок (DEBUG – на ECALL_DEBUG_NUMBER, 112 – на 112)
#define ECALL_TO_NAME 						"ECALL_TO" // // заменено по просьбе A.Соколова, "ECALL_TO";
#ifdef RELEASE
#define ECALL_TO_DEFAULT					"112"
#else
#define ECALL_TO_DEFAULT					"DEBUG"
#endif
#define ECALL_TO_DEBUG						"DEBUG"
#define ECALL_TO_112 						"112"

//Если УСВ была зарегистрирована в сети посредством нажатия на кнопку «Дополнительные функции»,
//то последующая регистрация УСВ в сети при нажатии на кнопку «Дополнительные функции» возможна не ранее
//чем через данный промежуток времени, минуты
#define TEST_REGISTRATION_PERIOD_NAME		"TEST_REGISTRATION_PERIOD"
#define TEST_REGISTRATION_PERIOD_DEFAULT	5
#define TEST_REGISTRATION_PERIOD_MIN		0
#define TEST_REGISTRATION_PERIOD_MAX		INT32_MAX

//Тип транспортного средства
#define VEHICLE_TYPE_NAME					"VEHICLE_TYPE"
#define VEHICLE_TYPE_DEFAULT				1
#define VEHICLE_TYPE_MIN					0
#define VEHICLE_TYPE_MAX					14

// Настройки GNSS_WDT
#define GNSS_WDT_TYPE_NAME					"GNSS_WDT"
#define GNSS_WDT_TYPE_DEFAULT				300
#define GNSS_WDT_TYPE_MIN					0
#define GNSS_WDT_TYPE_MAX					3600

//VIN номер транспортного средства
#define VIN_NAME							"VIN"
#define VIN_DEFAULT							"00000000000000000"

// Модуль порога срабатывания по всем осям
#define ZYX_THRESHOLD_TYPE_NAME				"ZYX_THRESHOLD"
#define ZYX_THRESHOLD_DEFAULT				2.5
#define ZYX_THRESHOLD_MIN					0.5
#define ZYX_THRESHOLD_MAX			        50

/** Названия аудиофайлов подсказок */
#define AUD_BY_IGNITION 		"by_ignition.wav"
#define AUD_BY_IGNITION_OFF 	"by_ignition_off.wav"
#define AUD_BY_SERVICE			"by_service.wav"
#define AUD_BY_SOS				"by_sos.wav"
#define AUD_CONFIRM_RESULT		"confirm_result.wav"
#define AUD_CRITICAL_ERROR		"critical_error.wav"
#define AUD_ECALL				"ecall.wav"
#define AUD_END_CONDITIONS		"end_conditions.wav"
#define AUD_END_TEST			"end_test.wav"
#define AUD_END_TEST_CALL		"end_test_call.wav"
#define AUD_ERROR				"error.wav"
#define AUD_MD_TEST				"md_test.wav"
#define AUD_NO_CONDITIONS		"no_conditions.wav"
#define AUD_NO_SATTELITES		"no_satellites.wav"
#define AUD_NO_TEST_CALL		"no_test_call.wav"
#define AUD_OK					"ok.wav"
#define AUD_ONE_TONE			"one_tone.wav"
#define AUD_PLAY_PHRASE			"play_phrase.wav"
#define AUD_SAY_PHRASE			"say_phrase.wav"
#define AUD_SELF_TEST			"self_test.wav"
#define AUD_START_TEST			"start_test.wav"
#define AUD_STOP_TEST			"stop_test.wav"
#define AUD_TEST_CALL			"test_call.wav"
#define AUD_TWO_TONES			"two_tones.wav"
#define AUD_NO_VIN			    "no_vin.wav"
#define AUD_ECALL_IMPOSS	    "ecall_imposs.wav"

typedef enum {
	VEHICLE_TYPE_M1			= 1,
	VEHICLE_TYPE_M2,
	VEHICLE_TYPE_M3,
	VEHICLE_TYPE_N1,
	VEHICLE_TYPE_N2,
	VEHICLE_TYPE_N3,
	VEHICLE_TYPE_L1e,
	VEHICLE_TYPE_L2e,
	VEHICLE_TYPE_L3e,
	VEHICLE_TYPE_L4e,
	VEHICLE_TYPE_L5e,
	VEHICLE_TYPE_L6e,
	VEHICLE_TYPE_L7e,
} T_VehicleType;

typedef union {
	UINT8 all;
//	struct {
//		UINT8 not_used:  2;
//		UINT8 hydrogen:  1;
//		UINT8 electricEnergy:  1;
//		UINT8 liquidPropaneGas:  1;
//		UINT8 compressedNaturalGas:  1;
//		UINT8 diesel:  1;
//		UINT8 gasoline:  1;
//	} type;
} T_VehiclePropulsionStorageType;

typedef union {
	//UINT64 all;
	struct {
		INT32 latitude;
		INT32 longitude;
	} single;
} T_VehicleLocation;

typedef enum {
	EC_TO_112			= 0,
	EC_TO_TEST			= 1,
	EC_TO_DEBUG			= 2,
} T_ecall_to;

int get_gnss_wdt(void);
BOOLEAN set_gnss_wdt(int value);

float get_zyx_threshold(void);
BOOLEAN set_zyx_threshold(char *buff, UINT32 buff_size);

int get_audio_prof_speed(void);
BOOLEAN set_audio_prof_speed(int value);

int get_call_auto_answer_time(void);
BOOLEAN set_call_auto_answer_time(int value);

int get_ccft(void);
BOOLEAN set_ccft(int value);

BOOLEAN get_codec_off(char *dest, UINT32 size);
BOOLEAN set_codec_off(char *value);

UINT8 get_auto_dial_attempts(void);
BOOLEAN set_auto_dial_attempts(int value);

BOOLEAN get_debug_number(char *dest, UINT32 size);
BOOLEAN set_debug_number(char *value);

int get_dial_duration(void);
BOOLEAN set_dial_duration(int value);

int get_manual_can_cancel(void);
BOOLEAN set_manual_can_cancel(int value);

int get_manual_dial_attempts(void);
BOOLEAN set_manual_dial_attempts(int value);

int get_msd_max_transm_time(void);
BOOLEAN set_msd_max_transm_time(int value);

int get_nad_dereg_time(void);
BOOLEAN set_nad_dereg_time(int value);

int get_real_nad_deregistration_time(void);
BOOLEAN set_real_nad_deregistration_time(int value);

int get_no_auto_trig(void);
BOOLEAN set_no_auto_trig(int value);

BOOLEAN get_sms_fallback_number(char *dest, UINT32 size);
BOOLEAN set_sms_fallback_number(char *value);
void get_sms_fallback_number_fast(char *dest);

BOOLEAN get_test_number(char *dest, UINT32 size);
BOOLEAN set_test_number(char *value);

BOOLEAN get_ecall_to(char *dest, UINT32 size);
void get_ecall_to_fast(char *dest, UINT32 size);
BOOLEAN set_ecall_to(char *value);

int get_gnss_data_rate(void);
BOOLEAN set_gnss_data_rate(int value);

int get_int_mem_transm_attempts(void);
BOOLEAN set_int_mem_transm_attempts(int value);

int get_int_mem_transm_interval(void);
BOOLEAN set_int_mem_transm_interval(int value);

BOOLEAN get_nac(char *dest, UINT32 size);
BOOLEAN set_nac(char *value);

BOOLEAN get_qac(char *dest, UINT32 size);
BOOLEAN set_qac(char *value);

BOOLEAN get_tac(char *dest, UINT32 size);
BOOLEAN set_tac(char *value);

int get_micg(void);
BOOLEAN set_micg(int value);

int get_spkg(void);
BOOLEAN set_spkg(int value);

int get_post_test_reg_time(void);
BOOLEAN set_post_test_reg_time(int value);

UINT8 get_propulsion_type(void);
BOOLEAN set_propulsion_type(int value);

BOOLEAN get_sms_center_number(char *dest, UINT32 size);
BOOLEAN set_sms_center_number(char *value);

BOOLEAN get_sms_fallback_debug_number(char *dest, UINT32 size);
BOOLEAN set_sms_fallback_debug_number(char *value);

int get_test_mode_end_dist(void);
BOOLEAN set_test_mode_end_dist(int value);

int get_test_reg_period(void);
BOOLEAN set_test_reg_period(int value);

int get_vehicle_type(void);
BOOLEAN set_vehicle_type(int value);

BOOLEAN get_vin(char *dest, UINT32 size);
BOOLEAN set_vin(char *value);

char *get_imei(void);
void set_imei(char *v);

extern int get_aut_trig(void);
PropManager *get_era_p(void);
/*!
  @brief
    Инициализация системы конфигурационных файлов
  @details
  @return TRUE инициализирована корректно, FALSE иначе
 */
BOOLEAN init_parameters(void);
int get_int_par(int *value);
BOOLEAN set_int_par(char *par_name, int *old, int new, char *err_text, PropManager *p_manag);
BOOLEAN get_str_par(char *dest, UINT32 size, char *src);
BOOLEAN set_str_par(char *par_name, char **dest, char *new, char *default_v, char *err_text, PropManager *p_manag);
typedef BOOLEAN (*get_str_parameter) (char *dest, UINT32 size);
BOOLEAN is20dB(void);
/*!
  @brief
    Чтение всех параметров конфигурации режима ЭРА и копирование их в переданный массив символов
  @details
    @param[in] buf 		- буфер, в который будут скопированы все параметры
    @param[in] buf_size - кол-во байт буфера
  	@return
 */
void get_all_params(char *buf, UINT32 buf_size);
#endif /* HDR_PARAMS_H_ */
