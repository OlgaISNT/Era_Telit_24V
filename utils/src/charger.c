//
// Created by klokov on 12.04.2022.
//
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "m2mb_hwTmr.h"
#include "log.h"
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "m2mb_hwTmr.h"
#include <stdio.h>
#include <string.h>
#include "azx_log.h"
#include "azx_utils.h"
#include "app_cfg.h"
#include "charger.h"
#include "../utils/hdr/averager.h"
#include "stdlib.h"
#include "char_service.h"
#include "at_command.h"
#include "gpio_ev.h"
#include "sys_utils.h"
#include "tasks.h"
#include "failures.h"
#include "factory_param.h"

#define CHECK_INTERVAL_IGN_ON  1000                 // интервал итераций при включенном зажигании, мС
#define CHECK_INTERVAL_IGN_OFF  30000               // интервал итераций при выключенном зажигании, мС
#define ACB_MAX_T 45                                // Максимальная температура при которой АКБ можно заряжать
#define ACB_MIN_T 0                                 // Минимальная температура при которой АКБ можно заряжать
#define ACB_CHECK_WB_MAX 4900                       // Максимальное напряжение АКБ при котором можно детектировать наличие АКБ в устройстве
#define ACB_CHECK_WB_MIN 800                        // Минимальное напряжение АКБ при котором можно детектировать наличие АКБ в устройстве
#define VOLTAGE_FOR_CHECK_HIGH_RESISTANCE 4800      // Уровень напряжения батареи при котором определяется высокое внутреннее сопротивление
#define CHARGE_INTERVAL 600000                      // Время одной итерации при заряде 10 мин
#define ACB_CH_THR_PERM_MIN 4100                    // Напряжение АКБ ниже которого нужно начинать зарядку
#define ACB_MIN_ACB_VALT 2500                       // Минимальное напряжение батареи при котором ее можно заряжать
#define ACB_MIN_ACB_ERROR 3400                      // Напряжение батареи ниже которого появляется ошибка о ее низком заряде
#define ACB_MAX_ACB_ERROR 3600                      // Напряжение батареи выше которого исчезает ошибка о ее низком заряде
#define ACB_PEAK_DETECT_SNSTVTY 5                   // Порог чувствительности при заряде
#define ACB_CH_MAX_TIME CHARGE_INTERVAL * 60 * 20   // Максимальное время заряда АКБ 20 часов

static CHARGE_STATE_CHARGER currentAcbChargeState;      // текущий флаг состояния заряда
static RESISTANCE_STATE_CHARGER currentResistanceState; // текущий флаг состояния внутреннего сопротивления
static TEMPERATURE_STATE_CHARGER currentAcbThermalState;// текущий флаг контроля температуры
static CONNECTION_STATE_CHARGER currentConnectionState; // текущий флаг подключения батареи
static GENERAL_STATE_CHARGER currentBatState;           // общее текущее состояние

static CHARGE_STATE_CHARGER prevAcbChargeState;         // последний флаг состояния заряда
static RESISTANCE_STATE_CHARGER prevResistanceState;    // последний флаг состояния внутреннего сопротивления
static TEMPERATURE_STATE_CHARGER prevAcbThermalState;   // последний флаг контроля температуры
static CONNECTION_STATE_CHARGER prevConnectionState;    // последний флаг подключения батареи
static RESERVE_BATTERY_STATE reserveBatteryState;       // текущее состояние резервной батареи

static UINT32 CHARGE_TIMER_INTERVAL;  // Текущее время одной итерации
static UINT32 LAST_TIMER_INTERVAL;    // Время предидущей итерации
static UINT32 cntChargeAction;
static UINT16 prev_v_bat = 0;         // Последнее значение напряжения питания модема

static Averager *averager = NULL;

static INT32 maxMovingAverageVoltOnPeriod; // максимальное значение скользящего среднего за период
static INT32 dV;                               // производная напряжения (на сколько напряжение выросло за период)
static INT32 timeCharging;                     // Время непрерывной зарядки (сек)

