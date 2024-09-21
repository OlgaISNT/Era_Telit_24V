/**
 * @file commands.c
 * @version 1.0.0
 * @date 08.07.2022
 * @author: DKozenkov     
 * @brief Обработка команд для протоколов версий c 1.0
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "azx_uart.h"
#include "tasks.h"
#include "app_cfg.h"
#include "atc_era.h"
#include "msd.h"
#include "msd_storage.h"
#include "sms_manager.h"
#include "codec.h"
#include "params.h"
#include "files.h"
#include "sys_utils.h"
#include "sys_param.h"
#include "m2mb_info.h"
#include "ecall.h"
#include "char_service.h"
#include "at_command.h"
#include "era_helper.h"
#include "commands.h"
#include "factory_param.h"
#include "failures.h"
#include "azx_string_utils.h"
#include "testing.h"
#include "util.h"
#include "sim.h"
#include "log.h"
#include "security_access.h"
/* Local defines ================================================================================*/
#define NO_SECURITY_KEY 0
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static UINT32 security_key = NO_SECURITY_KEY;
static UINT32 init_v = 0, counter = 0;
// текущий "уровень обслуживания" в терминах протокола
static volatile UINT8 s_level = 0;
static UINT8 buf[UART0_COMMAND_BUF_SIZE];	// буфер приёма команды
static volatile UINT32 d_size = 0; 	// кол-во уже принятых байт в буфере
static const char *ANS_RN 		= "\r\n";
static const char *ANS_OK 		= "OK\r\n";
static const char *ANS_ERROR 	= "ERROR\r\n";
static const char *ANS_INVALID 	= "INVALID\r\n";
// Перечень команд
static const char *_LOG_LEVEL 			= "LOG_LEVEL"; 		// установка другого уровня логирования, пример: SET_LOG_LEVEL=1
static const char *_FINISH_CONFIGURATION 	= "FINISH_CONFIGURATION";
static const char *_RESET_TO_FACTORY 	= "RESET_TO_FACTORY";  // Сброс всех параметров к настройкам по умолчанию, стирание VIN и перезагрузка модема
static const char *_CLEAR_FAILURES 		= "CLEAR_FAILURES";
static const char *_SET = "SET_";		// префикс команды установки значения параметра
static const char *_READ = "READ_";		// префикс команды чтения значения параметра
static const char *_SECURITY_ACCESS 	= "SECURITY_ACCESS";
static const char *_SECURITY_KEY 		= "SECURITY_KEY";
static const char *_CONFIG				= "CONFIG";
static const char *_TEMP_CONFIG			= "TEMP_CONFIG";
static const char *_DIAG				= "DIAG";
static const char *_FACTORY_CFG 		= "FACTORY_CFG";
static const char *_ICCIDG 				= "ICCIDG";
static const char *_VERSIONS			= "VERSIONS";
static const char *_HVERAPP				= "HVERAPP"; // чтение фактической версии приложения
static const char *_GNSS_COLD_RESET     = "GNSS_COLD_RESET";
static const char *_ENABLE_AUDIO     	= "ENABLE_AUDIO";
static const char *_DISABLE_AUDIO     	= "DISABLE_AUDIO";

static const char *_CLEAR_REL_COORD = "CLEAR_REL_COORD"; // удаление из файла последних известных достоверных координат
static const char *_ERA_M_CALL 		= "ERA_M_CALL"; 	  	  // запуск ручного ECALL
static const char *_ERA_A_CALL 		= "ERA_A_CALL"; 	  	  // запуск автоматического ECALL
static const char *_ERA_T_CALL 		= "ERA_T_CALL"; 	  	  // запуск процедуры тестирования
static const char *_ERA_T_CALL_ONLY = "ERA_T_CALL_ONLY";   	  // запуск только тестового вызова ECALL
static const char *_ERA_T_MD_ONLY   = "ERA_T_MD_ONLY";   	  // запуск только теста микрофона и динамика
static const char *_CANCEL_CALL 	= "CANCEL_CALL"; 	  	  // отмена любого вызова и удаление всех МНД из памяти

