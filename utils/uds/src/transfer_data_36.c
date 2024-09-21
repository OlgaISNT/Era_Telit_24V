//
// Created by klokov on 17.10.2023.
//

#include "../hdr/transfer_data_36.h"
#include "../hdr/uds.h"
#include "../hdr/uds_file_tool.h"
#include "string.h"
#include "log.h"
#include "../hdr/securityAccess_27.h"

static UINT16 pos_resp36(char *buf, UINT16 buf_size, char status);
static UINT16 save_file(char *data, char *buf, UINT16 buf_size);
static UINT16 read_file(char *data, char *buf, UINT16 buf_size);

extern UINT16 handler_service36(char *data, char *buf, UINT16 buf_size){
    if (data == NULL || buf == NULL || buf_size == 0 || data[1] != TRANSFER_DATA_SERVICE) {
        return negative_resp(buf, TRANSFER_DATA_SERVICE, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    if (!isTestMode()) {
        return negative_resp(buf, TRANSFER_DATA_SERVICE, SECURITY_ACCESS_DENIED);
    }

    switch (what_i_do_now()) {
        default:
        case IDLE:      // Никаких действий не требовалось, но данные сюда зачем-то были переданы
            return negative_resp(buf, TRANSFER_DATA_SERVICE, REQUEST_SEQUENCE_ERROR);
        case SAVE_FILE: // Ранее было объявлено, что сохраняем файл
            return save_file(data, buf, buf_size);
        case GET_FILE:  // Ранее было объявлено, что читаем файл
            return read_file(data, buf, buf_size);
    }
}

static UINT16 save_file(char *data, char *buf, UINT16 buf_size){
    // Ранее было объявлено, что сохраняется файл
    UINT8 seq = data[2];
    UINT8 data_size = data[0] - 2;
    char dta[data_size];
    memcpy(&dta[0], &data[3], data_size);
    UDS_FILE_STATUS state = add_data(dta, data_size, seq);
    switch (state) {
        case INVALID_SIZE:
        case NOT_STARTED:
            return negative_resp(buf, TRANSFER_DATA_SERVICE, REQUEST_SEQUENCE_ERROR);
        case SUCCESS_PACK_SAVE:
            return pos_resp36(buf, buf_size, seq);
        case INTERNAL_ERROR:
            return negative_resp(buf, TRANSFER_DATA_SERVICE, GENERAL_PROGRAMMING_FAILURE);
        default:
        case WRONG_SEQ:
            return negative_resp(buf, TRANSFER_DATA_SERVICE, WRONG_BLOCK_SEQUENCE_COUNTER);

    }
}

static UINT16 pos_resp36(char *buf, UINT16 buf_size, char status){
    (void) buf_size;
    buf[0] = TRANSFER_DATA_SERVICE | 0x40;
    buf[1] = status;
    return 2;
}

static UINT16 read_file(char *data, char *buf, UINT16 buf_size){
    (void) buf_size;
    UINT8 seq = data[2];
    char temp[512];
    memset(&temp[0], 0x00, 512);
    UINT8 read = 0;
    UDS_FILE_STATUS status = read_data(temp, 5, &read, seq);
    switch (status) {
        case INVALID_SIZE:      // Прочитано
        case NOT_STARTED:
            return negative_resp(buf, TRANSFER_DATA_SERVICE, REQUEST_SEQUENCE_ERROR);
        case SUCCESS_PACK_READ: // Пакет успешно прочитан
            buf[0] = TRANSFER_DATA_SERVICE | 0x40;
            memcpy(&buf[1], &temp[0], read);
            return 1 + read;
        default:
        case WRONG_SEQ:
            return negative_resp(buf, TRANSFER_DATA_SERVICE, WRONG_BLOCK_SEQUENCE_COUNTER);
    }
}