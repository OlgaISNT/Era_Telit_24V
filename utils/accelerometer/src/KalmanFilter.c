//
// Created by klokov on 20.07.2022.
//

#include <m2mb_os_api.h>
#include <tgmath.h>
#include "../hdr/KalmanFilter.h"
#include "averager.h"
#include "log.h"
#include "../hdr/accel_driver.h"

#define MOVING_AVERAGE_DELAY 25  // Задержка между измерениями 25 мСек
#define RECALIBRATE_PERIOD  3000 // Период

static Averager *averager;
static const UINT32 WINDOW = 100;
static FILTERING_MODE currentMode = DISABLE_FILTERING_MODE;

static FLOAT32 movingAverage = 0;
static BOOLEAN hasCalibrated = FALSE;

static FLOAT32 meanDeviation = 0;
static FLOAT32 varProcess = 1.0f;
static FLOAT32 ultraVarProcess = 0.000001f;

static FLOAT32 Pcx = 0.00f;
static FLOAT32 Gx = 0.00f;
static FLOAT32 Px = 1.0f;
static FLOAT32 Xpx = 0.00f;
static FLOAT32 Zpx = 0.00f;
static FLOAT32 Xex = 0.00f;

static FLOAT32 Pcy = 0.00f;
static FLOAT32 Gy = 0.00f;
static FLOAT32 Py = 1.0f;
static FLOAT32 Xpy = 0.00f;
static FLOAT32 Zpy = 0.00f;
static FLOAT32 Xey = 0.00f;

static FLOAT32 Pcz = 0.00f;
static FLOAT32 Gz = 0.00f;
static FLOAT32 Pz = 1.0f;
static FLOAT32 Xpz = 0.00f;
static FLOAT32 Zpz = 0.00f;
static FLOAT32 Xez = 0.00f;

static FLOAT32 ultraPcx = 0.00f;
static FLOAT32 ultraGx = 0.00f;
static FLOAT32 ultraPx = 1.0f;
static FLOAT32 ultraXpx = 0.00f;
static FLOAT32 ultraZpx = 0.00f;
static FLOAT32 ultraXex = 0.00f;

static FLOAT32 ultraPcy = 0.00f;
static FLOAT32 ultraGy = 0.00f;
static FLOAT32 ultraPy = 1.0f;
static FLOAT32 ultraXpy = 0.00f;
static FLOAT32 ultraZpy = 0.00f;
static FLOAT32 ultraXey = 0.00f;

static FLOAT32 ultraPcz = 0.00f;
static FLOAT32 ultraGz = 0.00f;
static FLOAT32 ultraPz = 1.0f;
static FLOAT32 ultraXpz = 0.00f;
static FLOAT32 ultraZpz = 0.00f;
static FLOAT32 ultraXez = 0.00f;

static void calibration(void);
static void release(void);
static void calcMovingAverage(void);
static void calcStandardDeviation(void);

INT32 KALMAN_task(INT32 type, INT32 param1, INT32 param2){
    (void) param1;
    (void) param2;
    switch (type) {
        case CALIBRATION:
            calibration();
            return M2MB_OS_SUCCESS;
        default:
            LOG_ERROR("ACCEL FILTER: Undefined task");
            return M2MB_OS_TASK_ERROR;

    }
}

extern BOOLEAN isHasCalibrated(void){
    return hasCalibrated;
}

extern void setFilteringMode(FILTERING_MODE mode){
    switch (mode) {
        case DISABLE_FILTERING_MODE:
//            LOG_DEBUG("ACCEL FILTER: DISABLE_FILTERING_MODE");
            varProcess = 1.0f;
            break;
        case LOW_FILTERING_MODE:
//            LOG_DEBUG("ACCEL FILTER: LOW_FILTERING_MODE");
            varProcess = 0.5f;
            break;
        case MEDIUM_FILTERING_MODE:
//            LOG_DEBUG("ACCEL FILTER: MEDIUM_FILTERING_MODE");
            varProcess = 0.1f;
            break;
        case HIGH_FILTERING_MODE:
//            LOG_DEBUG("ACCEL FILTER: HIGH_FILTERING_MODE");
            varProcess = 0.01f;
            break;
        case VERY_HIGH_FILTERING_MODE:
//            LOG_DEBUG("ACCEL FILTER: VERY_HIGH_FILTERING_MODE");
            varProcess = 0.0001f;
            break;
        case ULTRA_HIGH_FILTERING_MODE:
//            LOG_DEBUG("ACCEL FILTER: ULTRA_HIGH_FILTERING_MODE");
            varProcess = 0.00001f;
            break;
        case CUSTOM_FILTERING_MODE:
//            LOG_DEBUG("ACCEL FILTER: CUSTOM_FILTERING_MODE");
            varProcess = 0.00005f;
            break;
        default:
            break;
    }
    currentMode = mode;
}

