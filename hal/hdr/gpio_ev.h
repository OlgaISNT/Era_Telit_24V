/**
 * @file gpio_ev.h
 * @version 1.0.0
 * @date 08.04.2022
 * @author: AKlokov
 * @brief app description
 */

#ifndef HDR_GPIO_EV_H_
#define HDR_GPIO_EV_H_

#include "m2mb_types.h"
#include "m2mb_os_api.h"

typedef struct {
    unsigned int m2m_gpio_num;
    unsigned int m2m_gpio_dir;
    unsigned int m2m_gpio_val;
} m2m_gpio_info;

enum
{
    M2M_APP_GPIO_IN_PD = 0,
    M2M_APP_GPIO_IN_PU,
    M2M_APP_GPIO_IN_NP,
    M2M_APP_GPIO_IN_MAX
};

enum{
    M2M_APP_GPIO_DIR_IN = 0,
    M2M_APP_GPIO_DIR_OUT,
    M2M_APP_GPIO_DIR_MAX
};

typedef enum {
    INIT_GPIO,                         // -- инициализация сервиса gpio_ev
    SEND_GPIO_POOL,                    // -- таймер опроса GPIO
    GPIO_NEW_EVENT,
} GPIO_EV_COMMAND;

// 19 событий
typedef enum {
    EV_SOS_STUCK,               // 1 Кнопка SOS застряла
    EV_SOS_UNSTUCK,             // 2 Кнопка SOS перестала застревать
    EV_TEST_SUCCESS_CONFIRMED,  // 3 Подтверждение завершения тестирования
    EV_SOS_BTN_PRESSED,         // 4 кнопка SOS нажата
    EV_SOS_BTN_RELEASED,        // 5 кнопка SOS отпущена
    EV_DIAG_SOS_ON,             // 6 кнопка SOS подключена
    EV_DIAG_SOS_OFF,            // 7 кнопка SOS отключена
    EV_ACC_OPEN,                // 8 клемма ACC разомкнута
    EV_ACC_CLOSED,              // 9 клемма АСС замкнута
    EV_T15_ON,                  // 10 включена клемма 15
    EV_T15_OFF,                 // 11 выключена клемма 15
    EV_DIAG_SP_N_ON,            // 12 Динамик подключен
    EV_DIAG_SP_N_OFF,           // 13 Динамик отключен
    EV_SERVICE_BTN_ON,          // 14 Нажатие кнопки SERVICE
    EV_SERVICE_BTN_OFF,         // 15 Отпускание кнопки SERVICE
    EV_T30_ON,                  // 16 включена клемма 30
    EV_T30_OFF,                 // 17 выключена клемма 30
    EV_ECALL_MANUAL_SOS,        // 18 кнопкой SOS иницирован запуск ручного вызова ЭРА (кнопка SOS удерживалась нажатой дольше 1 сек)
    EV_ECALL_TEST_ERA_STARTED,  // 19 выполнена комбинация действий для запуска тестового вызова ЭРА (кнопка SOS была быстро нажата 5 раз)
    EV_DIAG_MIC_ON,             // 20 микрофон подключен
    EV_DIAG_MIC_OFF,            // 21 микрофон отключен
    EV_ADC_NTC_ON,              // 22 батарея подключена
    EV_ADC_NTC_OFF,             // 23 батарея отключена
    EV_SERVICE_STUCK,           // 24 Кнопка SERVICE застряла
    EV_SERVICE_UNSTUCK,         // 25 Кнопка SERVICE перестала застревать
} GPIO_EV;

/**
 * @brief Callback функция, вызываемая при событии от GPIO
 * Для получения callback необходимо зарегистрировать функцию через add_gpio_ev_handler()
 * @param[in] GPIO_EV gpio_ev код события GPIO.
 * @see add_gpio_ev_handler
 */
typedef void (*gpio_ev_cb)(GPIO_EV cmd);

/**
 * @brief добавление обработчика событий от GPIO
 * @param[in] callback функция, обрабатывающая события от GPIO
 * @see gpio_ev_cb
 */
void add_gpio_ev_handler(gpio_ev_cb ev_cb);

