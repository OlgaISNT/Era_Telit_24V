
/**
 * @file era_helper.c
 * @version 1.0.0
 * @date 08.04.2022
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
#include "atc_era.h"
#include "at_command.h"
#include "m2mb_os_api.h"
#include "era_helper.h"
#include "msd_storage.h"
#include "../../hal/hdr/gnss_rec.h"
#include "sys_utils.h"
#include "sys_param.h"
#include "asn1.h"
#include "msd.h"
#include "util.h"
#include "files.h"
#include "auxiliary.h"
#include "msd.h"
#include "sim.h"
#include "sms_handler.h"
#include "tasks.h"
#include "failures.h"
#include "codec.h"
#include "params.h"
#include "ecall.h"
#include "log.h"
#include "u_time.h"
#include "factory_param.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static volatile BOOLEAN urc_subscribed = FALSE;
static volatile UINT8 reg_status = 4; // текущий статус регистрации модуля в сети
/* Local function prototypes ====================================================================*/
static void urcs_handler_cb(const CHAR* msg);
static void urc_str_handler(char *msg);
static BOOLEAN command_ok(char *at_command);
static BOOLEAN setting_ok(char *at_command, char *exp_resp, char *correct_setting);
BOOLEAN uart_gpio_ok(void);
BOOLEAN config_uart_gpio(char *resp, UINT8 numb);
/* Static functions =============================================================================*/
//static void sim_result_cb (SIM_PROFILE sim_profile) {
//    //LOG_INFO("CB new SIM profile: %s", (sim_profile == ERA_PROFILE ? "ERA" : "COMM"));
//    if (sim_profile != ERA_PROFILE) {
//        LOG_ERROR("Invalid set SIM profile");
////        restart(5*1000);
//    }
//}

static void urcs_handler_cb(const CHAR* msg) {
    if (msg == NULL || strlen(msg) == 0) return;
    LOG_DEBUG("URC handler: %s", msg);
    char *start = (char *) msg;
    char *end = start + strlen(msg);
    char *p = NULL;
    char part[300];
    do {
        p = strstr_rn(start);
        int size;
        if (p != NULL) {
            size = p + 2 - start;
            memcpy(part, start, size);
            start = p + 2;
        } else {
            size = end - start;
            memcpy(part, start, size);
        }
        *(part + size) = 0;
        urc_str_handler(part); // LOG_DEBUG("TEST: %s, size: %i", part, strlen(part));
    } while (p != NULL && (p + 2) <= end && start < end);
}

