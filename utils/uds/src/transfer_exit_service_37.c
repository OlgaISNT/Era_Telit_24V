//
// Created by klokov on 17.10.2023.
//

#include "../hdr/transfer_exit_service_37.h"
#include "../hdr/uds.h"
#include "../hdr/uds_file_tool.h"
#include "../hdr/securityAccess_27.h"

extern UINT16 handler_service37(char *data, char *buf, UINT16 buf_size){
    if(data == NULL || buf == NULL || buf_size == 0){
        return negative_resp(buf, TRANSFER_EXIT_SERVICE, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    if (data[1] != TRANSFER_EXIT_SERVICE || data[0] != 1){
        return negative_resp(buf, TRANSFER_EXIT_SERVICE, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    if (!isTestMode()) {
        return negative_resp(buf, TRANSFER_EXIT_SERVICE, SECURITY_ACCESS_DENIED);
    }

    stop_action();

    buf[0] = TRANSFER_EXIT_SERVICE | 0x40;
    return 1;
}