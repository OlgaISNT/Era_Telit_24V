//
// Created by klokov on 30.11.2022.
//

#ifndef MAIN_REP_AIRBAGS_H
#define MAIN_REP_AIRBAGS_H
#include "m2mb_types.h"

// 0x31C
typedef struct {
    BOOLEAN NO_CRASH;          // Нет удара
    BOOLEAN BELT_CRASH;        // Ремень не пристегнут ???
    BOOLEAN FRONT_CRASH;       // Удар спереди
    BOOLEAN LEFT_SIDE_CRASH;   // Удар слева
    BOOLEAN RIGHT_SIDE_CRASH;  // Удар справа
    BOOLEAN REAR_CRASH;        // Удар сзади
} CHERRY_CRASH;

typedef enum {
    CHECK_AIRBAGS,
    NOTHING
} AIRBAGS_TASK;

extern INT32 AIRBAGS_task(INT32 type, INT32 param1, INT32 param2);
extern void airbagsCrashDetect(char D3);
extern void stopAirbagsDetect(void);
#endif //MAIN_REP_AIRBAGS_H
