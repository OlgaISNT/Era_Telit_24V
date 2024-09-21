//
// Created by klokov on 18.04.2022.
//

#ifndef UNTITLED2_AVERAGER_H
#define UNTITLED2_AVERAGER_H
#include "m2mb_types.h"

typedef struct Averager{
    UINT32 size;
    INT32 **m;
    UINT32 n;
    INT32 average;
    INT32 sum;
    BOOLEAN cleared;
} Averager;

/**
 * @brief Создает экземпляр структуры Averager и возвращает адрес на него
 *
 * @param[in] size размер внутреннего массива.
 *
 * @return экземпляр структуры Averager
 */
extern Averager *newAverager(UINT32 size);

/**
 * @brief Полный сброс содержимого для averager
 *
 * @param[in] averager адрес экземпляра averager в котором нужно сделать сброс
 */
extern void reset(Averager *average);

/**
 * @brief возвращает скользящее среднее для переанного экземпляра Averager
 *
 * @param[in] averager адрес экземпляра averager
 *
 * @return текущее скользящее среднее значение
 */
extern INT32 getAverage(Averager *averager);

/**
 * @brief Добавление нового значения
 *
 * @param[in] averager адрес экземпляра averager
 * @param[in] value обавляемое значение
 *
 * @return скользящее среднее значение
 */
extern INT32 add(Averager *averager, INT32 value);

/**
 * @brief  Удаляет экземпляр Averager, очищает память
 *
 * @param[in] averager адрес экземпляра averager который нужно удалить
 */
extern void deleteAverager(Averager *averager);

#endif //UNTITLED2_AVERAGER_H