static void acbCheckTimerActions(void);  // Функции выполняемые таймером проверки батареи
static void acbChargeTimerActions(void); // Функции выполняемые таймером заряда батареи

static BOOLEAN initCharger(void);       // Инициализация алгоритма заряда
static void charge_timer_handler(void); // Задачи таймера

static BOOLEAN logPermission(UINT32 actual, UINT32 expected); // Разрешение лога
static void acbHasConnected(void);    // Подключена ли батарея
static void acbTempCheck(void);       // Проверка температуры батареи
static void acbCharger(void);         // Проверка всех условий и разрешение заряда
static void acbResistanceCheck(void); // Проверка внутреннего сопротивления батареи
static void diffDetectAlg(void);      // Определение окончания заряда
static void setMaxVoltage(void);
static void chargeCycle(void);        // Основной цикл
/***************************************************************************************************
   CHARGER_task Нельзя вызывать эту функцию напрямую!
**************************************************************************************************/
extern INT32 CHARGER_task(INT32 type, INT32 param1, INT32 param2) {
    (void) param1;
    (void) param2;
//    LOG_DEBUG("CHARGER_task command: %i", type);
    switch (type) {
        case POOL_CHARGER:
            LOG_DEBUG("CHARGER: Starting the battery Charge Service.");
            chargeCycle();
            return M2MB_OS_SUCCESS;
        default:
            LOG_DEBUG("CHARGER: UNKNOWN CMD.");
            return M2MB_OS_SUCCESS;
    }
}

extern TEMPERATURE_STATE_CHARGER getBattTempState(void){
    return currentAcbThermalState;
}

extern RESERVE_BATTERY_STATE getBattState(void){
    return reserveBatteryState;
}
static BOOLEAN isInit = FALSE;

extern BOOLEAN startCharger(void) {
    if (!isInit) { // Если еще не запущен
        initCharger();
    }
    return TRUE;
}

// Основной цикл
static void chargeCycle(void){
    isInit = TRUE;
    while (isInit){
        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(CHARGE_TIMER_INTERVAL));
        charge_timer_handler();
    }
}

extern BOOLEAN stopTimer(void){
    isInit = FALSE;
    if (averager != NULL){
        deleteAverager(averager);
    }
    return TRUE;
}

static void charge_timer_handler(void) {
    if(isTestMode()){ // В тестовом режиме алгоритм заряда не работает

    } else {
        acbCheckTimerActions(); // Проверка батареи выполняется всегда
        LAST_TIMER_INTERVAL = CHARGE_TIMER_INTERVAL;
        if (isIgnition()) { // Если зажигание включено, то активный режим
            CHARGE_TIMER_INTERVAL = CHECK_INTERVAL_IGN_ON;
            if (cntChargeAction >= CHARGE_INTERVAL / CHARGE_TIMER_INTERVAL) { // Проверка алгоритма заряда
                acbChargeTimerActions();
                cntChargeAction = 0;
            }
            cntChargeAction++;
            if (CHARGE_TIMER_INTERVAL != LAST_TIMER_INTERVAL) {
                LOG_DEBUG("CHARGER: Active mode started");
            }
        } else {  // Если зажигание выключено, то спящий
            CHARGE_TIMER_INTERVAL = CHECK_INTERVAL_IGN_ON;
            cntChargeAction = 0;
            onCharge(FALSE);
            if (CHARGE_TIMER_INTERVAL != LAST_TIMER_INTERVAL) {
                LOG_DEBUG("CHARGER: Sleep mode started");
            }
        }
    }
}

extern void getStateBattery(void){
    switch (currentBatState) {
        case UNKNOWN_STATE:
            set_battery_failure(F_UNKNOWN);
            break;
        case BAT_OK:
            set_battery_failure(F_INACTIVE);
            break;
        case BAT_ERR:
            set_battery_failure(F_ACTIVE);
            break;
        default:
            break;
    }
}

