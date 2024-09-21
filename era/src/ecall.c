/**
 * @file calling.c
 * @version 1.0.0
 * @date 18.04.2022
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
#include "msd.h"
#include "sim.h"
#include "msd_storage.h"
#include "params.h"
#include "atc_era.h"
#include "era_helper.h"
#include "indication.h"
#include "util.h"
#include "azx_timer.h"
#include "ecall.h"

#include "../hdr/sms_manager.h"
#include "a_player.h"
#include "era_action.h"
#include "codec.h"
#include "gpio_ev.h"
#include "tasks.h"
#include "sys_utils.h"
#include "asn1.h"
#include "log.h"
#include "sys_param.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
typedef enum {
    IDLE = 1123, // ожидание команды на выполнение вызова
    PREPARE,	 // подготовка к выполнению вызова
    CALLING		 // выполнение вызова
} STEP;
typedef enum {
    A_ONE_TONE_1 = 1592,
    A_ECALL,
    A_ONE_TONE_2,
    A_ONE_TONE_3,
	A_ONE_TONE_4,
    A_ECALL_END
} AUDIO_FILES_CTX;
/* Local statics ================================================================================*/
static volatile STEP step = IDLE;
static ecall_result_cb callback;
static volatile BOOLEAN sos_pressed = FALSE;
static volatile BOOLEAN test_call = FALSE;
static volatile BOOLEAN auto_answer = FALSE;
static volatile BOOLEAN incoming_call = FALSE;
static volatile BOOLEAN urc_listening = FALSE;
static volatile BOOLEAN stopped = TRUE;
static volatile BOOLEAN sms_ecall_request = FALSE; // флаг получения запроса выполнения экстренного вызова по команде из SMS
static AZX_TIMER_ID transm_t = NO_AZX_TIMER_ID;
static AZX_TIMER_ID dereg_t = NO_AZX_TIMER_ID; // контрольный таймер дерегистрации
static AZX_TIMER_ID call_answer_t = NO_AZX_TIMER_ID;
static T_msd t_msd;		// текущее отправляемое МНД
static ECALL_TASK_EVENT ecall_event;
static ECALL_REPEAT_REASON repeat_reason;
/* Local function prototypes ====================================================================*/
static void start(ECALL_TASK_EVENT event, ECALL_REPEAT_REASON repeat_r);
static void new_msd(ECALL_TASK_EVENT event);
static void sim_prepared(void);
static void finish(ECALL_RESULT result);
static void check_manual_cancel(void);
static void ecall_timer_handler(void* ctx, AZX_TIMER_ID t_id);
static void dereg(void);
static void ecall_urc(ECALL_TASK_EVENT event, INT32 param1, INT32 param2);
static void ecall_player_result_cb (A_PLAYER_RESULT result, INT32 ctx, INT32 params);
static volatile BOOLEAN msd_inc_from_inband = FALSE;
static void starting_continue(void);
/* Static functions =============================================================================*/

static void start(ECALL_TASK_EVENT event, ECALL_REPEAT_REASON repeat_r) {
    ecall_event = event;
    repeat_reason = repeat_r;
    to_log_info_uart("Ecall event: %s", get_ecall_event_descr(event));
    send_to_sms_task(SMS_STOP_SENDING, 0, 0);
    stopped = FALSE;
    step = PREPARE;
    test_call = event == ECALL_TEST;
    set_device_mode(test_call ? MODE_TEST : MODE_ECALL);
    deinit_timer(&call_answer_t);
    deinit_timer(&dereg_t);
    deinit_timer(&transm_t);
    sos_pressed = FALSE;
    auto_answer = FALSE;
    send_to_ind_task(SHOW_ECALL, 0, 0);

    at_era_sendCommandExpectOk(2000, "ATS0=0\r");
    if (!on_mute()) restart_from_reason(ecall_event);
    if (is_one_and_one()) {
    	if (test_call) play_audio_file(AUD_ONE_TONE, 6, &ecall_player_result_cb, A_ONE_TONE_1, 0);
    	else play_audio_file(test_call ? AUD_TEST_CALL : AUD_ECALL, 6, &ecall_player_result_cb, A_ECALL, 0);
    } else {
    	starting_continue();
    }
}

