/**
 * @file gpio_ev.c
 * @version 1.0.0
 * @date 08.04.2022
 * @author:AKlokov
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "app_cfg.h"
#include "gpio_ev.h"
#include "log.h"
#include "at_command.h"
#include "m2mb_gpio.h"
#include "m2mb_uart.h"
#include "tasks.h"
#include "sys_utils.h"
#include "factory_param.h"
#include "math.h"
#include "atc_era.h"
#include "mcp2515.h"
#include "failures.h"

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <sys/types.h>
#include <ctype.h>
#include <getopt.h>
#include <time.h>
#include <linux/spi/spidev.h>
#include <poll.h>
#include <linux/types.h>
#include <linux/ioctl.h>

/* Local defines ================================================================================*/
#define PREFIX "/dev/GPIO"
#define GPIO_DEV_PATH "/dev/m2m_gpio"

#define RED_MODE_CTRL "3"
#define DIAG_SOS_IN "6"
#define IGNIT "5"
#define SOS_BTN "1"
#define SERVICE_BTN "8"
#define ACC "4"
#define EN_AUDIO "7"
#define GREEN_MODE_CTRL "10"
#define SERVICE_CTRL "9"
#define SOS_CTRL "2"
#define ON_OFF_MODE "3"  // (P14 C107 DSR нога UART (GPIO_32))
#define DCD 29
#define ADC_TIMER_INTERVAL 10000  // интервал опроса АЦП, 10 сек
#define TEMP_TIMER_INTERVAL 60000 // интервал получения температуры, 60 сек
#define GPIO_TIMER_INTERVAL 100   // интервал опроса GPIO
#define NOTIFY_DOG_INTERVAL 5000         // Отправляем вочдогу импульс каждые 5000 мСек
#define WIDTH_PULSE_FOR_DOG 3000         // Ширина импульса для
#define IGN_FIX_TIME        1000         // Время фиксации изменениия состояния КЛ15

#define KP_ADC_BAT 3.037          // Коэффициент пропорциональности ADC_BAT
#define THRESHOLD_VALUE_KL30 6000 // Пороговое значение клеммы 30   6B
#define MIC_NUMB_MISURMENTS 10    // Количество замеров для изменения состояния микрофона
#define M2M_GPIO_MAGIC 'g'
#define IOCTL_M2M_APP_GPIO_CLR _IOW( M2M_GPIO_MAGIC, 0, m2m_gpio_info)
#define IOCTL_M2M_APP_GPIO_SET_DIR _IOW( M2M_GPIO_MAGIC, 1, m2m_gpio_info)
#define IOCTL_M2M_APP_GPIO_SET_VAL _IOW( M2M_GPIO_MAGIC, 2, m2m_gpio_info)
#define IOCTL_M2M_APP_GPIO_GET_VAL _IOW( M2M_GPIO_MAGIC, 3, m2m_gpio_info)
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static gpio_ev_cb callback = NULL;

static INT32 DIAG_SOS_fd;
static INT32 IGNIT_fd;
static INT32 SOS_BTN_fd;
static INT32 SERVICE_BTN_fd;
static INT32 ACC_fd;
static INT32 EN_AUDIO_fd;
static INT32 GREEN_MODE_CTRL_fd;
static INT32 SERVICE_CTRL_fd;
static INT32 RED_MODE_CTRL_fd;
static INT32 ADC_NTC_fd;
static UINT8 valBtn = 0;
static UINT8 valDIAG = 0;
static UINT8 valSrvice = 0;

static INT32 dev = 0;
static m2m_gpio_info *m2m_gpio_dcd;
static m2m_gpio_info *m2m_gpio_dsr;
static m2m_gpio_info *m2m_gpio_dtr;
static m2m_gpio_info *m2m_gpio_rg;

static BOOLEAN isInit = FALSE; // Запущен ли сервис
static BOOLEAN isMicInit = FALSE;
static volatile BOOLEAN cycle_started = FALSE;
static UINT32 cntInitGpio;
static UINT32 ign_cnt = 0;
//static BOOLEAN wait = FALSE;
static BOOLEAN prevSosDiagState;         // Предыдущее состояние линии кнопки SOS
static BOOLEAN prevSpeakerState;         // Предыдущее состояние динамика
static BOOLEAN prevSOS_BTNState;         // Предыдущее состояние кнопки SOS
static BOOLEAN prevIgnitionState;        // Предыдущее состояние KL15
static BOOLEAN prevServiceBTNState;      // Предыдущее состояние кнопки SERVICE
static BOOLEAN prevACCState;             // Предыдущее состояние ACC
static BOOLEAN prevKL30State;            // Предыдущее состояние KL30
static BOOLEAN prevDiagMicState;         // Предыдущее состояние микрофона

static BOOLEAN currentAdcNtcState;       // Текущее состояние ADC_NTC
static BOOLEAN currentDiagMicState;      // Текущее состояние микрофона
static BOOLEAN currentSosDiagState;      // Текущее состояние линии кнопки SOS
static BOOLEAN currentSpeakerState;             // Текущее состояние динамика
static BOOLEAN currentSOS_BTNState = TRUE;      // Текущее состояние кнопки SOS
static BOOLEAN currentIgnitionState;     // Текущее состояние KL15
static BOOLEAN currentServiceBTNState;   // Текущее состояние кнопки SERVICE
static BOOLEAN currentACCState;          // Текущее состояние ACC
static BOOLEAN sosStckState;             // Застревание кнопки SOS
static BOOLEAN serviceStckState;         // Застревание кнопки SERVICE
static BOOLEAN currentEnableAudioState;  // Текущее состояние EN_AUDIO
static BOOLEAN currentRedModeState;      // Текущее состояние RED_MODE
static BOOLEAN currentOnOffModeState;    // Текущее состояние ON_OFF_MODE
static BOOLEAN currentGpsLnaEn;          // Текущее состояние GPS_LNA_EN
static BOOLEAN currentOnCharge;          // Текущее состояние ON_CHARGE
static BOOLEAN currentSosCtrl;           // Текущее состояние SOS_CTRL
static BOOLEAN currentKL30State;         // Текущее состояние KL30
static BOOLEAN needCycle;
static BOOLEAN currentGreenModeCtrl;     // Текущее состояние GREEN_MODE_CTRL
static BOOLEAN currentServiceCtrl;       // Текущее состояние SERVICE_CTRL (LED)
static BOOLEAN currentLedPwrCtrl;
static BOOLEAN currentWdState;

static UINT8 micCnt = 0;
static UINT8 micStage = 0;

static BOOLEAN hasEraCall;               // Происходит ли вызов ЭРА в данный момент
static BOOLEAN onIgnTest;                // Был ли запущен тест при включении зажигания

static INT32 currentTemp;                // Текущая температура
static INT32 currentKL30voltage;         // Текущее напряжение клеммы 30 мВ
static INT32 currentDIAG_MIC_N_voltage;  // Текущее напряжение для диагностики микрофона мВ
static INT32 currentADC_BATTERY_voltage; // Текущее напряжение батареи мВ

static UINT32 TIME_MANUAL_CALL_ERA = 1500;    // Минимальное время для запуска ручного вызова ЭРА
static UINT32 AVERAGE_FAST_PRESS_TIME = 1700; // Среднее время нажатия на клавишу
static UINT32 PAUSE_TIME = 1500;              // Допустимое время паузы между нажатиями
static UINT32 STUCK_TIME = 10000;             // Время за которое кнопка SOS считается зажатой
static UINT32 SOS_SERVICE_TIME_TEST = 2000;   // Время в течении которого нужно удерживать кнопки SOS и SERVICE нажатыми чтобы начать тестирование
static INT32 TRESHOLD_MIC_CONNECT = 150;      // Пороговое значение напряжения выше которого микрофон считается подключенным
static INT32 TRESHOLD_MIC_CONNECT_CHERRY = 30;      // Пороговое значение напряжения выше которого микрофон считается подключенным