//static const char *_RECORD_SOUND = "RECORD_SOUND";  // запись звука с микрофона в течение 5 сек с последующим воспроизведением
//static const char *_NEW_MSD		 = "NEW_MSD";  // для тестов
//static const char *_GET_MSD2	 = "GET_MSD2";  // для тестов
//static const char *_DELETE_ALL_MSD	 = "DELETE_ALL_MSD";  // для тестов
//static const char *_SEND_SMS	 = "SEND_SMS";	// для тестов
//static const char *_SIM_TO_ERA 		= "SIM_TO_ERA"; 	 // переключение на профиль ЭРА
//static const char *_SIM_TO_COMM 	= "SIM_TO_COMM"; 	  // переключение на коммерческий профиль
//static const char *_PRESS_SOS 		= "PRESS_SOS"; // имитация кратковременного нажатия кнопки SOS
/* Local function prototypes ====================================================================*/
static void clear_buf(void);
static void print_command(const char *comm);
static void parse_w_params(char *eq, char *end);
static void parse_wo_params(void);
static void read_command(void);
static void send_unknown_command(void);
static void print_inv_int(INT32 var);
static char check_int(const char *param_name, char *eq, char *end, INT32 *var);
static char check_str(const char *param_name, char *eq, char *end, char *arg);
static BOOLEAN check_wp(const char *NAME);
static void config_vin(char *vin);
static void activation(char *data);
static void set_audioprofile(char *data);
static void temp_config(char *data);
static void config_str_param(UINT8 serv_l, char *param_name, char *value, str_param_setter setter);
static void config_int_param(UINT8 serv_l, char *param_name, char *arg, int min, int max, int_param_setter setter);
static void send_int(char *param_name, int value);
static void send_str(char *param_name, get_str_parameter getter);
static void construct(const char *name, const char *value, char *dest);
static void iccidg_result_cb(BOOLEAN success, char *iccidg);
static void send_OK(void);
static void send_ERROR(void);
static BOOLEAN check_service(UINT8 min_service_level);
/* Static functions =============================================================================*/
static void clear_buf(void) {
    //LOG_DEBUG("clear UART0 rec buf");
    memset(buf, 0, UART0_COMMAND_BUF_SIZE);
    d_size = 0;
}

static void send_OK(void) {
    send_str_to_UART0(ANS_OK);
}
static void send_ERROR(void) {
    send_str_to_UART0(ANS_ERROR);
}

static BOOLEAN check_service(UINT8 min_service_level) {
    if (s_level >= min_service_level) return TRUE;
    else {
        send_ERROR();
        return FALSE;
    }
}

static void print_command(const char *comm) {
    LOG_INFO("Command: %s", comm);
}

static void print_inv_int(INT32 var) {
    LOG_WARN("Inv command arg: %i", var);
}