static void starting_continue(void) {
	//transm_t = init_timer("transm_t", (get_int_mem_transm_interval() * 60 - 2) * 1000, ecall_timer_handler);
    to_log_info_uart("current coordinates:: latitude:%i, longitude:%i, utc:%i", get_gnss_data().latitude, get_gnss_data().longitude, get_gnss_data().utc);
    msd_inc_from_inband = FALSE;
    if (repeat_reason == ECALL_MODEM_RESTARTED) {
        to_log_info_uart("Ecall is restarted");
        if (find_msd_inband(&t_msd)) to_log_info_uart("MSD INBAND found in the storage");
        else new_msd(ecall_event);
    } else if (repeat_reason == ECALL_SMS_REQUEST_FOR_MSD) {
        // новый МНД не создаётся
    } else {
        new_msd(ecall_event);
    }
    pause_gnss_request(TRUE); // TODO в идеале это делать не следует, но при активном опросе GNSS с помощью АТ-команд иногда появляются сбои
    m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
    cancel_call();
    if (!at_era_sendCommandExpectOk(2*1000, "AT+CREG=1\r")) {
        to_log_error_uart("Can't enable URC AT+CREG");
        restart_from_reason(ecall_event);
    }
    if (!at_era_sendCommandExpectOk(2*1000, "AT#ECALLTYPE=1\r")) {
        to_log_error_uart("Can't AT#ECALLTYPE=1");
        restart_from_reason(ecall_event);
    }
    send_msd_to_modem(&t_msd);
    if (is_one_and_one()) {
    	sim_prepared();
    } else {
    	//if (get_SIM_profile() == ERA_PROFILE) sim_prepared();
    	//else send_to_sim_task(SIM_PROFILE_TO_ERA, (INT32) &ecall_sim_result_cb, 0);
    	if (test_call) play_audio_file(AUD_ONE_TONE, 6, &ecall_player_result_cb, A_ONE_TONE_1, 0);
    	else play_audio_file(test_call ? AUD_TEST_CALL : AUD_ECALL, 6, &ecall_player_result_cb, A_ECALL, 0);

    }
}

static void new_msd(ECALL_TASK_EVENT event) {
    t_msd = create_msd(MSD_INBAND, event, _112, 1, FALSE); // чтобы тесты СМС успешно проходили, нужно, чтобы accident был не Test и ecallType = Settings._112, тогда МНД не будет содержать failures, т.к. если failures есть, то тест СМС проваливается
    if (event != ECALL_TEST) {
        if (!save_MSD(&t_msd)) to_log_error_uart("Can't save MSD in flash");
    }
}

static void sim_prepared(void) {
    if (stopped) return;
    LOG_DEBUG("SIM_PREPARED");
//    if (test_call) play_audio_file(AUD_ONE_TONE, 6, &ecall_player_result_cb, A_ONE_TONE_1, 0);
//    else play_audio_file(test_call ? AUD_TEST_CALL : AUD_ECALL, 6, &ecall_player_result_cb, A_ECALL, 0);
    check_manual_cancel();
}

static void check_manual_cancel(void) {
    if (stopped) return;
    LOG_DEBUG("check_manual_cancel, sos pressed:%i", sos_pressed);
    step = CALLING;
    if (sos_pressed) finish(ECALL_R_MANUAL_CANCEL);
    else {
        urc_listening = FALSE;
        send_to_era_action_task(ERA_A_START, ecall_event, 0);
    }
}

