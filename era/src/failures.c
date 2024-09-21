/**
 * @file failures.c
 * @version 1.0.0
 * @date 19.04.2022
 * @author: DKozenkov
 * @brief Диагностика неисправностей БЭГа
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "failures.h"
#include "m2mb_types.h"
#include "log.h"
#include "uart0.h"
#include "files.h"
#include "tasks.h"
#include "factory_param.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
FAILURE_STATUS mic_failure = F_UNKNOWN; 			// обрыв микрофона
FAILURE_STATUS speaker_failure = F_UNKNOWN; 		// неисправность динамика
FAILURE_STATUS sos_break_failure = F_INACTIVE; 		// обрыв кнопки SOS
FAILURE_STATUS uim_failure = F_UNKNOWN; 			// обнаружение "залипания" кнопки SOS
FAILURE_STATUS battery_failure = F_UNKNOWN; 		// неисправность встроенного аккумулятора
FAILURE_STATUS battery_low_voltage_failure = F_UNKNOWN; 	// слишком низкое напряжение встроенного аккумулятора
FAILURE_STATUS crash_sensor_failure = F_INACTIVE;	// неисправность датчика удара / акселерометра
FAILURE_STATUS audio_files_failure = F_UNKNOWN; 	// некомплект аудио файлов подсказок
FAILURE_STATUS gnss_failure = F_UNKNOWN;			// неисправность приёмника GNSS
FAILURE_STATUS raim_failure = F_ACTIVE; 			// недостоверные геоданные от приёмника GNSS
FAILURE_STATUS other_critical_failure = F_INACTIVE;	// другие критические ошибки
FAILURE_STATUS airbags_failure = F_INACTIVE;	    // ошибка сигнала подушек безопасности
FAILURE_STATUS codec_failure = F_UNKNOWN;           // ошибка конфигурирования кодека
FAILURE_STATUS volatile failures_ex = F_UNKNOWN;	// флаг наличия какой-либо неисправности
/* Local function prototypes ====================================================================*/
static void notify_ind(FAILURE_STATUS *f, FAILURE_STATUS *new_status);
/* Static functions =============================================================================*/
static void notify_ind(FAILURE_STATUS *f, FAILURE_STATUS *new_status) {
	FAILURE_STATUS p = failures_ex;
	*f = *new_status;
	if (p != failures_exist()) {
		send_to_ind_task(failures_ex == F_INACTIVE ? SHOW_NO_FAILURE : SHOW_FAILURE, 0, 0);
	}
}
/* Global functions =============================================================================*/

void set_codec_failure(FAILURE_STATUS f_status){
    if (codec_failure != f_status){
        notify_ind(&codec_failure, &f_status);
        LOG_INFO("%s:%d", CODEC_FAILURE_NAME, codec_failure);
    }
}

FAILURE_STATUS get_codec_failure(void){
    return codec_failure;
}

FAILURE_STATUS get_microphone_failure(void) {
	return mic_failure;
}

FAILURE_STATUS get_speaker_failure(void) {
	return speaker_failure;
}

FAILURE_STATUS get_sos_break_failure(void) {
	return sos_break_failure;
}

FAILURE_STATUS get_uim_failure(void) {
	return uim_failure;
}

FAILURE_STATUS get_battery_failure(void) {
	return battery_failure;
}

FAILURE_STATUS get_battery_low_voltage_failure(void) {
	return battery_low_voltage_failure;
}

FAILURE_STATUS get_crash_sensor_failure(void) {
	return crash_sensor_failure;
}

FAILURE_STATUS get_audio_files_failure(void) {
	return audio_files_failure;
}

FAILURE_STATUS get_gnss_failure(void) {
	return gnss_failure;
}

FAILURE_STATUS get_raim_failure(void) {
	return raim_failure;
}

FAILURE_STATUS get_other_critical_failure(void) {
	return other_critical_failure;
}

FAILURE_STATUS get_airbags_failure(void){
    return airbags_failure;
}

