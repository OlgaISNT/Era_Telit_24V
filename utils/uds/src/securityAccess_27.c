//
// Created by klokov on 13.10.2022.
//

#include "../../../m2mb/m2mb_bt.h"
#include <m2mb_hwTmr.h>
#include "../hdr/securityAccess_27.h"
#include "../hdr/uds.h"
#include "log.h"
#include "security_access.h"
#include "azx_utils.h"
#include "tasks.h"
#include "sys_param.h"
#include "factory_param.h"

static UINT16 securitySeed = 0x1679;
static UINT16 securityKeyTestMode = 0x0911;

static UINT32 SA_INVALID_KEY_INTERVAL = 10000; // Время ожидания
static UINT32 SA_RESET_INTERVAL = 5000; // Время ожидания
static UINT16 cntInvalidKeys = 0; // Счетчик неправильных ключей, которые присылает клиент
static UINT16 stage = 1;          // Текущий этап разблокировки
static BOOLEAN isAccess = FALSE;  // текущий уровень обслуживания
static BOOLEAN onSecurityTimer = FALSE; // запущен ли таймер задержки
static UINT16 requestForSeed(char *data, char *buf);
static UINT16 requestForSendKey(char *data, char *buf);
static void resetSecurityTimer(void);
static BOOLEAN testMode = FALSE;

INT32 CAN_SA_task(INT32 type, INT32 param1, INT32 param2){
    (void) param1;
    (void) param2;
    switch (type) {
        case INVALID_KEY:
            azx_sleep_ms(SA_INVALID_KEY_INTERVAL);
            resetSecurityTimer();
            return M2MB_OS_SUCCESS;
        case RESET_SA:
        default:
//            LOG_DEBUG("CAN: Attempt to reset SA");
            azx_sleep_ms(SA_RESET_INTERVAL);
            if ((getTM() == 0) & (!isAccess))return M2MB_OS_SUCCESS;

            if (isMessage()){
                notMessage();
                send_to_can_sa_task(RESET_SA,0,0); // Через 10 секунд повторная процедура
            } else {
                LOG_DEBUG("CAN: Reset SA");
                enableTestMode();
            }
            return M2MB_OS_SUCCESS;
    }
}

extern BOOLEAN isTestMode(void){
    return testMode;
}

extern void enableTestMode(void){
    resetProcedureSA();
    if((getTM() == 0)){
        if (testMode == FALSE){
            LOG_DEBUG("Enable test mode");
            testMode = TRUE;
            // Останавливаем алгоритм заряда
            onCharge(FALSE);
            onOffMode(TRUE);

            // Останавливаем индикацию
            onLedPwrCtrl(FALSE);
            redMode(FALSE);
            sosCtrl(FALSE);
            greenModeCtrl(FALSE);
        }
    }
}

static void disableTestMode(void){
    testMode = FALSE;
    send_to_ind_task(SHOW_FAILURE,0,0);
}

static void resetSecurityTimer(void){
    LOG_DEBUG("UDS: SA reset timer");
    cntInvalidKeys--; // Счетчик неправильных ключей уменьшается на 1
    onSecurityTimer = FALSE;
}

/**
 * @brief работает ли таймер задержки
 * @return TRUE - работает / FALSE - не работает
 */
extern BOOLEAN isEnableSecTimer(void){
    return onSecurityTimer;
}

/**
 * @brief возвращает текущий уровень доступа
 * @return текущий уровень доступа (0 - стандартный, 1 - процедура SA пройдена успешно)
 */
extern BOOLEAN isAccessLevel(void){
    return isAccess;
}

extern void resetProcedureSA(void){
    if (testMode){
        LOG_DEBUG("CAN_SA: TEST_MODE INACTIVE");
    }

    if (isAccess){
        LOG_DEBUG("CAN_SA: TERMINAL_MODE INACTIVE");
    }
    testMode = FALSE;
    isAccess = FALSE;
    stage = 1;
    disableTestMode();
}

/**
 * @brief Обработчик 13.1.3	SecurityAccess service (ID 0x27)
 * @param data      данные CAN фрейма
 * @param buf       буфер, в который будет сохранён ответ
 * @param buf_size  максимальный размер буфера
 * @return UINT16   кол-во байт, записанных в буфер ответа
 */
