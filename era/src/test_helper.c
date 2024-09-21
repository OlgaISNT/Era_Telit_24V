/**
 * @file test_helper.c
 * @version 1.0.0
 * @date 16.05.2022
 * @author: DKozenkov     
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_sys.h"
#include "app_cfg.h"
#include "m2mb_os_api.h"
#include "params.h"
#include "atc_era.h"
#include "indication.h"
#include "util.h"
#include "azx_timer.h"
#include "testing.h"
#include "codec.h"
#include "a_player.h"
#include "failures.h"
#include "tasks.h"
#include "ecall.h"
#include "sys_utils.h"
#include "gnss_rec.h"
#include "testing.h"
#include "factory_param.h"
#include "log.h"
#include "test_helper.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
typedef enum {
	THA_START = 3000,
	THA_SELF_TEST,
	THA_AUD_FILES_OK,
	THA_AUD_FILES_ERROR,
	THA_NO_SATTELITIES,
	THA_START_MD_TEST,
	THA_SAY_PHRASE,
	THA_PLAY_PHRASE,
	THA_REPRODUCTION,
	THA_PLAY_CONFIRM,
	THA_PLAY_BY_SOS_SERVICE,
	THA_PLAY_OK,
	THA_PLAY_ERROR,
	THA_PLAY_TEST_CALL,
	THA_PLAY_NO_TEST_CALL
} TH_AUDIO_FILES_CTX;
/* Local statics ================================================================================*/
static char *TEST_FILE_NAME = "test_aud.wav";
static AZX_TIMER_ID confirm_t = NO_AZX_TIMER_ID;	// таймер ожидания подтверждения успешности записи аудио
static AZX_TIMER_ID test_reg_t = NO_AZX_TIMER_ID;	// таймер
static volatile BOOLEAN test_reg_expired = TRUE;
static volatile BOOLEAN confirmation_listening = FALSE;
static volatile BOOLEAN only_md = FALSE;
/* Local function prototypes ====================================================================*/
static void play_start(void);
static void play_self_test(void);
static void test_help_player_result_cb (A_PLAYER_RESULT result, INT32 ctx, INT32 params);
static void check_aud_files(void);
static void check_gnss(void);
static void play_say_phrase(void);
static void record_phrase(void);
static void play_phrase(void);
static void reproduction(void);
static void play_confirm(void);
static void play_by_SOS_service(void);
static void wait_confirm(void);
static void test_help_timer_handler(void* ctx, AZX_TIMER_ID id);
static void play_no_test_call(void);
static void test_ecall_result_cb(ECALL_RESULT result);
static void start_test_call(void);
/* Static functions =============================================================================*/
// ******        Часть 1. САМОТЕСТИРОВАНИЕ **********
static void play_start(void) {
	if (!is_testing_working()) return;
	//play_test_call();
	only_md = FALSE;
	play_audio_file(AUD_START_TEST, 7, test_help_player_result_cb, THA_START, 0);
}

static void play_self_test(void) {
	if (!is_testing_working()) return;
	play_audio_file(AUD_SELF_TEST, 7, test_help_player_result_cb, THA_SELF_TEST, 0);
}

static void check_aud_files(void) {
	if (!is_testing_working()) return;
	BOOLEAN aud_files_ok = checkAudioFiles();
	set_audio_files_failure(aud_files_ok ? F_INACTIVE: F_ACTIVE);
	if (aud_files_ok) play_audio_file(failures_exist() ? AUD_ERROR : AUD_OK, 7, test_help_player_result_cb, THA_AUD_FILES_OK, 0);
	else play_audio_file(AUD_ERROR, 7, test_help_player_result_cb, THA_AUD_FILES_ERROR, 0);
}