void set_airbags_failure(FAILURE_STATUS f_status){
    if(is_airbags_disable()) { // Если использование подушек для данного авто не предусмотренно
        airbags_failure = F_INACTIVE; // Нет ошибки по подушкам
        return;
    }

    if (airbags_failure != f_status){
        notify_ind(&airbags_failure, &f_status);
        LOG_INFO("%s:%d", AIRBAGS_FAILURE_NAME, airbags_failure);
    }
}

void set_microphone_failure(FAILURE_STATUS f_status) {
	if (mic_failure != f_status) {
		notify_ind(&mic_failure, &f_status);
		LOG_INFO("%s:%d", MIC_FAILURE_NAME, mic_failure);
	}
}

void set_speaker_failure(FAILURE_STATUS f_status) {
	if (speaker_failure != f_status) {
		notify_ind(&speaker_failure, &f_status);
		LOG_INFO("%s:%d", SPEAKER_FAILURE_NAME, speaker_failure);
	}
}

void set_sos_break_failure(FAILURE_STATUS f_status) {
	if (sos_break_failure != f_status) {
		notify_ind(&sos_break_failure, &f_status);
		LOG_INFO("%s:%d", SOS_BREAK_FAILURE_NAME, sos_break_failure);
	}
}

void set_uim_failure(FAILURE_STATUS f_status) {
	if (uim_failure != f_status) {
		notify_ind(&uim_failure, &f_status);
		LOG_INFO("%s:%d", UIM_FAILURE_NAME, uim_failure);
	}
}

void set_battery_failure(FAILURE_STATUS f_status) {
	if (battery_failure != f_status) {
		notify_ind(&battery_failure, &f_status);
		LOG_INFO("%s:%d", BATTERY_FAILURE_NAME, battery_failure);
	}
}

void set_battery_low_voltage_failure(FAILURE_STATUS f_status) {
	if (battery_low_voltage_failure != f_status) {
		notify_ind(&battery_low_voltage_failure, &f_status);
		LOG_INFO("%s:%d", BATTERY_LOW_VOLTAGE_FAILURE_NAME, battery_low_voltage_failure);
	}
}

void set_crash_sensor_failure(FAILURE_STATUS f_status) {
	if (crash_sensor_failure != f_status) {
		notify_ind(&crash_sensor_failure, &f_status);
		LOG_INFO("%s:%d", CRASH_SENSOR_FAILURE_NAME, crash_sensor_failure);
	}
}

void set_audio_files_failure(FAILURE_STATUS f_status) {
	if (audio_files_failure != f_status) {
		notify_ind(&audio_files_failure, &f_status);
		LOG_INFO("%s:%d", AUDIO_FILES_FAILURE_NAME, audio_files_failure);
	}
}

void set_gnss_failure(FAILURE_STATUS f_status) {
	if (gnss_failure != f_status) {
		notify_ind(&gnss_failure, &f_status);
		LOG_INFO("%s:%d", GNSS_FAILURE_NAME, gnss_failure);
	}
}

void set_raim_failure(FAILURE_STATUS f_status) {
	if (raim_failure != f_status) {
		notify_ind(&raim_failure, &f_status);
		LOG_INFO("%s:%d", RAIM_FAILURE_NAME, raim_failure);
	}
}

void set_other_critical_failure(FAILURE_STATUS f_status){
	if (other_critical_failure != f_status) {
		notify_ind(&other_critical_failure, &f_status);
		LOG_INFO("%s:%d", OTHER_CRITICAL_FAILURES_NAME, other_critical_failure);
	}
}

