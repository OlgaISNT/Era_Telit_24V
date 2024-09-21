//
// Created by klokov on 04.11.2022.
//

#include "log.h"
#include "../hdr/clearDiagnosticInformation_14.h"
#include "../hdr/uds.h"
#include "failures.h"
#include "gpio_ev.h"

static UINT16 pos_resp_19(char *data, char *buf, UINT16 buf_size);

UINT16 handler_service14(char *data, char *buf, UINT16 buf_size) {
    // Проверка пакета
    if (data == NULL || data[1] != CLEAR_DIAGNOSTIC_INFORMATION || data[0] != 0x04) {
        return negative_resp(buf, CLEAR_DIAGNOSTIC_INFORMATION, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Проверка длины
//    for (UINT8 i = 5; i < 8; ++i) {
//        if (data[i] != NOT_INVOLVED_BYTE) {
//            return negative_resp(buf, CLEAR_DIAGNOSTIC_INFORMATION, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
//        }
//    }

    return pos_resp_19(data, buf, buf_size);
}

static UINT16 pos_resp_19(char *data, char *buf, UINT16 buf_size) {
    (void) buf_size;
    if ((data[2] == 0x20) & (data[3] == 0x00) & (data[4] == 0x00)) {
        LOG_DEBUG("CLEAR_DIAGNOSTIC_INFORMATION: BODY_GROUP");
    } else if ((data[2] == 0x30) & (data[3] == 0x00) & (data[4] == 0x00)) {
        LOG_DEBUG("CLEAR_DIAGNOSTIC_INFORMATION: NETWORK_GROUP");
    } else if ((data[2] == 0xFF) & (data[3] == 0xFF) & (data[4] == 0xFF)) {
        LOG_DEBUG("CLEAR_DIAGNOSTIC_INFORMATION: ALL_GROUPS");
        clear_failures();
        reset_diag_mic();
        reset_diag_speaker();
    } else {
        return negative_resp(buf, CLEAR_DIAGNOSTIC_INFORMATION, REQUEST_OUT_OF_RANGE);
    }
    buf[0] = CLEAR_DIAGNOSTIC_INFORMATION | 0x40;
    return 1;
}