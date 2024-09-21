/**
 * @file failures.h
 * @version 1.0.0
 * @date 19.04.2022
 * @author: DKozenkov
 * @brief Диагностика неисправностей БЭГа
 */

#ifndef HDR_FAILURES_H_
#define HDR_FAILURES_H_

#include "m2mb_types.h"

#define MIC_FAILURE_NAME 					"MIC_FAILURE"
#define SPEAKER_FAILURE_NAME 				"SPEAKER_FAILURE"
#define SOS_BREAK_FAILURE_NAME 				"SOS_BREAK_FAILURE"
#define UIM_FAILURE_NAME					"UIM_FAILURE"
#define BATTERY_FAILURE_NAME				"BATTERY_FAILURE"
#define BATTERY_LOW_VOLTAGE_FAILURE_NAME	"BATTERY_LOW_VOLTAGE_FAILURE"
#define CRASH_SENSOR_FAILURE_NAME			"CRASH_SENSOR_FAILURE"
#define AUDIO_FILES_FAILURE_NAME			"AUDIO_FILES_FAILURE"
#define GNSS_FAILURE_NAME					"GNSS_FAILURE"
#define RAIM_FAILURE_NAME					"RAIM_FAILURE"
#define OTHER_CRITICAL_FAILURES_NAME		"OTHER_CRITICAL_FAILURES"
#define AIRBAGS_FAILURE_NAME                "AIRBAGS_FAILURE"
#define CODEC_FAILURE_NAME                  "CODEC_FAILURE"

typedef enum { // возможные статусы диагностического сигнала неисправности
	F_INACTIVE = 0,	// нет неисправности
	F_ACTIVE = 1,	// неисправность
	F_UNKNOWN = 2	// статус неизвестен
} FAILURE_STATUS;

char *get_fail_st_descr(FAILURE_STATUS status);

FAILURE_STATUS get_codec_failure(void);

FAILURE_STATUS get_microphone_failure(void);

FAILURE_STATUS get_speaker_failure(void);

FAILURE_STATUS get_sos_break_failure(void);

FAILURE_STATUS get_uim_failure(void);

FAILURE_STATUS get_battery_failure(void);

FAILURE_STATUS get_battery_low_voltage_failure(void);

FAILURE_STATUS get_crash_sensor_failure(void);

FAILURE_STATUS get_audio_files_failure(void);

FAILURE_STATUS get_gnss_failure(void);

FAILURE_STATUS get_raim_failure(void);

FAILURE_STATUS get_other_critical_failure(void);

FAILURE_STATUS get_airbags_failure(void);

void set_codec_failure(FAILURE_STATUS f_status);

void set_airbags_failure(FAILURE_STATUS f_status);

void set_microphone_failure(FAILURE_STATUS f_status);

void set_speaker_failure(FAILURE_STATUS f_status);

void set_sos_break_failure(FAILURE_STATUS f_status);

void set_uim_failure(FAILURE_STATUS f_status);

void set_battery_failure(FAILURE_STATUS f_status);

void set_battery_low_voltage_failure(FAILURE_STATUS f_status);

void set_crash_sensor_failure(FAILURE_STATUS f_status);

void set_audio_files_failure(FAILURE_STATUS f_status);

void set_gnss_failure(FAILURE_STATUS f_status);

void set_raim_failure(FAILURE_STATUS f_status);

void set_other_critical_failure(FAILURE_STATUS f_status);

void clear_failures(void);

BOOLEAN failures_exist(void);
BOOLEAN high_prior_failures(void);
BOOLEAN medium_prior_failures(void);
BOOLEAN low_prior_failures(void);

/**
 * @brief Проверяет наличие аудиофайлов
 *
 * @return TRUE если все файлы успешно проверены. FALSE если с файлами есть проблемы.
 */
BOOLEAN checkAudioFiles(void);

void send_failures_to_uart0(void);

#endif /* HDR_FAILURES_H_ */