void send_failures_to_uart0(void) {
	char data[2000];
	memset(data, 0 , sizeof(data));
	sprintf(data, "%s=%s\r\n"	"%s=%s\r\n"	"%s=%s\r\n"	"%s=%s\r\n"	"%s=%s\r\n"	"%s=%s\r\n"
				  "%s=%s\r\n"	"%s=%s\r\n"	"%s=%s\r\n"	"%s=%s\r\n"	"%s=%s\r\n",
			MIC_FAILURE_NAME, 		get_fail_st_descr(get_microphone_failure()),
			SPEAKER_FAILURE_NAME, 	get_fail_st_descr(get_speaker_failure()),
			SOS_BREAK_FAILURE_NAME,	get_fail_st_descr(get_sos_break_failure()),
			UIM_FAILURE_NAME,		get_fail_st_descr(get_uim_failure()),
			BATTERY_FAILURE_NAME,	get_fail_st_descr(get_battery_failure()),
			BATTERY_LOW_VOLTAGE_FAILURE_NAME, get_fail_st_descr(get_battery_low_voltage_failure()),
			CRASH_SENSOR_FAILURE_NAME,	get_fail_st_descr(get_crash_sensor_failure()),
			AUDIO_FILES_FAILURE_NAME, get_fail_st_descr(get_audio_files_failure()),
			GNSS_FAILURE_NAME, 		get_fail_st_descr(get_gnss_failure()),
			RAIM_FAILURE_NAME,		get_fail_st_descr(get_raim_failure()),
			OTHER_CRITICAL_FAILURES_NAME, get_fail_st_descr(get_other_critical_failure())
	);
//	send_str_to_UART0(data);
}

void clear_failures(void) {
	if (mic_failure == F_ACTIVE) mic_failure = F_INACTIVE;
	if (speaker_failure == F_ACTIVE) speaker_failure = F_INACTIVE;
	if (sos_break_failure == F_ACTIVE) sos_break_failure = F_INACTIVE;
	if (uim_failure == F_ACTIVE) uim_failure = F_INACTIVE;
	if (battery_failure == F_ACTIVE) battery_failure = F_INACTIVE;
	if (battery_low_voltage_failure == F_ACTIVE) battery_low_voltage_failure = F_INACTIVE;
	if (crash_sensor_failure == F_ACTIVE) crash_sensor_failure = F_INACTIVE;
	if (audio_files_failure == F_ACTIVE) audio_files_failure = F_INACTIVE;
	if (gnss_failure == F_ACTIVE) gnss_failure = F_INACTIVE;
	if (raim_failure == F_ACTIVE) raim_failure = F_INACTIVE;
	if (other_critical_failure == F_ACTIVE) other_critical_failure = F_INACTIVE;
    if (airbags_failure == F_ACTIVE) airbags_failure = F_INACTIVE;
    if (codec_failure == F_ACTIVE) codec_failure = F_INACTIVE;
	send_to_ind_task(SHOW_NO_FAILURE , 0, 0);
}

BOOLEAN failures_exist(void) {
	BOOLEAN failures =  mic_failure == F_ACTIVE ||
			speaker_failure == F_ACTIVE ||
			sos_break_failure == F_ACTIVE ||
			uim_failure == F_ACTIVE ||
			battery_failure == F_ACTIVE ||
			crash_sensor_failure == F_ACTIVE ||
			audio_files_failure == F_ACTIVE ||
			gnss_failure == F_ACTIVE ||
			raim_failure == F_ACTIVE ||
			other_critical_failure == F_ACTIVE ||
            codec_failure == F_ACTIVE ||
            airbags_failure == F_ACTIVE;
			failures_ex = failures ? F_ACTIVE : F_INACTIVE;
	return failures;
}

// Неисправности с высоким приоритетом (для Cherry)
BOOLEAN high_prior_failures(void){
    BOOLEAN failures =  mic_failure == F_ACTIVE ||
                        speaker_failure == F_ACTIVE ||
                        sos_break_failure == F_ACTIVE ||
                        uim_failure == F_ACTIVE ||
                        battery_failure == F_ACTIVE ||
                        crash_sensor_failure == F_ACTIVE ||
                        audio_files_failure == F_ACTIVE ||
                        airbags_failure == F_ACTIVE ||
                        codec_failure == F_ACTIVE ||
                        other_critical_failure == F_ACTIVE;
    return failures;
}

// Неисправности со средним приоритетом (для Cherry)
BOOLEAN medium_prior_failures(void){
    BOOLEAN failures = battery_low_voltage_failure == F_ACTIVE;
    return failures;
}

