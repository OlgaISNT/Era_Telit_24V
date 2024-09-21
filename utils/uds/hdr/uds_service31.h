/**
 * @file uds_service31.h
 * @version 1.0.0
 * @date 31.08.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef UART_MC_UDS_SERVICE31_H_
#define UART_MC_UDS_SERVICE31_H_

typedef enum {
    CHECK_AVAILABLE_NETWORK,
    START_MIC_REC,
    PLAY_LAST_MIC_REC,
    EXECUTE_AT,
    UNKNOWN
} TEST_MODE_TASK_COMMAND;

UINT16 handler_service31(UINT8 *data, UINT16 data_size, UINT8 *buf, UINT16 buf_size);
INT32 TEST_MODE_task(INT32 type, INT32 param1, INT32 param2);

extern UINT8 getAvailableNetworks(void);
#endif /* UART_MC_UDS_SERVICE31_H_ */
