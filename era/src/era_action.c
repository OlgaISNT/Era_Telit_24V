/**
 * @file era_action.c
 * @version 1.0.0
 * @date 25.04.2022
 * @author: DKozenkov     
 * @brief Логика собственно звонка ЭРА
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "atc_era.h"
#include "tasks.h"
#include "indication.h"
#include "era_helper.h"
#include "sys_param.h"
#include "sys_utils.h"
#include "sim.h"
#include "msd_storage.h"
#include "log.h"
#include "era_action.h"
#include "codec.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
typedef enum {
    EA_ONE_TONE_1 = 2592,
    EA_TWO_TONES_1,
    EA_ONE_TONE_2
} EA_AUDIO_FILES_CTX;
/* Local statics ================================================================================*/
static volatile BOOLEAN urc_listening = FALSE;
static volatile BOOLEAN pull_mode = FALSE; 		// если true - значит совершается повторный вызов, при котором ранее МНД был успешно отправлен
static volatile UINT16 call_attempts = 0;  		// кол-во попыток дозвона
static volatile BOOLEAN msd_decoded = FALSE; 	// признак успешного получения МНД на платформе
static volatile BOOLEAN voice_active = FALSE;	// признак установления голосового соединения
static volatile BOOLEAN can_be_canceled = TRUE; // признак возможности отмены вызова кнопкой SOS
static volatile BOOLEAN dial_duration_expired = FALSE; // сработал таймер общего времени дозвона
static volatile BOOLEAN finished = FALSE;  		// логика сервиса вызова завершила свою работу
static volatile UINT8 SOS_pressed_numb = 0; 	// кол-во нажатий кнопки SOS при генерации DTMF
static volatile BOOLEAN sos_button_listen = FALSE;	// флаг отслеживания нажатий кнопки SOS
static volatile BOOLEAN stopped = TRUE;
static volatile BOOLEAN connected = FALSE;
static volatile INT32 result_action;
static AZX_TIMER_ID dial_dur_t = NO_AZX_TIMER_ID;
static AZX_TIMER_ID conn_wait_t = NO_AZX_TIMER_ID;
static AZX_TIMER_ID dials_delay_t = NO_AZX_TIMER_ID;
static AZX_TIMER_ID ccft_t = NO_AZX_TIMER_ID;
static AZX_TIMER_ID disc_catch_t = NO_AZX_TIMER_ID;

static AZX_TIMER_ID finish_t = NO_AZX_TIMER_ID;
static char *ecall_str = "AT+CECALL=3\r";
static ECALL_TASK_EVENT ecall_event;
/* Local function prototypes ====================================================================*/
static void era_a_urc(ERA_A_TASK_EVENT event, INT32 p1, INT32 p2);
static void start(void);
static void e_action_timer_handler(void* ctx, AZX_TIMER_ID id);
static void sos_button_pressed(void);
//static void check_reg(void);
static void send_CECALL(void);
static void finish(ECALL_RESULT result);
static void era_a_player_result_cb(A_PLAYER_RESULT result, INT32 ctx, INT32 params);
static char *get_era_a_event_descr(ERA_A_TASK_EVENT event);
static void make_call(void);
static void call_failed(void);
static void catch_disconnect(void);
static void disconnected(void);
/* Static functions =============================================================================*/
static void start(void) {
    stopped = FALSE;
    connected = FALSE;
    pull_mode = FALSE;
    call_attempts = 0;
    msd_decoded = FALSE;
    voice_active = FALSE;
    can_be_canceled = get_manual_can_cancel() == 1 ? TRUE : FALSE;
    dial_duration_expired = FALSE;
    finished = FALSE;
    SOS_pressed_numb = 0;
    if (ecall_event != ECALL_TEST) sos_button_listen = TRUE;
    urc_listening = TRUE;
    deinit_timer(&finish_t);
    deinit_timer(&dial_dur_t);
    dial_dur_t = init_timer("dial_dur_t", get_dial_duration() * 60 * 1000, e_action_timer_handler);

    if (is_registered_AT()) make_call();
    else {
        if (is_ecall_to_112() && ecall_event != ECALL_TEST) {
            //try_registration(); // он сам регистрируется
            make_call();
        } else {
            make_call();// try_registration();
            //to_log_info_uart("Waiting registration..");
        }
    }
}

