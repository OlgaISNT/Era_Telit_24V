//
// Created by klokov on 20.07.2022.
//

#ifndef MAIN_REP_KALMANFILTER_H
#define MAIN_REP_KALMANFILTER_H

#include <m2mb_types.h>
#include "averager.h"

typedef enum {
    CALIBRATION
} TASK_FILTER;

typedef enum {
    DISABLE_FILTERING_MODE,
    LOW_FILTERING_MODE,
    MEDIUM_FILTERING_MODE,
    HIGH_FILTERING_MODE,
    VERY_HIGH_FILTERING_MODE,
    ULTRA_FILTERING_MODE,
    ULTRA_HIGH_FILTERING_MODE,
    CUSTOM_FILTERING_MODE
}FILTERING_MODE;

INT32 KALMAN_task(INT32 type, INT32 param1, INT32 param2);
extern BOOLEAN isHasCalibrated(void);
extern void setFilteringMode(FILTERING_MODE mode);
extern INT32 filterX(INT32 val);
extern INT32 filterY(INT32 val);
extern INT32 filterZ(INT32 val);
extern INT32 ultraFilterX(INT32 val);
extern INT32 ultraFilterY(INT32 val);
extern INT32 ultraFilterZ(INT32 val);
extern FILTERING_MODE getCurrentFilterMode(void);

#endif //MAIN_REP_KALMANFILTER_H