static void finish(ECALL_RESULT result) {
    step = IDLE;
    if (stopped) return;
    stopped = TRUE;
    set_device_mode(MODE_ERA);
    deinit_timer(&transm_t);
    to_log_info_uart("Ecall finished: %s", get_ecall_result_desc(result));
    //if (get_gnss_data().fix == 0) {
    //	LOG_INFO("fix=0, so reset GNSS");
    if (is_one_and_one()) {
    	LOG_INFO("Not reset GNSS due to:: dial_dur:%i, int_mem_transm:%i", get_dial_duration(), get_int_mem_transm_interval());
    } else {
    	send_to_gnss_task(RESET_GNSS, 3, 0);
    }

    m2mb_os_taskSleep(M2MB_OS_MS2TICKS(1000));
    pause_gnss_request(FALSE);
    m2mb_os_taskSleep(M2MB_OS_MS2TICKS(2000));
    //}
    if (!test_call) {
        if (result != ECALL_R_MANUAL_CANCEL && result != ECALL_R_FORCED_STOP) off_mute();
        send_to_sms_task(SMS_SEND_ALL_FROM_STORAGE, 0, 0);
    }
    if (result != ECALL_R_FORCED_STOP && result != ECALL_R_MANUAL_CANCEL) {
        int dereg_timeout;
        if (test_call) {
            dereg_timeout = get_post_test_reg_time();
            auto_answer = TRUE; // По просьбе А.Соколова, это нужно для "теста тележки" в лаборатории
            subscribe_for_urcs();
            urc_listening = TRUE;
            call_answer_t = init_timer("call_answer_t", dereg_timeout * 1000, ecall_timer_handler);
        } else {
            if (is_ecall_to_112()) dereg_timeout = get_real_nad_deregistration_time() * 60;
            else dereg_timeout = get_post_test_reg_time(); // поскольку звонок на подсистему тестирования использует команду модема для тестового звонка
            auto_answer = TRUE;
            subscribe_for_urcs();
            urc_listening = TRUE;
            UINT32 cat = get_call_auto_answer_time() * 60 * 1000;
            call_answer_t = init_timer("call_answer_t", cat, ecall_timer_handler);
            //to_log_info_uart(" - - - CALLBACK_ANSWER_TIMER started for: %i sec (%i min)", cat / 1000, get_call_auto_answer_time());
        }
        to_log_info_uart("Reg status: %s, deregistration through: %i sec", get_reg_st_descr(get_reg_status_AT()), dereg_timeout);
        dereg_t = init_timer("dereg_t", (dereg_timeout + 10) * 1000, ecall_timer_handler);
    } else if (result == ECALL_R_MANUAL_CANCEL || result == ECALL_R_FORCED_STOP) {
        delete_inband_msd();
        play_audio_file(AUD_END_TEST_CALL, 6, &ecall_player_result_cb, A_ECALL_END, result);
        send_to_ind_task(CANCEL_FLASHING, 0, 0);
    } else {
        delete_MSD(0);
        send_to_ind_task(CANCEL_FLASHING, 0, 0);
    }
    if (result != ECALL_R_MANUAL_CANCEL && callback != NULL) callback(result);
}


static void ecall_timer_handler(void* ctx, AZX_TIMER_ID id) {
    (void)ctx;
    if (transm_t == id) {
        to_log_info_uart("transm_t expired");
        send_to_era_action_task(ERA_A_TRANSM_T, 0, 0);
    } else if (dereg_t == id) {
        to_log_info_uart("dereg_t expired");
        send_to_ecall_task(ECALL_DEREGISTRATION, 0, 0);
    } else if (call_answer_t == id) {
        to_log_info_uart(" - - - CALL_ANSWER_TIMER expired! - - -");
        auto_answer = FALSE;
        to_log_info_uart("Auto answer is disabled");
        send_to_ind_task(CANCEL_FLASHING, 0, 0);
        //if (!isIgnition()) to_sleep();
    } else to_log_error_uart("Ecall unknown t_id: %i", id);
}

static void dereg(void) {
    if (step != IDLE || !stopped) return;
    urc_listening = FALSE;
    deinit_timer(&call_answer_t);
    send_to_ind_task(CANCEL_FLASHING, 0, 0);
    if (auto_answer) {
        LOG_WARN("Callback wait canceled due to deregistration timer");
        auto_answer = FALSE;
    }
    to_sleep();
    //deregistration(); // фактически модем будет дерегистрироваться сам по внутреннему таймеру
}