static UINT32 currentStage;  // Текущая стадия запуска тестирования
static UINT32 cntTime;       // Счетчик для запуска тестирования
static UINT32 cntInitGpio;   // Количество проинициализированных пинов
static UINT32 cntManualEra;
static UINT32 cntIterTestCallEra;
static UINT32 cntTestCallEra;
static UINT32 cntPause;
static UINT32 stckSosCnt;
static UINT32 stckServiceCnt;
static UINT32 updateADC_temp;
static UINT32 sosServiceCnt;
static FLOAT32 KP_KL30 = 1;       // Коэффициент пропорциональности KL30
static UINT8 stageWd = 0;
static UINT32 wdCnt = 0;

/* Local function prototypes ====================================================================*/
static BOOLEAN startWatching(void); // Запуск сервиса
static BOOLEAN checkPin(INT32 fd, char *description); // Проверка инициализации пина
static BOOLEAN checkClosePin(INT32 fd, char *description); // Проверка закрытия пина
static BOOLEAN initW(void);

static void kl30Evnt(void);      // Событие клеммы 30
static void watchProcess(void);  // Опрос GPIO
static void getADCVal(void);     // Считать показания АЦП
static void updateTemp(void);    // Получить текущую температуру
static void gpio_timer_handler(void);
static void sos_on_adc(void);

static void currentDiagSosState(void);  // Обновление состояния вывода DIAG_SOS_IN
static void currentDiagSpNState(void);  // Обновление состояния вывода DIAG_SP_N (GPIO_06)
static void currentKL15State(void);     // Обновление состояния входа IGNIT (GPIO_05)
static void currentServiceBState(void); // Обновление состояния кнопки сервис (GPIO_08)
static void currentAccState(void);      // Обновление состояния входа ACC (GPIO_04)
static void manualSos(void);            // Отслеживание запуска ручного вызова ЭРА (кнопка SOS удерживалась нажатой дольше 1 сек)
static void eraTestCall(void);          // Отслеживание запуска тестового вызова ЭРА (кнопка SOS была быстро нажата 5 раз)
static void sosServiceTest(void);       // Отслеживание запуска тестового вызова ЭРА (кнопки SOS и SERVICE были зажаты одновременно на 3 сек.)
static void stuckSos(void);             // Отслеживание застревания кнопки SOS
static void stuckService(void);         // Отслеживание застревания кнопки SERVICE
static void diagMicState(void);         // Отслеживание подключения/отключения микрофона
static void poolingAdc(void);           // Опрос АЦП
static void testMode(void);             // Запуск режима тестирования
static void notifyDog(void);            // Управление WD для MODEL_ID 11
/* Static functions =============================================================================*/

extern BOOLEAN init_gpio(gpio_ev_cb ev_cb) {  // инициализация сервиса
    if (!isInit) {
        // Если сервис еще не запущен
        if (!startWatching()) { // Запуск сервиса
            return FALSE;
        }
        add_gpio_ev_handler(ev_cb);
        onIgnTest = TRUE;
        currentStage = 0;
        cntTime = 0;
        cntInitGpio = 0;
        cntManualEra = 0;
        hasEraCall = FALSE;
        cntIterTestCallEra = 0;
        cntTestCallEra = 0;
        cntPause = 0;
        stckSosCnt = 0;
        stageWd = 0;
        wdCnt = 0;
        stckServiceCnt = 0;
        updateADC_temp = 0;
        sosServiceCnt = 0;
        isInit = FALSE;
        isMicInit = FALSE;
        onOffMode(TRUE);
        needCycle = TRUE;
        send_to_gpio_task(GPIO_NEW_EVENT, EV_SOS_UNSTUCK, 0);     // Кнопка SOS изначально считается незастрявшей
        send_to_gpio_task(GPIO_NEW_EVENT, EV_SERVICE_UNSTUCK, 0); // Кнопка SERVICE изначально считается незастрявшей
        send_to_gpio_tt_task(SEND_GPIO_POOL, 0, 0);
    }
    return TRUE;
}

static BOOLEAN startWatching(void) {
    LOG_DEBUG("WT: Line watcher launched.");
    if (!initW()) {
        deinit_gpio();
        isMicInit = FALSE;
        isInit = FALSE;
        return FALSE;
    }
    return TRUE;
}

extern M2MB_OS_RESULT_E deinit_gpio(void) {
    // освобождение ресурсов, занятых сервисом gpio
    m2mb_gpio_close(SOS_BTN_fd);
    m2mb_gpio_close(ADC_NTC_fd);
    m2mb_gpio_close(DIAG_SOS_fd);
    m2mb_gpio_close(RED_MODE_CTRL_fd);
    m2mb_gpio_close(ACC_fd);
    m2mb_gpio_close(IGNIT_fd);
    m2mb_gpio_close(EN_AUDIO_fd);
    m2mb_gpio_close(SERVICE_BTN_fd);
    m2mb_gpio_close(SERVICE_CTRL_fd);
    m2mb_gpio_close(GREEN_MODE_CTRL_fd);

    m2mb_os_free(m2m_gpio_dcd);
    m2mb_os_free(m2m_gpio_dsr);
    m2mb_os_free(m2m_gpio_dtr);
    m2mb_os_free(m2m_gpio_rg);

    checkClosePin(SOS_BTN_fd, "SOS_BTN (GPIO_01)");
    checkClosePin(ADC_NTC_fd, "ADC_NTC (GPIO_02)");
    checkClosePin(RED_MODE_CTRL_fd, "RED_MODE_CTRL (GPIO_03)");
    checkClosePin(ACC_fd, "ACC (GPIO_04)");
    checkClosePin(IGNIT_fd, "IGNITION (GPIO_05)");
    checkClosePin(DIAG_SOS_fd, "DIAG_SOS_IN (GPIO_06)");
    checkClosePin(EN_AUDIO_fd, "EN_AUDIO (GPIO_07)");
    checkClosePin(SERVICE_BTN_fd, "SERVICE_BTN (GPIO_08)");
    checkClosePin(SERVICE_CTRL_fd, "SERVICE_CTRL (GPIO_09)");
    checkClosePin(GREEN_MODE_CTRL_fd, "GREEN_MODE_CTRL (GPIO_10)");

    needCycle = FALSE;
    close(dev);
    isInit = FALSE;
    isMicInit = FALSE;
    LOG_DEBUG("WT: deinit gpio was finished");
    return M2MB_OS_SUCCESS;
}

extern INT32 getMicAdc(void){
    return get_adc(3);
}

/* Global functions =============================================================================*/
INT32 GPIO_TT_task(INT32 type, INT32 param1, INT32 param2) {
    (void)param1;
    (void)param2;
    switch (type) {
        case SEND_GPIO_POOL:
            gpio_timer_handler();
            break;
        default:
            return M2MB_OS_SUCCESS;
    }
    return M2MB_OS_SUCCESS;
}

/***************************************************************************************************
   GPIO_task handles gpio. Нельзя вызывать эту функцию напрямую!
 **************************************************************************************************/
