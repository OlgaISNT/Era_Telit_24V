//
// Created by klokov on 21.10.2022.
//

#ifndef MAIN_REP_READDTCINFORMATION_19_H
#define MAIN_REP_READDTCINFORMATION_19_H

#include "m2mb_types.h"

#define REPORT_DTC_BY_STATUS_MASK  (0x02)

extern UINT16 handler_service19(char *data, char *buf, UINT16 buf_size);

typedef struct {
    BOOLEAN TEST_FAILED;                              // BIT0 01h
    BOOLEAN TEST_FAILED_THIS_MONITORING_CYCLE;        // BIT1 02h
    BOOLEAN PENDING_DTC;                              // BIT2 04h
    BOOLEAN CONFIRMED_DTC;                            // BIT3 08h
    BOOLEAN TEST_NOT_COMPLETED_SINCE_LAST_CLEAR;      // BIT4 10h
    BOOLEAN TEST_FAILED_SINCE_LAST_CLEAR;             // BIT5 20h
    BOOLEAN TEST_NOT_COMPLETED_THIS_MONITORING_CYCLE; // BIT6 40h
    BOOLEAN WARNING_INDICATOR_REQUESTED;              // BIT7 80h
} DTC_STATUS_MASK ;

#endif //MAIN_REP_READDTCINFORMATION_19_H
