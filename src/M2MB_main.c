/**
  @file
    M2MB_main.c
  @author 	DKozenkov
  @date		24.03.2022
*/
/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "azx_utils.h"
#include "azx_tasks.h"
#include "m2mb_uart.h"
#include "m2mb_rtc.h"
#include "m2mb_gpio.h"
#include "../hal/hdr/gnss_rec.h"
#include "sys_utils.h"
#include "sys_param.h"
#include "gpio_ev.h"
#include "testing.h"
#include "params.h"
#include "at_command.h"
#include "atc_era.h"
#include "commands.h"
#include "app_cfg.h"
#include "log.h"
#include "msd_storage.h"
#include "sim.h"
#include "u_time.h"
#include "util.h"
#include "tasks.h"
#include "codec.h"
#include "a_player.h"
#include "charger.h"
#include "gpio_ev.h"
#include "prop_manager.h"
#include "prop.h"
#include "era_helper.h"
#include "failures.h"
#include "test_helper.h"
#include "charger.h"
#include "indication.h"
#include "ecall.h"
#include "../utils/prop/hdr/prop_manager.h"
#include "../utils/prop/hdr/prop.h"
#include "../utils/hdr/gnss_util.h"
#include "../utils/hdr/averager.h"
#include "char_service.h"
#include "factory_param.h"
#include "../utils/uds/hdr/data_updater.h"

/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/

/* Local statics ================================================================================*/
/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/

/// Voltage rev_2 koef 20.3
//// DTC readDTC_Information 19   limits
//// factory.cfg REV=2  , delete MD5
//

// Changes in readDataByIdentif 22
  /*case 0x0003: // + boardVoltage (1 byte)
            size = 1;
            INT32 val = getKL30voltage() + 1000; // Очень сильная погрешность
            for (int i = 0; i < 25500; i += 100) {
                if (i >= val){
                    goto exit;
                } else {
                    content[0]++;
                }
            }
            exit:
       //     content[0] = (char) ((getKL30voltage() + 1000)/100);*/


//////////////////

/***************************************************************************************************
   \User Entry Point of Appzone

   \param [in] Module Id

   \details Main of the appzone user
**************************************************************************************************/
void ati_response(int cmd_id, char *resp) {
	LOG_INFO("CmdId: %i, Resp: %s", cmd_id, resp);
}

void creg_event_cb(const CHAR* msg) {
	LOG_INFO("received data from STN command: <%s>\r\n", msg);
}

void on_exit_callback(int signal) {
	 LOG_INFO("Era app is exiting by signal %d", signal);
	 deinit_gnss();
	 deinit_gpio();
	 close_uart();
	 ati_deinit();
	 exit(signal);
}

