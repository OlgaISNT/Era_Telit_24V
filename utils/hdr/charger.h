//
// Created by klokov on 12.04.2022.
//
#ifndef ERA_CHARGER_H
#define ERA_CHARGER_H

#include "m2mb_types.h"

typedef enum {
    POOL_CHARGER,    // запуск сервиса CHARGER
    STOP_CHARGER,	 // останов сервиса
    NEXT_ITER_CHARGE // очередная итерация
} CHARGER_TASK_COMMAND;

typedef enum {
    UNKNOWN_STATE,
    BAT_OK,
    BAT_ERR
} GENERAL_STATE_CHARGER;

typedef enum {
    ACB_CHARGED,
    ACB_NOT_CHARGED,
    ACB_CHARGING_NOW,
    ACB_DO_NOT_CHARGE,
    ACB_LONG_CHARGE,
    UNDEFINED_CHARGE_STATE
} CHARGE_STATE_CHARGER;

typedef enum {
    HAS_CONNECTION,
    HAS_NOT_CONNECTION,
    UNDEFINED_CONNECTION_STATE
} CONNECTION_STATE_CHARGER;

typedef enum {
    BAT_LOW_VOLTAGE,
    BAT_OVERCHARGE_VOLTAGE,
    BAT_OLD,
    UNDEFINED_VOLTAGE_STATE
} VOLTAGE_STATE_CHARGER;

typedef enum {
    NORMAL_RESISTANCE,
    ABNORMAL_RESISTANCE,
    UNDEFINED_RESISTANCE_STATE
} RESISTANCE_STATE_CHARGER;

typedef enum {
    NORMAL_TEMPERATURE,
    OVERHEAT,
    HYPOTHERMIA,
    UNDEFINED_TEMPERATURE_STATE
} TEMPERATURE_STATE_CHARGER;

typedef enum {
    HAS_BREAK_WIRE,
    NORMAL_CONNECTION,
    UNDEFINED_WIRES_STATE
} WIRES_STATE_CHARGER;

typedef enum{
    NO_CHARGING,
    TRICKLE_CHARGING,
    FAST_CHARGING,
    CHECK_OLD,
    CHECK_ABSENCE,
    UNDEFINED,
    STABILIZATION,
    CARE_CHARGE
} RESERVE_BATTERY_STATE;

extern RESERVE_BATTERY_STATE getBattState(void);
extern TEMPERATURE_STATE_CHARGER getBattTempState(void);
/*!
  @brief
    Обработчик команд взаимодействия с сервисом CHARGER
  @details
    Обработчик команд, перечисленных в CHARGER_TASK_COMMAND.
    Используется через функцию send_to_charger_task(), см. tasks.h
  @param[in] type тип команды для CHARGER, см. enum CHARGER_TASK_COMMAND
  @param[in] param1 параметр 1 команды, интерпретация зависит от типа команды
  @param[in] param2 параметр 2 команды, интерпретация зависит от типа команды
  @return M2MB_OS_SUCCESS в случае успешного выполнения, иначе код ошибки

  @note
    Примеры
  @code
	Запуск сервиса CHARGER:

  	  if (M2MB_OS_SUCCESS != send_to_charger_task(INIT_GPIO, 0, 0)) {
  	  	  LOG_CRITICAL("Init charger error");
  	  }
  @endcode
 */
//INT32 CHARGER_task(INT32 type, INT32 param1, INT32 param2);

/**
 * @brief Возвращает общее текущее состояние батареи
 *
 * @return UNKNOWN/BAT_OK/BAT_ERR
 */
extern void getStateBattery(void);
extern BOOLEAN startCharger(void);
extern BOOLEAN stopTimer(void);
extern INT32 CHARGER_task(INT32 type, INT32 param1, INT32 param2);

extern UINT16 get_v_bat(void);
#endif //ERA_CHARGER_H
