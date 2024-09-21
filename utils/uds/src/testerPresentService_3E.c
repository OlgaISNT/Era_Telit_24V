//
// Created by klokov on 07.10.2022.
//

#include <m2mb_types.h>
#include "../hdr/testerPresentService_3E.h"
#include "mcp2515.h"
#include "log.h"
#include "../hdr/uds.h"

static CHAR TESTER_PRESENT_POSITIVE_RESPONSE = 0x7E;   // ID ответа
// Доступные байты субфункции
static CHAR notSuppressPosRspMsgIndicationBit = 0x00;
static CHAR suppressPosRspMsgIndicationBit = 0x80;

static UINT16 positiveResponse(CHAR subF, char *buf);   // Положительный ответ

extern UINT16 handler_service3E(char *frame, char *buf){
    // Пришел запрос на подключение
    // Проверяем правильность фрейма
    if (frame == NULL) {
        return negative_resp(buf, TESTER_PRESENT, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Начиная с 3его байта все байты должны быть заполнены 0xAA
//    for (int i = (UINT8) frame[0] + 1; i < 8; i++){
//        if (frame[i] != 0xAA) {
//            return negative_resp(buf, TESTER_PRESENT, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
//        }
//    }

    // Проверка байта субфункции
    if ((frame[2] != suppressPosRspMsgIndicationBit) & (frame[2] != notSuppressPosRspMsgIndicationBit)){
        return negative_resp(buf, TESTER_PRESENT, SUBFUNCTION_NOT_SUPPORTED);
    }

    // Положительный ответ
    return positiveResponse(frame[2], buf);
}

static UINT16 positiveResponse(CHAR subF, char *buf){
    buf[0] = TESTER_PRESENT_POSITIVE_RESPONSE;
    buf[1] = subF & 0x7F;   // В 3ем байте [6;0]биты являются эхом subF
    return 2;
}