static void make_call(void) {
    LOG_DEBUG("ERA_A make_call");
    cancel_call();
    send_to_ind_task(SHOW_ECALL_DIALING, 0, 0);
    if (pull_mode) LOG_INFO("PULL mode");
    else {
        if (!at_era_sendCommandExpectOk(5*1000, "AT#MSDPUSH\r")) {
            to_log_error_uart("Can't set PUSH mode");
            restart_from_reason(ecall_event);
            return;
        } else to_log_info_uart("PUSH mode");
    }
    deinit_timer(&conn_wait_t);
    conn_wait_t = init_timer("conn_wait_t", 25 * 1000, e_action_timer_handler); // 20
    char str[128];
    if (ecall_event == ECALL_TEST) {
        if (!get_test_number(str, sizeof(str))) to_log_error_uart("Can't get test numb");
        if (!at_era_sendCommandExpectOk(3*1000, "AT#TESTNUM=0,%s\r", str)) {
            to_log_error_uart("Can't set test numb");
            restart_from_reason(ecall_event);
            return;
        }
        ecall_str = "AT+CECALL=0\r";
        send_CECALL();// check_reg();
    } else {
        if (is_ecall_to_112()) {
            to_log_info_uart("ECALL 112!");
            ecall_str = ecall_event == ECALL_MANUAL_SOS ? "AT+CECALL=2\r" : "AT+CECALL=3\r";
            send_CECALL();
        } else {
            if (!get_debug_number(str, sizeof(str))) to_log_error_uart("Can't get debug numb");
            if (!at_era_sendCommandExpectOk(5*1000, "AT#TESTNUM=0,%s\r", str)) {
                to_log_error_uart("Can't set debug numb");
                restart_from_reason(ecall_event);
                return;
            }
            ecall_str = "AT+CECALL=0\r";
            send_CECALL();// check_reg();
        }
    }
}

static void send_CECALL(void) {
    if (stopped) return;
    char resp[300] = {0};
    int r = at_era_sendCommand(15*1000, resp, sizeof(resp), "AT#MONI\r");
    LOG_INFO("#MONI: %s", resp);
    if (r > 30) {
    	to_log_info_uart("ECALL: %s", ecall_str);
    	at_era_sendCommand(0, resp, sizeof(resp), ecall_str); // эта команда не возвращает OK, поэтому нет смысла делать проверки
    } else {
    	deinit_timer(&conn_wait_t);
    	m2mb_os_taskSleep(M2MB_OS_MS2TICKS(1000));
    	send_to_era_action_task(ERA_A_CALL_FAILED, 0, 0);
    }
}

static void call_failed(void) {
    LOG_INFO("ECALL failed");
    cancel_call();
    send_to_ind_task(SHOW_ECALL_IMPOSSIBLE, 0, 0);
    deinit_timer(&conn_wait_t);
    call_attempts++;
    int attempts = (ecall_event == ECALL_MANUAL_SOS || ecall_event == ECALL_TEST) ? get_manual_dial_attempts() : get_auto_dial_attempts();

    if (is_one_and_one()) {
    	int prev = attempts;
    	attempts = 2;
    	LOG_INFO("attempts changed from %i to %i", prev, attempts);
    }

    to_log_info_uart("Attempt: #%i, max:%i", call_attempts, attempts);
    if (call_attempts >= attempts || (ecall_event == ECALL_TEST && call_attempts >= 1)) {
    	if (!at_era_sendCommandExpectOk(3*1000, "AT\r")) {
    		if (!at_era_sendCommandExpectOk(3*1000, "AT\r")) restart_from_reason(ECALL_DEFAULT);
    	}
    	if (ecall_event == ECALL_TEST) {
            play_audio(AUD_END_TEST_CALL);
        }
        finish(ERA_R_ATTEMPTS_ENDED);
    } else {
        if (dial_duration_expired) {
            finish(ERA_R_DIAL_DURATION_TIMER);
        } else {
            int timeout = 60 * get_dial_duration() / attempts - 10;
            if (timeout < 5) timeout = 5;
            to_log_info_uart("Dials delay: %i sec", timeout);
            deinit_timer(&dials_delay_t);
            dials_delay_t = init_timer("dials_delay_t", timeout * 1000, e_action_timer_handler);
            //if (!is_ecall_to_112() || ecall_event == ECALL_TEST) try_registration();
        }
    }
}