static void check_gnss(void) {
	if (!is_testing_working()) return;
	BOOLEAN gnss_valid = is_gnss_valid();
	set_gnss_failure(gnss_valid ? F_INACTIVE : F_ACTIVE);
	set_raim_failure(gnss_valid ? F_INACTIVE : F_ACTIVE);
	if (is_gnss_valid()) run_md_test(FALSE);
	else play_audio_file(AUD_NO_SATTELITES, 7, test_help_player_result_cb, THA_NO_SATTELITIES, 0);
}


// ******        Часть 2. ТЕСТ МИКРОФОНА И ДИНАМИКА **********
void run_md_test(BOOLEAN only_m_d) {
	if (!is_testing_working()) return;
	only_md = only_m_d;
	play_audio_file(AUD_MD_TEST, 10, test_help_player_result_cb, THA_START_MD_TEST, 0);
}

static void play_say_phrase(void) {
	if (!is_testing_working()) return;
	play_audio_file(AUD_SAY_PHRASE, 7, test_help_player_result_cb, THA_SAY_PHRASE, 0);
}

static void record_phrase(void) {
	if (!is_testing_working()) return;
	if (record_audio(TEST_FILE_NAME, 5000) == ERR_CODEC_NO_ERROR) play_phrase();
	else finish_testing(TEST_R_RECORDER_FAULT, FALSE);
}

static void play_phrase(void) {
	if (!is_testing_working()) return;
	play_audio_file(AUD_PLAY_PHRASE, 7, test_help_player_result_cb, THA_PLAY_PHRASE, 0);
}

static void reproduction(void) {
	if (!is_testing_working()) return;
	play_audio_file(TEST_FILE_NAME, 10, test_help_player_result_cb, THA_REPRODUCTION, 0);
}

static void play_confirm(void) {
	if (!is_testing_working()) return;
	play_audio_file(AUD_CONFIRM_RESULT, 7, test_help_player_result_cb, THA_PLAY_CONFIRM, 0);
}

static void play_by_SOS_service(void) {
	if (!is_testing_working()) return;
	play_audio_file(is_model_serv_butt() ? AUD_BY_SERVICE : AUD_BY_SOS,	7, test_help_player_result_cb, THA_PLAY_BY_SOS_SERVICE, 0);
}

static void wait_confirm(void) {
	if (!is_testing_working()) return;
	//send_to_ind_task(EVI_WAIT_MD_TEST_CONF, 0, 0);
	//set_ign_listening(FALSE);
	confirmation_listening = TRUE;
	deinit_timer(&confirm_t);
	confirm_t = init_timer("confirm_t", 5 * 1000, test_help_timer_handler);
}

static void test_help_timer_handler(void* ctx, AZX_TIMER_ID id) {
	(void)ctx;
	if (confirm_t == id) {
		//send_to_ind_task(EVI_NO_WAIT_MD_TEST_CONF, 0, 0);
		//set_ign_listening(TRUE);
		confirmation_listening = FALSE;
		to_log_info_uart("confirm_t expired");
		send_to_testing_task(TEST_CONFIRM_TIMER, 0, 0);
	} else if (test_reg_t == id) {
		to_log_info_uart("test_reg_t expired");
		test_reg_expired = TRUE;
	} else to_log_error_uart("Test_h unknown t_id: %i", id);
}


// ******        Часть 3. ТЕСТОВЫЙ ВЫЗОВ **********
void play_test_call(void) {
	if (!is_testing_working()) return;
	play_audio_file(AUD_TEST_CALL, 7, test_help_player_result_cb, THA_PLAY_TEST_CALL, 0);
}

static void start_test_call(void) {
	if (test_reg_expired && !is_waiting_call()) {
		if (!is_testing_working()) return;
		if (get_test_reg_period() != 0) {
			test_reg_expired = FALSE;
			deinit_timer(&test_reg_t);
			test_reg_t = init_timer("test_reg_t", get_test_reg_period() * 60 * 1000, test_help_timer_handler);
		}
		send_to_ecall_task(ECALL_TEST, ECALL_NO_REPEAT_REASON, (INT32) &test_ecall_result_cb);
	} else {
		to_log_info_uart("Testing: %s", test_reg_expired ? "NOW_WAITING_CALLBACK" : "TEST_REG_NOT_EXPIRED");
		play_no_test_call();
	}
}

