#include <m2mb_types.h>
#include <memory.h>
#include "../hdr/uds.h"
#include "../hdr/testerPresentService_3E.h"
#include "../hdr/readDataByIdentifier_22.h"
#include "log.h"
#include "../hdr/flowControlFrame.h"
#include "../hdr/securityAccess_27.h"
#include "../hdr/readDataByLocalIdentifier_21.h"
#include "../hdr/readDTCInformation_19.h"
#include "../hdr/inputOutputControlByIdentifier_2F.h"
#include "../hdr/clearDiagnosticInformation_14.h"
#include "../hdr/writeDataByIdentifier_2E.h"
#include "azx_ati.h"
#include "../hdr/uds_service31.h"
#include "../hdr/ECUReset_11.h"
#include "../hdr/diagnosticSessionControl_10.h"
#include "../hdr/request_file_transfer_38.h"
#include "../hdr/transfer_data_36.h"
#include "../hdr/transfer_exit_service_37.h"
#include "../hdr/request_download_service_34.h"
#include <time.h>

#define BUF_SIZE_UDS 1024
#define BUF_SIZE_PR (BUF_SIZE_UDS + 1024)

// Требования для терминала
#define WAIT_BLOCKS 2
#define MINIMUM_SEPARATION_TIME 0   // Минимальная задержка при отправке кадров

static char b_uds[BUF_SIZE_UDS];  // буфер для формируемого ответного UDS сообщения
static UINT16 sizeForSend;        // Количество байт из ответного буфера, которые требуется отправить
static UINT16 sended;             // Количество байт из ответного буфера, которые уже были отправлены
static char service = NOT_INVOLVED_BYTE;
static UINT16 sizeForReceive;     // Количество байт данных которое ожидается принять
static UINT16 received;           // Количество байт данных которое уже принято
static UINT16 waitBlocks = WAIT_BLOCKS;    // Количество фреймов которое ожидается после отправки кадра управления потоком
static BOOLEAN waitConsecutiveFrame = FALSE;
static UINT16 currentRecFrame = 0;
static BOOLEAN hasMessage = FALSE;

static char b_pr[BUF_SIZE_PR];               // буфер принятого сообщения
static FLOW_CONTROL_FRAME flowControlFrame;  // фрейм управления потоком
static BOOLEAN nextFrameKey = FALSE;         // в следующем фрейме ожидается ключ
static BOOLEAN nextFlowControl = FALSE;      // ожидается, что следующий фрейм будет фрейм контроля потока
static UINT16 currentFrame = 0;              // номер текущего фрейма из серии фреймов
static void send_uds(void);
static void sendSingleFrame(void);
static void sendFirstFrame(void);
static void sendConsecutiveFrame(void);
static void sendWaitForRecieve(void);
static BOOLEAN validBuff(void);

static void resetFlowControlFrame(void);     // Сброс содержимого фрейма управления потоком
static void clearRecBuf(void);
static void clearSendBuf(void);              // очистка буфера передачи
static void printDataWithName(char *data, char *name, UINT16 size);

BOOLEAN isMessage(void){
    return hasMessage;
}

void notMessage(void){
    hasMessage = FALSE;
}

static void printDataWithName(char *data, char *name, UINT16 size){
    AZX_LOG_INFO("%s: ",name);
    for (UINT16 i = 0; i < size; i++){
        AZX_LOG_INFO("%02X ", data[i]);
    }
    AZX_LOG_INFO("\r\n");
}

static void resetFlowControlFrame(void){
    flowControlFrame.blockSize = 0;
    flowControlFrame.minimumSeparationTime = 0;
    flowControlFrame.currentStatus = UNDEFINED_FS;
}

static void sendWaitForRecieve(void){
    resetFlowControlFrame();
    waitBlocks = WAIT_BLOCKS;
    flowControlFrame.blockSize = WAIT_BLOCKS;
    flowControlFrame.minimumSeparationTime = MINIMUM_SEPARATION_TIME;
    flowControlFrame.currentStatus = READY_TO_RECEIVE;
    sizeForSend = flowFrame(flowControlFrame, b_uds, BUF_SIZE_UDS);
    sendMessage(UDS_MODEM_ID_CHERY,NOT_REMOTE,b_uds, 8);
    waitConsecutiveFrame = TRUE; // Ожидаем прием кадров
}

/**
 * @brief Обработчик UDS запроса
 * @param data массив с данными запроса
 * @param size кол-во байт в массиве
 * @return BOOLEAN TRUE - запрос корректен, FALSE - некорректен
 */