// парсинг одной строки из многострокового URC
static void urc_str_handler(char *msg) {
    if (strstr(msg, "RINGING") != NULL) {					// ^SXCALLSTAT:4
        send_to_era_action_task(ERA_A_URC_RINGING, 0, 0);
    } else if (strstr(msg, "RING") != NULL) {				// ^SXCALLSTAT:4
        send_to_era_action_task(ERA_A_URC_RING, 0, 0);
        send_to_ecall_task(ECALL_URC_RING, 0, 0);
    } else if (strstr(msg, "NO CARRIER") != NULL) {
        send_to_era_action_task(ERA_A_URC_NO_CARRIER, 0, 0);
        send_to_ecall_task(ECALL_URC_NO_CARRIER, 0, 0);
    } else if (strstr(msg, "+CREG: ") != NULL && strlen(msg) > 7) {
        INT32 p2 = *(strstr(msg, "+CREG: ") + 7) - 0x30;
        reg_status = (UINT8) p2;
        send_to_era_action_task(ERA_A_URC_CREG, -1, p2);
        send_to_ecall_task(ECALL_URC_CREG, -1, p2);
    } else if (strstr(msg, "DIALING") != NULL) {			// ^SXCALLSTAT:2
        send_to_era_action_task(ERA_A_URC_DIALING, 0, 0);
    } else if (strstr(msg, "RELEASED") != NULL) {
        send_to_era_action_task(ERA_A_URC_RELEASED, 0, 0);
    } else if (strstr(msg, "SCONNEC") != NULL) { // "DISCONNECTED"
        send_to_era_action_task(ERA_A_URC_DISCONNECTED, 0, 0);
        send_to_ecall_task(ECALL_URC_DISCONNECTED, 0, 0);
    } else if (strstr(msg, "CONNECTED") != NULL) {			// ^SXCALLSTAT:7
        send_to_era_action_task(ERA_A_URC_CONNECTED, 0, 0);
        send_to_ecall_task(ECALL_URC_CONNECTED, 0, 0);
    } else if (strstr(msg, "BUSY") != NULL) {
        send_to_era_action_task(ERA_A_URC_BUSY, 0, 0);
    } else if (strstr(msg, "#ECALLEV:") != NULL && strlen(msg) < 50) {
        char *s = strstr(msg, "#ECALLEV:");
        char *c = strrchr(msg, ',');
        char *e = strrchr(msg, '\r');
        if (e == NULL) {
            LOG_ERROR("uhr1");
            return;
        }
        INT32 p1 = -1, p2 = -1;
        s += 8;
        if (!get_int(s, c == NULL ? e : c, &p1)) LOG_ERROR("uhr2");
        if (c != NULL) {
            if (!get_int(c, e, &p2)) LOG_WARN("uhr3");
        }
        //LOG_DEBUG("mes: %s, p1: %i, p2: %i", msg, p1, p2);
        send_to_era_action_task(ERA_A_URC_ECALLEV, p1, p2);
    }
}

static BOOLEAN command_ok(char *at_command) {
    if (!at_era_sendCommandExpectOk(5000, at_command)) {
        LOG_CRITICAL("Can't exec: %s", at_command);
        return FALSE;
    } else return TRUE;
}

static BOOLEAN setting_ok(char *at_command, char *exp_resp, char *correct_setting) {
    char resp[100];
    int length = at_era_sendCommand(2500, resp, sizeof(resp), at_command);
    if (length == 0) {
        LOG_CRITICAL("Can't execute: %s", at_command);
        return FALSE;
    } else {
        if (strstr(resp, exp_resp) == NULL) {
            LOG_WARN("The setting will be changed to <%s> after restart", exp_resp);
            if (!at_era_sendCommandExpectOk(3000, correct_setting)) LOG_CRITICAL("Can't exec: %s", correct_setting);
            return FALSE;
        } return TRUE;
    }
}


BOOLEAN uart_gpio_ok(void) {
    char resp[200];
    char *atc = "AT#V24CFG?\r";
    int length = at_era_sendCommand(2000, resp, sizeof(resp), atc);
    BOOLEAN r = TRUE;
    if (length == 0) {
        LOG_CRITICAL("Can't execute: %s", atc);
        r = FALSE;
    } else {
        BOOLEAN ok = config_uart_gpio(resp, 0);	if (r) r = ok;
        ok = config_uart_gpio(resp, 2);			if (r) r = ok;
        ok = config_uart_gpio(resp, 3);			if (r) r = ok;
        ok = config_uart_gpio(resp, 4);			if (r) r = ok;
    }
    return r;
}

BOOLEAN config_uart_gpio(char *resp, UINT8 numb) {
    char cond[50];
    memset(cond, 0, 50);
    sprintf(cond, "#V24CFG: %i,2\r", numb);
    if (strstr(resp, cond) == NULL) {
        at_era_sendCommandExpectOk(2000, "AT#V24CFG=%i,2,1\r", numb);
        return FALSE;
    } return TRUE;
}