static void acbChargeTimerActions(void) {
    if (currentAcbChargeState == ACB_CHARGING_NOW) {
        dV = maxMovingAverageVoltOnPeriod - getAverage(averager);
        timeCharging += CHARGE_INTERVAL;
        diffDetectAlg();
    } else if (currentConnectionState == HAS_CONNECTION) {
        LOG_DEBUG("CHARGER: Now battery is not charging. Current battery voltage = %d mV. "
                  "Current battery temperature = %d C.", getBatteryVolatage(), getCurrentTemp());
    } else {
        LOG_DEBUG("CHARGER: battery is not inserted");
    }
}

static void diffDetectAlg(void) {
    if (dV >= ACB_PEAK_DETECT_SNSTVTY) {
        LOG_DEBUG("CHARGER: Charging completed. "
                  "Time charging = %d sec. "
                  "Max moving average voltage = %d mV. "
                  "Moving average voltage = %d mV. dV = %d mV.",
                  timeCharging / 1000, maxMovingAverageVoltOnPeriod, getAverage(averager), dV);
        currentAcbChargeState = ACB_CHARGED;
        timeCharging = 0;
        onCharge(FALSE);
    } else {
        LOG_DEBUG("CHARGER: Charging in progress. "
                  "Time charging = %d sec. "
                  "Max moving average voltage = %d mV. "
                  "Moving average voltage = %d mV. dV = %d mV.",
                  timeCharging / 1000, maxMovingAverageVoltOnPeriod, getAverage(averager), dV);
    }
    if (getBatteryVolatage() > VOLTAGE_FOR_CHECK_HIGH_RESISTANCE) {
        currentResistanceState = ABNORMAL_RESISTANCE;
        LOG_DEBUG("CHARGER: The voltage of one of the cells exceeds 1.8V. "
                  "The element is considered damaged due to high internal resistance.");
    }
}

static BOOLEAN initCharger(void) {
    onCharge(FALSE);
    cntChargeAction = 0;
    maxMovingAverageVoltOnPeriod = 0;
    timeCharging = 0;
    averager = newAverager(50);
    currentBatState = UNKNOWN_STATE;
    currentAcbThermalState = UNDEFINED_TEMPERATURE_STATE;
    currentResistanceState = UNDEFINED_RESISTANCE_STATE;
    prevConnectionState = UNDEFINED_CONNECTION_STATE;
    currentConnectionState = UNDEFINED_CONNECTION_STATE;
    prevAcbChargeState = UNDEFINED_CHARGE_STATE;
    currentAcbChargeState = UNDEFINED_CHARGE_STATE;
    CHARGE_TIMER_INTERVAL = CHECK_INTERVAL_IGN_ON;
    send_to_charger_task(POOL_CHARGER,0,0); // Запуск пулинга
    return TRUE;
}

static void acbCheckTimerActions(void) {
    acbHasConnected();
    acbResistanceCheck();
    acbTempCheck();
    acbCharger();
    setMaxVoltage();
    getStateBattery();
}

static void setMaxVoltage(void) {
//    LOG_DEBUG("volt: %d", getBatteryVolatage());
    if (!isAutoVaz()){ // Если автомобиль сделан НЕ Автовазом
        // Если напряжение АКБ падает ниже 3400 мВ, то появляется ошибка BATTERY_LOW_VOLTAGE_FAILURE
        if (getBatteryVolatage() <= ACB_MIN_ACB_ERROR){
            set_battery_low_voltage_failure(F_ACTIVE); // Наличие данной ошибки не запрещает заряд батареи
        }

        // Ошибка BATTERY_LOW_VOLTAGE_FAILURE пропадает сразу, как только напряжение батареи поднимается выше 3600 мВ
        if (getBatteryVolatage() >= ACB_MAX_ACB_ERROR){
            set_battery_low_voltage_failure(F_INACTIVE);
        }
    } else { // Если автомобиль сделан Автовазом
        if (getBatteryVolatage() >= ACB_MIN_ACB_VALT){ // Если напряжение АКБ >= 2500
            set_battery_low_voltage_failure(F_INACTIVE); // Отсутствие ошибки BATTERY_LOW_VOLTAGE_FAILURE
        } else {
            set_battery_low_voltage_failure(F_ACTIVE); // Появление ошибки BATTERY_LOW_VOLTAGE_FAILURE
        }
    }

    if (currentAcbChargeState == ACB_CHARGING_NOW) {
        add(averager, getBatteryVolatage());
        maxMovingAverageVoltOnPeriod = (getAverage(averager) > maxMovingAverageVoltOnPeriod) ?
                                       getAverage(averager) : maxMovingAverageVoltOnPeriod;
    } else {
        maxMovingAverageVoltOnPeriod = 0;
        onCharge(FALSE);
        timeCharging = 0;
        if (!averager->cleared) {
            reset(averager);
        }
    }
}

