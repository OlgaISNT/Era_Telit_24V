//
// Created by klokov on 17.10.2022.
//

#ifndef MAIN_REP_READDATABYLOCALIDENTIFIER_21_H
#define MAIN_REP_READDATABYLOCALIDENTIFIER_21_H

#include "m2mb_types.h"

#define TCM_STATUS  (0x01)
#define GSM_STATUS  (0x02)
#define ACCEL_CALIB_STATUS  (0x03)
#define SYSTEM_IDENTIFICATION  (0x80)
#define VIN  (0x81)
#define TRACEABILITY_INFORMATION  (0x84)

extern UINT16 handler_service21(char *data, char *buf, UINT16 buf_size);
extern void update2101(void);
extern void update2102(void);
extern void update2103(void);
extern void update2180(void);
extern void update2181(void);
extern void update2184(void);

#endif //MAIN_REP_READDATABYLOCALIDENTIFIER_21_H
