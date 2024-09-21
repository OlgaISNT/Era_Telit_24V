//
// Created by klokov on 11.10.2022.
//

#ifndef MAIN_REP_READDATABYIDENTIFIER_22_H
#define MAIN_REP_READDATABYIDENTIFIER_22_H
#include "m2mb_types.h"

UINT16 handler_service22(UINT16 data_id, char *buf, UINT16 max_len);
extern void readDataById(UINT16 data_id, char *buf, UINT16 max_len);
extern void get_parameter(UINT16 data_id);

#endif //MAIN_REP_READDATABYIDENTIFIER_22_H