void gpio_cb(GPIO_EV gpio_ev) {
	LOG_DEBUG("gpio_event:%i", gpio_ev);
	switch (gpio_ev) {
		case EV_T15_ON:  // 1
            LOG_DEBUG("EV_T15_ON");
			send_to_ind_task(SHOW_IGN_ON, 0, 0);
			send_to_testing_task(TEST_IGNITION_STATE, 1, 0);
			manage_sleep(isIgnition(), getKL30());
			break;
		case EV_T15_OFF: // 2
            LOG_DEBUG("EV_T15_OFF");
			send_to_ind_task(SHOW_IGN_OFF, 0, 0);
			send_to_testing_task(TEST_IGNITION_STATE, 0, 0);
			manage_sleep(isIgnition(), getKL30());
			break;
		case EV_SOS_BTN_PRESSED: // 3
            LOG_DEBUG("EV_SOS_BTN_PRESSED");
            send_to_ecall_task(ECALL_EVENT_SOS_PRESSED, ECALL_NO_REPEAT_REASON, 0);
			send_to_era_action_task(ERA_A_SOS_BUTTON_PRESSED, 0, 0);
			break;
		case EV_ECALL_MANUAL_SOS: // 4
            LOG_DEBUG("EV_ECALL_MANUAL_SOS");
			send_to_ecall_task(ECALL_MANUAL_SOS, ECALL_NO_REPEAT_REASON, 0);
			break;
		case EV_ECALL_TEST_ERA_STARTED: // 5
			send_to_testing_task(START_TESTING, 0, 0);
			break;
		case EV_TEST_SUCCESS_CONFIRMED: // 6
            LOG_DEBUG("EV_TEST_SUCCESS_CONFIRMED");
			send_to_testing_task(TEST_CONFIRMATION_DONE, 0, 0);
			break;
		case EV_SOS_STUCK:  // 7
            LOG_DEBUG("EV_SOS_STUCK");
            set_sos_break_failure(F_ACTIVE);
			set_uim_failure(F_ACTIVE);
            break;
		case EV_SOS_UNSTUCK:  // 8
            LOG_DEBUG("EV_SOS_UNSTUCK");
            set_sos_break_failure(F_INACTIVE);
			set_uim_failure(F_INACTIVE);
			break;
        case EV_SOS_BTN_RELEASED:        // 9 кнопка SOS отпущена
            LOG_DEBUG("EV_SOS_BTN_RELEASED");
            break;
        case EV_DIAG_SOS_ON:             // 10 кнопка SOS подключена
            LOG_DEBUG("EV_DIAG_SOS_ON");
            set_sos_break_failure(F_INACTIVE);
            break;
        case EV_DIAG_SOS_OFF:            // 11 кнопка SOS отключена
            LOG_DEBUG("EV_DIAG_SOS_OFF");
            set_sos_break_failure(F_ACTIVE);
			break;
        case EV_ACC_OPEN:                // 12 клемма ACC разомкнута
            LOG_DEBUG("EV_ACC_OPEN");
            break;
        case EV_ACC_CLOSED:              // 13 клемма АСС замкнута
            LOG_DEBUG("EV_ACC_CLOSED");
            break;
        case EV_DIAG_SP_N_ON:            // 14 Динамик подключен
            LOG_DEBUG("EV_DIAG_SP_N_ON");
            set_speaker_failure(F_INACTIVE);
            break;
        case EV_DIAG_SP_N_OFF:           // 15 Динамик отключен
            LOG_DEBUG("EV_DIAG_SP_N_OFF");
            set_speaker_failure(F_ACTIVE);
            break;
        case EV_SERVICE_BTN_ON:          // 16 Нажатие кнопки SERVICE
            LOG_DEBUG("EV_SERVICE_BTN_ON");
            break;
        case EV_SERVICE_BTN_OFF:         // 17 Отпускание кнопки SERVICE
            LOG_DEBUG("EV_SERVICE_BTN_OFF");
            break;
        case EV_T30_ON:                  // 18 включена клемма 30
            LOG_DEBUG("EV_T30_ON");
            manage_sleep(isIgnition(), getKL30());
            break;
        case EV_T30_OFF:                 // 19 выключена клемма 30
            LOG_DEBUG("EV_T30_OFF");
            manage_sleep(isIgnition(), getKL30());
			send_to_testing_task(TEST_IGNITION_STATE, 0, 0);
            break;
        case EV_DIAG_MIC_ON:            // 20 Микрофон подключен
            LOG_DEBUG("EV_DIAG_MIC_ON");
            set_microphone_failure(F_INACTIVE);
            break;
        case EV_DIAG_MIC_OFF:           // 21 Микрофон отключен
            LOG_DEBUG("EV_DIAG_MIC_OFF");
            set_microphone_failure(F_ACTIVE);
            break;
        case EV_ADC_NTC_ON:             // 22 батарея подключена
//            LOG_DEBUG("EV_ADC_NTC_ON");
            break;
        case EV_ADC_NTC_OFF:            // 23 батарея отключена
//            LOG_DEBUG("EV_ADC_NTC_OFF");
            break;
        case EV_SERVICE_STUCK:          // 24 кнопка SERVICE застряла
            LOG_DEBUG("EV_SERVICE_STUCK");
            break;
        case EV_SERVICE_UNSTUCK:        // 25 кнопка SERVICE перестала застревать
            LOG_DEBUG("EV_SERVICE_UNSTUCK");
            break;
		default: break;
	}
}