static void parse_w_params(char *eq, char *end) {
    INT32 var = 0;
    char arg[200];
    memset(arg, 0, sizeof(arg));
    if (check_int(_LOG_LEVEL, eq, end, &var)) {
        if (get_int(eq, end, &var)) {
            print_command((char *)buf);
            if (var < AZX_LOG_LEVEL_TRACE || var > AZX_LOG_LEVEL_CRITICAL) {
                print_inv_int(var);
                send_ERROR();
            } else {
                set_log_level(var);
                send_OK();
            }
        }
    }
    else if (check_str(AUDIOPROFILE_POSITION_NAME, eq, end, arg)) 		set_audioprofile(arg);
    else if (check_str(AUDIOPROFILE_SPEED_NAME, eq, end, arg)) 			config_int_param(1, AUDIOPROFILE_SPEED_NAME, arg, AUDIOPROFILE_SPEED_MIN, AUDIOPROFILE_SPEED_MAX, set_audio_prof_speed);
    else if (check_str(CALL_AUTO_ANSWER_TIME_NAME, eq, end, arg)) 		config_int_param(1, CALL_AUTO_ANSWER_TIME_NAME, arg, CALL_AUTO_ANSWER_TIME_MIN, CALL_AUTO_ANSWER_TIME_MAX, set_call_auto_answer_time);
    else if (check_str(CCFT_NAME, eq, end, arg)) 						config_int_param(1, CCFT_NAME, arg, CCFT_MIN, CCFT_MAX, set_ccft);
    else if (check_str(CODEC_OFF_NAME, eq, end, arg)) 					config_str_param(1, CODEC_OFF_NAME, arg, set_codec_off);
    else if (check_str(QAC_NAME, eq, end, arg)) 						config_str_param(1, QAC_NAME, arg, set_qac);
    else if (check_str(NAC_NAME, eq, end, arg)) 						config_str_param(1, NAC_NAME, arg, set_nac);
    else if (check_str(TAC_NAME, eq, end, arg)) 						config_str_param(1, TAC_NAME, arg, set_tac);
    else if (check_str(ECALL_AUTO_DIAL_ATTEMPTS_NAME, eq, end, arg)) 	config_int_param(1, ECALL_AUTO_DIAL_ATTEMPTS_NAME, arg, ECALL_AUTO_DIAL_ATTEMPTS_MIN, ECALL_AUTO_DIAL_ATTEMPTS_MAX, set_auto_dial_attempts);
    else if (check_str(ECALL_MANUAL_DIAL_ATTEMPTS_NAME, eq, end, arg)) 	config_int_param(1, ECALL_MANUAL_DIAL_ATTEMPTS_NAME, arg, ECALL_MANUAL_DIAL_ATTEMPTS_MIN, ECALL_MANUAL_DIAL_ATTEMPTS_MAX, set_manual_dial_attempts);
    else if (check_str(ECALL_DIAL_DURATION_NAME, eq, end, arg)) 		config_int_param(1, ECALL_DIAL_DURATION_NAME, arg, ECALL_DIAL_DURATION_MIN, ECALL_DIAL_DURATION_MAX, set_dial_duration);
    else if (check_str(ECALL_MANUAL_CAN_CANCEL_NAME, eq, end, arg)) 	config_int_param(1, ECALL_MANUAL_CAN_CANCEL_NAME, arg, ECALL_MANUAL_CAN_CANCEL_MIN, ECALL_MANUAL_CAN_CANCEL_MAX, set_manual_can_cancel);
    else if (check_str(ECALL_MSD_MAX_TRANSMISSION_TIME_NAME, eq, end, arg)) config_int_param(1, ECALL_MSD_MAX_TRANSMISSION_TIME_NAME, arg, ECALL_MSD_MAX_TRANSMISSION_TIME_MIN, ECALL_MSD_MAX_TRANSMISSION_TIME_MAX, set_msd_max_transm_time);
    else if (check_str(ECALL_NAD_DEREGISTRATION_TIME_NAME, eq, end, arg)) config_int_param(1, ECALL_NAD_DEREGISTRATION_TIME_NAME, arg, ECALL_NAD_DEREGISTRATION_TIME_MIN, ECALL_NAD_DEREGISTRATION_TIME_MAX, set_nad_dereg_time);
    else if (check_str(POST_TEST_REGISTRATION_TIME_NAME, eq, end, arg)) config_int_param(1, POST_TEST_REGISTRATION_TIME_NAME, arg, POST_TEST_REGISTRATION_TIME_MIN, POST_TEST_REGISTRATION_TIME_MAX, set_post_test_reg_time);
    else if (check_str(ECALL_NO_AUTOMATIC_TRIGGERING_NAME, eq, end, arg)) config_int_param(1, ECALL_NO_AUTOMATIC_TRIGGERING_NAME, arg, ECALL_NO_AUTOMATIC_TRIGGERING_MIN, ECALL_NO_AUTOMATIC_TRIGGERING_MAX, set_no_auto_trig);
    else if (check_str(INT_MEM_TRANSMIT_ATTEMPTS_NAME, eq, end, arg)) 	config_int_param(1, INT_MEM_TRANSMIT_ATTEMPTS_NAME, arg, INT_MEM_TRANSMIT_ATTEMPTS_MIN, INT_MEM_TRANSMIT_ATTEMPTS_MAX, set_int_mem_transm_attempts);
    else if (check_str(INT_MEM_TRANSMIT_INTERVAL_NAME, eq, end, arg)) 	config_int_param(1, INT_MEM_TRANSMIT_INTERVAL_NAME, arg, INT_MEM_TRANSMIT_INTERVAL_MIN, INT_MEM_TRANSMIT_INTERVAL_MAX, set_int_mem_transm_interval);
    else if (check_str(PROPULSION_TYPE_NAME, eq, end, arg))				config_int_param(1, PROPULSION_TYPE_NAME, arg, PROPULSION_TYPE_MIN, PROPULSION_TYPE_MAX, set_propulsion_type);
    else if (check_str(MICG_NAME, eq, end, arg))						config_int_param(1, MICG_NAME, arg, MICG_MIN, MICG_MAX, set_micg);
    else if (check_str(SPKG_NAME, eq, end, arg))						config_int_param(1, SPKG_NAME, arg, SPKG_MIN, SPKG_MAX, set_spkg);
    else if (check_str(SMS_CENTER_NUMBER_NAME, eq, end, arg)) 			config_str_param(1, SMS_CENTER_NUMBER_NAME, arg, set_sms_center_number);
    else if (check_str(ECALL_SMS_FALLBACK_NUMBER_NAME, eq, end, arg)) 	config_str_param(1, ECALL_SMS_FALLBACK_NUMBER_NAME, arg, set_sms_fallback_number);
    else if (check_str(SMS_FALLBACK_DEBUG_NUMBER_NAME, eq, end, arg)) 	config_str_param(1, SMS_FALLBACK_DEBUG_NUMBER_NAME, arg, set_sms_fallback_debug_number);
    else if (check_str(ECALL_TEST_NUMBER_NAME, eq, end, arg)) 			config_str_param(1, ECALL_TEST_NUMBER_NAME, arg, set_test_number);
    else if (check_str(ECALL_DEBUG_NUMBER_NAME, eq, end, arg)) 			config_str_param(1, ECALL_DEBUG_NUMBER_NAME, arg, set_debug_number);
    else if (check_str(ECALL_TO_NAME, eq, end, arg))					{
        if (strcmp(ECALL_TO_DEBUG, arg) == 0 || strcmp(ECALL_TO_112, arg) == 0) config_str_param(1, ECALL_TO_NAME, arg, set_ecall_to);
        else send_ERROR();
    }
    else if (check_str(TEST_REGISTRATION_PERIOD_NAME, eq, end, arg))	config_int_param(1, TEST_REGISTRATION_PERIOD_NAME, arg, TEST_REGISTRATION_PERIOD_MIN, TEST_REGISTRATION_PERIOD_MAX, set_test_reg_period);
    else if (check_str(VEHICLE_TYPE_NAME, eq, end, arg)) 				config_int_param(1, VEHICLE_TYPE_NAME, arg, VEHICLE_TYPE_MIN, VEHICLE_TYPE_MAX, set_vehicle_type);
    else if (check_str(VIN_NAME, eq, end, arg))  						config_vin(arg);
    else if (check_str(_CONFIG, eq, end, arg))							activation(arg);
    else if (check_str(_TEMP_CONFIG, eq, end, arg))						temp_config(arg);
    else send_unknown_command();
    clear_buf();
}