/* Global functions =============================================================================*/
void subscribe_for_urcs(void) {
    if (!urc_subscribed) {
        urc_subscribed = TRUE;
        at_era_addUrcHandler("RING", urcs_handler_cb); // он же получит и RINGING (AT#DIALMODE)
        at_era_addUrcHandler("NO CARRIER", urcs_handler_cb);
        at_era_addUrcHandler("+CREG:", urcs_handler_cb);
        at_era_addUrcHandler("DIALING", urcs_handler_cb);
        at_era_addUrcHandler("CONNECTED", urcs_handler_cb);
        at_era_addUrcHandler("RELEASED", urcs_handler_cb);
        at_era_addUrcHandler("DISCONNECTED", urcs_handler_cb); // Поскольку возникает проблема с парсингом этого URC, когда он идёт сразу же за "#ECALLEV:" в одном АТ-канале
        at_era_addUrcHandler("#ECALLEV:", urcs_handler_cb);

        //ati_addUrcHandler("DISCONNECTED", urcs_handler_cb);
    }
}

BOOLEAN ecall_modem_init(void) {
    BOOLEAN config_restart = FALSE;

    if (!setting_ok("AT#PORTCFG?\r", "#PORTCFG: 0,0", "AT#PORTCFG=0\r")) 		config_restart = TRUE;
    if (!setting_ok("AT#CALLDISA?\r", "#CALLDISA: 0,0", "AT#CALLDISA=0,0\r")) 	config_restart = TRUE;
    if (!setting_ok("AT#DIALMODE?\r", "#DIALMODE: 2", "AT#DIALMODE=2\r")) 		config_restart = TRUE;
    if (!setting_ok("AT#ECONLY?\r",   "#ECONLY: 2,", "AT#ECONLY=2\r")) 	    config_restart = TRUE; // возможно, эту команду можно не использовать впоследствии
    if (!setting_ok("AT+COPS?\r", "+COPS: 0", "AT+COPS=0\r")) 					config_restart = TRUE;
    if (!setting_ok("AT#ISMSCFG?\r", "#ISMSCFG: 0", "AT#ISMSCFG=0\r"))			config_restart = TRUE;
    if (!command_ok("AT#VAUX=1,1\r")) 												                config_restart = TRUE;
    if (!command_ok("AT#SPIEN=1\r"))                                                                 config_restart = TRUE;
    if (!uart_gpio_ok())                                                                                        config_restart = TRUE;
    if (!setting_ok("AT#ENSIM2?\r", "#ENSIM2: 1", "AT#ENSIM2=1\r"))            config_restart = TRUE;

    switch (getSimSlotNumber()) {
        LOG_DEBUG("Selected sim slot %d", getSimSlotNumber());
        case 0:
            if (!setting_ok("AT#SIMSELECT?\r", "#SIMSELECT: 1", "AT#SIMSELECT=1\r")) {}
            break;
        case 1:
            if (!setting_ok("AT#SIMSELECT?\r", "#SIMSELECT: 2", "AT#SIMSELECT=2\r")) {}
            break;
        default:
            LOG_ERROR("INCORRECT SIM SLOT NUMBER");
            break;
    }

    if (!setting_ok("AT#SIMDET?\r", "#SIMDET: 1,", "AT#SIMDET=1\r")) {
        //config_restart = TRUE;
        if (!command_ok("AT&W\r")) LOG_ERROR("Can't AT&W");
        if (!command_ok("AT&P\r")) LOG_ERROR("Can't AT&P");
        //m2mb_os_taskSleep( M2MB_OS_MS2TICKS(3*1000)); // задержка необходима для применения команды AT#SIMDET
    }

    //if (!command_ok("AT#SIMDET=1\r")) config_restart = TRUE;
    //else m2mb_os_taskSleep( M2MB_OS_MS2TICKS(3*1000)); // задержка необходима для применения команды AT#SIMDET

    if (!command_ok("AT#ECALLURC=2\r")) return FALSE;

    if (!at_era_sendCommandExpectOk(3000, "AT+CALM=0\r")) LOG_ERROR("Can't AT_CALM0");
    if (!at_era_sendCommandExpectOk(3000, "ATX1\r")) LOG_ERROR("Can't ATX1");

    int time = get_nad_dereg_time();// это изменено по требованию А.И.Соколова, EConfig.intMemTransmitInterval * EConfig.intMemTransmitAttempts;
    if (time < 5) time = 5; // п.7.5.3.22 ГОСТ33464
    if (get_nad_dereg_time() > time) {
        if (get_nad_dereg_time() > get_call_auto_answer_time()) {
            time = get_nad_dereg_time();
        } else {
            time = get_call_auto_answer_time();
        }
    } else if (get_call_auto_answer_time() > time) {
        time = get_call_auto_answer_time();
    }
    char resp[200];//time = 1; // - это для теста
    set_real_nad_deregistration_time(time);
    INT32 length = at_era_sendCommand(3000, resp, sizeof(resp), "AT#ECALLNWTMR?\r");
    if (length == 0) {
        LOG_CRITICAL("Can't ECALLNWTMR");
        return FALSE;
    }
    char data[200];
    memset(data, 0, sizeof(data));
    int ecall_dt = get_real_nad_deregistration_time();
    int test_dt = get_post_test_reg_time() / 60;
    if (ecall_dt < 1 || ecall_dt > 2184) {
        LOG_ERROR("Inv RNDT");
        ecall_dt = 1;
    }
    if (test_dt < 1 || ecall_dt > 2184) {
        LOG_ERROR("Inv PTRT");
        test_dt = 1;
    }
    sprintf(data, "#ECALLNWTMR: %d,%d", ecall_dt, test_dt);
    if (strstr(resp, data) == NULL) {
        if (!at_era_sendCommandExpectOk(3000, "AT#ECALLNWTMR=%d,%d\r", ecall_dt, test_dt)) LOG_ERROR("Can't ECALLNWTMR");
        config_restart = TRUE;
    }

    length = at_era_sendCommand(3000, resp, sizeof(resp), "AT#ECALLTMR?\r");
    if (length == 0) {
        LOG_CRITICAL("Can't ECALLTMR");
        return FALSE;
    }
    memset(data, 0, sizeof(data));
    const int al_ack = 5000, sig_dur = 2000, send_msd_per = 5000;
    sprintf(data, "#ECALLTMR: %d,%d,%d,%d,%d", al_ack, sig_dur, send_msd_per, get_msd_max_transm_time(), get_ccft());
    if (strstr(resp, data) == NULL) {
        BOOLEAN ok = at_era_sendCommandExpectOk(3000, "AT#ECALLTMR=%d,%d,%d,%d,%d\r", al_ack, sig_dur, send_msd_per, get_msd_max_transm_time(), get_ccft());
        if (ok) {
            length = at_era_sendCommand(3000, resp, sizeof(resp), "AT#ECALLTMR?\r");
            if (length == 0 || strstr(resp, data) == NULL) {
                LOG_CRITICAL("Can't ECALLTMR");
                return FALSE;
            }
        } else {
            LOG_CRITICAL("Can't set ECALLTMR");
            return FALSE;
        }
    }

    get_SIM_profile();// if (get_SIM_profile() != ERA_PROFILE) send_to_sim_task(SIM_PROFILE_TO_ERA, (INT32) &sim_result_cb, 0);

    set_audio_files_failure(checkAudioFiles() ? F_INACTIVE : F_ACTIVE);

    if (config_restart) {
        restart(3*1000);
        return FALSE;
    }

    subscribe_for_urcs();

    length = at_era_sendCommand(3000, resp, sizeof(resp), "AT+GSN=1\r");
    if (length < 17) LOG_ERROR("Can't AT+GSN");
    else {
        char *p = strstr(resp, "+GSN: ");
        if (p != NULL) set_imei(p + 6);
    }

    // конфигурирование СМС сервиса
    if (!at_era_sendCommandExpectOk(2000, "AT+CSMS=1\r")) LOG_ERROR("Can't AT+CSMS");
    if (!at_era_sendCommandExpectOk(2000, "AT+CMGF=0\r")) LOG_ERROR("Can't AT+CMGF");
    if (!init_sms_handler()) to_log_error_uart("Error init sms");

    if (!init_MSD(sizeof(T_msd))) to_log_error_uart("Can't init MSD storage");

    return TRUE;
}

