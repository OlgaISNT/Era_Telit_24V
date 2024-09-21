//
// Created by klokov on 15.08.2022.
//

#ifndef MAIN_REP_GEOMETRY_SERVICE_H
#define MAIN_REP_GEOMETRY_SERVICE_H
#include <m2mb_types.h>

/**
 * Описание вектора.
 * (Подразумевается, что начало вектора находится в точке (0,0,0))
 */
typedef struct Vector{
    FLOAT32 gX; // координата по оси ОХ
    FLOAT32 gY; // координата по оси ОY
    FLOAT32 gZ; // координата по оси ОZ
    FLOAT32 modVector; // Модуль вектора
} Vector;

/**
     * Описание кватерниона на основе координат оси, выраженной вектором, и угла поворота
     * (Всегда подразумевается, что начало вектора находится в точке (0,0,0))
     */
typedef struct{
    Vector vector;
    FLOAT32 w;  // угол на который нужно совершить поворот вокруг переданной оси
} Quaternion;

/*!
  @brief
    Возвращает указатель на экземпляр структуры vector.
    Начало вектора всегда в точке (0,0,0)
  @details
    @param[in] x - координата по оси Х
    @param[in] y - координата по оси Y
    @param[in] z - координата по оси Z
    @param[in] dest - экземпляр куда добавить
 */
extern Vector getNewVector(FLOAT32 x, FLOAT32 y, FLOAT32 z);

/*!
  @brief
    Возвращает указатель на экземпляр структуры quaternion.
    Начало вектора всегда в точке (0,0,0)
  @details
    @param[in] vector - указатель на вектор
    @param[in] w - угол поворота
  	@return указатель на экземпляр
 */
extern Quaternion getNewQuaternion(Vector vector, FLOAT32 w);

/**
 * @brief Считает модуль вектора
 * Начало вектора всегда в точке (0,0,0)
 * @param[in] vector вектор
 * @return модуль вектора.
 */
extern FLOAT32 getMod(Vector vector);

extern FLOAT32 getYDeviation(Vector *vector);
extern FLOAT32 getZDeviation(Vector *vector);
extern FLOAT32 getXDeviation(Vector *vector);
extern BOOLEAN validateQuat(Quaternion quaternion);
extern FLOAT32 getQuatLength(Quaternion quaternion);
extern FLOAT32 getScalar(Vector v1, Vector v2);
extern Vector crossProdVector(Vector v1, Vector v2);
extern Quaternion createQuat(Vector v, FLOAT32 ang);
extern Quaternion quatScale(Quaternion q, FLOAT32 val);
extern Quaternion quatNormalize(Quaternion q);
extern void turnAroundOZ(Vector v, Vector dest);
extern Quaternion quatInvert(Quaternion q);
extern Quaternion quatMulVector(Quaternion a, Vector b);
extern Quaternion quatMulQuat(Quaternion a, Quaternion b);
extern Vector rotateVect(Quaternion q, Vector v);
extern void parseVectFromString(char *descriptor, Vector dest);
extern Quaternion parseQuatFromString(char *descriptor);
extern void vectToString(Vector vector, char *dest);
extern void quatToString(Quaternion quaternion, char *dest);
extern FLOAT32 getAngleBetweenTwoVector(Vector v1, Vector v2);
extern Vector getNorm(Vector v);
extern FLOAT32 getAxisDeviation(FLOAT32 a, FLOAT32 b);
#endif //MAIN_REP_GEOMETRY_SERVICE_H
