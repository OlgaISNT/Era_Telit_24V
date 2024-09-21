//
// Created by klokov on 13.10.2022.
//

#ifndef MAIN_REP_SECURITYACCESS_27_H
#define MAIN_REP_SECURITYACCESS_27_H

#include <m2mb_types.h>

#define REQUEST_SEED 0x03
#define SEND_KEY 0x04
#define TEST_MODE 0x05

typedef enum {
    INVALID_KEY,
    RESET_SA
} CAN_SA_COMMAND;

extern UINT16 handler_service27(char *data, char *buf, UINT16 buf_size);
extern BOOLEAN isAccessLevel(void);
extern void resetProcedureSA(void);
extern BOOLEAN isEnableSecTimer(void);
INT32 CAN_SA_task(INT32 type, INT32 param1, INT32 param2);
extern BOOLEAN isTestMode(void);
extern void enableTestMode(void);
#endif //MAIN_REP_SECURITYACCESS_27_H