INT32 GPIO_task(INT32 type, INT32 param1, INT32 param2) {
    (void) param2;
//    if (type != EV_ADC_TIMER) LOG_DEBUG("GPIO_task command: %i", type);
    switch (type) {
        case GPIO_NEW_EVENT: // Событие GPIO
            switch (param1) {
                case EV_SOS_BTN_PRESSED:
                    if (callback != NULL) {
                        callback(EV_SOS_BTN_PRESSED);         // 1
                        if (!is_model_serv_butt()) callback(EV_TEST_SUCCESS_CONFIRMED);  // 2
                    }
                    break;
                case EV_SOS_BTN_RELEASED:
                    if (callback != NULL) callback(EV_SOS_BTN_RELEASED);  // 3
                    break;
                case EV_DIAG_SOS_ON:
                    if (callback != NULL) {
                        callback(EV_DIAG_SOS_ON);   // 4
                        if (is_diag_mic_disable()) callback(EV_DIAG_MIC_ON);
                    }
                    break;
                case EV_DIAG_SOS_OFF:
                    if (callback != NULL) {
                        callback(EV_DIAG_SOS_OFF);  // 5
                        if (is_diag_mic_disable()) callback(EV_DIAG_MIC_OFF);
                    }
                    break;
                case EV_ACC_OPEN:
                    if (callback != NULL) callback(EV_ACC_OPEN);     // 6
                    break;
                case EV_ACC_CLOSED:
                    if (callback != NULL) callback(EV_ACC_CLOSED);   // 7
                    break;
                case EV_T15_ON:
                    if (callback != NULL) callback(EV_T15_ON);       // 8
                    break;
                case EV_T15_OFF:
                    if (callback != NULL) callback(EV_T15_OFF);      // 9
                    break;
                case EV_DIAG_SP_N_ON:
                    if (callback != NULL) callback(EV_DIAG_SP_N_ON); // 10
                    break;
                case EV_DIAG_SP_N_OFF:
                    if (callback != NULL) callback(EV_DIAG_SP_N_OFF); // 11
                    break;
                case EV_SERVICE_BTN_ON:
                    if (callback != NULL)  {
                        callback(EV_SERVICE_BTN_ON); // 12
                        if (is_model_serv_butt()) callback(EV_TEST_SUCCESS_CONFIRMED);  // 3
                    }
                    break;
                case EV_SERVICE_BTN_OFF:
                    if (callback != NULL) callback(EV_SERVICE_BTN_OFF); // 13
                    break;
                case EV_T30_ON:
                    if (callback != NULL) callback(EV_T30_ON); // 14
                    break;
                case EV_T30_OFF:
                    if (callback != NULL) callback(EV_T30_OFF); // 15
                    break;
                case EV_ECALL_TEST_ERA_STARTED:
                    if (callback != NULL) callback(EV_ECALL_TEST_ERA_STARTED); // 16
                    break;
                case EV_ECALL_MANUAL_SOS:
                    if (callback != NULL) callback(EV_ECALL_MANUAL_SOS); // 17
                    break;
                case EV_SOS_STUCK:
                    if (callback != NULL) callback(EV_SOS_STUCK);  // 18
                    break;
                case EV_SOS_UNSTUCK:
                    if (callback != NULL) callback(EV_SOS_UNSTUCK); // 19
                    break;
                case EV_DIAG_MIC_ON:
                    if (callback != NULL && !is_diag_mic_disable()) {
                        callback(EV_DIAG_MIC_ON); // 20
//                        if (is_diag_sos_disable()) callback(EV_DIAG_SOS_ON);
                    }
                    break;
                case EV_DIAG_MIC_OFF:
                    if (callback != NULL && !is_diag_mic_disable()) {
                        callback(EV_DIAG_MIC_OFF); // 21
//                        if (is_diag_sos_disable()) callback(EV_DIAG_SOS_OFF);
                    }
                    break;
                case EV_ADC_NTC_ON:
                    if (callback != NULL) callback(EV_ADC_NTC_ON); // 22
                    break;
                case EV_ADC_NTC_OFF:
                    if (callback != NULL) callback(EV_ADC_NTC_OFF); // 23
                    break;
                case EV_SERVICE_STUCK:
                    if (callback != NULL) callback(EV_SERVICE_STUCK); // 24
                    break;
                case EV_SERVICE_UNSTUCK:
                    if (callback != NULL) callback(EV_SERVICE_UNSTUCK); // 25
                    break;
                default:
                    return M2MB_OS_SUCCESS;
            }
            break;
    }
    return M2MB_OS_SUCCESS;
}

// Реализация
void add_gpio_ev_handler(gpio_ev_cb ev_cb){
    callback = ev_cb;
}

extern void mcpReset(BOOLEAN state){
    if (state == TRUE) {
        m2mb_gpio_write(SERVICE_BTN_fd, M2MB_GPIO_HIGH_VALUE);
    } else {
        m2mb_gpio_write(SERVICE_BTN_fd, M2MB_GPIO_LOW_VALUE);
    }
}

static BOOLEAN checkClosePin(INT32 fd, char *description){
    if (fd == -1) {
        LOG_ERROR("WT: Unable to close pin > %s", description);
        return FALSE;
    } else {
        //LOG_TRACE("WT: Pin closed successfully > %s", description);
        return TRUE;
    }
}

static BOOLEAN checkPin(INT32 fd, char *description) {
    if (fd == -1) {
        LOG_ERROR("WT: Unable to initialize pin > %s", description);
        return FALSE;
    } else {
        LOG_TRACE("WT: Pin initialized successfully > %s", description);
        return TRUE;
    }
}

extern INT32 getKL30Adc(void){
    return get_adc(2) / 1000;
}

extern INT32 getBatAdc(void){
    return get_adc(1) / 1000;
}

// 7.38594267288645 + 0.0522633119851168*x + 4.70046001755387e-13*x^5 + 8.03474310962977e-7*x^3 - 0.000305967575565296*x^2 - 9.76177049177583e-10*x^4
static void getADCVal(void){
    currentADC_BATTERY_voltage = get_adc(1) / 1000 * KP_ADC_BAT;
    currentKL30voltage = get_adc(2) / 1000;
    if(get_current_rev() == 0){
        // Пересчитываем коэффициент для КЛ30
        FLOAT32 A = powf(currentKL30voltage, 5);
        FLOAT32 B = powf(currentKL30voltage, 4);
        FLOAT32 C = powf(currentKL30voltage, 3);
        FLOAT32 D = powf(currentKL30voltage, 2);
        if(with_can()){
            KP_KL30 = 7.38594267288645 + 0.0522633119851168 * currentKL30voltage + 4.70046001755387 * powf(10, -13) * A
                      + 8.03474310962977 * powf(10, -7) * C - 0.000305967575565296 * D - 9.76177049177583 * powf(10, -10) * B;
        } else {
            KP_KL30 = 10.7618591898727 + 8.94354306696602 * powf(10, -8) * C + 1.35065059398316 * powf(10, -13) * A
                      - 8.58869213429814 * powf(10, -5) * currentKL30voltage - 1.42703392529191 * powf(10, -5) * D - 1.82339403683989
                                                                                                                     * powf(10, -10) * B;
        }

        currentKL30voltage = currentKL30voltage * KP_KL30;
    } else if(get_current_rev() == 1){
        KP_KL30 = 9.66;
        currentKL30voltage = currentKL30voltage * KP_KL30;
    } else if (get_current_rev() == 2){
        KP_KL30 = 20.13;

        currentKL30voltage = currentKL30voltage * KP_KL30;
        LOG_DEBUG("currentKL30voltage:%d", currentKL30voltage);
    }

    currentDIAG_MIC_N_voltage = get_adc(3) / 1000;
//    LOG_DEBUG("KL30 %d MIC %d", currentKL30voltage, currentDIAG_MIC_N_voltage);
}