/*!
  @brief
    Обработчик команд взаимодействия с сервисом GPIO_EV
  @details
    Обработчик команд, перечисленных в GPIO_EV_COMMAND.
    Используется через функцию send_to_gpio_task(), см. tasks.h
  @param[in] type тип команды для GPIO_EV, см. enum GPIO_EV_COMMAND
  @param[in] param1 параметр 1 команды, интерпретация зависит от типа команды
  @param[in] param2 параметр 2 команды, интерпретация зависит от типа команды
  @return M2MB_OS_SUCCESS в случае успешного выполнения, иначе код ошибки

  @note
    Примеры
  @code
	Инициализация сервиса gpio_ev:
	  void gpio_cb(GPIO_EV gpio_ev) {
		  LOG_WARN("gpio_event:%i", gpio_ev);
	  }

  	  if (M2MB_OS_SUCCESS != send_to_gpio_task(INIT_GPIO, (INT32) &gpio_cb, 0)) {
  	  	  LOG_CRITICAL("Init gpio error");
  	  }
  @endcode
 */
INT32 GPIO_task(INT32 type, INT32 param1, INT32 param2);

INT32 GPIO_TT_task(INT32 type, INT32 param1, INT32 param2);

/**
 * @brief Запуск сервиса GPIO
 * @param[in] ev_cb функция обратного вызова
 * @return TRUE/FALSE.
*/
extern BOOLEAN init_gpio(gpio_ev_cb ev_cb);

/**
 * @brief Меняет состояние выхода EN_AUDIO(GPIO_07) на требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 */
extern void enAudio(BOOLEAN state);

/**
 * @brief Возвращает текущее состояние вывода EN_AUDIO
 * @return TRUE/FALSE.
 */
extern BOOLEAN getEnAudioState(void);

/**
 * @brief Возвращает текущее состояние вывода ADC_NTC
 * @return TRUE/FALSE.
 */
extern BOOLEAN getADC_NTC_State(void);


/**
 * @brief Меняет состояние выхода GREEN_MODE_CONTROL(GPIO_10) на требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 */
extern void greenModeCtrl(BOOLEAN state);

/**
 * @brief Меняет состояние выхода SERVICE_CONTROL(GPIO_09) на требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 */
extern void serviceControl(BOOLEAN state);

/**
 * @brief Меняет состояние выхода EN_CAN(GPIO_02) на требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 * @return результат выполнения команды TRUE/FALSE.
 */
extern BOOLEAN enableCan(BOOLEAN state);

/**
 * @brief Меняет состояние выхода MCP_RESET на требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 */
extern void mcpReset(BOOLEAN state);

/**
 * @brief Меняет состояние выхода SOS_CTRL(GPIO_33) на требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 * @return результат выполнения команды TRUE/FALSE.
 */
extern BOOLEAN sosCtrl(BOOLEAN state);


/**
 * @brief Меняет состояние выхода DIAG_SP_N(UART0 DTR) на требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 * @return результат выполнения команды TRUE/FALSE.
 */
extern BOOLEAN redMode(BOOLEAN state);

/**
 * @brief Возвращает текущее состояние KL_ACC
 *
 * @return TRUE/FALSE - ВКЛ/ВЫКЛ
 */
extern BOOLEAN getAccState(void);

/**
 * @brief Возвращает текущее состояние KL30
 *
 * @return TRUE/FALSE - ВКЛ/ВЫКЛ
 */
extern BOOLEAN getKL30(void);

/**
 * @brief Возвращает текущее состояние DIAG_SP_N (UART0 DTR)
 *
 * @return TRUE/FALSE - ВКЛ/ВЫКЛ
 */
extern BOOLEAN getRedMode(void);

/**
 * @brief Меняет состояние выхода ON_CHARGEна требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 * @return результат выполнения команды TRUE/FALSE.
 */
extern BOOLEAN onCharge(BOOLEAN state);

/**
 * @brief Возвращает текущее состояние KL30
 *
 * @return TRUE/FALSE - ВКЛ/ВЫКЛ
 */
extern BOOLEAN isIgnition(void);

/**
 * @brief Возвращает текущее состояние ON_CHARGE
 *
 * @return TRUE/FALSE - ВКЛ/ВЫКЛ
 */
extern BOOLEAN getOnCharge(void);