static void ecall_urc(ECALL_TASK_EVENT event, INT32 param1, INT32 param2) {
    if (!urc_listening) return;
    (void) param1;
    LOG_DEBUG("ecall_urc: %s, %i, %i", get_ecall_event_descr(event), param1, param2);
    switch (event) {
        case ECALL_URC_RING:
            if (auto_answer && !incoming_call) {
            	send_to_ind_task(SHOW_RING_SIGNAL, 0, 0);
            	on_mute();
            	m2mb_os_taskSleep( M2MB_OS_MS2TICKS(100));
            	incoming_call = TRUE;
            	play_audio_file(AUD_ONE_TONE, 3, &ecall_player_result_cb, A_ONE_TONE_2, 0);
            	//generateDTMF(0); ecall_player_result_cb(A_PLAYER_OK, A_ONE_TONE_2, 0);
            }
            break;
        case ECALL_URC_NO_CARRIER:
            play_audio_file(AUD_ONE_TONE, 3, &ecall_player_result_cb, A_ONE_TONE_3, 0);
            break;
        case ECALL_URC_CREG:
            //if (strstr(msg, ": 0") != NULL || strstr(msg, ": 4") != NULL) { // strstr(msg, ": 3") != NULL || - потому что этот модем после 3 может 5
            if (param2 == 0 || param2 == 4) {
                deinit_timer(&dereg_t);
                deinit_timer(&call_answer_t);
                urc_listening = FALSE;
                send_to_ind_task(CANCEL_FLASHING, 0, 0);
            } else if (param2 == 2) {
                to_log_info_uart("T324X is running: %i", is_T324X_running());
            }
            break;
        case ECALL_URC_CONNECTED:
            if (auto_answer && incoming_call) send_to_ind_task(SHOW_VOICE_CHANNEL, 0, 0);
            break;
        case ECALL_URC_DISCONNECTED:
            if (incoming_call) {
                incoming_call = FALSE;
                send_to_ind_task(SHOW_RING_SIGNAL, 0, 0);
                m2mb_os_taskSleep( M2MB_OS_MS2TICKS(100));
                play_audio_file(AUD_ONE_TONE, 3, &ecall_player_result_cb, A_ONE_TONE_4, 0);
            }
            break;
        default: to_log_error_uart("Unknown ecall URC");
            break;
    }
}

static void ecall_player_result_cb(A_PLAYER_RESULT result, INT32 ctx, INT32 params) {
    ECALL_RESULT ecall_result = (ECALL_RESULT) params;
    if (result != A_PLAYER_OK) to_log_error_uart("Can't play file_ctx: %i", ctx);
    switch(ctx) {
        case A_ONE_TONE_1:
        case A_ECALL:
            if (is_one_and_one()) {
            	starting_continue();
            } else {
            	sim_prepared();
            }
            break;
        case A_ONE_TONE_2:
            if (auto_answer) {
            	m2mb_os_taskSleep( M2MB_OS_MS2TICKS(900));
            	send_to_ind_task(CANCEL_SHOW_RING_SIGNAL, 0, 0);
            	answer_call();
            }
            break;
        case A_ONE_TONE_3:
            if (incoming_call) {
                incoming_call = FALSE;
                off_mute();
            }
            break;
        case A_ONE_TONE_4:
        	send_to_ind_task(CANCEL_SHOW_RING_SIGNAL, 0, 0);
        	send_to_ind_task(CANCEL_FLASHING, 0, 0);
            off_mute();
        	break;
        case A_ECALL_END:
            off_mute();
            if (callback != NULL) callback(ecall_result);
            break;
        default:
            to_log_error_uart("ACtx unknown:%i", ctx);
            finish(ECALL_R_AUDIO_CTX_UNKNOWN);
            break;
    }
}


/* Global functions =============================================================================*/
T_msd get_current_msd(void) {
    return t_msd;
}

BOOLEAN is_incoming_call(void) {
    return incoming_call;
}

void increment_msd_id(BOOLEAN from_inband) {
    print_hex("MSD before id inc", (char *)&t_msd, sizeof(t_msd));
    UINT8 new_msd_id = t_msd.msdMessage.msdStructure.messageIdentifier;
    if (msd_inc_from_inband) { // этим устраняется проблема с двойным инкрементированием идентификатора МНД при первом получении через СМС команды с запросом повторной отправки МНД
        msd_inc_from_inband = FALSE;
        LOG_INFO("MSD id has already been incremented from in-band logic");
    } else {
        if (new_msd_id < 0xFF) new_msd_id++;
        LOG_INFO("MSD id was incremented");
    }
    msd_inc_from_inband = from_inband;
    T_GnssData gnss_data = get_gnss_data();
    gnss_data.utc = t_msd.msdMessage.msdStructure.timestamp;
    gnss_data.fix = t_msd.msdMessage.msdStructure.controlType.positionCanBeTrusted;
    LOG_INFO("incr_msd, gnss_data.utc:%i, gnss_data.fix:%i", gnss_data.utc, gnss_data.fix);
    T_msd upd_msd = msd_fill(t_msd.msdFlag,
                             t_msd.msdMessage.msdStructure.controlType.automaticActivation,
                             t_msd.msdMessage.msdStructure.controlType.testCall,
                             new_msd_id,
                             0,
                             gnss_data);
    t_msd = upd_msd;
    print_hex("MSD after id inc", (char *)&t_msd, sizeof(t_msd));
    send_msd_to_modem(&t_msd);
}