static void kl30Evnt(void){
    prevKL30State = currentKL30State;
    if(currentKL30voltage > THRESHOLD_VALUE_KL30){ // Клемма  30 замкнута
        currentKL30State = TRUE;
        if(prevKL30State == FALSE) {
            send_to_gpio_task(GPIO_NEW_EVENT, EV_T30_ON, 0);   // 1
        }
    } else{  // Клемма  30 разомкнута
        currentKL30State = FALSE;
        if(prevKL30State == TRUE) {
            send_to_gpio_task(GPIO_NEW_EVENT, EV_T30_OFF, 0);  // 2
        }
    }
}

extern BOOLEAN redMode(BOOLEAN state){
    INT32 res;
    if (state == TRUE) {
        res = m2mb_gpio_write(RED_MODE_CTRL_fd, M2MB_GPIO_HIGH_VALUE);
    } else {
        res = m2mb_gpio_write(RED_MODE_CTRL_fd, M2MB_GPIO_LOW_VALUE);
    }
    if(res == 0){
        currentRedModeState = state;
        return TRUE;
    }
    LOG_DEBUG("red_mode error");
    return FALSE;
}

extern BOOLEAN getRedMode(void){
    return currentRedModeState;
}

extern BOOLEAN onOffMode(BOOLEAN state){
    INT32 resp = 0;
    m2m_gpio_dsr->m2m_gpio_val = state;
    resp = ioctl(dev, IOCTL_M2M_APP_GPIO_SET_DIR, m2m_gpio_dsr);
    if(resp){
        perror("WT: change ON_OFF_MODE failure \n");
        return FALSE;
    }
    currentOnOffModeState = state;
    return TRUE;
}

extern BOOLEAN getOnOffMode(void){
    return currentOnOffModeState;
}

extern BOOLEAN gpsLnaEn(BOOLEAN state){
    char cmd[20];
    sprintf(cmd, "AT$GPSP=%d\r", state);
    char resp[200];
    ati_sendCommandEx(1000, resp, sizeof(resp), cmd);
    if(strstr (resp, "OK") == NULL){ // Если сообщение не содержит "OK"
        LOG_ERROR("WT: Can't change state GPS_LNA_EN");
        return FALSE;
    }
    currentGpsLnaEn = state;
    return TRUE;
}

extern BOOLEAN getGpsLnaEn(void){
    return currentGpsLnaEn;
}

extern INT32 getBatteryVolatage(void){
    return currentADC_BATTERY_voltage;
}

extern INT32 getKL30voltage(void){
    return currentKL30voltage;
}

extern BOOLEAN getKL30(void){
    return currentKL30State;
}

extern INT32 getDiagMicVoltage(void){
    return currentDIAG_MIC_N_voltage;
}

static BOOLEAN initW(void) {
    cntInitGpio = 0;
    // SOS_BTN/GPIO_01/С8 (INPUT) +
    SOS_BTN_fd = m2mb_gpio_open(PREFIX SOS_BTN, 0);
    m2mb_gpio_ioctl(SOS_BTN_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_INPUT);
    m2mb_gpio_ioctl(SOS_BTN_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_NO_PULL);
    if (checkPin(SOS_BTN_fd, "SOS_BTN (GPIO_01)")) cntInitGpio++; // 1

    // ADC_NTC GPIO_02 ПОКА НЕ ИСПОЛЬЗУЕТСЯ -
    ADC_NTC_fd = m2mb_gpio_open(PREFIX SOS_CTRL, 0);
    m2mb_gpio_ioctl(ADC_NTC_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_INPUT);
    m2mb_gpio_ioctl(ADC_NTC_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_NO_PULL);
    m2mb_gpio_ioctl(ADC_NTC_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_MEDIUM_DRIVE);
    if (checkPin(ADC_NTC_fd, "(GPIO_02)")) cntInitGpio++; // 2

    // RED_MODE_CTRL/GPIO_03/С10 (OUTPUT) +
    RED_MODE_CTRL_fd = m2mb_gpio_open(PREFIX RED_MODE_CTRL, 0);
    m2mb_gpio_ioctl(RED_MODE_CTRL_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_OUTPUT);
    m2mb_gpio_ioctl(RED_MODE_CTRL_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_HIGH_DRIVE);
    m2mb_gpio_ioctl(RED_MODE_CTRL_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_NO_PULL);
    if (checkPin(RED_MODE_CTRL_fd, "RED_MODE_CTRL (GPIO_03)")) cntInitGpio++; // 3

    // ACC/GPIO_04/С11 (INPUT) +
    ACC_fd = m2mb_gpio_open(PREFIX ACC, 0);
    if(with_can()){
        m2mb_gpio_ioctl(ACC_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_OUTPUT);
        m2mb_gpio_ioctl(ACC_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_MEDIUM_DRIVE);
        if (checkPin(ACC_fd, "WD_OUT (GPIO_04)")) cntInitGpio++; // 3
    } else {
        m2mb_gpio_ioctl(ACC_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_INPUT);
        m2mb_gpio_ioctl(ACC_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_LOW_DRIVE);
        m2mb_gpio_ioctl(ACC_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_NO_PULL);
        if (checkPin(ACC_fd, "ACC (GPIO_04)")) cntInitGpio++; // 4
    }

    // IGNITION/GPIO_05/В14 (INPUT) +
    IGNIT_fd = m2mb_gpio_open(PREFIX IGNIT, 0);
    m2mb_gpio_ioctl(IGNIT_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_INPUT);
    m2mb_gpio_ioctl(IGNIT_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_PULL_UP);
    m2mb_gpio_ioctl(IGNIT_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_LOW_DRIVE);
    if (checkPin(IGNIT_fd, "IGNITION (GPIO_05)")) cntInitGpio++; // 5

    // DIAG_SOS_IN/GPIO_06/С12 (INPUT) +
    DIAG_SOS_fd = m2mb_gpio_open(PREFIX DIAG_SOS_IN, 0);
    if (with_can()){
        m2mb_gpio_ioctl(DIAG_SOS_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_OUTPUT);
        m2mb_gpio_ioctl(DIAG_SOS_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_MEDIUM_DRIVE);
        if (checkPin(DIAG_SOS_fd, "LED_PWR_CTRL (GPIO_06)")) cntInitGpio++; // 3
    } else {
        m2mb_gpio_ioctl(DIAG_SOS_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_INPUT);
        m2mb_gpio_ioctl(DIAG_SOS_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_NO_PULL);
        m2mb_gpio_ioctl(DIAG_SOS_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_LOW_DRIVE);
        if (checkPin(DIAG_SOS_fd, "DIAG_SOS_IN (GPIO_06)")) cntInitGpio++; // 6
    }

    // EN_AUDIO/GPIO_07/С13 (OUTPUT) +
    EN_AUDIO_fd = m2mb_gpio_open(PREFIX EN_AUDIO, 0);
    m2mb_gpio_ioctl(EN_AUDIO_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_OUTPUT);
    m2mb_gpio_ioctl(EN_AUDIO_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_NO_PULL);
    m2mb_gpio_ioctl(EN_AUDIO_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_MEDIUM_DRIVE);
    if (checkPin(EN_AUDIO_fd, "EN_AUDIO (GPIO_07)")) cntInitGpio++; // 7

    // SERVICE_BTN/GPIO_08/К15 (INPUT) +
    SERVICE_BTN_fd = m2mb_gpio_open(PREFIX SERVICE_BTN, 0);
    if (with_can()) { // Если это блок с CAN
        m2mb_gpio_ioctl(SERVICE_BTN_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_OUTPUT);
        m2mb_gpio_ioctl(SERVICE_BTN_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_NO_PULL);
        m2mb_gpio_ioctl(SERVICE_BTN_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_MEDIUM_DRIVE);
        if (checkPin(SERVICE_BTN_fd, "MCP_RESET (GPIO_08)")) cntInitGpio++; // 8
    } else {
        m2mb_gpio_ioctl(SERVICE_BTN_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_INPUT);
        m2mb_gpio_ioctl(SERVICE_BTN_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_NO_PULL);
        m2mb_gpio_ioctl(SERVICE_BTN_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_LOW_DRIVE);
        if (checkPin(SERVICE_BTN_fd, "SERVICE_BTN (GPIO_08)")) cntInitGpio++; // 8
    }

    // SERVICE_CTRL/GPIO_09/L15 (OUTPUT) +
    SERVICE_CTRL_fd = m2mb_gpio_open(PREFIX SERVICE_CTRL, 0);
    m2mb_gpio_ioctl(SERVICE_CTRL_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_OUTPUT);
    m2mb_gpio_ioctl(SERVICE_CTRL_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_NO_PULL);
    m2mb_gpio_ioctl(SERVICE_CTRL_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_MEDIUM_DRIVE);
    if (checkPin(SERVICE_CTRL_fd, "SERVICE_CTRL (GPIO_09)")) cntInitGpio++; // 9

    // GREEN_MODE_CTRL/GPIO_10/G15 (OUTPUT) +
    GREEN_MODE_CTRL_fd = m2mb_gpio_open(PREFIX GREEN_MODE_CTRL, 0);
    m2mb_gpio_ioctl(GREEN_MODE_CTRL_fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_OUTPUT);
    m2mb_gpio_ioctl(GREEN_MODE_CTRL_fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_NO_PULL);
    m2mb_gpio_ioctl(GREEN_MODE_CTRL_fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_MEDIUM_DRIVE);
    if (checkPin(GREEN_MODE_CTRL_fd, "GREEN_MODE_CTRL (GPIO_10)")) cntInitGpio++; // 10

    // UART PINS
    dev = open(GPIO_DEV_PATH, O_RDWR);

    // SOS_CTRL DCD/N14 (20) +
    m2m_gpio_dcd = (m2m_gpio_info *)malloc(sizeof(m2m_gpio_info));
    memset(m2m_gpio_dcd, 0x00, sizeof(m2m_gpio_info));
    m2m_gpio_dcd->m2m_gpio_num = 20;
    m2m_gpio_dcd->m2m_gpio_dir = M2M_APP_GPIO_DIR_OUT;

    // ON_CHARGE RING/R14 (22) +
    m2m_gpio_rg = (m2m_gpio_info *)malloc(sizeof(m2m_gpio_info));
    memset(m2m_gpio_rg, 0x00, sizeof(m2m_gpio_info));
    m2m_gpio_rg->m2m_gpio_num = 22;
    m2m_gpio_rg->m2m_gpio_dir = M2M_APP_GPIO_DIR_OUT;

    // ON_OFF_MODE DSR/P14 (23) +
    m2m_gpio_dsr = (m2m_gpio_info *)malloc(sizeof(m2m_gpio_info));
    memset(m2m_gpio_dsr, 0x00, sizeof(m2m_gpio_info));
    m2m_gpio_dsr->m2m_gpio_num = 23;
    m2m_gpio_dsr->m2m_gpio_dir = M2M_APP_GPIO_DIR_OUT;

    // DIAG_SP_N DTR/M14 (24) +
    m2m_gpio_dtr = (m2m_gpio_info *)malloc(sizeof(m2m_gpio_info));
    memset(m2m_gpio_dtr, 0x00, sizeof(m2m_gpio_info));
    m2m_gpio_dtr->m2m_gpio_num = 24;
    m2m_gpio_dtr->m2m_gpio_dir = M2M_APP_GPIO_DIR_IN;
    m2m_gpio_dtr->m2m_gpio_val = M2M_APP_GPIO_IN_NP;
    if (ioctl(dev, IOCTL_M2M_APP_GPIO_SET_DIR, m2m_gpio_dtr)) {
        LOG_ERROR("WT: Configure DIAG_SP_N DTR (24) failure");
    } else {
        LOG_DEBUG("WT: Configure DIAG_SP_N DTR (24) success");
        cntInitGpio++;
    }

    LOG_DEBUG("%d out of %d pins initialized", cntInitGpio, 11);
    if (cntInitGpio != 11) {
        return FALSE;
    }
    return TRUE;
}