static void read_command(void) {
    if (check_wp(_CONFIG)) {
        send_OK();
        char ans[1500];
        get_all_params(ans, sizeof(ans));
        send_str_to_UART0(ans);
    } else if (check_wp(_DIAG)) {
        send_OK();
        send_failures_to_uart0();
        char ans[100];
        concat3("BEG_MODE=", get_mode_descr(get_device_mode()), ANS_RN, ans, sizeof(ans));		send_str_to_UART0(ans);
        T_GnssData gnss_d = get_gnss_data();
        char val[60];
        memset(val, 0, sizeof(val));
        sprintf(val, "%s_%s", (gnss_d.fix > 0 ? gnss_d.latitude_i : "NA"), (gnss_d.fix > 0 ? gnss_d.longitude_i : "NA"));
        concat3("LATITUDE_LONGITUDE=", val, ANS_RN, ans, sizeof(ans)); 							send_str_to_UART0(ans);
        concat3("REG_STATUS=", get_reg_st_descr(get_reg_status()), ANS_RN, ans, sizeof(ans));	send_str_to_UART0(ans);
    } else if (check_wp(_FACTORY_CFG)) {
        send_OK();
        char ans[1500];
        get_factory_params(ans, sizeof(ans));
        send_str_to_UART0(ans);
    } else if (check_wp(_ICCIDG)) {
        send_str_to_UART0(ANS_OK);
        read_iccidg(iccidg_result_cb);
    } else if (check_wp(_HVERAPP)) {
        send_str_to_UART0(ANS_OK);
        char ans[150];
        char ver[50] = {0};
        set_build_time(ver);
        concat3("", ver, ANS_RN, ans, sizeof(ans)); send_str_to_UART0(ans);
    } else if (check_wp(_VERSIONS)) {
        send_str_to_UART0(ANS_OK);
        char ans[150];

        concat3("APP_VERSION=", "7.2", ANS_RN, ans, sizeof(ans));	send_str_to_UART0(ans);
        char ver[50] = {0};
        set_build_time(ver);
        concat3("ID=", ver, ANS_RN, ans, sizeof(ans)); send_str_to_UART0(ans);
        //send_str_to_UART0("ID=1407221230\r\n");
//			char data[128];
//			memset(data, 0, sizeof(data));
//			sprintf(data, "%s, %s", __DATE__, __TIME__);
//			concat3("APP_DATE_TIME=", data, ANS_RN, ans, sizeof(ans));	send_str_to_UART0(ans);

        char *i_data;
        M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
        M2MB_INFO_HANDLE h;
        retVal = m2mb_info_init(&h);
        retVal = m2mb_info_get(h, M2MB_INFO_GET_SW_VERSION, &i_data);
        if ( retVal == M2MB_RESULT_SUCCESS )  {
            concat3("FW_VERSION=", i_data, ANS_RN, ans, sizeof(ans));	send_str_to_UART0(ans);
        } else LOG_DEBUG("Inv get FWV");
        m2mb_info_deinit(h);
    }
    else if (check_wp(AUDIOPROFILE_POSITION_NAME)) 		send_int(AUDIOPROFILE_POSITION_NAME, get_audio_prof_pos());
    else if (check_wp(AUDIOPROFILE_SPEED_NAME)) 		send_int(AUDIOPROFILE_SPEED_NAME, get_audio_prof_speed());
    else if (check_wp(CALL_AUTO_ANSWER_TIME_NAME)) 		send_int(CALL_AUTO_ANSWER_TIME_NAME, get_call_auto_answer_time());
    else if (check_wp(CCFT_NAME)) 						send_int(CCFT_NAME, get_ccft());
    else if (check_wp(CODEC_OFF_NAME)) 					send_str(CODEC_OFF_NAME, get_codec_off);
    else if (check_wp(QAC_NAME)) 						send_str(QAC_NAME, get_qac);
    else if (check_wp(NAC_NAME)) 						send_str(NAC_NAME, get_qac);
    else if (check_wp(TAC_NAME)) 						send_str(TAC_NAME, get_qac);
    else if (check_wp(ECALL_AUTO_DIAL_ATTEMPTS_NAME)) 	send_int(ECALL_AUTO_DIAL_ATTEMPTS_NAME, get_auto_dial_attempts());
    else if (check_wp(ECALL_MANUAL_DIAL_ATTEMPTS_NAME)) send_int(ECALL_MANUAL_DIAL_ATTEMPTS_NAME, get_manual_dial_attempts());
    else if (check_wp(ECALL_DIAL_DURATION_NAME)) 		send_int(ECALL_DIAL_DURATION_NAME, get_dial_duration());
    else if (check_wp(ECALL_MANUAL_CAN_CANCEL_NAME)) 	send_int(ECALL_MANUAL_CAN_CANCEL_NAME, get_manual_can_cancel());
    else if (check_wp(ECALL_MSD_MAX_TRANSMISSION_TIME_NAME)) send_int(ECALL_MSD_MAX_TRANSMISSION_TIME_NAME, get_msd_max_transm_time());
    else if (check_wp(ECALL_NAD_DEREGISTRATION_TIME_NAME)) send_int(ECALL_NAD_DEREGISTRATION_TIME_NAME, get_nad_dereg_time());
    else if (check_wp(POST_TEST_REGISTRATION_TIME_NAME)) send_int(POST_TEST_REGISTRATION_TIME_NAME, get_post_test_reg_time());
    else if (check_wp(ECALL_NO_AUTOMATIC_TRIGGERING_NAME)) send_int(ECALL_NO_AUTOMATIC_TRIGGERING_NAME, get_no_auto_trig());
    else if (check_wp(INT_MEM_TRANSMIT_ATTEMPTS_NAME)) 	send_int(INT_MEM_TRANSMIT_ATTEMPTS_NAME, get_int_mem_transm_attempts());
    else if (check_wp(INT_MEM_TRANSMIT_INTERVAL_NAME)) 	send_int(INT_MEM_TRANSMIT_INTERVAL_NAME, get_int_mem_transm_interval());
    else if (check_wp(PROPULSION_TYPE_NAME)) 			send_int(PROPULSION_TYPE_NAME, get_propulsion_type());
    else if (check_wp(MICG_NAME)) 						send_int(MICG_NAME, get_micg());
    else if (check_wp(SPKG_NAME)) 						send_int(SPKG_NAME, get_spkg());
    else if (check_wp(SMS_CENTER_NUMBER_NAME)) 			send_str(SMS_CENTER_NUMBER_NAME, get_sms_center_number);
    else if (check_wp(ECALL_SMS_FALLBACK_NUMBER_NAME)) 	send_str(ECALL_SMS_FALLBACK_NUMBER_NAME, get_sms_fallback_number);
    else if (check_wp(SMS_FALLBACK_DEBUG_NUMBER_NAME)) 	send_str(SMS_FALLBACK_DEBUG_NUMBER_NAME, get_sms_fallback_debug_number);
    else if (check_wp(ECALL_TEST_NUMBER_NAME)) 			send_str(ECALL_TEST_NUMBER_NAME, get_test_number);
    else if (check_wp(ECALL_DEBUG_NUMBER_NAME))			send_str(ECALL_DEBUG_NUMBER_NAME, get_debug_number);
    else if (check_wp(ECALL_TO_NAME))					send_str(ECALL_TO_NAME, get_ecall_to);
    else if (check_wp(TEST_REGISTRATION_PERIOD_NAME)) 	send_int(TEST_REGISTRATION_PERIOD_NAME, get_test_reg_period());
    else if (check_wp(VEHICLE_TYPE_NAME)) 				send_int(VEHICLE_TYPE_NAME, get_vehicle_type());
    else if (check_wp(VIN_NAME))						send_str(VIN_NAME, get_vin);
    else send_unknown_command();
}

