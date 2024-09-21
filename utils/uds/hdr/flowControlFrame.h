//
// Created by klokov on 11.10.2022.
//

#ifndef MAIN_REP_FLOWCONTROLFRAME_H
#define MAIN_REP_FLOWCONTROLFRAME_H

#include "m2mb_types.h"

// Flow Status говорит отправителю First Frame о статусе получателя
// 00 - Готов к приему CF фреймов, 01 - Жди, 02 – Переполнение.
typedef enum{
    READY_TO_RECEIVE,      // 0x00 - готов к приему
    WAIT,                  // 0x01 - жди
    OVERFLOW,              // 0x02 - переполнение
    UNDEFINED_FS           // статус неопределен
} FLOW_STATUS;

typedef struct {
    UINT16 blockSize;                // Количество фреймов которые необходимо передать получателю
    UINT16 minimumSeparationTime;    // Минимальное время между передаваемыми фреймами.
    FLOW_STATUS currentStatus;       // Текущий статус получателя
} FLOW_CONTROL_FRAME;

extern FLOW_CONTROL_FRAME handler_flowControlFrame(char *data);
extern UINT16 flowFrame(FLOW_CONTROL_FRAME flowControlFrame, char *buf, UINT16 bufSize);

#endif //MAIN_REP_FLOWCONTROLFRAME_H