extern BOOLEAN getSosCtrl(void){
    return currentSosCtrl;
}

// TRUE/FALSE - подключена/отключена
extern BOOLEAN getADC_NTC_State(void){
    return currentAdcNtcState;
}

extern BOOLEAN sosCtrl(BOOLEAN state) {
    INT32 resp = 0;
    m2m_gpio_dcd->m2m_gpio_val = state;
    resp = ioctl(dev, IOCTL_M2M_APP_GPIO_SET_DIR, m2m_gpio_dcd);
    if(resp){
        LOG_ERROR("WT: change SOS_CTRL failure \n");
        return FALSE;
    }
    currentSosCtrl = state;
    return TRUE;
}

extern void serviceControl(BOOLEAN state) {
    INT32 res = -1;
    if (state == TRUE) {
        res = m2mb_gpio_write(SERVICE_CTRL_fd, M2MB_GPIO_HIGH_VALUE);
    } else {
        res = m2mb_gpio_write(SERVICE_CTRL_fd, M2MB_GPIO_LOW_VALUE);
    }
    if (res == 0) currentServiceCtrl = state;
}

extern BOOLEAN getServiceCtrl(void){
    return currentServiceCtrl;
}

extern void greenModeCtrl(BOOLEAN state) {
    INT32 res = -1;
    if (state == TRUE) {
        res = m2mb_gpio_write(GREEN_MODE_CTRL_fd, M2MB_GPIO_HIGH_VALUE);
    } else {
        res = m2mb_gpio_write(GREEN_MODE_CTRL_fd, M2MB_GPIO_LOW_VALUE);
    }
    if (res == 0) currentGreenModeCtrl = state;
}

extern BOOLEAN getGreenModeCtrl(void){
    return currentGreenModeCtrl;
}

extern void enAudio(BOOLEAN state) {
    INT32 res;
    if (get_model_id() == 10){ // Если Cherry
        INT32 res1;
        INT32 res2;
        if (state == TRUE) {
            res1 = m2mb_gpio_write(SERVICE_CTRL_fd, M2MB_GPIO_HIGH_VALUE);
            res2 = m2mb_gpio_write(EN_AUDIO_fd, M2MB_GPIO_HIGH_VALUE);
        } else {
            res1 = m2mb_gpio_write(SERVICE_CTRL_fd, M2MB_GPIO_LOW_VALUE);
            res2 = m2mb_gpio_write(EN_AUDIO_fd, M2MB_GPIO_LOW_VALUE);
        }
        if ((res1 == 0) & (res2 == 0)){res = 0;} else { res = -1;}
    } else { // Если не cherry
        if (state == TRUE) {
            res = m2mb_gpio_write(EN_AUDIO_fd, M2MB_GPIO_HIGH_VALUE);
        } else {
            res = m2mb_gpio_write(EN_AUDIO_fd, M2MB_GPIO_LOW_VALUE);
        }
    }
    if(res == 0) currentEnableAudioState = state;
}

extern BOOLEAN getEnAudio(void){
    return currentEnableAudioState;
}

static void updateTemp(void){
    currentTemp = get_temp();
}

extern INT32 getCurrentTemp(void){
    return currentTemp;
}

static void testMode(void){
    if (is_model_serv_butt()) sosServiceTest(); // Отслеживание запуска тестового вызова ЭРА через одновременное нажатие кнопок SOS и SERVICE в течении 3 секунд
    else eraTestCall(); // Отслеживание запуска тестового вызова ЭРА через 5и кратное быстрое нажатие кнопки SOS.
}