static void config_int_param(UINT8 serv_l, char *param_name, char *arg, int min, int max, int_param_setter setter) {
    if (check_service(serv_l)) {
        LOG_INFO("Trying to set %s to: %s", param_name, arg);
        INT32 r = 0;
        if (azx_str_to_l(arg, &r) == 0 && r >= min && r <= max) {
            BOOLEAN ok = setter(r);
            send_str_to_UART0(ok ? ANS_OK : ANS_ERROR);
        } else {
            LOG_WARN("%s inv arg", param_name);
            send_ERROR();
        }
    }
}

static void config_str_param(UINT8 serv_l, char *param_name, char *value, str_param_setter setter) {
    if (check_service(serv_l)) {
        LOG_INFO("Trying to set %s to: %s", param_name, value);
        if (strlen(value) < 100) {
            BOOLEAN ok = setter(value);
            send_str_to_UART0(ok ? ANS_OK : ANS_ERROR);
        } else {
            LOG_WARN("%s inv arg", param_name);
            send_ERROR();
        }
    }
}

static char check_int(const char *param_name, char *eq, char *end, INT32 *var) {
    char ok = strstr((char *)buf, param_name) != NULL && get_int(eq, end, var);
    if (ok) print_command((char *)buf);
    return ok;
}

static char check_str(const char *param_name, char *eq, char *end, char *arg) {
    char *p = (char *)buf + sizeof(_SET); 		//LOG_INFO("p:%i", p);
    char *f = strstr((char *)buf, param_name);	//LOG_INFO("f:%i", f);
    char ok = f != NULL && p == f && get_str(eq, end, arg);
    if (ok) print_command((char *)buf);
    return ok;
}

static BOOLEAN check_wp(const char *NAME) {
    BOOLEAN ok = strstr((char *)buf, NAME) != NULL;
    return ok;
}

