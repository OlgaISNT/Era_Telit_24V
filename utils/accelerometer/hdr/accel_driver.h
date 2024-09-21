//
// Created by klokov on 20.07.2022.
//

#ifndef MAIN_REP_ACCEL_DRIVER_H
#define MAIN_REP_ACCEL_DRIVER_H

#include <m2mb_types.h>
#include "geometry_service.h"

typedef enum {
    START_ACCEL_SERVICE,
    ACCEL_POOL,
    GET_HORIZON_QUAT,
} ACCEL_TASK;

typedef struct{
    char xL;
    char xH;
    char yL;
    char yH;
    char zL;
    char zH;
} RAW;

typedef struct{
    Vector origVector;          // Вектор с неконвертированными и нефильтрованными показаниями акселерометра
    Vector nonFiltVector;       // Вектор с неотфильтрованными показаниями акселерометра
    Vector filtVector;          // Вектор содержащий отфильтрованные показания акселерометра
    Vector g1;                  // Вектор гравитации после поворота в горизонт
    Vector g1Auto;              // Текущий вектор гравитации
    Vector ultraFiltVector;
    Quaternion qHor;            // Кватернион поворота в горизонт
    FLOAT32 currentTippingAngle; // Текущий угол завала
    FLOAT32 maxTippingAngle;     // Максимальный угол завала
    Vector normCurrentVector;   // Нормированный текущий вектор
    Vector normGravVector;
    RAW rawData;                // Сырые данные с акселерометра
} Accel;

INT32 ACCEL_task(INT32 type, INT32 param1, INT32 param2);
extern void stopAccelService(void);
extern INT32 getValY(void);
extern INT32 getValZ(void);
extern INT32 getValX(void);
extern Vector getVector(void);
extern UINT8 getCurrentRollAng(void);
extern RAW getRawData(void);
extern BOOLEAN getHorQuat(void);
extern BOOLEAN needHorQuat(void);
extern void incrementSpec(void);
extern BOOLEAN isEnableSpecF(void);
extern char getWhoAmI(void);
extern void resetAccelPref(void);
#endif //MAIN_REP_ACCEL_DRIVER_H