T_msd create_msd(T_MSDFlag flag, ECALL_TASK_EVENT event, UINT8 ecallType, UINT8 id, UINT8 memorized) {
    UINT8 automatic =
            event == ECALL_AIRBAGS ||
            event == ECALL_FRONT ||
            event == ECALL_LEFT ||
            event == ECALL_RIGHT ||
            event == ECALL_REAR ||
            event == ECALL_ROLLOVER ||
            event == ECALL_SIDE ||
            event == ECALL_FRONT_OR_SIDE ||
            event == ECALL_ANOTHER;
    // LOG_DEBUG("AUTOMATIC %02X", automatic);
    // Ручной 0х00
    // Автоматический 0x01
    if (event == ECALL_MANUAL_SOS){
        automatic = 0x00;
    }
    UINT8 test = event == ECALL_TEST || ecallType != _112;
    T_GnssData gnss_data = get_gnss_data();
    to_log_info_uart("msd coordinates:: latitude:%i, longitude:%i, utc:%i, fix:%i", gnss_data.latitude, gnss_data.longitude, gnss_data.utc, gnss_data.fix);
    to_log_info_uart("is_gnss_req_stopped: %i", is_gnss_req_stopped());
    to_log_info_uart("gnss_data.fix: %i", gnss_data.fix);
    if (is_gnss_resetted()) gnss_data.utc = 0;
//    else if ((is_gnss_req_stopped() || gnss_data.fix == 0) && get_rtc_timestamp() > gnss_data.utc) {
//    	to_log_info_uart("rtc: %i", get_rtc_timestamp());
//    	gnss_data.utc = get_rtc_timestamp();
//    }
	to_log_info_uart("gnss_data.utc: %i", gnss_data.utc);
    T_msd t_msd = msd_fill(flag, automatic, test, id, memorized, gnss_data);
    return t_msd;
}