static void e_action_timer_handler(void* ctx, AZX_TIMER_ID id) {
    (void) ctx;
    if (dial_dur_t == id) {
        dial_duration_expired = TRUE;
        to_log_info_uart("dial_dur_t expired");
        if (voice_active) to_log_info_uart("voice channel active");
        else send_to_era_action_task(ERA_A_DIAL_DUR_T, 0, 0);
    } else if (conn_wait_t == id) {
        to_log_info_uart("conn_wait_t expired");
        send_to_era_action_task(ERA_A_CALL_FAILED, 0, 0);
    } else if (disc_catch_t == id) {
        to_log_info_uart("disc_catch_t expired");
        send_to_era_action_task(ERA_A_DISC_CATCH_T, 0, 0);
    } else if (dials_delay_t == id) {
        to_log_info_uart("dials_delay_t expired");
        send_to_era_action_task(ERA_A_MAKE_CALL, 0, 0);
    } else if (ccft_t == id) {
        to_log_info_uart("ccft_t expired");
        send_to_era_action_task(ERA_A_CCFT_T, 0, 0);
    } else if (finish_t == id) {
    	send_to_ind_task(SHOW_RING_SIGNAL, 0, 0);
        play_audio_file(AUD_ONE_TONE, 3, &era_a_player_result_cb, EA_ONE_TONE_1, result_action);
    } else to_log_error_uart("E_action unknown t_id: %i", id);

}

