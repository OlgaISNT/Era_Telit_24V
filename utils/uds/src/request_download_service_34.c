//
// Created by klokov on 17.10.2023.
//

#include "../hdr/request_download_service_34.h"
#include "../hdr/uds.h"
#include <string.h>
#include "log.h"
#include "../hdr/uds_file_tool.h"
#include "../hdr/securityAccess_27.h"

extern UINT16 handler_service34(char *data, char *buf, UINT16 buf_size) {
    if (data == NULL || buf == NULL || buf_size == 0) {
        if (data == NULL || buf == NULL || buf_size == 0 || data[2] != REQUEST_FILE_TRANSFER) {
            return negative_resp(buf, REQUEST_DOWNLOAD_SERVICE, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
        }
    }

    if (!isTestMode()) {
        return negative_resp(buf, REQUEST_DOWNLOAD_SERVICE, SECURITY_ACCESS_DENIED);
    }


    char dataFormatIdentifier = data[3];             // 0x00 указывает, что ни метод сжатия, ни метод шифрования не используются.
    UINT8 addressAndLengthFormatIdentifier = data[4];
    if (addressAndLengthFormatIdentifier == 0){
        return negative_resp(buf, REQUEST_DOWNLOAD_SERVICE, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }


    char path[addressAndLengthFormatIdentifier];
    memset(&path[0], 0x00, addressAndLengthFormatIdentifier);
    memcpy(&path[0], &data[5], addressAndLengthFormatIdentifier);

    UINT32 file_size = 0;
    for (int i = 0; i < 4; i++) {
        int shift = (4 - 1 - i) * 8;
        file_size += (data[5 + addressAndLengthFormatIdentifier + i] & 0x000000FF) << shift;
    }

    if (file_size == 0){
        return negative_resp(buf, REQUEST_DOWNLOAD_SERVICE, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    if(is_file_exits(path)) {
        return negative_resp(buf, REQUEST_DOWNLOAD_SERVICE, CONDITIONS_NOT_CORRECT);
    }

    if(start_save_file(file_size, (dataFormatIdentifier == 0x00) ? A_PLUS : AB, path, addressAndLengthFormatIdentifier)){
        buf[0] = REQUEST_DOWNLOAD_SERVICE | 0x40;
        buf[1] = 0x40; // 4 bytes for size
        memcpy(&buf[2], &data[5 + addressAndLengthFormatIdentifier], 4);
        return 6;
    } else {
        return negative_resp(buf, REQUEST_DOWNLOAD_SERVICE, BUSY_REPEAT_REQUEST);
    }
}