/**
 * @brief Возвращает текущую температуру модема
 *
 * @return в градусах
 */
extern INT32 getCurrentTemp(void);

/**
 * @brief Меняет состояние выхода ON_OFF_MODE(UART0 DSR) на требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 * @return результат выполнения команды TRUE/FALSE.
 */
extern BOOLEAN onOffMode(BOOLEAN state);

/**
 * @brief Меняет состояние выхода LED_PWR_CTRL на требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 * @return результат выполнения команды TRUE/FALSE.
 */
extern BOOLEAN onLedPwrCtrl(BOOLEAN state);

/**
 * @brief Возвращает текущее состояние DIAG_MIC
 *
 * @return TRUE/FALSE - подключен/отключен
 */
extern BOOLEAN getMicState(void);

/**
 * @brief Возвращает текущее состояние DIAG_SPEAKER
 *
 * @return TRUE/FALSE - подключен/отключен
 */
extern BOOLEAN getSpeakerState(void);

/**
 * @brief Возвращает текущее состояние ON_OFF_MODE (UART0 DSR)
 *
 * @return TRUE/FALSE - ВКЛ/ВЫКЛ
 */
extern BOOLEAN getOnOffMode(void);

/**
 * @brief Меняет состояние выхода GPS_LNA_EN на требуемое
 * @param[in] state требуемое состояние TRUE/FALSE - вкл/выкл
 * @return результат выполнения команды TRUE/FALSE.
 */
extern BOOLEAN gpsLnaEn(BOOLEAN state);

/**
 * @brief Возвращает текущее состояние GPS_LNA_EN (UART0 DSR)
 *
 * @return TRUE/FALSE - ВКЛ/ВЫКЛ
 */
extern BOOLEAN getGpsLnaEn(void);

/**
 * @brief Возвращает текущее состояние SOS_CTRL (GPIO_33)
 *
 * @return TRUE/FALSE - светится/не светится
 */
extern BOOLEAN getSosCtrl(void);

/**
 * @brief Возвращает текущее состояние SERVICE_CTRL
 *
 * @return TRUE/FALSE - светится/не светится
 */
extern BOOLEAN getServiceCtrl(void);

/**
 * @brief Возвращает текущее состояние SOS_BTN
 *
 * @return TRUE/FALSE - светится/не светится
 */
extern BOOLEAN getSosState(void);

/**
 * @brief Сбросить состояние диагностики микрофона
 */
extern void reset_diag_mic(void);

/**
 * @brief Сбросить состояние диагностики динамика
 */
extern void reset_diag_speaker(void);

/**
 * @brief Возвращает текущее состояние GREEN_MODE_CTRL
 *
 * @return TRUE/FALSE - светится/не светится
 */
extern BOOLEAN getGreenModeCtrl(void);

/**
 * @brief Возвращает текущее состояние EN_AUDIO
 *
 * @return TRUE/FALSE - нажата/не нажата
 */
extern BOOLEAN getEnAudio(void);

/**
 * @brief Возвращает текущее состояние SERVICE_BTN
 *
 * @return TRUE/FALSE - нажата/не нажата
 */
extern BOOLEAN getServiceState(void);

/**
 * @brief Возвращает текущее напряжение батареи
 *
 * @return напряжение в мВ
 */
extern INT32 getBatteryVolatage(void);

/**
 * @brief Возвращает текущее напряжение KL30
 *
 * @return напряжение в мВ
 */
extern INT32 getKL30voltage(void);

/**
 * @brief Возвращает текущее напряжение диагностика микрофона
 *
 * @return напряжение в мВ
 */
extern INT32 getDiagMicVoltage(void);

/**
 * @brief Деинициализация GPIO
 *
 * @return
 */
extern M2MB_OS_RESULT_E deinit_gpio(void);

extern INT32 getMicAdc(void);
extern INT32 getKL30Adc(void);
extern INT32 getBatAdc(void);
extern UINT8 getValSosBnt(void);
extern UINT8 getValSosDiag(void);
extern UINT8 getValServiceBtn(void);
extern UINT8 getCurrWdOutState(void);
extern void setWdOut(BOOLEAN state);

#endif /* HDR_GPIO_EV_H_ */