static void parse_wo_params(void) {
    char *s = strstr((char *)buf, _READ);
    if (s != NULL && s == (char *)buf) read_command();
    else if (check_wp(_FINISH_CONFIGURATION)) {
        if (check_service(1)) {
            set_service_level(0);
            send_OK();
        }
    }
    else if (check_wp(_RESET_TO_FACTORY)) {
        if (check_service(1)) {
            send_OK();
            if (!delete_file(LOCALPATH_S ERA_PARAMS_FILE_NAME)) LOG_WARN("Can't delete %s", ERA_PARAMS_FILE_NAME);
            if (!delete_file(LOCALPATH_S SYS_PARAMS_FILE_NAME)) LOG_WARN("Can't delete %s", SYS_PARAMS_FILE_NAME);
            char *res = multConc(4, LOCALPATH, "/", RES_POSTFIX, ERA_PARAMS_FILE_NAME);
            if (!delete_file(res)) LOG_WARN("Can't delete _cfg");
            free(res);
            delete_all_MSDs();
            restart(1*1000);
        }
    } else if (check_wp(_GNSS_COLD_RESET)) {
        send_OK();
        send_to_gnss_task(RESET_GNSS, 1, 0);
    } else if (check_wp(_ENABLE_AUDIO)) {
        if (on_mute()) send_OK();
        else send_ERROR();
    } else if (check_wp(_DISABLE_AUDIO)) {
        if (off_mute()) send_OK();
        else send_ERROR();
    } else if (check_wp(_ERA_M_CALL)) {
        send_OK();
        send_to_ecall_task(ECALL_MANUAL_SOS, ECALL_NO_REPEAT_REASON, 0);
    } else if (check_wp(_ERA_A_CALL)) {
        send_OK();
        send_to_ecall_task(ECALL_FRONT, ECALL_NO_REPEAT_REASON, 0);
    } else if (check_wp(_ERA_T_MD_ONLY)) {
        if (is_testing_working()) send_ERROR();
        else {
            send_OK();
            send_to_testing_task(START_TEST_MD_ONLY, 0, 0);
        }
    } else if (check_wp(_ERA_T_CALL_ONLY)) {
        if (is_testing_working()) send_ERROR();
        else {
            send_OK();
            send_to_testing_task(START_TEST_CALL_ONLY, 0, 0);
        }
    } else if (check_wp(_ERA_T_CALL)) {
        if (is_testing_working()) send_ERROR();
        else {
            send_OK();
            send_to_testing_task(START_TESTING, 0, 0);
        }
    } else if (check_wp(_CANCEL_CALL)) {
        if (is_testing_working()) send_to_testing_task(TEST_FORCED_STOP, 0, 0);
        else if (is_ecall_in_progress()) {
            send_to_ecall_task(ECALL_FORCED_STOP, 0, 0);
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(3000));
        }
        //deregistration();
        delete_all_MSDs();
        send_OK();
        restart(2000);
    } else if (check_wp(_CLEAR_FAILURES)) {
        clear_failures(); send_OK();
    } else if (check_wp(_CLEAR_REL_COORD)) {
        send_to_gnss_task(CLEAR_LAST_RELIABLE_COORD, 0, 0);
        send_OK();
//	} else if (check_wp(_RECORD_SOUND)) {
//		send_OK();
//		LOG_DEBUG("Sound recording started for 5 sec..");
//		char *a_file = "test_aud.wav";
//		record_audio(a_file, 5000);
//		play_audio(a_file);
//
//	} else if (check_wp(_NEW_MSD)) {
//		send_OK();
//		T_msd t_msd = create_msd(MSD_INBAND, ECALL_MANUAL_SOS, (UINT8)ECALL_TEST, (UINT8)1, (UINT8)0);
//		print_hex("MSD1", (char *) (&t_msd), sizeof(t_msd));
//		save_MSD(&t_msd);


/*
		t_msd = create_msd(MSD_SMS, ECALL_MANUAL_SOS, (UINT8)ECALL_TEST, (UINT8)1, (UINT8)0);
		print_hex("MSD2", (char *) (&t_msd), sizeof(t_msd));
		save_MSD(&t_msd);

		t_msd = create_msd(MSD_SAVED, ECALL_MANUAL_SOS, (UINT8)ECALL_TEST, (UINT8)1, (UINT8)0);
		print_hex("MSD3", (char *) (&t_msd), sizeof(t_msd));
		save_MSD(&t_msd);

		LOG_DEBUG("find msd saved: %i", find_msd_saved(&t_msd));
		print_hex("MSD_saved", (char *) (&t_msd), sizeof(t_msd));

		LOG_DEBUG("find msd sms: %i", find_msd_sms(&t_msd));
		print_hex("MSD_SMS", (char *) (&t_msd), sizeof(t_msd));
*/
        //LOG_DEBUG("find msd inband: %i", find_msd_inband(&t_msd));
        //print_hex("MSD_inband", (char *) (&t_msd), sizeof(t_msd));

//		LOG_DEBUG(" msd check: %i", msd_check(&t_msd));
//		move_inband_to_sms();
//
//		LOG_DEBUG("find msd sms: %i", find_msd_sms(&t_msd));
//		print_hex("MSD_SMS", (char *) (&t_msd), sizeof(t_msd));
//		LOG_DEBUG(" msd check: %i", msd_check(&t_msd));

//	} else if (check_wp(_GET_MSD2)) {
//		send_OK();
//		T_msd t_msd;
//		if (read_MSD(2, &t_msd)) print_hex("READ_MSD2", (char *) (&t_msd), sizeof(t_msd));
//	} else if (check_wp(_DELETE_ALL_MSD)) {
//		send_OK();
//		delete_all_MSDs();
//	} else if (check_wp(_SEND_SMS)) {
//		send_OK();
//		send_to_sms_task(SMS_SEND_ALL_FROM_STORAGE, 0, 0);
//	} else if (check_wp(_PRESS_SOS)) {
//		send_to_ecall_task(ECALL_EVENT_SOS_PRESSED, ECALL_NO_REPEAT_REASON, 0);
//		send_to_era_action_task(ERA_A_SOS_BUTTON_PRESSED, 0, 0);
//		send_to_testing_task(TEST_CONFIRMATION_DONE, 0, 0);
//	} else if (check_wp(_SIM_TO_ERA)) {
//		send_OK();
//		send_to_sim_task(SIM_PROFILE_TO_ERA, 0, 0);
//	} else if (check_wp(_SIM_TO_COMM)) {
//		send_OK();
//		send_to_sim_task(SIM_PROFILE_TO_COMM, 0, 0);
    } else send_unknown_command();
    clear_buf();
}

