//
// Created by klokov on 15.08.2022.
//
#include "../hdr/geometry_service.h"
#include <m2mb_os_api.h>
#include <memory.h>
#include <iwlib.h>
#include "log.h"
#include "../hdr/geometry_service.h"
#include "char_service.h"

#define ONE_RADIAN 57.29577951308f
static void getSubDataBetween(char *str, char a, char b, char *dest);

extern Vector getNewVector(FLOAT32 x, FLOAT32 y, FLOAT32 z) {
    Vector dest = {0};
    dest.gX = x;
    dest.gY = y;
    dest.gZ = z;
    dest.modVector = getMod(dest);
    return dest;
}

extern Quaternion getNewQuaternion(Vector vector, FLOAT32 w){
    Quaternion dest;
    dest.vector.gX = vector.gX;
    dest.vector.gY = vector.gY;
    dest.vector.gZ = vector.gZ;
    dest.vector.modVector = vector.modVector;
    dest.w = w;
    return dest;
}

extern FLOAT32 getMod(Vector vector){
    return sqrtf(vector.gX * vector.gX + vector.gY * vector.gY + vector.gZ * vector.gZ);
}

// Получить пропорциональный угол отклонения по оси Y
extern FLOAT32 getYDeviation(Vector *vector) {
    return (FLOAT32) (acosf(vector->gY / vector->modVector) * ONE_RADIAN - 90) * (-1);
}

// Получить пропорциональный угол отклонения по оси Z
extern FLOAT32 getZDeviation(Vector *vector) {
    FLOAT32 ang = (FLOAT32) (acosf(vector->gZ / vector->modVector) * ONE_RADIAN - 180);
    ang = (vector->gZ >= 0) ? ang : ang * (-1);
    return ang;
}

// Получить пропорциональный угол отклонения по оси X
extern FLOAT32 getXDeviation(Vector *vector) {
    return (FLOAT32) (acosf(vector->gY / vector->modVector) * ONE_RADIAN - 90) * (-1);
}

// Расчет длины кватерниона
extern FLOAT32 getQuatLength(Quaternion quaternion){
    return (FLOAT32) sqrtf(quaternion.w * quaternion.w
                          + quaternion.vector.gX * quaternion.vector.gX
                          + quaternion.vector.gY * quaternion.vector.gY
                          + quaternion.vector.gZ * quaternion.vector.gZ);
}

// Проверка кватерниона
extern BOOLEAN validateQuat(Quaternion quaternion){
    if (getQuatLength(quaternion) != 1.0f) return FALSE;
    return TRUE;
}

// Скалярное произведение векторов
extern FLOAT32 getScalar(Vector v1, Vector v2){
    return (v1.gX * v2.gX + v1.gY * v2.gY + v1.gZ * v2.gZ);
}

// Векторное произведение векторов
extern Vector crossProdVector(Vector v1, Vector v2) {
    FLOAT32 x = ((v1.gY * v2.gZ) - (v1.gZ * v2.gY));
    FLOAT32 y = ((v1.gZ * v2.gX) - (v1.gX * v2.gZ));
    FLOAT32 z = ((v1.gX * v2.gY) - (v1.gY *v2.gX));
    return getNewVector(x, y, z);
}

// Нормирование вектора
extern Vector getNorm(Vector v){
    FLOAT32 modVector = getMod(v);
    FLOAT32 gx = v.gX/modVector;
    FLOAT32 gy = v.gY/modVector;
    FLOAT32 gz = v.gZ/modVector;
    return getNewVector(gx,gy,gz);
}

// Получить кватернион из вектора и угла разворота вокруг него
extern Quaternion createQuat(Vector v, FLOAT32 ang){
    Vector rotV = getNorm(v);
    FLOAT32 w = cosf(ang / 2 / ONE_RADIAN);
    FLOAT32 x = rotV.gX * sinf(ang / 2 / ONE_RADIAN);
    FLOAT32 y = rotV.gY * sinf(ang / 2 / ONE_RADIAN);
    FLOAT32 z = rotV.gZ * sinf(ang / 2 / ONE_RADIAN);
    Vector vect = getNewVector(x,y,z);
    return getNewQuaternion(vect, w);
}

extern Quaternion quatScale(Quaternion q, FLOAT32 val){
    FLOAT32 w = q.w * val;
    FLOAT32 x = q.vector.gX * val;
    FLOAT32 y = q.vector.gY * val;
    FLOAT32 z = q.vector.gZ * val;
    Vector vector = getNewVector(x, y, z);
    return getNewQuaternion(vector, w);
}

extern Quaternion quatNormalize(Quaternion q){
    FLOAT32 len = 1/getQuatLength(q);
    return quatScale(q, len);
}

// Получить пропорциональный угол отклонения оси
extern FLOAT32 getAxisDeviation(FLOAT32 a, FLOAT32 b){
    return acosf(a / b) * ONE_RADIAN ;
}