// ecallda,1 - MSD sending is started - detected the Start message  // #ECALLEV:0 eCall START message detected, MSD sending is started
// ecallda,2 - LL-ACK message: MSD was received correctly 			// #ECALLEV:1 eCall LL-ACK message detected, MSD was received correctly
// ecallda,3 - AL-ACK: MSD was successfully decoded 				// #ECALLEV:2 eCall HL-ACK (AL-ACK) message detected, MSD was successfully decoded
// #ECALLEV:5 T5 expiring, IVS automatically unmutes downlink and uplink
// #ECALLEV:6 T6 expiring, IVS automatically unmutes downlink and uplink
// #ECALLEV:7 T7 expiring, IVS automatically unmutes downlink and uplink
// #ECALLEV:11 IVS disconnects microphone and speaker from speech codec and connects In-band Modem to speech codec
// #ECALLEV:12 	IVS disconnects In-band Modem from speech codec and connects microphone and speaker to speech codec
// #ECALLEV:16 synchronization between IVS and PSAP is lost
// <data>: 0 - Positive ACK, 1 - Clear-down
static void era_a_urc(ERA_A_TASK_EVENT event, INT32 p1, INT32 p2) {
    if (!urc_listening) return;
    (void) p1;
    to_log_info_uart("era_a_urc: %s, %i, %i", get_era_a_event_descr(event), p1, p2);
    switch (event) {
        case ERA_A_URC_DIALING:
            to_log_info_uart("ERA_A: DIAILING..");
            break;
        case ERA_A_URC_RINGING:
            break;
        case ERA_A_URC_RING:
            to_log_info_uart("ERA_A: RING");
            if (ecall_event == ECALL_TEST) return;
            deinit_timer(&conn_wait_t);
            deinit_timer(&dials_delay_t);
            on_mute();
            answer_call();
            break;
        case ERA_A_URC_CONNECTED:
            to_log_info_uart("ERA_A: CONNECTED");
            connected = TRUE;
            deinit_timer(&conn_wait_t);
            deinit_timer(&dials_delay_t);
            deinit_timer(&ccft_t);
            ccft_t = init_timer("ccft_t", get_ccft() * 60 * 1000, e_action_timer_handler);
            if (get_era_restart() > 0) {
                if (!set_era_restart(0)) LOG_ERROR("Can't set era_r");
            }
            break;
        case ERA_A_URC_ECALLEV:
            switch (p1) {
                case 0: // #ECALLEV:0 eCall START message detected
                    to_log_info_uart("eCall START message detected");
                    send_to_ind_task(SHOW_SENDING_MSD, 0, 0);
                    play_audio_file("two_tones.wav", 3, NULL, EA_TWO_TONES_1, 0);
                    break;
                case 1: // #ECALLEV:1 eCall LL-ACK message detected, MSD was received correctly
                    to_log_info_uart("eCall LL-ACK message detected, MSD was received correctly");
                    msd_decoded = TRUE;
                    break;
                case 2:	// #ECALLEV:2 eCall HL-ACK (AL-ACK) message detected, MSD was successfully decoded
                    to_log_info_uart("eCall HL-ACK (AL-ACK) message detected, %s", p2 == 0 ? "MSD was successfully decoded" : "MSD not decoded, clear-down");
                    pull_mode = TRUE;
                    if (p2 == 0) msd_decoded = TRUE;
                    break;
                case 5: // #ECALLEV:5 T5 expiring, IVS automatically unmutes downlink and uplink
                case 6: // #ECALLEV:6 T6 expiring, IVS automatically unmutes downlink and uplink
                case 7: // #ECALLEV:7 T7 expiring, IVS automatically unmutes downlink and uplink
                    if (!msd_decoded) {
                        to_log_info_uart("Timeout MSD sent");
                        pull_mode = TRUE;
                        move_inband_to_sms(); // для строго соответствия п.6.2.2 ГОСТ33467
                        if (ecall_event != ECALL_TEST) send_to_sms_task(SMS_SEND_ALL_FROM_STORAGE, 0, 0);
                    }
                    break;
                case 11: // #ECALLEV:11 IVS disconnects microphone and speaker from speech codec and connects In-band Modem to speech codec
                    to_log_info_uart("IVS disconnects microphone and speaker from speech codec and connects In-band Modem to speech codec");
                    //deinit_timer(&disc_catch_t); // таймер отключен, т.к. иногда ответ CEER содержит коды статуса предыдущего вызова, из-за чего отменяется новый вызов
                    //disc_catch_t = init_timer("disc_catch_t", 1500, e_action_timer_handler);
                    break;
                case 12: // #ECALLEV:12 IVS disconnects In-band Modem from speech codec and connects microphone and speaker to speech codec
                    to_log_info_uart("IVS disconnects In-band Modem from speech codec and connects microphone and speaker to speech codec");
                    to_log_info_uart("Voice communication is possible");
                    voice_active = TRUE;
                    send_to_ind_task(SHOW_VOICE_CHANNEL, 0, 0);
                    if (msd_decoded || pull_mode) {
                        play_audio_file(AUD_ONE_TONE, 3, NULL, EA_ONE_TONE_2, 0);
                        can_be_canceled = FALSE;
                        if (msd_decoded && ecall_event != ECALL_TEST) increment_msd_id(TRUE);
                    }
                    deinit_timer(&disc_catch_t);
                    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(100));
                    catch_disconnect();
                    break;
                case 16: // #ECALLEV:16 synchronization between IVS and PSAP is lost
                    to_log_info_uart("Synchronization between IVS and PSAP is lost");
                    // call_failed();
                    break;
                default:
                    to_log_error_uart("Unknown ECALL status URC: %i", p1);
                    call_failed();
                    break;
            }
            break;
        case ERA_A_URC_BUSY:
            to_log_info_uart("Line is BUSY");
            call_failed();
            break;
        case ERA_A_URC_NO_CARRIER:
        case ERA_A_URC_DISCONNECTED:
            disconnected();
            break;
        case ERA_A_URC_CREG:
            if (p2 == 1 || p2 == 5) {
                to_log_info_uart("Era_a: Registered");
            } else if (p2 == 0) {
                to_log_info_uart("Era_a: Deregistered");
                //send_CECALL();
                m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1*1000));
                //if (!is_ecall_to_112() || ecall_event == ECALL_TEST) {}
            }
            break;
        case ERA_A_URC_RELEASED:
            to_log_info_uart("Call was released");
            //deinit_timer(&conn_wait_t);
            //send_to_era_action_task(ERA_A_CALL_FAILED, 0, 0);
            //if (get_SIM_profile() == ERA_PROFILE && is_registered())
            //if (!is_ecall_to_112() || ecall_event == ECALL_TEST) deregistration();
            //m2mb_os_taskSleep( M2MB_OS_MS2TICKS(2*1000));
            //make_call();
            //if (!is_ecall_to_112() || ecall_event == ECALL_TEST) try_registration();
            break;
        default: to_log_error_uart("Unknown era_a URC");
            break;
    }
}