void M2MB_main( int argc, char **argv )
{
  (void)argc;
  (void)argv;
  m2mb_os_init();
  log_init();
  signal(SIGTERM, on_exit_callback);
  char ver[50] = {0};
  set_app_version(ver);
  LOG_INFO("-----\r\nEra app is starting. This is v%s. LOG_LEVEL: %d", ver, azx_log_getLevel());
  if (ati_init() != M2MB_OS_SUCCESS) {
	  LOG_CRITICAL("ati_init error");
	  restart(5*1000);
	  return;
  }

  if (!init_parameters()) {
	  LOG_CRITICAL("Can't init parameters");
	  restart(5*1000);
	  return;
  }

  int r = init_tasks();
  if (r != M2MB_OS_SUCCESS) {
	  LOG_CRITICAL("Init tasks error:%i", r);
	  restart(5*1000);
  }

  if (!init_gpio(&gpio_cb)) LOG_CRITICAL("Init gpio error");

  ati_sendCommandExpectOkEx(2*1000, "AT\r"); // эта команда устраняет ошибку АТ-интерпретатора на некоторых "странных" модификациях модемов
  on_codec(); // кодек включается, чтобы работала диагностика микрофона
  //on_mute(); // Это включать не нужно, согласно ГОСТ
  play_audio(AUD_ONE_TONE); // Это нужно выполнить, чтобы начало правильно работать АЦП модема, измеряющее напр. на DIAG_MIC. Удивительный модем
  azx_sleep_ms(2000); // Задержка, чтобы на Cherry не был слышен гудок при включении
  send_to_gnss_task(START_GNSS,  1000 / get_gnss_data_rate(), 0);

  if (!startCharger()) LOG_CRITICAL("Init charger error");

  if (!ecall_modem_init()) {
	  LOG_CRITICAL("Can't ecall init");
	  restart(5*1000);
	  return;
  }

  if(with_can()){
      if (M2MB_OS_SUCCESS != send_to_update_data_task(UPDATE_DATA, 0, 0)){
          LOG_CRITICAL("Init data_updater error");
      }

      if (M2MB_OS_SUCCESS != send_to_can_task(START_CAN_SERV,0,0)) {
          LOG_CRITICAL("Init can error");
      }

      if (M2MB_OS_SUCCESS != send_to_airbags_task(CHECK_AIRBAGS,0,0)){
          LOG_CRITICAL("Init airbags error");
      }

      if(M2MB_OS_SUCCESS !=  send_to_accel_task(START_ACCEL_SERVICE, 0,0)){
          LOG_CRITICAL("Init accel error");
      }


      if(get_number_MSDs() > 0){
          UINT16 cnt = get_tr_cnt();
          if(cnt > 10){
              // Удаление всех МНД
              if(delete_all_MSDs()){
                  set_tr_cnt(0);
                  LOG_DEBUG("Unsent MSD was deleted. Current cnt %d/%d", cnt, get_manual_dial_attempts());
              }
          } else {
              // Если при запуске обнаружено неотправленные МНД, то увеличиваем счетчик на 1
              set_tr_cnt(cnt + 1);
              LOG_DEBUG("Unsent MSD was detected. Current cnt %d/%d", cnt, get_manual_dial_attempts());
          }
      } else {
          // Если МНД не обнаружено, то сбрасываем счетчик в 0
          set_tr_cnt(0);
      }
  } else {
      send_to_uart0_task(UART0_START_LISTENING, 0, 0);
  }

  if (get_reg_status_AT() == 3) {
	  to_log_info_uart("reg_st=3, so AT+COPS=0");
	  at_era_sendCommandExpectOk(10 * 1000, "AT+COPS=0\r");
  }
  if (get_era_restart() > 0) {
	  if (get_era_restart() >= MAX_RESTART_NUMBER) {
		  LOG_ERROR("Fatal modem error");
		  //set_other_critical_failure(F_ACTIVE);
		  set_era_restart(0);
	  } else {
		  LOG_WARN("Was restarted, reason: %s", get_ecall_event_descr(get_restart_reason()));
		  switch(get_restart_reason()) {
		  	  case ECALL_EVENT_SOS_PRESSED:
		  	  case ECALL_MANUAL_SOS:
		  	  case ECALL_ANOTHER:
		  		  send_to_ecall_task(ECALL_MANUAL_SOS, ECALL_MODEM_RESTARTED, 0);
		  		  break;
		  	  case ECALL_TEST:
		  		  set_era_restart(0);
		  		  send_to_ecall_task(ECALL_TEST, ECALL_MODEM_RESTARTED, 0);
		  		  break;
		  	  case ECALL_FRONT:
		  	  case ECALL_LEFT:
		  	  case ECALL_RIGHT:
		  	  case ECALL_REAR:
		  	  case ECALL_ROLLOVER:
		  	  case ECALL_SIDE:
		  	  case ECALL_FRONT_OR_SIDE:
		  		  send_to_ecall_task(get_restart_reason(), ECALL_MODEM_RESTARTED, 0);
		  		  break;
		  	  default:
		  		  set_era_restart(0);
		  		  break;
		  }
	  }
  } else {
	  T_msd msd;
	  if (find_msd_inband(&msd)) {
		  switch (msd.msdFlag) {
		  	  case MSD_INBAND: {
		  		  ECALL_TASK_EVENT event = (msd.msdMessage.msdStructure.controlType.automaticActivation == 1) ? ECALL_ANOTHER : ECALL_MANUAL_SOS;
		  		  send_to_ecall_task(event, ECALL_MODEM_RESTARTED, 0);
		  		  break;
		  	  }
		  	  case MSD_SMS:
		  	  case MSD_SAVED:
		  		  break;
		  	  default:
		  		  delete_MSD(0);
		  		  break;
		  }
	  }
  }

//  UINT32 bytes_available;
//  while(TRUE){
//      azx_sleep_ms(1000);
//      M2MB_OS_RESULT_E res = m2mb_os_memInfo( M2MB_OS_MEMINFO_BYTES_AVAILABLE, &bytes_available );
//      if ( res == M2MB_OS_SUCCESS )LOG_INFO("bytes_available: %i", bytes_available);
//      else LOG_ERROR("Unable to get mem info");
//  }
}