//void ecall_sim_result_cb(SIM_PROFILE sim_profile) {
//    if (step == IDLE) return;
//    if (sim_profile == UNKNOWN_PROFILE) {
//        LOG_CRITICAL("Invalid set SIM profile");
//        restart_from_reason(ecall_event);
//    } else send_to_ecall_task(ECALL_SIM_PREPARED, 0, 0);
//}

BOOLEAN is_waiting_call(void) {
    return auto_answer;
}

/***************************************************************************************************
   ECALL_task Нельзя вызывать эту функцию напрямую!
**************************************************************************************************/
INT32 ECALL_task(INT32 event, INT32 param1, INT32 param2) {
    (void) param1;
    LOG_INFO("ECALL_task event: %s", get_ecall_event_descr(event));
//    if(hasIccid() == FALSE){
//        LOG_INFO("SIM has a problem...");
//        return M2MB_OS_SUCCESS;
//    }
    switch(event) {
        case ECALL_SMS_REQUEST:
            LOG_INFO("Wait SMS sending..");
            sms_ecall_request = TRUE;
            break;
        case ECALL_SMS_SENT:
            if (sms_ecall_request) {
                sms_ecall_request = FALSE;
                m2mb_os_taskSleep( M2MB_OS_MS2TICKS(3*1000));
                LOG_INFO("SMS respond sent, waiting RAIM"); // по просьбе А.Архипова от 07.02.24 для прохождения теста 6.19
                for (int i = 0; i < 50; i++) {
                	UINT8 fix = get_gnss_data().fix;
                	LOG_INFO("GNSS fix:%i", fix);
                	if (fix > 0) break;
                	m2mb_os_taskSleep( M2MB_OS_MS2TICKS(500));
                }
                send_to_ecall_task(ECALL_MANUAL_SOS, ECALL_NO_REPEAT_REASON, 0);
            }
            break;
   		case ECALL_MANUAL_SOS:
   		case ECALL_TEST:
   		case ECALL_FRONT:
   		case ECALL_LEFT:
   		case ECALL_RIGHT:
   		case ECALL_REAR:
   		case ECALL_ROLLOVER:
   		case ECALL_SIDE:
   		case ECALL_FRONT_OR_SIDE:
        case ECALL_AIRBAGS:
   		case ECALL_ANOTHER:	{
            ecall_result_cb cb = param2 == 0 ? NULL : (ecall_result_cb) param2;
            //if (is_testing_working() && event != ECALL_TEST) send_to_testing_task(TEST_FORCED_STOP, TRUE, 0);
            if (is_testing_working() && event != ECALL_TEST) {
                LOG_WARN("Testing now, ECALL prohibited");
                break;
            }
            if (step != IDLE) {
                LOG_WARN("ECALL already started, step: %d", step);
                if (cb != NULL) cb(ECALL_R_IN_PROGRESS);
                break;
            }
            callback = cb;
            ECALL_REPEAT_REASON restart_reason = (ECALL_REPEAT_REASON) param1;
            start(event, restart_reason);

               if((event == ECALL_AIRBAGS) | (event == ECALL_FRONT)){         // Если инициатор вызова подушки безопасности
                   int cnt = get_airbags_cnt();
                   cnt++;
                   set_airbags_cnt(cnt);
               } else if (event == ECALL_ROLLOVER){ // Если инициатор вызова акселерометр
                   int cnt = get_rollover_cnt();
                   cnt++;
                   set_rollover_cnt(cnt);
               } else if (event != ECALL_TEST){  // Если инициатор вызова все остально кроме процедуры тестирования
                   int cnt = get_manual_cnt() + 1;
                   if(!set_manual_cnt(cnt)){
                       LOG_WARN("manual cnt not add");
                   }
               }
			break;
		}
        case ECALL_EVENT_SOS_PRESSED:
            if (step != PREPARE) break;
            if (is_testing_working()) {
                LOG_WARN("SOS was pressed, but ignored - testing");
                break;
            }
            sos_pressed = TRUE;
            to_log_info_uart("ECall SOS was pressed");
            break;
        case ECALL_SIM_PREPARED:
            if (step == IDLE) break;
            sim_prepared();
            break;
        case ECALL_DEREGISTRATION:
            dereg();
            break;
        case ECALL_FORCED_STOP:
            if (step == IDLE) break;
            send_to_era_action_task(ERA_A_FORCED_STOP, 0, 0);
            m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1500));
            finish(ECALL_R_FORCED_STOP);
            break;
        case ECALL_URC_RING:
        case ECALL_URC_NO_CARRIER:
        case ECALL_URC_CREG:
        case ECALL_URC_CONNECTED:
        case ECALL_URC_DISCONNECTED:
            ecall_urc(event, param1, param2);
            break;
        case ECALL_ERA_A_CALLBACK: {
            ECALL_RESULT result = (ECALL_RESULT) param1;
            LOG_DEBUG("era_a_result_cb %i", result);
            switch (result) {
            	case ERA_R_CCFT_TIMER:
            	case ECALL_R_SUCCESS: 		finish(ECALL_R_SUCCESS); 		break;
                case ERA_R_MANUAL_CANCEL: 	finish(ECALL_R_MANUAL_CANCEL); 	break;
                case ERA_R_FORCED_STOP:		finish(ECALL_R_FORCED_STOP);	break;
                default:					finish(ECALL_R_FAULT_CALLING); 	break;
            }
            break;
        }
        default:
            if (step == IDLE) break;
            to_log_error_uart("Ecall event unknown:%i", event);
            finish(ECALL_R_EVENT_UNKNOWN);
            break;
    }
    return M2MB_OS_SUCCESS;
}


