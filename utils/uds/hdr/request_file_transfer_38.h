//
// Created by klokov on 16.10.2023.
//

#ifndef ERA_TELIT_REQUEST_FILE_TRANSFER_38_H
#define ERA_TELIT_REQUEST_FILE_TRANSFER_38_H
#include "m2mb_types.h"

typedef enum {
    GET_FILE_SUMMARY_38 = 0x00,  // 0x00
    ADD_FILE_38 = 0x01,          // 0x01
    DELETE_FILE_38 = 0x02,       // 0x02
    REPLACE_FILE_38 = 0x03,      // 0x03
    READ_FILE_38 = 0x04,         // 0x04
    READ_DIRECTORY_38 = 0x05,    // 0x05
    GET_LIST_OF_FILES_38 = 0x06, // 0x06
    RESERVED_38                  // 0x07-0xFF
} MODE_OF_OPERATION;

extern UINT16 handler_service38(char *data, char *buf, UINT16 buf_size);

#endif //ERA_TELIT_REQUEST_FILE_TRANSFER_38_H
