/*
 * tasks.h
 * Created on: 01.04.2022, Author: DKozenkov
 */

#ifndef TASKS_TASKS_H_
#define TASKS_TASKS_H_

#include "../../era/hdr/sms_manager.h"
#include "gnss_rec.h"
#include "uart0.h"
#include "gpio_ev.h"
#include "charger.h"
#include "sim.h"
#include "ecall.h"
#include "era_action.h"
#include "indication.h"
#include "testing.h"
#include "mcp2515.h"
#include "../uds/hdr/securityAccess_27.h"
#include "can_util.h"
#include "airbags.h"
#include "../accelerometer/hdr/accel_driver.h"
#include "../accelerometer/hdr/KalmanFilter.h"
#include "../uds/hdr/uds_service31.h"


/*!
  @brief
    Инициализация механизма управления тасками

  @details
    Функцию следует вызывать до любого взаимодействия с тасками
  @param[in] file_path
    Имя файла, размер которого нужно определить

  @return
    M2MB_OS_SUCCESS или код ошибки из перечисления AZX_TASKS_ERR_E

  @note

  @b
    Example
  @code
    <C code example>
  @endcode
 */
int init_tasks(void);

/*!
  @brief
    Отправка сообщения в таск приёмника GNSS

  @details
    Функцию следует вызывать до любого взаимодействия с тасками
	@param[in] type
    Тип передаваемого в таск сообщения, возможные типы перечислены в gnss.h
	@param[in] param1
    INT32 переменная (может быть указателем на структуру, функцию и пр.)
	@param[in] param2
    INT32 переменная (может быть указателем на структуру, функцию и пр.)

  @return
    M2MB_OS_SUCCESS или M2MB_OS_TASK_ERROR в случае ошибки

  @note

  @b
    Example
  @code
    <C code example>
  @endcode
 */
M2MB_OS_RESULT_E send_to_gnss_task(GNSS_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_uart0_task(UART0_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_gpio_task(GPIO_EV_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_charger_task(CHARGER_TASK_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_can_task(CAN_TASK_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_sim_task(SIM_TASK_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_ecall_task(ECALL_TASK_EVENT type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_era_action_task(ERA_A_TASK_EVENT type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_ind_task(IND_TASK_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_sms_task(SMS_TASK_EVENT type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_testing_task(TESTING_TASK_EVENT type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_gnss_tt_task(GNSS_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_gpio_tt_task(GPIO_EV_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_atc_tt_task(INT32 type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_can_tt_task(CAN_EVENT type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_can_sa_task(CAN_SA_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_airbags_task(AIRBAGS_TASK type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_accel_task(ACCEL_TASK type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_Kalman_task(TASK_FILTER type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_test_mode_task(TEST_MODE_TASK_COMMAND type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_update_data_task(INT32 type, INT32 param1, INT32 param2);

M2MB_OS_RESULT_E send_to_uds_file_tool_task(INT32 type, INT32 param1, INT32 param2);
void destroy_tasks(void);

#endif /* TASKS_TASKS_H_ */