UINT16 handler_service27(char *data, char *buf, UINT16 buf_size){
    (void) buf_size;
    UINT16 size = 0;

    // Проверяем правильность фрейма
    if (data == NULL || data[1] != SECURITY_ACCESS){
        return negative_resp(buf, SECURITY_ACCESS, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Доступ к системе безопасности состоит из двух этапов
    // 1) Клиент запрашивает у ТСМ разблокировку. ТСМ должен отправить Security Seed.
    // 2) Клиент должен запросить безопасный доступ отправив вычисленный ключ.
    // ТСМ должен решить, допускать ли клиента к запрошенному уровню доступа.
    // Проверяем субфункцию
    if ((data[2] == REQUEST_SEED) | (data[2] == SEND_KEY)){
        switch (data[2]) {
            case REQUEST_SEED: // 1 этап
                size = requestForSeed(data, buf);
                break;
            case SEND_KEY:    //  2 этап
                size = requestForSendKey(data, buf);
                break;
        }
    } else {
        return negative_resp(buf, SECURITY_ACCESS, SUBFUNCTION_NOT_SUPPORTED);
    }
    return size;
}

// 0x03
// На первом этапе отправляем Security Seed
static UINT16 requestForSeed(char *data, char *buf){
//    LOG_DEBUG("SecurityAccess: requestForSeed");
    if (!isAccess){
        // Если процедура Security Access еще не была завершена успешно,
        // то ТСМ должен отправить Security Seed.
        buf[0] = data[1] | 0x40;
        buf[1] = REQUEST_SEED;
        buf[2] = securitySeed >> 8;
        buf[3] = securitySeed;
        buf[4] = 0x00;
        buf[5] = 0x00;

        // Переходим на следующий этап
        // Ожидаем ключ в следующем пакете
        stage = 2;
        buf[6] = 0xFF;  // Метка ожидания ключа для приемщика СAN фреймов
    } else {
        // Если процедура Security Access ранее уже была успешно завершена,
        // но заново поступил запрос RequestSeed, то
        // этот сервер должен ответить службой сообщения положительного
        // ответа SecurityAccess-RequestSeed с начальным значением, равным нулю (0).
        buf[0] = data[1] | 0x40;
        buf[1] = REQUEST_SEED;
        buf[2] = 0x00;
        buf[3] = 0x00;
        buf[4] = 0x00;
        buf[5] = 0x00;
        buf[6] = 0x00;
    }
    return 7;
}

// 0x04
static UINT16 requestForSendKey(char *data, char *buf){
//    LOG_DEBUG("SecurityAccess: requestForSendKey");
    // Если первый этап не был пройден, и клиент сразу прислал какой-то ключ
    if (stage != 2){
        // Не выполнены условия необходимые для процедуры Security Access
        LOG_DEBUG("SecurityAccess: conditionsNotCorrect");
        return negative_resp(buf, SECURITY_ACCESS, CONDITIONS_NOT_CORRECT);
    }

    // Проверяем полученный ключ
    UINT16 key = (data[3] << 8) | data[4];
    if (key == securityKeyTestMode){ // Если прислан ключ активации режима тестирования
        LOG_DEBUG("CAN_SA: TEST_MODE ACTIVE");
        SA_RESET_INTERVAL = 60000; // Таймаут SA для тестового режима
        enableTestMode();
        testMode = TRUE;
    } else if (CAN_SA_CalculateKey(securitySeed) == key){
        LOG_DEBUG("CAN_SA: TERMINAL_MODE ACTIVE");
        SA_RESET_INTERVAL = 10000;
        // Если ключ посчитан правильно
        isAccess = TRUE;
        disableTestMode();
    } else { // TODO если невалидный ключ
        // Недопустимый ключ требует, чтобы клиент начал все сначала
        // с сообщением о положительном ответе сообщения SecurityAccess-RequestSeed.
        resetProcedureSA(); // Сброс
        cntInvalidKeys++;   // +1 к неправильно введенным ключам
        LOG_DEBUG("SecurityAccess: invalidKey %02X %02X attempt %d", data[3], data[4], cntInvalidKeys);
        if (cntInvalidKeys == 3){
            // После 3-й ложной попытки TCM должен подождать 10 секунд,
            // прежде чем принять следующее сообщение «Request Seed»,
            LOG_DEBUG("SecurityAccess: timer ON");
            onSecurityTimer = TRUE;
            send_to_can_sa_task(INVALID_KEY,0,0);
        }
        return negative_resp(buf, SECURITY_ACCESS, invalidKey);
    }

    buf[0] = data[1] | 0x40;
    buf[1] = data[2] & 0x7F; // securityAccessType - эхо [6,0] битов субфункции
    send_to_can_sa_task(RESET_SA,0,0); // Через 10 секунд повторная процедура
    return 2;
}