// Процедуры которые выполняет таймер
static void watchProcess(void) {
    if(with_can()){
        notifyDog();
    }
    poolingAdc();                 // Опрос АЦП
    if(get_current_rev() == 0){   // Для плат Chery со стандартным бипом
        currentDiagSosState();    // Проверка кнопки SOS. События: нажата/отпущена/подключена/отключена
        diagMicState();           // Проверка микрофона. События: подключен/getAdcNtc();            // Проверка ADC_NTC. События: подключено/отключено
    } else if(is_two_wire_bip()){ // Для плат Chery с двухпроводным бипом
        sos_on_adc();             // Ошибка микрофона, кнопки SOS
    }

    stuckSos();             // Проверка кнопки SOS. События: зажата/отжата
    stuckService();         // Проверка кнопки SERVICE. События: зажата/отжата
    currentKL15State();     // Проверка KL15. События: замкнута/разомкнута
    currentServiceBState(); // Проверка кнопки SERVICE. События: нажата/отпущена
    currentAccState();      // Проверка KL_ACC. События: замкнута/разомкнута
    manualSos();            // Отслеживание запуска ручного вызова ЭРА. События: ручной вызов эра
    kl30Evnt();             // Проверка KL30. События: замкнута/разомкнута
    currentDiagSpNState();  // Проверка динамика. События: подключен/отключен
    testMode();             // Определение запуска режима тестирования
}

static void poolingAdc(void){
    if(is_two_wire_bip()){
        updateTemp();
        getADCVal();
    } else {
        if (updateADC_temp > GPIO_TIMER_INTERVAL / GPIO_TIMER_INTERVAL) {
            updateTemp();
            getADCVal();
            updateADC_temp = 0;
        }
        updateADC_temp++;
    }
}

extern void reset_diag_mic(void){
    currentDiagMicState = !currentDiagMicState;
}

extern void reset_diag_speaker(void){
    currentSpeakerState = !currentSpeakerState;
}


static void diagMicState(void) {
    prevDiagMicState = currentDiagMicState;
    // LOG_DEBUG("MIC VOLT %d, temp %d", currentDIAG_MIC_N_voltage, currentTemp);
    if (get_model_id() == 4) { // Для MODEL_ID = 4 с измененной схемотехникой.
        /*
         * Когда MUTE включен:
         *  - подключенный микрофон до 500 мВ
         *  - отключенный микрофон выше 500 мВ
         *
         *  Когда MUTE выключен
         *  - подключенный микрофон более 800 мВ
         *  - отключенный микрофон менее 800 мВ
         *
         *  Все значения взяты на глаз. (053130r4_12)
         */
        if (currentDIAG_MIC_N_voltage < 500 || currentDIAG_MIC_N_voltage > 800){
            currentDiagMicState = TRUE;  // Микрофон подключен
            if (prevDiagMicState != currentDiagMicState || !isInit) {
                send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_MIC_ON, 0); // 20
            }
        } else {
            currentDiagMicState = FALSE; // Микрофон отключен
            if (prevDiagMicState != currentDiagMicState || !isInit) {
                send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_MIC_OFF, 0); // 21
            }
        }
    } else if ((get_model_id() == 10) || with_can()) { // Для Cherry
        if (currentDIAG_MIC_N_voltage >= TRESHOLD_MIC_CONNECT_CHERRY && currentDIAG_MIC_N_voltage <= 650) {
                if (micStage == 1) micCnt = 0; // Если ранее микрофон был отключен, то сброс счетчика
                micStage = 0;
                if (micCnt > MIC_NUMB_MISURMENTS){
                    currentDiagMicState = TRUE;  // Микрофон подключен
                    if (prevDiagMicState != currentDiagMicState || !isMicInit) {
                        send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_MIC_ON, 0); // 20
                        isMicInit = TRUE;
                    }
                    micCnt = 0;
                } else {
                    micCnt++;
                }
            } else {
                if (micStage == 0) micCnt = 0; // Если ранее микрофон был подключен, то сброс счетчика
                micStage = 1;
                if (micCnt > MIC_NUMB_MISURMENTS) {
                    currentDiagMicState = FALSE; // Микрофон отключен
                    if (prevDiagMicState != currentDiagMicState || !isMicInit) {
                        send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_MIC_OFF, 0); // 21
                        isMicInit = TRUE;
                    }
                    micCnt = 0;
                } else {
                    micCnt++;
                }
            }
        } else { // Для остальных MODEL_ID
        if(currentDIAG_MIC_N_voltage >= TRESHOLD_MIC_CONNECT){
            currentDiagMicState = TRUE;  // Микрофон подключен
            if(prevDiagMicState != currentDiagMicState || !isInit){
                send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_MIC_ON, 0); // 20
            }
        } else{
            currentDiagMicState = FALSE; // Микрофон отключен
            if(prevDiagMicState != currentDiagMicState || !isInit){
                send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_MIC_OFF, 0); // 21
            }
        }
    }
}

extern BOOLEAN getMicState(void){
    return currentDiagMicState;
}

static void sosServiceTest(void){
    if(currentSOS_BTNState && currentServiceBTNState){
        sosServiceCnt++;
        if(sosServiceCnt > SOS_SERVICE_TIME_TEST/GPIO_TIMER_INTERVAL && !hasEraCall){
            send_to_gpio_task(GPIO_NEW_EVENT, EV_ECALL_TEST_ERA_STARTED, 0); // 5
            sosServiceCnt = 0;
        }
    } else{
        sosServiceCnt = 0;
    }
}

extern UINT8 getValServiceBtn(void){
    return valSrvice;
}

static void stuckService(void){
    if(currentServiceBTNState){
        stckServiceCnt++;
        if(stckServiceCnt == STUCK_TIME/GPIO_TIMER_INTERVAL) { // Кнопка SERVICE зажата больше 10 сек
            serviceStckState = TRUE;
            send_to_gpio_task(GPIO_NEW_EVENT, EV_SERVICE_STUCK, 0);  // 24
        }
    } else{
        stckServiceCnt = 0;
        if (serviceStckState == TRUE) {  // Если ранее было зафиксировано застревание кнопки SERVICE
            serviceStckState = FALSE;
            send_to_gpio_task(GPIO_NEW_EVENT, EV_SERVICE_UNSTUCK, 0); // 25
        }
    }
}

static void stuckSos(void){
    if(currentSOS_BTNState){
        stckSosCnt++;
        if(stckSosCnt == STUCK_TIME/GPIO_TIMER_INTERVAL) { // Кнопка SOS зажата больше 10 сек
            sosStckState = TRUE;
            send_to_gpio_task(GPIO_NEW_EVENT, EV_SOS_STUCK, 0);  // 3
        }
    } else{
        stckSosCnt = 0;
        if (sosStckState == TRUE) {  // Если ранее было зафиксировано застревание кнопки SOS
            sosStckState = FALSE;
            send_to_gpio_task(GPIO_NEW_EVENT, EV_SOS_UNSTUCK, 0); // 4
        }
    }
}