extern FILTERING_MODE getCurrentFilterMode(void){
    return currentMode;
}

extern INT32 ultraFilterX(INT32 val){
    ultraPcx = ultraPx + ultraVarProcess;
    ultraGx = ultraPcx / (ultraPcx + meanDeviation);
    ultraPx = (1 - ultraGx) * ultraPcx;
    ultraXpx = ultraXex;
    ultraZpx = ultraXpx;
    ultraXex = ultraGx * (val - ultraZpx) + ultraXpx;
    return (INT32) ultraXex;
}

extern INT32 ultraFilterY(INT32 val) {
    ultraPcy = ultraPy + ultraVarProcess;
    ultraGy = ultraPcy / (ultraPcy + meanDeviation);
    ultraPy = (1 - ultraGy) * ultraPcy;
    ultraXpy = ultraXey;
    ultraZpy = ultraXpy;
    ultraXey = ultraGy * (val - ultraZpy) + ultraXpy;
    return (INT32) ultraXey;
}

extern INT32 ultraFilterZ(INT32 val) {
    ultraPcz = ultraPz + ultraVarProcess;
    ultraGz = ultraPcz / (ultraPcz + meanDeviation);
    ultraPz = (1 - ultraGz) * ultraPcz;
    ultraXpz = ultraXez;
    ultraZpz = ultraXpz;
    ultraXez = ultraGz * (val - ultraZpz) + ultraXpz;
    return (INT32) ultraXez;
}

extern INT32 filterX(INT32 val){
    if (varProcess == 1.0f){
        return val;
    }
    Pcx = Px + varProcess;
    Gx = Pcx / (Pcx + meanDeviation);
    Px = (1 - Gx) * Pcx;
    Xpx = Xex;
    Zpx = Xpx;
    Xex = Gx * (val - Zpx) + Xpx;
    return (INT32) Xex;
}

extern INT32 filterY(INT32 val) {
    if (varProcess == 1.0f){
        return val;
    }
    Pcy = Py + varProcess;
    Gy = Pcy / (Pcy + meanDeviation);
    Py = (1 - Gy) * Pcy;
    Xpy = Xey;
    Zpy = Xpy;
    Xey = Gy * (val - Zpy) + Xpy;
    return (INT32) Xey;
}

extern INT32 filterZ(INT32 val) {
    if (varProcess == 1.0f){
        return val;
    }
    Pcz = Pz + varProcess;
    Gz = Pcz / (Pcz + meanDeviation);
    Pz = (1 - Gz) * Pcz;
    Xpz = Xez;
    Zpz = Xpz;
    Xez = Gz * (val - Zpz) + Xpz;
    return (INT32) Xez;
}

static void calibration(void){
    LOG_DEBUG("ACCEL FILTER: Filter calibration was started");
    m2mb_os_taskSleep(M2MB_OS_MS2TICKS(RECALIBRATE_PERIOD));
    setFilteringMode(ULTRA_HIGH_FILTERING_MODE);
    calcMovingAverage();
    calcStandardDeviation();
    LOG_DEBUG("ACCEL FILTER: Calibration was finished");
    m2mb_os_taskSleep(M2MB_OS_MS2TICKS(3000));
    setFilteringMode(VERY_HIGH_FILTERING_MODE);
    hasCalibrated = TRUE;
}

static void calcMovingAverage(void){
    averager = newAverager(WINDOW);
    release();
    UINT32 cnt = 0;
    LOG_DEBUG("ACCEL FILTER: calculate moving average");
    while (cnt < WINDOW){
        add(averager, getValZ());
        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(MOVING_AVERAGE_DELAY));
        cnt++;
    }
    movingAverage = (FLOAT32) getAverage(averager);
    deleteAverager(averager);
    LOG_DEBUG("ACCEL FILTER: Current moving average %f or %d", movingAverage, getAverage(averager));
}

static void release(void){
    if (!averager->cleared) reset(averager);
}

static void calcStandardDeviation(void){
    LOG_DEBUG("ACCEL FILTER: calculate the standard deviation");
    UINT32 cnt = 0;
    FLOAT32 val = 0;
    while (cnt < WINDOW){
        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(MOVING_AVERAGE_DELAY));
        val += ((FLOAT32) getValZ() - movingAverage) * ((FLOAT32) getValZ() - movingAverage);
//        LOG_DEBUG("%f += (%d - %f) * (%d - %f)", val, getValZ(), movingAverage, getValZ(), movingAverage);
        cnt++;
    }
    val = val /(FLOAT32) WINDOW;
    LOG_DEBUG("ACCEL FILTER: exit while val %f size %d", val, WINDOW);
    meanDeviation = sqrt(val);
    LOG_DEBUG("ACCEL: New deviation value: %f", meanDeviation);
}