BOOLEAN is_ecall_in_progress(void) {
    return step != IDLE;
}

char *get_ecall_event_descr(ECALL_TASK_EVENT event) {
    switch(event) {
        case ECALL_EVENT_SOS_PRESSED:	return "SOS_PRESSED";
        case ECALL_MANUAL_SOS: 		return "MANUAL_SOS";
        case ECALL_SMS_REQUEST:		return "ECALL_SMS_REQUEST";
        case ECALL_SMS_SENT:		return "ECALL_SMS_SENT";
        case ECALL_TEST:			return "TEST";
        case ECALL_FRONT:			return "FRONT";
        case ECALL_LEFT:			return "LEFT";
        case ECALL_RIGHT:			return "RIGHT";
        case ECALL_REAR:			return "REAR";
        case ECALL_ROLLOVER:		return "ROLLOVER";
        case ECALL_SIDE:			return "SIDE";
        case ECALL_FRONT_OR_SIDE:	return "FRONT_OR_SIDE";
        case ECALL_ANOTHER: 		return "ANOTHER";
        case ECALL_SIM_PREPARED:	return "SIM_PREPARED";
        case ECALL_DEREGISTRATION:  return "DEREGISTRATION";
        case ECALL_FORCED_STOP:		return "FORCED_STOP";
        case ECALL_ERA_A_CALLBACK: return "ERA_A_CALLBACK";
        case ECALL_URC_RING:		return "URC_RING";
        case ECALL_URC_NO_CARRIER:  return "URC_NO_CARRIER";
        case ECALL_URC_CREG:		return "URC_CREG";
        case ECALL_URC_CONNECTED:	return "URC_CONNECTED";
        case ECALL_URC_DISCONNECTED:return "URC_DISCONNECTED";
        case ECALL_AIRBAGS:         return "ECALL_AIRBAGS";
        default:					return "UNKNOWN";
    }
}

char *get_ecall_result_desc(ECALL_RESULT result) {
    switch(result) {
        case ECALL_R_SUCCESS:			return "SUCCESS";
        case ECALL_R_IN_PROGRESS:		return "IN_PROGRESS";
        case ECALL_R_AUDIO_CTX_UNKNOWN:	return "AUDIO_CTX_UNKNOWN";
        case ECALL_R_EVENT_UNKNOWN:		return "EVENT_UNKNOWN";
        case ECALL_R_MANUAL_CANCEL:		return "MANUAL_CANCEL";
        case ECALL_R_FAULT_CALLING:		return "FAULT_CALLING";
        case ECALL_R_FORCED_STOP:		return "FORCED_STOP";
        case ERA_R_MANUAL_CANCEL:		return "MANUAL_CANCEL";
        case ERA_R_ATTEMPTS_ENDED:		return "ATTEMPTS_ENDED";
        case ERA_R_DIAL_DURATION_TIMER:	return "DIAL_DURATION_TIMER";
        case ERA_R_TRANSM_INT:			return "TRANSM_INT";
        case ERA_R_NO_SERVICE:			return "NO_SERVICE";
        case ERA_R_CCFT_TIMER:			return "CCFT_TIMER";
        case ERA_R_FORCED_STOP:			return "ERA_R_FORCED_STOP";
        default:						return "UNKNOWN";
    }
}