static void send_int(char *param_name, int value) {
    char val[12];
    sprintf(val, "%d", value);
    char ans[60] = {0};
    send_OK();
    construct(param_name, val, ans);
    send_str_to_UART0(ans);
}

static void send_str(char *param_name, get_str_parameter getter) {
    char ans[120] = {0};
    char val[100] = {0};
    if (getter(val, sizeof(val))) {
        send_OK();
        construct(param_name, val, ans);
        send_str_to_UART0(ans);
    } else send_ERROR();
}

static void construct(const char *name, const char *value, char *dest) {
    strcat(dest, name);
    strcat(dest, "=");
    strcat(dest, value);
    strcat(dest, ANS_RN);
}

static void send_unknown_command(void) {
    LOG_WARN("Unknown command: %s", buf);
    send_str_to_UART0(ANS_INVALID);
}

static void iccidg_result_cb(BOOLEAN success, char *iccidg) {
    if (success) {
        char ans[100];
        concat3("ICCIDG=", iccidg, ANS_RN, ans, sizeof(ans));
        send_str_to_UART0(ans);
    } else {
        send_ERROR();
    }
}

static void config_vin(char *vin) {
    if (check_service(1)) {
        LOG_INFO("Trying to set VIN: %s", vin);
        char str[50];
        get_vin(str, sizeof(str));
        if ((strlen(str) == 17 && strstr(str, VIN_DEFAULT) == NULL) || strstr(vin, VIN_DEFAULT) != NULL) {
            send_ERROR();
            LOG_WARN("VIN already set");
            return;
        }
        if (strlen(vin) != 17) {
            send_ERROR();
            LOG_WARN("VIN length != 17");
        } else {
            BOOLEAN ok = set_vin(vin);
            LOG_INFO("VIN set to %s", vin);
            send_str_to_UART0(ok ? ANS_OK : ANS_ERROR);
            if (ok) {
                set_service_level(0);
                set_activation_flag(TRUE);
            }
        }
    }
}

static BOOLEAN parse_activation_data(char *d) {
    if (strlen(d) < 17 + 1 + 1 + 1 + 1) return FALSE;
    //LOG_INFO("d:%X", d);
    char *e = strchr(d, '_'); //LOG_INFO("e:%X", e);
    if (e == NULL || e - d != 17) return FALSE;
    char vin[18];
    memset(vin, 0, sizeof(vin));
    memcpy(vin, d, 17);
    //LOG_INFO("vin:%s", vin);
    e++;
    char *e2 = strchr(e, '_'); 	//LOG_INFO("e2:%X", e2);
    if (e2 == NULL || (e2 - e < 1) || (e2 - e > 2)) return FALSE;
    char type[10];
    memset(type, 0, sizeof(type));
    memcpy(type, e, e2 - e);		//LOG_INFO("type:%s", type);
    INT32 veh_type;
    if (azx_str_to_l(type, &veh_type) != 0  || veh_type < VEHICLE_TYPE_MIN || veh_type > VEHICLE_TYPE_MAX) return FALSE;
    //LOG_INFO("veh_type:%i", veh_type);
    e2++;
    int len = strlen(e2);
    if (len == 0 || len > 3) return FALSE;
    char prop[len + 1];
    strcpy(prop, e2);				//LOG_INFO("prop:%s", prop);
    INT32 prop_type;
    if (azx_str_to_l(prop, &prop_type) != 0 || prop_type < PROPULSION_TYPE_MIN || prop_type > PROPULSION_TYPE_MAX) return FALSE;
    //LOG_INFO("prop_type:%i", prop_type);

    if (set_vin(vin)) LOG_INFO("VIN to %s", vin);
    else {
        LOG_ERROR("Can't set VIN");
        return FALSE;
    }
    if (set_vehicle_type(veh_type)) LOG_INFO("Veh_type to %i", veh_type);
    else {
        LOG_ERROR("Can't set veh type");
        return FALSE;
    }
    if (set_propulsion_type(prop_type)) LOG_INFO("Prop_type to %i", prop_type);
    else {
        LOG_ERROR("Can't set prop type");
        return FALSE;
    }
    return TRUE;
}

