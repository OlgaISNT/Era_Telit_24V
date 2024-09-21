//
// Created by klokov on 11.10.2022.
//

#include "../hdr/flowControlFrame.h"
#include "log.h"
#include "../hdr/uds.h"

extern FLOW_CONTROL_FRAME handler_flowControlFrame(char *data) {
    FLOW_CONTROL_FRAME controlFrame;
    if ((data == NULL) || (data[0] & 0x30) != 0x30) {
        controlFrame.currentStatus = UNDEFINED_FS;
        LOG_ERROR("UDS: incorrect flow control frame");
        return controlFrame;
    }

    // Определение статуса получателя
    char status = data[0] & 0x03;
    switch (status) {
        case 0x00:
            controlFrame.currentStatus = READY_TO_RECEIVE;
            break;
        case 0x01:
            controlFrame.currentStatus = WAIT;
            break;
        case 0x02:
            controlFrame.currentStatus = OVERFLOW;
            break;
        default:
            controlFrame.currentStatus = UNDEFINED_FS;
            return controlFrame;
    }

    // Количество фреймов, которые получатель готов принять
    controlFrame.blockSize = (UINT16) data[1];

    // Минимальное время между фреймами
    controlFrame.minimumSeparationTime = (UINT16) data[2];

    return controlFrame;
}

extern UINT16 flowFrame(FLOW_CONTROL_FRAME flowControlFrame, char *buf, UINT16 bufSize){
    (void) bufSize;
    buf[0] = 0x30 | flowControlFrame.currentStatus;
    buf[1] = flowControlFrame.blockSize;
    buf[2] = flowControlFrame.minimumSeparationTime;
    buf[3] = NOT_INVOLVED_BYTE;
    buf[4] = NOT_INVOLVED_BYTE;
    buf[5] = NOT_INVOLVED_BYTE;
    buf[6] = NOT_INVOLVED_BYTE;
    buf[7] = NOT_INVOLVED_BYTE;
    return 8;
}