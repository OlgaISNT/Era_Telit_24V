/**
 * @file testing.c
 * @version 1.0.0
 * @date 15.05.2022
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
#include "msd_storage.h"
#include "atc_era.h"
#include "era_helper.h"
#include "indication.h"
#include "util.h"
#include "azx_timer.h"
#include "testing.h"
#include "a_player.h"
#include "failures.h"
#include "codec.h"
#include "tasks.h"
#include "sys_utils.h"
#include "test_helper.h"
#include "log.h"
#include "gpio_ev.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
typedef enum {
	TA_IS_WAITING_CALLBACK = 2001,
	TA_FINISH,
	TA_CRITICAL_ERR,
} T_AUDIO_FILES_CTX;
typedef enum {
	IDLE = 0,			// тестирование остановлено, ожидает запуска
	STARTED,			// тестирование запущено
	STOPPING			// тестирование в процессе остановки
} T_STATE;

/* Local statics ================================================================================*/
static volatile UINT8 state = IDLE;
static testing_result_cb callback = NULL;
static volatile BOOLEAN ign_listening = FALSE;
void set_ign_listening(BOOLEAN value) {
	if (state != IDLE) ign_listening = value;
}
static void ign_state(BOOLEAN state);
static void finalization(void);
static void play_critical_error(TESTING_RESULT res);
static void play_finish(TESTING_RESULT res, BOOLEAN stop);
static void testing_player_result_cb (A_PLAYER_RESULT result, INT32 ctx, INT32 params);
static void final_job(INT32 res);
/* Local function prototypes ====================================================================*/
static void start(TESTING_TASK_EVENT event);
/* Static functions =============================================================================*/
static void start(TESTING_TASK_EVENT event) {
	if (get_device_mode() == MODE_ECALL) {
		to_log_info_uart("Is ecalling now");
		if (callback != NULL) callback(TEST_R_IS_ECALLING_NOW);
		return;
	}
	if (state != IDLE) {
		to_log_info_uart("Testing is already started");
		if (callback != NULL) callback(TEST_R_IN_PROGRESS);
		return;
	} else if (is_waiting_call()) {
		to_log_info_uart("Is waiting a callback yet");
		if (!is_incoming_call()) {
			on_mute();
			play_audio_file(AUD_NO_CONDITIONS, 7, testing_player_result_cb, TA_IS_WAITING_CALLBACK, TEST_R_IS_WAITING_CALLBACK);
		} else if (callback != NULL) callback(TEST_R_IS_WAITING_CALLBACK);
		return;
	} else {
		to_log_info_uart("Start of testing");
	}
	state = STARTED;
	send_to_ind_task(SHOW_TEST_STARTED, 0, 0);
	ign_listening = TRUE;
	if (!on_mute()) {
		finish_testing(TEST_R_CODEC_FAULT, FALSE);
		return;
	}
	//clear_failures();
	//if (!is_registered()) try_registration();
	switch (event) {
		case START_TEST_CALL_ONLY:	play_test_call(); break;
		case START_TEST_MD_ONLY:	run_md_test(TRUE); break;
		default:					run_self_test(); break;
	}
}

void finish_testing(TESTING_RESULT res, BOOLEAN audio_skip) {
	if (state != STARTED) return;
	state = STOPPING;
//	to_log_info_uart("Testing result: %s", get_testing_res_descr(res));
	if (res == TEST_R_CODEC_FAULT) {
		set_other_critical_failure(F_ACTIVE);
		//restart_from_reason(ECALL_TEST); // TODO возможно, нужна перезагрузка модема
		play_critical_error(res);// play_finish(res, FALSE);
	} else {
		switch(res) {
			case TEST_R_FORCED_STOPPED:
				if (audio_skip) final_job(res);
				else play_finish(res, TRUE);
				break;
			case TEST_R_RECORDER_FAULT:
				set_other_critical_failure(F_ACTIVE);
				play_critical_error(res);
				break;
			case TEST_R_CALL_FAIL:
			default:
				play_finish(res, FALSE);
				break;
		}
	}
}

static void play_critical_error(TESTING_RESULT res) {
	play_audio_file(AUD_CRITICAL_ERROR, 7, testing_player_result_cb, TA_CRITICAL_ERR, res);
}

static void play_finish(TESTING_RESULT res, BOOLEAN stop) {
	play_audio_file(stop ? AUD_STOP_TEST : AUD_END_TEST, 7, testing_player_result_cb, TA_FINISH, res);
}