static void eraTestCall(void) {
    if (currentSOS_BTNState) {
        if (cntIterTestCallEra > AVERAGE_FAST_PRESS_TIME / GPIO_TIMER_INTERVAL)
            goto reset;// Кнопка SOS удерживается дольше допустимого - сброс
        cntPause = 0;
        cntIterTestCallEra++;
    } else {
        if (cntTestCallEra == 6 && !currentServiceBTNState) { // При 5 нажатий на кнопку
            if (!hasEraCall) { // Запуск ручного вызова ЭРА (EV_ECALL_TEST_ERA_STARTED)
                hasEraCall = TRUE;
                LOG_DEBUG("WT: >>>>>>>>>>> Launching a testing procedure");
                send_to_gpio_task(GPIO_NEW_EVENT, EV_ECALL_TEST_ERA_STARTED, 0); // 5
                // Сброс счетчика запуска экстренного вызова
                cntManualEra = 0;
                goto reset;
            } else { // сброс
                LOG_DEBUG(">>>>>>>>>>>>>>>>>>>>>>>>>>> RESET");
                reset:
                // Сброс счетчиков запуска процедуры тестирования
                cntPause = 0;
                cntTestCallEra = 0;
                cntIterTestCallEra = 0;
                return;
            }
        } else { // 1,2,3,4 отпускание кнопки
            cntIterTestCallEra = 0;
            if (cntPause == 0) cntTestCallEra++;
            if (cntPause > PAUSE_TIME / GPIO_TIMER_INTERVAL)
                goto reset;// Если пауза между нажатиями больше допустимой - сброс счетчиков запуска процедуры тестирования
            cntPause++;
        }
    }
}

static void manualSos(void) {
    if (currentSOS_BTNState) { // Если нажата кнопка SOS
        if (cntManualEra > TIME_MANUAL_CALL_ERA / GPIO_TIMER_INTERVAL) {
            if (!hasEraCall) { // Запуск ручного вызова ЭРА (EV_ECALL_MANUAL_SOS)
                hasEraCall = TRUE;
                LOG_DEBUG("WT: >>>>>>>>>>> Performed a combination of actions to launch an ERA emergency call");
                send_to_gpio_task(GPIO_NEW_EVENT, EV_ECALL_MANUAL_SOS, 0);  // 6
                // Сброс счетчика запуска экстренного вызова
                cntManualEra = 0;
                // Сброс счетчиков запуска процедуры тестирования
                cntPause = 0;
                cntTestCallEra = 0;
                cntIterTestCallEra = 0;
            }
        }
        cntManualEra++;
    } else {
        hasEraCall = FALSE;
        // Сброс счетчика запуска экстренного вызова
        cntManualEra = 0;
    }
}

extern BOOLEAN onCharge(BOOLEAN state){
    INT32 resp = 0;
    m2m_gpio_rg->m2m_gpio_val = state;
    resp = ioctl(dev, IOCTL_M2M_APP_GPIO_SET_DIR, m2m_gpio_rg);
    if(resp){
        perror("WT: change SOS_CTRL failure \n");
        return FALSE;
    }
    currentOnCharge = state;
    return TRUE;
}

extern BOOLEAN getOnCharge(void){
    return currentOnCharge;
}

extern void setWdOut(BOOLEAN state){
    INT32 res = -1;
    if (state == TRUE) {
        res = m2mb_gpio_write(ACC_fd, M2MB_GPIO_HIGH_VALUE);
    } else {
        res = m2mb_gpio_write(ACC_fd, M2MB_GPIO_LOW_VALUE);
    }
    if (res == 0) currentWdState = state;
}

extern UINT8 getCurrWdOutState(void){
    return currentWdState;
}

static void notifyDog(void){
    switch (stageWd) {
        case 0: // Ждем 5 сек и выставляем лог 1
            if (wdCnt >= ((UINT32)NOTIFY_DOG_INTERVAL)/((UINT32)GPIO_TIMER_INTERVAL)) {
                setWdOut(TRUE);
                stageWd = 1;
                wdCnt = 0;
            } else {
                wdCnt++;
            }
            break;
        case 1: // Ждем 500 мСек и убираем лог 1
            if(wdCnt >= ((UINT32)WIDTH_PULSE_FOR_DOG/((UINT32)GPIO_TIMER_INTERVAL))){
                setWdOut(FALSE);
                stageWd = 0;
                wdCnt = 0;
            }else {
                wdCnt++;
            }
            break;
        default:
            stageWd = 0;
            break;
    }
}

static void currentAccState(void) {
    if (with_can()){
        prevACCState = currentACCState;
        M2MB_GPIO_VALUE_E valueACC;
        m2mb_gpio_read(ACC_fd, &valueACC);
        if (valueACC != 1) {
            currentACCState = TRUE;
            if (currentACCState != prevACCState || !isInit) { // Клемма АСС замкнута
                send_to_gpio_task(GPIO_NEW_EVENT, EV_ACC_CLOSED, 0);  // 7
            }
        } else {
            currentACCState = FALSE;
            if (currentACCState != prevACCState || !isInit) { // Клемма АСС разомкнута
                send_to_gpio_task(GPIO_NEW_EVENT, EV_ACC_OPEN, 0);  // 8
            }
        }
    }
}



static void currentServiceBState(void) {
    prevServiceBTNState = currentServiceBTNState;
    M2MB_GPIO_VALUE_E valueService;
    m2mb_gpio_read(SERVICE_BTN_fd, &valueService);
    valSrvice = valueService;
    if (valueService == 1) {
        currentServiceBTNState = TRUE;
        if (currentServiceBTNState != prevServiceBTNState || !isInit) { // Кнопка SERVICE нажата
            send_to_gpio_task(GPIO_NEW_EVENT, EV_SERVICE_BTN_ON, 0);  // 9
        }
    } else {
        currentServiceBTNState = FALSE;
        if (currentServiceBTNState != prevServiceBTNState || !isInit) { // Кнопка SERVICE отпущена
            send_to_gpio_task(GPIO_NEW_EVENT, EV_SERVICE_BTN_OFF, 0);  // 10
        }
    }
}

extern BOOLEAN getServiceState(void){
    return currentServiceBTNState;
}

static M2MB_GPIO_VALUE_E prev_value = 0;
static void currentKL15State(void) {
    prevIgnitionState = currentIgnitionState;
    M2MB_GPIO_VALUE_E value;
    m2mb_gpio_read(IGNIT_fd, &value);
    if(prev_value != value) {
        ign_cnt = 0;
    }
    if (value == 0) {
        if(ign_cnt >= IGN_FIX_TIME/GPIO_TIMER_INTERVAL) {
            currentIgnitionState = TRUE;
            if (prevIgnitionState != currentIgnitionState || !isInit) { // Зажигание включено
                send_to_gpio_task(GPIO_NEW_EVENT, EV_T15_ON, 0);  // 11
            }
            ign_cnt = 0;
        } else {
            ign_cnt++;
        }
    } else {
        if(ign_cnt >= IGN_FIX_TIME/GPIO_TIMER_INTERVAL) {
            currentIgnitionState = FALSE;
            if (prevIgnitionState != currentIgnitionState || !isInit) { // Зажигание выключено
                send_to_gpio_task(GPIO_NEW_EVENT, EV_T15_OFF, 0);  // 12
            }
            ign_cnt = 0;
        } else {
            ign_cnt++;
        }
    }
    prev_value = value;
}

extern BOOLEAN isIgnition(void) {
    return currentIgnitionState;
}

extern BOOLEAN getAccState(void){
    return currentACCState;
}

static void currentDiagSpNState(void) {
    prevSpeakerState = currentSpeakerState;
    ioctl(dev, IOCTL_M2M_APP_GPIO_GET_VAL, m2m_gpio_dtr);
//    LOG_DEBUG("UART_DTR %d", m2m_gpio_dtr->m2m_gpio_val);
    if (m2m_gpio_dtr->m2m_gpio_val == 0) {
        currentSpeakerState = FALSE;
        if (currentSpeakerState != prevSpeakerState || !isInit) { // Динамик отключен
            send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_SP_N_OFF, 0); // 14
        }
    } else {
        currentSpeakerState = TRUE;
        if (currentSpeakerState != prevSpeakerState || !isInit) { // Динамик подключен
            send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_SP_N_ON, 0); // 15
        }
    }
}