/*
 * Если напряжение одной из ячеек превышает 1800 мВ,
 * то элемент считается поврежденным из-за высокого внутреннего сопротивления
 */
static void acbResistanceCheck(void) {
    prevResistanceState = currentResistanceState;
    if (getBatteryVolatage() >= VOLTAGE_FOR_CHECK_HIGH_RESISTANCE && currentConnectionState == HAS_CONNECTION) {
        currentResistanceState = ABNORMAL_RESISTANCE;
        if (logPermission(currentResistanceState, prevResistanceState)) {
            LOG_DEBUG("CHARGER: The battery has a high internal resistance. "
                      "Current battery voltage = %d mV. "
                      "Current battery temp = %d C.", getBatteryVolatage(), getCurrentTemp());
        }
    } else if (getBatteryVolatage() < VOLTAGE_FOR_CHECK_HIGH_RESISTANCE && currentConnectionState == HAS_CONNECTION) {
        currentResistanceState = NORMAL_RESISTANCE;
        if (logPermission(currentResistanceState, prevResistanceState)) {
            LOG_DEBUG("CHARGER: The battery has a normal internal resistance. "
                      "Current battery voltage = %d mV. "
                      "Current battery temp = %d C.", getBatteryVolatage(), getCurrentTemp());
        }
    } else{
        currentResistanceState = UNDEFINED_RESISTANCE_STATE;
        if(logPermission(currentResistanceState, prevResistanceState)){
            LOG_DEBUG("The internal resistance of the battery cannot be determined. Please connect the battery");
        }
    }
}

/*
 * Независимо от состояний ON_OFF_MODE и ON_CHARGE:
 * - При отключенной батарее ADC_BATTERY составляет около 5000 мВ
 * - При подключенной батарее ADC_BATTERY просаживается
 *
 * Если ADC_BATTERY > 4900 батарея отсутствует (либо в очень плохом состоянии)
 */
static void acbHasConnected(void) {
    prevConnectionState = currentConnectionState;
    if (getOnOffMode()) {
        if (getBatteryVolatage() > ACB_CHECK_WB_MAX || getBatteryVolatage() < ACB_CHECK_WB_MIN) {
            currentConnectionState = HAS_NOT_CONNECTION;
            if (logPermission(currentConnectionState, prevConnectionState)) {
                LOG_DEBUG("CHARGER: Battery has been disconnected");
                if (!isAutoVaz()){ // Если автомобиль сделан не автовазом
                    // При подключении батареи если ее напряжение ниже 3400, то появляется ошибка о низком напряжении
                    if (getBatteryVolatage() <= ACB_MIN_ACB_ERROR){
                        set_battery_low_voltage_failure(F_ACTIVE);
                    } else{
                        set_battery_low_voltage_failure(F_INACTIVE);
                    }
                }
            }
        } else {
            currentConnectionState = HAS_CONNECTION;
            if (logPermission(currentConnectionState, prevConnectionState)) {
                LOG_DEBUG("CHARGER: Battery has been connected");
            }
        }
    } else {
        currentConnectionState = UNDEFINED_CONNECTION_STATE;
        if (logPermission(currentConnectionState, prevConnectionState)) {
            LOG_DEBUG("CHARGER: Unable to detect battery. Please enable ON_OFF_MODE.");
        }
    }
}