static void play_no_test_call(void) {
	if (!is_testing_working()) return;
	play_audio_file(AUD_NO_TEST_CALL, 7, test_help_player_result_cb, THA_PLAY_NO_TEST_CALL, 0);
}

static void test_ecall_result_cb(ECALL_RESULT result) {
	//LOG_INFO("TEST result: %s", get_ecall_result_desc(result));
	if (result == ECALL_R_SUCCESS || result == ECALL_R_MANUAL_CANCEL || result == ECALL_R_FORCED_STOP) {
		finish_testing(TEST_R_SUCCESS, FALSE);
	} else finish_testing(TEST_R_CALL_FAIL, FALSE);
}

static void test_help_player_result_cb (A_PLAYER_RESULT result, INT32 ctx, INT32 params) {
	(void) params;
	if (result != A_PLAYER_OK) to_log_error_uart("T:Can't play file_ctx: %i", ctx);
	switch(ctx) {
		case THA_START:
			play_self_test();
			break;
		case THA_SELF_TEST:
			check_aud_files();
			break;
		case THA_AUD_FILES_OK:
			check_gnss();
			break;
		case THA_AUD_FILES_ERROR:
			finish_testing(TEST_R_AUD_FILES_INCOMPLETE, FALSE);
			break;
		case THA_NO_SATTELITIES:
			run_md_test(FALSE);
			break;
		case THA_START_MD_TEST:
			play_say_phrase();
			break;
		case THA_SAY_PHRASE:
			record_phrase();
			break;
		case THA_PLAY_PHRASE:
			reproduction();
			break;
		case THA_REPRODUCTION:
			play_confirm();
			break;
		case THA_PLAY_CONFIRM:
			play_by_SOS_service();
			break;
		case THA_PLAY_BY_SOS_SERVICE:
			wait_confirm();
			break;
		case THA_PLAY_OK:
		case THA_PLAY_ERROR:
			if (only_md) finish_testing(ctx == THA_PLAY_OK ? TEST_R_SUCCESS : TEST_R_CALL_FAIL, FALSE);
			else play_test_call();
			break;
		case THA_PLAY_TEST_CALL:
//			if (test_reg_expired && !is_waiting_call()) start_test_call();
//			else {
//				to_log_info_uart("Testing: %s", test_reg_expired ? "NOW_WAITING_CALLBACK" : "TEST_REG_NOT_EXPIRED");
//				play_no_test_call();
//			}
			start_test_call();
			break;
		case THA_PLAY_NO_TEST_CALL:
			finish_testing(TEST_R_REGISTERED_YET, FALSE);
			break;
		default:
			to_log_error_uart("TActx unknown:%i", ctx);
			finish_testing(TEST_R_AUD_CTX_UNKNOWN, FALSE);
			break;
	}
}

/* Global functions =============================================================================*/
void run_self_test(void) {
	play_start();
}

void test_confirmation_done(void) {
	if (!confirmation_listening) return;
	confirmation_listening = FALSE;
	deinit_timer(&confirm_t);
	set_microphone_failure(getMicState() ? F_INACTIVE : F_ACTIVE); // TRUE/FALSE - подключен/отключен
    set_speaker_failure(getSpeakerState() ?  F_INACTIVE : F_ACTIVE);
    set_sos_break_failure(F_INACTIVE);
	play_audio_file(AUD_OK, 5, test_help_player_result_cb, THA_PLAY_OK, 0);
}

void confirm_timer_expired(void) {
	confirmation_listening = FALSE;
    set_microphone_failure(F_ACTIVE);
    set_speaker_failure(F_ACTIVE);
    set_sos_break_failure(F_ACTIVE);
	play_audio_file(AUD_ERROR, 7, test_help_player_result_cb, THA_PLAY_ERROR, 0);
}