extern BOOLEAN getSpeakerState(void){
    return currentSpeakerState;
}

extern BOOLEAN onLedPwrCtrl(BOOLEAN state){
    INT32 res = -1;
    if (state == TRUE) {
        res = m2mb_gpio_write(DIAG_SOS_fd, M2MB_GPIO_HIGH_VALUE);
    } else {
        res = m2mb_gpio_write(DIAG_SOS_fd, M2MB_GPIO_LOW_VALUE);
    }
    if(res == 0){
        currentLedPwrCtrl = state;
        return TRUE;
    }
    return FALSE;
}

extern UINT8 getValSosBnt(void){
    return valBtn;
}

extern UINT8 getValSosDiag(void){
    return valDIAG;
}

static void sos_on_adc(void){
//    LOG_DEBUG("ADC VOLT %d, temp %d", currentDIAG_MIC_N_voltage, currentTemp);
    prevSosDiagState = currentSosDiagState;
    prevSOS_BTNState = currentSOS_BTNState;


//    Мы когда он/офф выключаем, там сразу все напряжения начинают меняться. И считывать показания ацп и gpio уже бессмысленно.
    if(!currentOnOffModeState) goto normal;

    // Показания АЦП
    // RED бип отключен 1737
    // RED бип подключен, сос отпущен 1493
    // RED бип подключен, сос нажат 1265

    // GREEN бип отключен 1680
    // GREEN бип подключен, сос отпущен 1517
    // GREEN бип подключен, сос нажат 1284

    // valBtn = 1 Кнопка SOS нажата
    // valBtn = 0 Кнопка SOS отпущена
    // valueDIAG == 0 Кнопка SOS подключена
    // valueDIAG == 1 Кнопка SOS отключена
    if(currentDIAG_MIC_N_voltage > 1620){ // Бип отключен, SOS отпущен
        valBtn = 0;
        valDIAG = 1;
        currentSOS_BTNState = currentSosDiagState = FALSE;         // Кнопка SOS не нажата
        currentDiagMicState = FALSE;
        if(prevSosDiagState != currentSosDiagState || !isInit) {
            send_to_gpio_task(GPIO_NEW_EVENT, EV_SOS_BTN_RELEASED, 0); // Кнопка SOS отпущена
            send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_SOS_OFF, 0);     // Ошибка кнопки SOS
            send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_MIC_OFF, 0);     // Ошибка микрофона
        }
    } else if((currentDIAG_MIC_N_voltage <= 1620) & (currentDIAG_MIC_N_voltage >= 1350)){  // (270мВ) Бип подключен, SOS отпущен
        normal:
        valBtn = valDIAG = 0;
        currentSosDiagState = TRUE; // Сброс ошибки кнопки SOS
        currentDiagMicState = TRUE;
        if(prevSosDiagState != currentSosDiagState || !isInit) {
            send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_SOS_ON, 0); // Микрофон подключен
            send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_MIC_ON, 0); // Кнопка SOS подключена
        } // Событие отпускание кнопки SOS
        currentSOS_BTNState = FALSE; // Кнопка SOS отпущена
        if (prevSOS_BTNState != currentSOS_BTNState || !isInit) {
            send_to_gpio_task(GPIO_NEW_EVENT, EV_SOS_BTN_RELEASED, 0); // Кнопка SOS отпущена
        }
    } else { // Бип подключен, SOS нажат
        valBtn = 1;
        valDIAG = 0;
        currentSosDiagState = TRUE; // Сброс ошибки кнопки SOS
        currentDiagMicState = TRUE;
        if(prevSosDiagState != currentSosDiagState || !isInit) {
            send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_SOS_ON, 0); // Микрофон подключен
            send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_MIC_ON, 0); // Кнопка SOS подключена
        }
        currentSOS_BTNState = TRUE; // Кнопка SOS нажата
        if (prevSOS_BTNState != currentSOS_BTNState || !isInit){ // Кнопка SOS нажата
            send_to_gpio_task(GPIO_NEW_EVENT, EV_SOS_BTN_PRESSED, 0);
        }
    }
}

static void currentDiagSosState(void) {
    prevSosDiagState = currentSosDiagState;
    prevSOS_BTNState = currentSOS_BTNState;

    M2MB_GPIO_VALUE_E valueDIAG;
    m2mb_gpio_read(DIAG_SOS_fd, &valueDIAG);

    M2MB_GPIO_VALUE_E valueBtn;
    m2mb_gpio_read(SOS_BTN_fd, &valueBtn);
    valBtn = valueBtn;
    valDIAG = valueDIAG;
//    LOG_DEBUG("SOS_BTN %d", valueBtn);
//    LOG_DEBUG("DIAG_SOS %d", valueDIAG);

        if(is_diag_sos_disable()){ // Если диагностика обрыва кнопки SOS отключена
            if(valueBtn == 1){ // Кнопка SOS нажата
                if (prevSOS_BTNState == FALSE || !isInit){
                    currentSOS_BTNState = TRUE;
                    send_to_gpio_task(GPIO_NEW_EVENT, EV_SOS_BTN_PRESSED, 0); // 19
                }
            } else { // Кнопка SOS отпущена
                if (prevSOS_BTNState == TRUE || !isInit) {
                    currentSOS_BTNState = FALSE;
                    send_to_gpio_task(GPIO_NEW_EVENT, EV_SOS_BTN_RELEASED, 0); // 17
                }
            }
            // Кнопка SOS всегда считается неоторванной
            if (prevSosDiagState == FALSE || !isInit) { // Кнопка SOS подключена
                currentSosDiagState = TRUE;
                send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_SOS_ON, 0);  // 16
            }
        } else{ // Если диагностика обрыва кнопки SOS не отключена
            if (valueBtn == 0 && valueDIAG == 0) {
                if (prevSosDiagState == FALSE || !isInit) { // Кнопка SOS подключена
                    currentSosDiagState = TRUE;
                    send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_SOS_ON, 0);  // 16
                }
                if (prevSOS_BTNState == TRUE) {  //  Кнопка SOS отпущена
                    currentSOS_BTNState = FALSE;
                    send_to_gpio_task(GPIO_NEW_EVENT, EV_SOS_BTN_RELEASED, 0); // 17
                }
            } else if (valueBtn == 0 && valueDIAG == 1) {
                if (prevSosDiagState == TRUE || !isInit) { // Кнопка SOS отключена
                    currentSosDiagState = FALSE;
                    send_to_gpio_task(GPIO_NEW_EVENT, EV_DIAG_SOS_OFF, 0);  // 18
                }
            } else if (valueBtn == 1 && valueDIAG == 0) {
//        LOG_DEBUG("WT: unreachable state"); // Промежуточное состояние. Не требует реакции
            } else {
                if (prevSOS_BTNState == FALSE) { // Кнопка SOS нажата
                    currentSOS_BTNState = TRUE;
                    send_to_gpio_task(GPIO_NEW_EVENT, EV_SOS_BTN_PRESSED, 0); // 19
                }
            }
        }
}

extern BOOLEAN getSosState(void){
    return currentSOS_BTNState;
}

static void gpio_timer_handler(void) {
    if (cycle_started) return;
    cycle_started = TRUE;
    while (needCycle) {
        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(GPIO_TIMER_INTERVAL));
        watchProcess();
        isInit = TRUE; // Инициализация считается завершенной по окончании 0 цикла
    }
    isInit = FALSE;
    cycle_started = FALSE;
}