BOOLEAN handle_uds_request(char *data, UINT16 size) {
    hasMessage = TRUE;
    if (size < 2) return FALSE;
    BOOLEAN res = FALSE;

    // Если терминал пытается что-то передать
    if((data[0] != 0x30) & ((data[0] & 0x10) == 0x10) & (data[7] != NOT_INVOLVED_BYTE)){
        clearRecBuf();
        sizeForReceive = ((data[0] - 0x10) << 8) | data[1];
        memcpy(&b_pr[0], data, 8);
        service = b_pr[2];
        received += 8;
        sendWaitForRecieve();
        currentRecFrame++;
        return TRUE;
    }

    // Если ожидался прием Consecutive кадров
    if (waitConsecutiveFrame){
        if (data[0] == (0x20 | currentRecFrame)){
            memcpy(&b_pr[received], &data[1], 7);
            currentRecFrame++;
            received += 7;
            waitBlocks --;

            if (received - 2 >= sizeForReceive) {
                goto down;
            };

            if (waitBlocks == 0){
                sendWaitForRecieve();
            } else{
                waitConsecutiveFrame = TRUE;
            }

            return TRUE;
        }
    }

    clearRecBuf();
    memcpy(&b_pr[0], &data[0], 8);
    received += 8;
    service = b_pr[1];
    down:

    // Если сработал таймер задержки внутри Security Access,
    // то любой запрос в течение времени работы таймера будет отклонен
    // с отрицательным кодом ответа «Требуемое время задержки не истекло» (37h)
    if(isEnableSecTimer()){
        sizeForSend = negative_resp(b_uds, SECURITY_ACCESS, requiredTimeDelayNotExpired);
        send_uds();
        return TRUE;
    }

    // Если следующим кадром ожидается получение ключа для Security Access
    if (nextFrameKey){
        // Если ожидалось получение ключа, но пришел какой-то другой пакет
        if ((data[1] != SECURITY_ACCESS) | (data[2] != 0x04)){
            resetProcedureSA();  // То сброс процедуры Security Access
        }
    }

    if ((data[0] == 0x30) & ((data[3] == NOT_INVOLVED_BYTE) | (data[3] == 0x00))){ // Получен фрейм управления потоком
        flowControlFrame = handler_flowControlFrame(data);
        if (flowControlFrame.currentStatus == READY_TO_RECEIVE){ // Если получатель готов к приему
            if (nextFlowControl & (sizeForSend != 0)){ // Если есть данные для передачи и ожидался
                nextFlowControl = FALSE;
                send_uds(); // начинаем передачу
                return TRUE;
            }
            return FALSE;
        } else if (flowControlFrame.currentStatus == WAIT){
            // TODO если получатель просит подождать
            nextFlowControl = FALSE;
            return FALSE;
        } else if (flowControlFrame.currentStatus == OVERFLOW) {
            // TODO если получатель сообщает о переполнении
            nextFlowControl = FALSE;
            return FALSE;
        } else{
            // TODO если статус получателя не определен
            nextFlowControl = FALSE;
            return FALSE;
        }
    }

    clearSendBuf();
    switch (service) {
        case TESTER_PRESENT:
            sizeForSend = handler_service3E(b_pr, b_uds);
            break;
        case READ_DATA_BY_IDENTIFIER:
            sizeForSend = handler_service22((b_pr[2] << 8) | b_pr[3], b_uds, BUF_SIZE_UDS);
            break;
        case SECURITY_ACCESS:
            sizeForSend = handler_service27(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        case READ_DATA_BY_LOCAL_IDENTIFIER:
            sizeForSend = handler_service21(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        case READ_DTC_INFORMATION:
            sizeForSend = handler_service19(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        case INPUT_OUTPUT_CONTROL_BY_IDENTIFIER:
            sizeForSend = handler_service2F(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        case WRITE_DATA_BY_IDENTIFIER:
            sizeForSend = handler_service2E(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        case DIAGNOSTIC_SESSION_CONTROL: // не используется
            sizeForSend = handler_service10(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        case ECU_RESET_SERVICE:
            sizeForSend = handler_service11(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        case ROUTINE_CONTROL:
            sizeForSend = handler_service31((UINT8 *) ((received <= 8) ? (b_pr + 1) : (b_pr + 2)), (received <= 8) ? b_pr[0] : ((b_pr[0] - 0x10) << 8) | b_pr[1], (UINT8 *) b_uds, BUF_SIZE_UDS);
            break;
        case CLEAR_DIAGNOSTIC_INFORMATION:
            sizeForSend = handler_service14(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        case TRANSFER_DATA_SERVICE:
            sizeForSend = handler_service36(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        case TRANSFER_EXIT_SERVICE:
            sizeForSend = handler_service37(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        case REQUEST_FILE_TRANSFER:
            sizeForSend = handler_service38(b_pr, b_uds, BUF_SIZE_UDS);
            break;
        default:
            printDataWithName(b_pr,
                              ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> undefined frame <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<",
                              received);
            break;
    }
    send_uds();
    clearRecBuf();
    return res;
}

static void send_uds(void) {
    // Если количество байт которое нужно отправить равно 0
    if (sizeForSend == 0) return; // Ничего не отправляем

    // Если сервис Security Access передает security seed
    if ((b_uds[0] == 0x67) & (b_uds[6] == 0xFF)){
        b_uds[6] = 0x00;
        nextFrameKey = TRUE; // То в следующем кадре ожидается ключ
    }

    // Если количество передаваемых байт до 7 штук включительно
    if (sizeForSend < 8){
        sendSingleFrame();
    } else {
        if (sended == 0){ // Если еще ни одного кадра не было отправлено
            sendFirstFrame();
        } else{
            sendConsecutiveFrame();
        }
    }
}

// Отправить однократный фрейм. Фрейм вся информация которого умещается в один CAN кадр.
static void sendSingleFrame(void){
    char temp[8];
    memcpy(&temp[1], b_uds, sizeForSend);
    // 0м байтом ставим размер кадра
    temp[0] = (CHAR) sizeForSend;
    // Пустые байты заполняем 0хАА
    memset(&temp[sizeForSend + 1], NOT_INVOLVED_BYTE, 7 - sizeForSend);
    // Отправляем сообщение
    sendMessage(UDS_MODEM_ID_CHERY,NOT_REMOTE,temp, 8);
}

// Отправить первый фрейм из серии фреймов
static void sendFirstFrame(void){
    char temp[8];
    // Первые 2 байта размер
    temp[0] = (sizeForSend >> 8) + 0x10;
    temp[1] = sizeForSend;
    memcpy(&temp[2], b_uds, 6);
    nextFlowControl = TRUE; // Ожидаем кадр управления потоком
    sended += 6;            // 6 байт было отправлено
    currentFrame++;
    sendMessage(UDS_MODEM_ID_CHERY,NOT_REMOTE,temp, 8);
}

static void sendConsecutiveFrame(void){
    char temp[20];
    UINT8 cnt = 1;
    temp[0] = 0x20 | currentFrame;
    // Если в кадре управления потоком не указано сколько байт ожидается, то передаем все
    if (flowControlFrame.blockSize == 0){
        for (UINT16 i = sended; i < sizeForSend; i++){
            temp[cnt] = b_uds[i];
            if (cnt == 7){
                if (flowControlFrame.minimumSeparationTime != 0) {
                    azx_sleep_ms(flowControlFrame.minimumSeparationTime);
                }
                sendMessage(UDS_MODEM_ID_CHERY,NOT_REMOTE,temp, 8); // Отправка
                cnt = 1;
                currentFrame++;
                temp[0] = 0x20 | currentFrame;
                sended += 7;
            } else {
                cnt++;
            }
        }

        // Если последний пакет был заполнен не полностью
        if (sizeForSend != sended){
            memset(&temp[cnt], NOT_INVOLVED_BYTE, 8 - cnt);
            if (flowControlFrame.minimumSeparationTime != 0) {
                azx_sleep_ms(flowControlFrame.minimumSeparationTime);
            }
            sendMessage(UDS_MODEM_ID_CHERY,NOT_REMOTE,temp, 8); // Отправка
        }
    } else{ // Иначе передаем сколько указано и ждем следующий фрейм управления потоком
        for (UINT16 j = 0; j < flowControlFrame.blockSize; j++){
            // Собираем кадр
            temp[0] = 0x20 | currentFrame;
            if(sizeForSend - sended >= 7){
                memcpy(&temp[1], &b_uds[sended], 7);
                sended += 7;
            } else {
                memcpy(&temp[1], &b_uds[sended], sizeForSend - sended);
                memset(&temp[1 + sizeForSend - sended], NOT_INVOLVED_BYTE, 8 - (sizeForSend - sended));
                sended += sizeForSend - sended;
            }
            if (flowControlFrame.minimumSeparationTime != 0) {
                azx_sleep_ms(flowControlFrame.minimumSeparationTime);
            }
            sendMessage(UDS_MODEM_ID_CHERY,NOT_REMOTE,temp, 8); // Отправка
            currentFrame++;
        }
        if (sended != sizeForSend){
            nextFlowControl = TRUE;
        } else{
            clearSendBuf();
        }
    }
}

UINT16 negative_resp(char *buf, UINT8 req_service_id, UINT8 negative_response_code) {
    buf[0] = SIDRSIDNRQ;
    buf[1] = req_service_id;
    buf[2] = negative_response_code;
    return 3;
}

static void clearSendBuf(void) {
    memset(b_uds,0, BUF_SIZE_UDS);
    sizeForSend = 0;
    sended = 0;
    currentFrame = 0;
    nextFlowControl = FALSE;
    nextFrameKey = FALSE;
    waitConsecutiveFrame = FALSE;
    resetFlowControlFrame();
}

static void clearRecBuf(void){
    memset(b_pr,0, BUF_SIZE_PR);
    sizeForReceive = 0;
    received = 0;
    waitConsecutiveFrame = FALSE;
    service = NOT_INVOLVED_BYTE;
    currentRecFrame = 0;
    resetFlowControlFrame();
    if(FALSE) validBuff();
}

static BOOLEAN validBuff(void){
    // Если пакет полностью состоит из одинаковых байтов
    UINT16 cnt = 0;
    for (UINT16 i = 0; i < sizeForSend; i++){
        if (b_uds[i] == b_uds[0]) cnt++;
    }
    if (cnt == sizeForSend) {
        LOG_DEBUG("BUFFER CONTENT IS NOT ACTUAL");
        return FALSE; // Ничего не отправляем
    }
    return TRUE;
}

#undef BUF_SIZE