static void acbCharger(void) {
    prevAcbChargeState = currentAcbChargeState;
    if (currentConnectionState == HAS_NOT_CONNECTION || currentConnectionState == UNDEFINED_CONNECTION_STATE) {
        /*-----------------------------------------------------------
        |что:		|	батарея отсутствует в разъеме               |
        |-----------|-----------------------------------------------|
        |что делать:|	формируем общую ошибку, не заряжаем			|
        -----------------------------------------------------------*/
        onCharge(FALSE);
        currentAcbChargeState = ACB_DO_NOT_CHARGE; // Запрет на заряд батареи
        currentBatState = BAT_ERR;
        timeCharging = 0;
        reserveBatteryState = CHECK_ABSENCE;
        if (logPermission(currentConnectionState, prevConnectionState)) {
            LOG_DEBUG("CHARGER: Battery not inserted");
        }
    } else if (currentAcbThermalState == HYPOTHERMIA || currentAcbThermalState == OVERHEAT) {
        /*-----------------------------------------------------------
        |что:		|	АКБ перегрелся или заморожен				|
        |-----------|-----------------------------------------------|
        |что делать:|	не формируем общую ошибку, не заряжаем		|
        -----------------------------------------------------------*/
        onCharge(FALSE);
        currentAcbChargeState = ACB_DO_NOT_CHARGE; // Запрет на заряд батареи
        currentBatState = BAT_OK;
        timeCharging = 0;
        reserveBatteryState = NO_CHARGING;
        if (logPermission(currentAcbThermalState, prevAcbThermalState)) {
            LOG_DEBUG("CHARGER: Temperature battery out of charging range. "
                      "Current battery voltage = %d mV. "
                      "Current battery temp = %d C.", getBatteryVolatage(), getCurrentTemp());
        }
    }else if (currentResistanceState == ABNORMAL_RESISTANCE) {
        /*-----------------------------------------------------------
        |что:		|	скорее всего внутреннее R АКБ слишком велико|
        |-----------|-----------------------------------------------|
        |что делать:|	формируем общую ошибку, не заряжаем			|
        -----------------------------------------------------------*/
        onCharge(FALSE);
        currentAcbChargeState = ACB_DO_NOT_CHARGE; // Запрет на заряд батареи
        currentBatState = BAT_OK;
        timeCharging = 0;
        reserveBatteryState = CHECK_OLD;
        if (logPermission(currentResistanceState, prevResistanceState)) {
            LOG_DEBUG("CHARGER: currentBatVoltage too high, most likely the internal R of ACC is too high. "
                      "Current battery voltage = %d mV. "
                      "Current temp = %d C. ", getBatteryVolatage(), getCurrentTemp());
        }

    }
    else if (timeCharging >= ACB_CH_MAX_TIME) {
        /*-----------------------------------------------------------
        |что:		|	слишком долгая зарядка АКБ					|
        |-----------|-----------------------------------------------|
        |что делать:|	формируем общую ошибку, не заряжаем			|
        -----------------------------------------------------------*/
        onCharge(FALSE);
        currentAcbChargeState = ACB_DO_NOT_CHARGE; // Запрет на заряд батареи
        currentBatState = BAT_OK;
        timeCharging = 0;
        reserveBatteryState = CHECK_OLD;
        if (logPermission(currentAcbChargeState, prevAcbChargeState)) {
            LOG_DEBUG("CHARGER: Too long battery charging time. "
                      "Current battery voltage = %d mV. "
                      "Current battery temp = %d C. "
                      "Time charging = %d sec.", getBatteryVolatage(), getCurrentTemp(), timeCharging / 1000);
        }
    }
    else if (isIgnition() && getKL30() && getOnOffMode()) {
        /*-----------------------------------------------------------
        |что:		|	АКБ допущен до заряда:						|
        |			|	- АКБ не перегрет, не переохлажден,			|
        |			|	не перезаряжен, не заряжается слишком долго	|
        |			|	- KL15 (зажигание) нажат					|
        |			|	- KL30 (питание БЭГ) нажат					|
        |			|	- ON_OFF_MODE TRUE      					|
        |-----------|-----------------------------------------------|
        |что делать:|	заряжать									|
        -----------------------------------------------------------*/
        if ((getBatteryVolatage() >= ACB_MIN_ACB_VALT && getBatteryVolatage() <= ACB_CH_THR_PERM_MIN)
            || currentAcbChargeState == ACB_CHARGING_NOW) {
            /*-----------------------------------------------------------
           |что:		|	заряд нужно начать или он уже идет			|
           |-----------|------------------------------------------------|
           |что делать:|	заряжать									|
           -----------------------------------------------------------*/
            onCharge(TRUE);
            currentAcbChargeState = ACB_CHARGING_NOW;
            currentBatState = BAT_OK;
            reserveBatteryState = FAST_CHARGING;
            add(averager, getBatteryVolatage());
            if (logPermission(currentAcbChargeState, prevAcbChargeState)) {
                LOG_DEBUG("CHARGER: Charge was started. "
                          "Current battery voltage = %d mV. "
                          "Current temp = %d C. ", getBatteryVolatage(), getCurrentTemp());
            }
        } else {
            /*-----------------------------------------------------------
            |что:		|	батарея не требует заряда            		 |
            |-----------|------------------------------------------------|
            |что делать:|	ничего не делать						     |
            -----------------------------------------------------------*/
            currentAcbChargeState = ACB_CHARGED;
            currentBatState = BAT_OK;
            timeCharging = 0;
            reserveBatteryState = NO_CHARGING;
            if (logPermission(currentAcbChargeState, prevAcbChargeState)) {
                LOG_DEBUG("CHARGER: The battery is now active. "
                          "Current battery voltage = %d mV. "
                          "Current temp = %d C.", getBatteryVolatage(), getCurrentTemp());
            }
        }
    }
}

