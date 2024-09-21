//
// Created by klokov on 27.02.2023.
//

#ifndef ERA_TELIT_DATA_UPDATER_H
#define ERA_TELIT_DATA_UPDATER_H

#include "m2mb_types.h"

typedef enum {
    UPDATE_DATA
} UPDATE_TASK_COMMAND;

INT32 UPDATER_task(INT32 type, INT32 param1, INT32 param2);

extern void finishUpdate(void);
#endif //ERA_TELIT_DATA_UPDATER_H