BOOLEAN send_msd_to_modem(T_msd *msd) {
    UINT8 calling_buffer_hex[MSD_LENGTH];
    char calling_buffer_char[MSD_LENGTH * 2];
    UINT8 num = asn1_encode_msd(msd, calling_buffer_hex);
    if ( num == 255 ) {
        to_log_error_uart("msd no encode");
        return FALSE;
    }

    num = (UINT8)auxiliary_hex_to_char(calling_buffer_hex, num, calling_buffer_char, MSD_LENGTH * 2);

    if ( num == 255 ) {
        to_log_error_uart("msd no convert");
        return FALSE;
    }
    //print_hex("ASN1_MSD_", (char *)calling_buffer_hex, num / 2);
    //char *msd_test = "022418040400000000000000000000000007F0200000001FFFFFFFFFFFFFFFFFF00401004000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    //BOOLEAN ok = set_MSD(msd_test);
    BOOLEAN ok = set_MSD(calling_buffer_char);
    if (!ok) to_log_error_uart("can't set MSD: %s", calling_buffer_char);
    return ok;// bsp_gsm_set_msd(calling_buffer_char, num);
}

void answer_call(void) {
    to_log_info_uart("Answer_call");
    //on_mute();
    char resp[100];
    at_era_sendCommand(0, resp, sizeof(resp), "ATA\r"); // ответа не будет
}

BOOLEAN cancel_call(void) {
    to_log_info_uart("ATH");
    BOOLEAN result = at_era_sendCommandExpectOk(2*1000, "ATH\r");
    if (!result) LOG_WARN("Can't cancel CALL");
    return result;
}

//BOOLEAN deregistration(void) {
//	if (get_SIM_profile() == ERA_PROFILE && is_registered()) {
//		to_log_info_uart("try deregistration..");
//		if (at_era_sendCommandExpectOk(10 * 1000, "AT+COPS=2\r")) {
//			to_log_info_uart("It's deregistered");
//			return TRUE;
//		} else {
//			to_log_error_uart("Deregistration error");
//			return FALSE;
//		}
//	} else return TRUE;
//}