// Неисправности с низким приоритетом (для Cherry)
BOOLEAN low_prior_failures(void){
    BOOLEAN failures =
            (gnss_failure != F_INACTIVE) | (raim_failure != F_INACTIVE) | !is_gnss_valid();
    return failures;
}


BOOLEAN checkAudioFiles(void){
    UINT32 normFiles = 0;
    if (check_file_size(AUDIOFILE_PATH AUD_BY_IGNITION, 64364)) normFiles++;     // 1
    if (check_file_size(AUDIOFILE_PATH AUD_BY_IGNITION_OFF, 61164)) normFiles++; // 2
    if (check_file_size(AUDIOFILE_PATH AUD_BY_SERVICE, 72684))normFiles++;       // 3
    if (check_file_size(AUDIOFILE_PATH AUD_BY_SOS, 90604))normFiles++;           // 4
    if (check_file_size(AUDIOFILE_PATH AUD_CONFIRM_RESULT, 97324))normFiles++;   // 5
    if (check_file_size(AUDIOFILE_PATH AUD_CRITICAL_ERROR, 81644))normFiles++;   // 6
    if (check_file_size(AUDIOFILE_PATH AUD_ECALL, 39404))normFiles++;            // 7
    if (check_file_size(AUDIOFILE_PATH AUD_END_CONDITIONS, 129004))normFiles++;  // 8
    if (check_file_size(AUDIOFILE_PATH AUD_END_TEST, 85164))normFiles++;         // 9
    if (check_file_size(AUDIOFILE_PATH AUD_END_TEST_CALL, 62764))normFiles++;    // 10
    if (check_file_size(AUDIOFILE_PATH AUD_ERROR, 65964))normFiles++;            // 11
    if (check_file_size(AUDIOFILE_PATH AUD_MD_TEST, 104684))normFiles++;         // 12
    if (check_file_size(AUDIOFILE_PATH AUD_NO_CONDITIONS, 108204))normFiles++;   // 13
    if (check_file_size(AUDIOFILE_PATH AUD_NO_SATTELITES, 55724))normFiles++;    // 14
    if (check_file_size(AUDIOFILE_PATH AUD_NO_TEST_CALL, 122924))normFiles++;    // 15
    if (check_file_size(AUDIOFILE_PATH AUD_OK, 65644))normFiles++;               // 16
    if (check_file_size(AUDIOFILE_PATH AUD_ONE_TONE, 6764))normFiles++;          // 17
    if (check_file_size(AUDIOFILE_PATH AUD_PLAY_PHRASE, 81644))normFiles++;      // 18
    if (check_file_size(AUDIOFILE_PATH AUD_SAY_PHRASE, 74924))normFiles++;       // 19
    if (check_file_size(AUDIOFILE_PATH AUD_SELF_TEST, 92844))normFiles++;        // 20
    if (check_file_size(AUDIOFILE_PATH AUD_START_TEST, 86764))normFiles++;       // 21
    if (check_file_size(AUDIOFILE_PATH AUD_STOP_TEST, 84844))normFiles++;        // 22
    if (check_file_size(AUDIOFILE_PATH AUD_TEST_CALL, 70124))normFiles++;        // 23
    if (check_file_size(AUDIOFILE_PATH AUD_TWO_TONES, 10284))normFiles++;        // 24
    if (check_file_size(AUDIOFILE_PATH AUD_NO_VIN, 68844))normFiles++;           // 25
    if (check_file_size(AUDIOFILE_PATH AUD_ECALL_IMPOSS, 79404))normFiles++;     // 26
    if (normFiles != 26) LOG_ERROR("audio files are incomplete");
    return (normFiles == 26);
}

char *get_fail_st_descr(FAILURE_STATUS status) {
	switch(status) {
		case F_INACTIVE: 	return "INACTIVE";
		case F_ACTIVE:		return "ACTIVE";
		default:			return "UNKNOWN";
	}
}
