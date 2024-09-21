//
// Created by klokov on 18.04.2022.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "averager.h"

static void fillArray(Averager *averager, INT32 value);

extern Averager *newAverager(UINT32 size){
    Averager *averager = (Averager*) m2mb_os_malloc(sizeof(Averager));
    averager->size = size;
    averager->m = m2mb_os_malloc(sizeof(INT32*) * size);
    reset(averager);
    fillArray(averager, 0);
    return averager;
}

extern void reset(Averager *average){
    fillArray(average, 0);
    average->n = 0;
    average->sum = 0;
    average->average = 0;
    average -> cleared = TRUE;
}

extern INT32 getAverage(Averager *averager){
    return averager -> average;
}

extern INT32 add(Averager *averager, INT32 value){
    if(averager->cleared == TRUE){
        averager->cleared = FALSE;
        fillArray(averager, value);
        averager->sum = averager->size * value;
        averager->average = value;
    } else{
        INT32 lastVal = (INT32) *(averager->m + averager->n);
        averager->sum = averager->sum - lastVal;
        averager->sum = averager->sum + value;
        averager->average = (INT32) averager->sum /(INT32)  averager->size;
        *(averager->m + averager->n) = (INT32 *) value;
    }
    averager->n++;
    if (averager->n >= averager->size){
        averager->n = 0;
    }
    return averager->average;
}

static void fillArray(Averager *averager, INT32 value){
    INT32 *x = (INT32*) value;
    for (UINT32 i = 0; i < averager->size; ++i) {
        *(averager->m + i) = x;
    }
}

extern void deleteAverager(Averager *averager){
    m2mb_os_free(averager->m);
    m2mb_os_free(averager);
}