// отключение СИМ вызывает дерегистрацию
void deregistration(void) {
    if (!at_era_sendCommandExpectOk(2000, "AT#SIMDET=0\r")) {
        m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1*1000));
        if (!at_era_sendCommandExpectOk(2000, "AT#SIMDET=0\r")) LOG_ERROR("Can't SIMDET=0");
    }
    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1*1000));
    if (!at_era_sendCommandExpectOk(2000, "AT#SIMDET=1\r")) {
        m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1*1000));
        if (!at_era_sendCommandExpectOk(2000, "AT#SIMDET=1\r")) LOG_ERROR("Can't SIMDET=1");
    }
}

//BOOLEAN try_registration(void) {
//    to_log_info_uart("Trying registration..");
//	BOOLEAN ok = at_era_sendCommandExpectOk(10 * 1000, "AT+COPS=0\r");
//    if (!ok) to_log_error_uart("Can't AT+COPS=0");
//    return ok;
//}

UINT8 get_reg_status(void) {
    return reg_status;
}

UINT8 get_reg_status_AT(void) {
    char resp[100];
    UINT8 status = 4;
    INT32 n = ati_sendCommandEx(6000, resp, sizeof(resp), "AT+CREG?\r");
    if (n <= 0) {
        to_log_error_uart("Can't <AT+CREG?>");
    } else {
        char *reg = strrchr(resp,',');
        if (reg != NULL && strstr(resp, "+CREG: ")) {
            status = (*++reg) - 0x30;
        }
    }
    to_log_info_uart("Reg status: %i (%s)", status, get_reg_st_descr(status));
    reg_status = status;
    return status;
}

BOOLEAN is_registered_AT(void) {
    UINT8 s = get_reg_status_AT();
    return s == 1 || s == 5;
}

void generateDTMF(UINT8 number) {
    if (at_era_sendCommandExpectOk(3000, "AT+VTS=%d,10\r", number)) LOG_INFO("DTMF %c was generated", number);
    else to_log_error_uart("Can't DTMF %c", number);
}

BOOLEAN is_ecall_to_112(void) {
    char str[50];
    if (!get_ecall_to(str, sizeof(str))) {
        to_log_error_uart("Can't get ecall to");
        return TRUE;
    }
    if (strstr(str, ECALL_TO_DEBUG) == NULL) return TRUE;
    else return FALSE;
}

char *get_reg_st_descr(UINT8 reg_status) {
    switch(reg_status) {
        case 0: return "not registered, not searching";
        case 1: return "registered, home network";
        case 2: return "not registered, but searching";
        case 3: return "registration denied";
        case 4: return "unknown";
        case 5: return "registered, roaming";
        default: return "unknown status";
    }
}

void restart_from_reason(ECALL_TASK_EVENT event) {
    to_log_info_uart("Restart, event: %s", get_ecall_event_descr(event));
    if (get_era_restart() < MAX_RESTART_NUMBER) set_era_restart(get_era_restart() + 1);
    else {
        LOG_CRITICAL("No more attempts to restart");
        return;
    }
    set_restart_reason(event);
    restart(1000);
}