///**
// * Поворот вокруг оси Z
// * Z' = Z;
// * X' = X * cos(a) - Y * sin(a);
// * Y' = X * sin(a) + Y * cos(a); => при Y' = 0; a = -atang(y/x)
// */
extern void turnAroundOZ(Vector v, Vector dest){
    FLOAT32 ang = 0 - atanf(v.gY / v.gX);
    FLOAT32 x = (v.gX * cosf(ang)) - v.gY * sinf(ang);
    dest = getNewVector(x, 0.0f, 0.0f);
    (void) dest;
}

extern Quaternion quatInvert(Quaternion q){
    FLOAT32 x = (-1 * q.vector.gX);
    FLOAT32 y = (-1 * q.vector.gY);
    FLOAT32 z = (-1 * q.vector.gZ);
    Vector v = getNewVector(x, y, z);
    return getNewQuaternion(v, q.w);
}

extern Quaternion quatMulVector(Quaternion a, Vector b){
    FLOAT32 w = - a.vector.gX * b.gX - a.vector.gY * b.gY - a.vector.gZ * b.gZ;
    FLOAT32 x = a.w * b.gX + a.vector.gY * b.gZ - a.vector.gZ * b.gY;
    FLOAT32 y = a.w * b.gY - a.vector.gX * b.gZ + a.vector.gZ * b.gX;
    FLOAT32 z = a.w * b.gZ + a.vector.gX * b.gY - a.vector.gY * b.gX;
    Vector v= getNewVector(x,y,z);
    return getNewQuaternion(v,w);
}

extern Quaternion quatMulQuat(Quaternion a, Quaternion b){
    FLOAT32 w = a.w * b.w - a.vector.gX * b.vector.gX - a.vector.gY * b.vector.gY - a.vector.gZ * b.vector.gZ;
    FLOAT32 x = a.w * b.vector.gX + a.vector.gX * b.w + a.vector.gY * b.vector.gZ - a.vector.gZ * b.vector.gY;
    FLOAT32 y = a.w * b.vector.gY - a.vector.gX * b.vector.gZ + a.vector.gY * b.w + a.vector.gZ * b.vector.gX;
    FLOAT32 z = a.w * b.vector.gZ + a.vector.gX * b.vector.gY - a.vector.gY * b.vector.gX + a.vector.gZ * b.w;
    Vector v = getNewVector(x,y,z);
    return getNewQuaternion(v,w);
}

// Поворот вектора кватернионом
extern Vector rotateVect(Quaternion q, Vector v){
    Quaternion t = quatMulVector(q, v);
    Quaternion qInvert = quatInvert(q);
    Quaternion norm = quatNormalize(qInvert);
    Quaternion x = quatMulQuat(t, norm);
    return getNewVector(x.vector.gX, x.vector.gY, x.vector.gZ);

}

extern void vectToString(Vector vector, char *dest) {
    sprintf(dest, "vx%fy%fz%f", vector.gX, vector.gY, vector.gZ);
}

extern void quatToString(Quaternion quaternion, char *dest) {
    sprintf(dest, "qw%fx%fy%fz%f", quaternion.w, quaternion.vector.gX, quaternion.vector.gY, quaternion.vector.gZ);
}

// Получить угол между двумя векторами
extern FLOAT32 getAngleBetweenTwoVector(Vector v1, Vector v2){
    return (FLOAT32) (getScalar(v1, v2) / (getMod(v1) * getMod(v2)));
}

static void getSubDataBetween(char *str, char a, char b, char *dest){
    UINT8 startIndex = 0;
    UINT8 endIndex = 0;

    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == a) startIndex = i;
        if (str[i] == b) endIndex = i;
    }
    if (b == ' ') endIndex = strlen(str) - 1;

    memcpy(&dest[0], &str[startIndex + 1], (endIndex - startIndex + 1));
    if (strlen(dest) == 0) {
        dest = "0.0";
    }
}

// Пример: "vx0y0z-0.014340015"
extern void parseVectFromString(char *descriptor, Vector dest) {
    trim(descriptor);
    if (!descriptor[0] == 'v') return;

    char xStr[30];
    char yStr[30];
    char zStr[30];
    getSubDataBetween(descriptor,'x','y',xStr);
    getSubDataBetween(descriptor,'y','z',yStr);
    getSubDataBetween(descriptor,'z',' ',zStr);
    FLOAT32 x = atof(xStr);
    FLOAT32 y = atof(yStr);
    FLOAT32 z = atof(zStr);
    dest = getNewVector(x, y, z);
    (void) dest;
}

// Пример: "qw0.9998972x0y0z-0.014340015"
extern Quaternion parseQuatFromString(char *descriptor) {
    trim(descriptor);
//    if (!descriptor[0] == 'q') return; // Лучше вернуть NULL, чем пустой кватернион

    char wStr[30];
    char xStr[30];
    char yStr[30];
    char zStr[30];
    getSubDataBetween(descriptor,'w','x',wStr);
    getSubDataBetween(descriptor,'x','y',xStr);
    getSubDataBetween(descriptor,'y','z',yStr);
    getSubDataBetween(descriptor,'z',' ',zStr);

    FLOAT32 w = atof(wStr);
    FLOAT32 x = atof(xStr);
    FLOAT32 y = atof(yStr);
    FLOAT32 z = atof(zStr);
    Vector v = getNewVector(x,y,z);
    return getNewQuaternion(v, w);
}