static void activation(char *data) {
    if (check_service(1)) {
        LOG_INFO("act_data:%s", data);
        if (parse_activation_data(data)) {
            set_service_level(0);
            set_activation_flag(TRUE);
            send_OK();
        } else send_ERROR();
    }
}

static void set_audioprofile(char *data) {
    if (check_service(1)) {
        UINT8 p_pos = get_audio_prof_pos();
        config_int_param(1, AUDIOPROFILE_POSITION_NAME, data, AUDIOPROFILE_POSITION_MIN, AUDIOPROFILE_POSITION_MAX, set_audio_prof_pos);
        UINT8 pos = get_audio_prof_pos();
        if (pos != p_pos) {
            LOG_INFO("Change audioprofile to %i", pos);
            PropManager *era_p;
            era_p = get_era_p();
            setString(era_p, NAC_NAME, is20dB() ? NAC_DEFAULT_20dB : NAC_DEFAULT, FALSE);
            setString(era_p, QAC_NAME, is20dB() ? QAC_DEFAULT_20dB : QAC_DEFAULT, FALSE);
            setString(era_p, TAC_NAME, is20dB() ? TAC_DEFAULT_20dB : TAC_DEFAULT, TRUE);
            if (!at_era_sendCommandExpectOk(3000, "AT#ACDBEXT=2,%d\r", pos)) LOG_ERROR("Can't change audioprofile");
            else {
                LOG_INFO("Audioprofile changed to %i", pos);
                restart(3000);
            }
        }
    }
}

static void temp_config(char *data) {
    if (check_service(1)) {
        LOG_INFO("temp_data:%s", data);
        if (parse_activation_data(data)) send_OK();
        else send_ERROR();
    }
}

static void security_access_handler(char *eq, char *end) {
    INT32 var = 0;
    if (get_int(eq, end, &var)) {
        print_command((char *)buf);
        if (var < 0 || var > 255) {
            print_inv_int(var);
            send_ERROR();
        } else {
            send_OK();
            DWORD seed = (DWORD)rnd_generate();
            LOG_INFO("seed:%lu", seed);
            security_key = SA_CalculateKey(var, seed, get_mask());
            //LOG_INFO("key:%lu", security_key);
            char ans[100];
            char seed_str[50] = {0};
            snprintf(seed_str, sizeof(seed_str), "%lu", seed);
            concat3("SEED=", seed_str, ANS_RN, ans, sizeof(ans));
            send_str_to_UART0(ans);
        }
    } else send_str_to_UART0(ANS_INVALID);
}

static void security_key_handler(char *eq, char *end) {
    UINT32 var = 0;
    if (get_uint(eq, end, &var)) {
        print_command((char *)buf);
        if (security_key != NO_SECURITY_KEY && security_key == var) {
            send_OK();
            set_service_level(1);
        } else send_ERROR();
        security_key = NO_SECURITY_KEY;
    } else send_str_to_UART0(ANS_INVALID);
}

/* Global functions =============================================================================*/
void command_handler(UINT8 *data, UINT32 size) {
    UINT32 shift = 0;
    for (UINT32 i = 0; i < size; i++) { // исключение из входного массива принятых данных байт, содержащих '0' и байт, предшествовавших ему
        if (data[i] == 0) shift = i + 1;
    }
    data += shift;
    size -= shift;
    if (size <= 0) return;
    if (UART0_COMMAND_BUF_SIZE - d_size - 2 < size) {
        LOG_DEBUG("UART0 buf is full and will be cleared!");
        clear_buf();
    }
    if (size > UART0_COMMAND_BUF_SIZE - 2) {
        LOG_DEBUG("UART0 rec too much data, ignore!");
        return;
    }
    memcpy((UINT8*)(buf + d_size), data, size);
    d_size += size;
    //print_hex("new command", (char *)buf, d_size);
    UINT32 v = get_free_RAM();
    if (init_v == 0) init_v = v;
    LOG_INFO("free: %i  delta: %i  count: %i", v, v - init_v, counter++);
    char *end = strrchr((char *)buf, '\r');
    if (end != NULL) {
        *(end + 1) = '\n';
        send_str_to_UART0((const char*) buf);
        char *eq = strrchr((char *)buf, '=');
        if (eq != NULL) {
            char *set = strstr((char *)buf, _SET);
            char *security_access = strstr((char *)buf, _SECURITY_ACCESS);
            char *security_key = strstr((char *)buf, _SECURITY_KEY);
            if (set != NULL && set == (char *)buf) parse_w_params(eq, end);
            else if (security_access != NULL && security_access == (char *) buf) {
                security_access_handler(eq, end);
            } else if (security_key != NULL && security_key == (char *) buf) {
                security_key_handler(eq, end);
            } else {
                send_unknown_command();
                //clear_buf();
            }
        } else {
            parse_wo_params();
        }
        clear_buf();
    }
}

void set_service_level(UINT8 level) {
    s_level = level;
    LOG_INFO("Service level:%i", level);
}