static BOOLEAN logPermission(UINT32 actual, UINT32 expected) {
    if (expected != actual) { // Если состояния разные
        return TRUE;
    }
    return FALSE;
}

/*
 * Т.к. нет показаний с датчика NTC на батарее,
 * то используются показания температуры модема
 */
static void acbTempCheck(void) {
    prevAcbThermalState = currentAcbThermalState;
    if (currentConnectionState == HAS_CONNECTION) {
        if (getCurrentTemp() < ACB_MIN_T) {
            currentAcbThermalState = HYPOTHERMIA;
            if (logPermission(currentAcbThermalState, prevAcbThermalState)) {
                LOG_DEBUG("CHARGER: Hypothermia detect: %d C", getCurrentTemp());
            }
        } else if (getCurrentTemp() > ACB_MAX_T) {
            currentAcbThermalState = OVERHEAT;
            if (logPermission(currentAcbThermalState, prevAcbThermalState)) {
                LOG_DEBUG("CHARGER: Overheat detect: %d C", getCurrentTemp());
            }
        } else {
            currentAcbThermalState = NORMAL_TEMPERATURE;
            if (logPermission(currentAcbThermalState, prevAcbThermalState)) {
                LOG_DEBUG("CHARGER: Battery is at normal temperature: %d C", getCurrentTemp());
            }
        }
    } else {
        currentAcbThermalState = UNDEFINED_TEMPERATURE_STATE;
        if (logPermission(currentAcbThermalState, prevAcbThermalState)) {
            LOG_DEBUG("CHARGER: Unable to detect battery temperature");
        }
    }
}

extern UINT16 get_v_bat(void){
    char resp[256];
    memset(resp,0x00,256);
    ati_sendCommandEx(1000, resp, sizeof(resp), "AT#CBC");

    for(unsigned int j = 0; j < strlen(resp); j++){
        if(resp[j] == '\n' || resp[j] == '\r' || resp[j] == ' ' || resp[j] == '\t'){
            deleteChar(resp,j);
            j--;
        }
    }
    resp[strlen(resp) - 2] = '\0';

    // Убираем всегда первые 7 символов
    for (UINT16 i = 7; i < strlen(resp); i++){
        resp[i - 7] = resp[i];
    }
    resp[strlen(resp) - 7] = '\0';

    prev_v_bat = atoi(resp);
    return prev_v_bat;
}