static void finalization(void) {
	off_mute();
	state = IDLE;
	ign_listening = FALSE;
	send_to_ind_task(CANCEL_FLASHING, 0, 0);
}

static void ign_state(BOOLEAN state) {
	LOG_DEBUG("TEST ign state: %i", state);
	if (state == 0) send_to_testing_task(TEST_FORCED_STOP, 0, 0);
}

static void testing_player_result_cb (A_PLAYER_RESULT result, INT32 ctx, INT32 params) {
	INT32 res = params;
	if (result != A_PLAYER_OK) to_log_error_uart("TS:Can't play file_ctx: %i", ctx);
	switch(ctx) {
		case TA_IS_WAITING_CALLBACK:
			if (!is_incoming_call() && get_device_mode() != MODE_ECALL) off_mute();
			if (callback != NULL) callback(res);
			break;
		case TA_CRITICAL_ERR:
			play_finish(res, FALSE);
			break;
		case TA_FINISH:
			final_job(res);
			break;
		default:
			to_log_error_uart("TActx unknown:%i", ctx);
			finish_testing(TEST_R_AUD_CTX_UNKNOWN, FALSE);
			break;
	}
}

static void final_job(INT32 res) {
	finalization();
	if (callback != NULL) callback(res);
	manage_sleep(isIgnition(), getKL30());
	//send_to_testing_task(TEST_START, 0, 0); // TODO убрать в финальной версии
}

/* Global functions =============================================================================*/

/***************************************************************************************************
   TESTING_task Нельзя вызывать эту функцию напрямую!
**************************************************************************************************/
INT32 TESTING_task(INT32 event, INT32 param1, INT32 param2) {
   	//(void) param2;
	LOG_DEBUG("TESTING_task event: %s", get_testing_event_descr(event));
   	switch(event) {
   		case START_TESTING:
   		case START_TEST_CALL_ONLY:
   		case START_TEST_MD_ONLY: {
               if(is_testing_working()) break;
               if (isIgnition() && getKL30()) {
                   callback = param2 == 0 ? NULL : (testing_result_cb) param2;
                   start(event);
               } else LOG_INFO("Testing wasn't started: T15:%i, T30:%i", isIgnition(), getKL30());
               break;
           }
   		case TEST_IGNITION_STATE:
   			if (ign_listening) ign_state((BOOLEAN) param1);
   			break;
   		case TEST_FORCED_STOP:
   			finish_testing(TEST_R_FORCED_STOPPED, param1);
   			break;
   		case TEST_CONFIRMATION_DONE:
   			test_confirmation_done();
   			break;
   		case TEST_CONFIRM_TIMER:
   			confirm_timer_expired();
   			break;
   		default:
			break;
   	}
   	return M2MB_OS_SUCCESS;
}

char *get_testing_event_descr(TESTING_TASK_EVENT event) {
	switch(event) {
		case START_TESTING:				return "START";
		case TEST_IGNITION_STATE:		return "IGNITION_STATE";
		case TEST_FORCED_STOP:			return "TEST_FORCED_STOP";
		case TEST_CONFIRMATION_DONE:	return "TEST_CONFIRMATION_DONE";
		case TEST_CONFIRM_TIMER:		return "CONFIRM_TIMER";
		default:						return "UNKNOWN";
	}
}

char *get_testing_res_descr(TESTING_RESULT result) {
	switch(result) {
		case TEST_R_SUCCESS:				return "SUCCESS";
		case TEST_R_IN_PROGRESS:			return "IN_PROGRESS";
		case TEST_R_CODEC_FAULT:			return "CODEC_FAULT";
		case TEST_R_AUD_FILES_INCOMPLETE:	return "AUD_FILES_INCOMPLETE";
		case TEST_R_RECORDER_FAULT:			return "RECORDER_FAULT";
		case TEST_R_FORCED_STOPPED:			return "FORCED_STOPPED";
		case TEST_R_CALL_FAIL:				return "CALL_FAIL";
		case TEST_R_AUD_CTX_UNKNOWN:		return "AUD_CTX_UNKNOWN";
		case TEST_R_REGISTERED_YET:			return "REGISTERED_YET";
		case TEST_R_IS_WAITING_CALLBACK: 	return "IS_WAITING_CALLBACK";
		case TEST_R_IS_ECALLING_NOW:		return "IS_ECALLING_NOW";
		default:							return "UNKNOWN";
	}
}

BOOLEAN is_testing_working(void) {return state == STARTED;}