static void disconnected(void) {
    SOS_pressed_numb = 0;
    voice_active = FALSE;
    connected = FALSE;
    if (finished) LOG_ERROR("ERA_R already finished");
    else {
        if (dial_duration_expired) {
            finished = TRUE;
            finish(ERA_R_DIAL_DURATION_TIMER);
        } else {
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(1500));
            CEER_CALL_STATUS ceer = get_CEER_status();
            if (ceer == CEER_UNKNOWN_STATUS) {
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(1000));
                ceer = get_CEER_status();
            }
            if (ceer == CEER_NORMAL_CALL_CLEARING || ceer == CEER_NORMAL_UNSPECIFIED) {
                finished = TRUE;
                finish(ECALL_R_SUCCESS);
            } else {
                LOG_INFO("voice channel was disconnected");
                send_to_era_action_task(ERA_A_CALL_FAILED, 0, 0);
            }
        }
    }
}


static void catch_disconnect(void) {
    CEER_CALL_STATUS ceer = get_CEER_status(); // резервный способ обнаружения завершения вызова, если URC DISCONNECTED не был принят библиотекой azx_ati.c
    //if (ceer == CEER_NORMAL_CALL_CLEARING || ceer == CEER_DISCONNECTED) {
    to_log_info_uart("catch_disc ceer:%i", ceer);
    if (ceer == CEER_DISCONNECTED || ceer == CEER_PHONE_IS_OFFLINE) { // ! ceer == CEER_NORMAL_CALL_CLEARING - если это добавить, то иногда при звонке только на 112 будет появляться этот статус при действующем соединении
        to_log_info_uart("DISCONNECT was revealed");
        finished = TRUE;
        finish(ECALL_R_SUCCESS);
    }
}

static void sos_button_pressed(void) {
    if (!sos_button_listen) return;
    to_log_info_uart("ERA_A: SOS was pressed");
    if (voice_active) {
        if (SOS_pressed_numb < 3) {
            generateDTMF(SOS_pressed_numb);
            SOS_pressed_numb++;
        }
    } else {
        if (ecall_event != ECALL_MANUAL_SOS) return;
        if (can_be_canceled && !finished) {
            to_log_info_uart("Cancellation ECALL with the SOS button");
            finished = TRUE;
            cancel_call();
            finish(ERA_R_MANUAL_CANCEL);
        }
    }
}

static void finish(ECALL_RESULT result) {
    if (stopped) return;
    //if (is_ecall_to_112() && ecall_event != ECALL_TEST) try_registration();
    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(500)); // для исключения проблемы после DISCONNECT
    result_action = result;
    stopped = TRUE;
    urc_listening = FALSE;
    sos_button_listen = FALSE;
    deinit_timer(&dial_dur_t);
    deinit_timer(&conn_wait_t);
    deinit_timer(&dials_delay_t);
    deinit_timer(&ccft_t);
    deinit_timer(&disc_catch_t);
    //print_msd_storage_status();
    if (result == ERA_R_FORCED_STOP) {
        cancel_call();
    } else {
        if (ecall_event != ECALL_TEST && result != ERA_R_NO_SERVICE) {
            if (msd_decoded || result == ERA_R_MANUAL_CANCEL) {
                if (msd_decoded) delete_inband_msd(); // удаление МНД в случае ручной отмены выполняется в ecall.c
            } else {
                move_inband_to_sms();
            }
        }
    }
    print_msd_storage_status();
    deinit_timer(&finish_t); 	//m2mb_os_taskSleep( M2MB_OS_MS2TICKS(700)); // задержка нужна, чтобы модем смог воспроизвести аудиофайл ниже после предшествующего переключения speech codec
    finish_t = init_timer("finish_t", 1500, e_action_timer_handler);
}