CEER_CALL_STATUS get_CEER_status(void) {
    char msg[300];
    int length = at_era_sendCommand(3000, msg, sizeof(msg), "AT+CEER\r");
    if (length < 10) {
        to_log_error_uart("Can't AT+CEER");
        return CEER_UNKNOWN_STATUS;
    } else {
        to_log_info_uart("CEER:%s", msg);
        char *s = strstr(msg, "\r\n+CEER: "), *e;
        if (s != NULL && s == msg && ((e = strstr(s + 2, "\r\n")) != NULL) ) {
        	char *ceer2 = (msg + length > e + 2) && strstr(e + 2, "\r\n")  ? e + 2 : NULL;
        	s += 2;
            *e = 0;
            to_log_info_uart("CEER1:%s", s);
            if 		(strstr(s, "Normal call clearing") != NULL) 		return CEER_NORMAL_CALL_CLEARING;
            else if	(strstr(s, "al call clear") != NULL) 				return CEER_NORMAL_CALL_CLEARING;
            else if	(strstr(s, "Normal call cl") != NULL) 				return CEER_NORMAL_CALL_CLEARING;

                // такое сообщение может иногда появляться, когда оператор положил трубку
            else if (ceer2 && (strstr(s, "Low level failure") || strstr(s, "no redial allowed"))
            			   && (strstr(ceer2, "Normal, unspecified")
            					   || strstr(ceer2, "al, unspeci")
								   || strstr(ceer2, "Normal, unspe"))) 	return CEER_NORMAL_UNSPECIFIED;
            else if	(strstr(s, "Normal, unspecified") != NULL) 			return CEER_NORMAL_UNSPECIFIED;
            else if	(strstr(s, "al, unspeci") != NULL) 					return CEER_NORMAL_UNSPECIFIED;
            else if	(strstr(s, "Normal, unspe") != NULL) 				return CEER_NORMAL_UNSPECIFIED;

                // такое сообщение возникает, если связь была прервана, например, глушилкой GSM
            else if (strstr(s, "Interworking, unspecified") != NULL)	return CEER_INTERWORKING_UNSPECIFIED;
            else if (strstr(s, "Interworking,") != NULL)				return CEER_INTERWORKING_UNSPECIFIED;
            else if (strstr(s, "rking, unspec") != NULL)				return CEER_INTERWORKING_UNSPECIFIED;
                // такое сообщение возникает, когда связь прерывается при изменении BCH band
            else if (strstr(s, "Invalid/incomplete number") != NULL)	return CEER_INTERWORKING_UNSPECIFIED;
            else if (strstr(s, "Invalid/incom") != NULL)				return CEER_INTERWORKING_UNSPECIFIED;
            else if (strstr(s, "d/incomplete num") != NULL)				return CEER_INTERWORKING_UNSPECIFIED;

            else if (strstr(s, "MM no service") != NULL) 				return CEER_MM_NO_SERVICE;
            else if (strstr(s, "DISCONNECTED") != NULL) 				return CEER_DISCONNECTED;
            else if (strstr(s, "Implicitly detached") != NULL) 			return CEER_IMPLICITY_DETACHED;
            else if (strstr(s, "Phone is offline") != NULL)				return CEER_PHONE_IS_OFFLINE;
            else {
                LOG_WARN("Unknown ceer: %s", s);
                return CEER_UNKNOWN_STATUS;
            }
        } else return CEER_UNKNOWN_STATUS;
    }
}


BOOLEAN on_mute(void) {
    enAudio(TRUE);
    to_log_info_uart("on_mute");
    if (on_codec() != ERR_CODEC_NO_ERROR) {
        LOG_WARN("Can't config codec");
        return FALSE;
    }
    return TRUE;
}

BOOLEAN is_T324X_running(void) {
    char resp[100];
    INT32 n = ati_sendCommandEx(6000, resp, sizeof(resp), "AT#ECONLY?\r");
    if (n <= 0) {
        to_log_error_uart("Can't <AT#ECONLY?>");
        return FALSE;
    } else {
        if (strstr(resp, "#ECONLY: 2,2")) return TRUE;
        else return FALSE;
    }
}

BOOLEAN off_mute(void) {
    enAudio(FALSE);
    to_log_info_uart("off_mute");
//    if (off_codec() != ERR_CODEC_NO_ERROR) { // выключение кодека отключено, чтобы работала диагностика микрофона
//		LOG_ERROR("Can't config codec");
//		return FALSE;
//	}
    return TRUE;
}

BOOLEAN is_one_and_one(void) {
	return get_dial_duration() == 1 || get_int_mem_transm_interval() == 1;
}