static void era_a_player_result_cb(A_PLAYER_RESULT result, INT32 ctx, INT32 params) {
    if (result != A_PLAYER_OK) to_log_error_uart("Can't play ctx:%i", ctx);
    switch(ctx) {
        case EA_ONE_TONE_1: {
        	send_to_ind_task(CANCEL_SHOW_RING_SIGNAL, 0, 0);
        	ECALL_RESULT result = (ECALL_RESULT) params;
            if (result != ERA_R_ATTEMPTS_ENDED) send_to_ind_task(CANCEL_FLASHING, 0, 0);
            send_to_ecall_task(ECALL_ERA_A_CALLBACK, result, 0);
            break;
        }
        default:
            to_log_error_uart("ACtx unknown:%i", ctx);
            finish(ECALL_R_AUDIO_CTX_UNKNOWN);
            break;
    }
}

static char *get_era_a_event_descr(ERA_A_TASK_EVENT event) {
    switch(event) {
        case ERA_A_START: 				return "START";
        case ERA_A_TRANSM_T:			return "TRANSM_T";
        case ERA_A_FORCED_STOP:			return "ERA_A_FORCED_STOP";
        case ERA_A_SOS_BUTTON_PRESSED:	return "SOS_BUTTON_PRESSED";
        case ERA_A_DIAL_DUR_T:			return "DIAL_DUR_T";
        case ERA_A_CALL_FAILED:			return "CALL_FAILED";
        case ERA_A_DISC_CATCH_T:		return "DISC_CATCH_T";
        case ERA_A_MAKE_CALL:			return "MAKE_CALL";
        case ERA_A_CCFT_T:				return "CCFT_T";
        case ERA_A_URC_RINGING:			return "URC_RINGING";
        case ERA_A_URC_RING:			return "URC_RING";
        case ERA_A_URC_NO_CARRIER:		return "URC_NO_CARRIER";
        case ERA_A_URC_CREG:			return "URC_CREG";
        case ERA_A_URC_DIALING:			return "URC_DIALING";
        case ERA_A_URC_CONNECTED:		return "URC_CONNECTED";
        case ERA_A_URC_RELEASED:		return "URC_RELEASED";
        case ERA_A_URC_DISCONNECTED: 	return "URC_DISCONNECTED";
        case ERA_A_URC_BUSY:			return "URC_BUSY";
        case ERA_A_URC_ECALLEV:  		return "URC_ECALLEV";
        default:						return "UNKNOWN";
    }
}

/* Global functions =============================================================================*/
/***************************************************************************************************
   ERA_ACTION_task Нельзя вызывать эту функцию напрямую!
**************************************************************************************************/
INT32 ERA_ACTION_task(INT32 event, INT32 param1, INT32 param2) {
//    to_log_info_uart("ERA_A_task event: %s, p1:%i, p2:%i", get_era_a_event_descr(event), param1, param2);
    switch(event) {
        case ERA_A_START: {
            ecall_event = (ECALL_TASK_EVENT) param1;
            start();
            break;
        }
        case ERA_A_TRANSM_T:
            finished = TRUE;
            finish(ERA_R_TRANSM_INT);
            break;
        case ERA_A_SOS_BUTTON_PRESSED:
            sos_button_pressed();
            break;
        case ERA_A_DIAL_DUR_T:
            finish(ERA_R_DIAL_DURATION_TIMER);
            break;
        case ERA_A_CALL_FAILED:
            call_failed();
            break;
        case ERA_A_DISC_CATCH_T:
            catch_disconnect();
            break;
        case ERA_A_MAKE_CALL:
            make_call();
            break;
        case ERA_A_CCFT_T:
            finished = TRUE;
            cancel_call();
            finish(ERA_R_CCFT_TIMER);
            break;
        case ERA_A_URC_RINGING:
        case ERA_A_URC_RING:
        case ERA_A_URC_NO_CARRIER:
        case ERA_A_URC_CREG:
        case ERA_A_URC_DIALING:
        case ERA_A_URC_CONNECTED:
        case ERA_A_URC_RELEASED:
        case ERA_A_URC_DISCONNECTED:
        case ERA_A_URC_BUSY:
        case ERA_A_URC_ECALLEV:
            era_a_urc(event, param1, param2);
            break;
        case ERA_A_FORCED_STOP:
            finish(ERA_R_FORCED_STOP);
            break;
        default:
            to_log_error_uart("Unknown era_a event: %s", get_era_a_event_descr(event));
            break;
    }
    return M2MB_OS_SUCCESS;
}
