//
// Created by klokov on 16.10.2023.
//

#include "../hdr/request_file_transfer_38.h"
#include "../hdr/uds.h"
#include <string.h>
#include <m2mb_fs_posix.h>
#include "log.h"
#include "../hdr/securityAccess_27.h"
#include <stdio.h>
#include <fcntl.h>
#include "m2mb_types.h"
#include "../hdr/uds_file_tool.h"


#define MAX_ANSWER_SIZE 4096
static UINT16 pos_resp_38(char *data, char *buf, UINT16 buf_size);
static UINT16 delete_file(char *data, char *buf, UINT16 buf_size);
static UINT16 get_summary(char *data, char *buf, UINT16 buf_size);
static UINT16 save_file(char *data, char *buf, UINT16 buf_size);
static UINT16 read_file(char *data, char *buf, UINT16 buf_size);
static UINT16 get_list_of_files(char *data, char *buf, UINT16 buf_size);
static UINT16 replace_file(char *data, char *buf, UINT16 buf_size);

static BOOLEAN file_exist(char *path_file);
static unsigned long get_crc32(char *path_file);

extern UINT16 handler_service38(char *data, char *buf, UINT16 buf_size){
    if (data == NULL || buf == NULL || buf_size == 0 || data[2] != REQUEST_FILE_TRANSFER){
        return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    if(!isTestMode()){ // Доступно только в тестовом режиме
        return negative_resp(buf, REQUEST_FILE_TRANSFER, SECURITY_ACCESS_DENIED);
    }

    memset(&buf[0], 0x00, buf_size);
    return pos_resp_38(data, buf, buf_size);
}

static UINT16 pos_resp_38(char *data, char *buf, UINT16 buf_size){
    MODE_OF_OPERATION mode = data[3];
    switch (mode) {
        case GET_FILE_SUMMARY_38:
            return get_summary(data, buf, buf_size);
        case ADD_FILE_38:
            return save_file(data, buf, buf_size);
        case DELETE_FILE_38: // Удалить файл
            return delete_file(data, buf, buf_size);
        case REPLACE_FILE_38:
            return replace_file(data, buf, buf_size);
        case READ_FILE_38:
            return read_file(data, buf, buf_size);
        case GET_LIST_OF_FILES_38: // Получить список файлов в директории
            return get_list_of_files(data, buf, buf_size);
        default:
            return negative_resp(buf, REQUEST_FILE_TRANSFER, REQUEST_OUT_OF_RANGE);
    }
}

static UINT16 get_list_of_files(char *data, char *buf, UINT16 buf_size){
    if (data == NULL || buf == NULL || buf_size == 0) return FALSE;

    UINT16 path_file_size = (data[4] << 8) | data[5];
    if (path_file_size == 0)  return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    char path_file[path_file_size];
    memset(path_file, 0x00, path_file_size);
    memcpy(&path_file[0], &data[6], path_file_size);

    char answer[MAX_ANSWER_SIZE];
    if(get_list_files(path_file, path_file_size, answer, MAX_ANSWER_SIZE)){
        UINT32 sz = strlen(answer);
        if (buf_size < sz) negative_resp(buf, REQUEST_FILE_TRANSFER, GENERAL_PROGRAMMING_FAILURE);
        LOG_INFO("SIZE = %d \r\n%s ", sz, answer);
        buf[0] = REQUEST_FILE_TRANSFER | 0x40;
        buf[1] = GET_LIST_OF_FILES_38;
        buf[2] = sz >> 8;
        buf[3] = sz;
        print_hex("answ: ", answer, sz);
        memcpy(&buf[4], answer, sz);
        return 4 + sz;
    }
    return negative_resp(buf, REQUEST_FILE_TRANSFER, CONDITIONS_NOT_CORRECT);
}

static UINT16 get_summary(char *data, char *buf, UINT16 buf_size){
    if (data == NULL || buf == NULL || buf_size == 0 || buf_size < 3) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    UINT16 pack_sz = ((data[0] - 0x10) << 8) | data[1];
    pack_sz += 2;

    if (pack_sz < 6) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    UINT16 path_file_size = (data[4] << 8) | data[5];
    if (pack_sz < 6 + path_file_size) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    char path_file[path_file_size];
    memset(path_file, 0x00, path_file_size);
    memcpy(&path_file[0], &data[6], path_file_size);

    memset(buf, 0x00, buf_size);
    buf[0] = REQUEST_FILE_TRANSFER | 0x40;
    buf[1] = GET_FILE_SUMMARY_38;

    // Файл существует?
    if(file_exist(path_file)){
        buf[2] = 0x01;
        // Какой размер у файла?
        UINT32 full_sz = get_size_file(path_file);
        buf[3] = (UINT8) full_sz >> 24;
        buf[4] = (UINT8) full_sz >> 16;
        buf[5] = (UINT8) full_sz >> 8;
        buf[6] = (UINT8) full_sz & 0x000000FF;

        // Какой hash у файла?
        unsigned long hash = get_crc32(path_file);
        char hs[9];
        sprintf(hs, "%08X", hash);
        memcpy(&buf[7], &hs[0], 8);
        LOG_DEBUG(">%s< is exist, size = %d, crc32=%08X\r\n", path_file, full_sz, hash);
    }
    return 15;
}

static UINT16 delete_file(char *data, char *buf, UINT16 buf_size) {
    if (data == NULL || buf == NULL || buf_size == 0) return FALSE;
    UINT16 pack_sz = ((data[0] - 0x10) << 8) | data[1];
    pack_sz += 2;
    if (pack_sz < 6) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    UINT16 path_file_size = (data[4] << 8) | data[5];

    if (pack_sz < 6 + path_file_size) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    char path_file[path_file_size];
    memset(path_file, 0x00, path_file_size);
    memcpy(&path_file[0], &data[6], path_file_size);
    if (delete_file_if_exist(path_file)== TRUE) {
        buf[0] = REQUEST_FILE_TRANSFER | 0x40;
        buf[1] = DELETE_FILE_38;
        return 2;
    } else {
        return negative_resp(buf, REQUEST_FILE_TRANSFER, CONDITIONS_NOT_CORRECT);
    }
}

static BOOLEAN file_exist(char *path_file){
    FILE *file_stream = fopen(path_file, "r");
    // Открываем файл
    if (file_stream == NULL) {
        LOG_DEBUG("FILE TRANSFER 38: File does not exist >%s<", path_file);
        return FALSE;
    } else {
        fclose(file_stream);
        return TRUE;
    }
}



static unsigned long get_crc32(char *path_file){
    if (path_file == NULL) return 0;
    unsigned long crc_table[256];
    unsigned long crc;
    for (int i = 0; i < 256; i++) {
        crc = i;
        for (int j = 0; j < 8; j++) crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;
        crc_table[i] = crc;
    };
    crc = 0xFFFFFFFFUL;
    INT32 fd = m2mb_fs_open	(path_file, M2MB_O_RDONLY);
    if (fd == -1) {
        AZX_LOG_ERROR("Can't open %s\r\n", path_file);
        return 0;
    }
    UINT8 buf[1000];
    INT32 nb = 0;
    do {
        nb = m2mb_fs_read(fd, buf, sizeof(buf));
        if (nb > 0) {
            INT32 pos = 0;
            while (pos != nb) crc = crc_table[(crc ^ buf[pos++]) & 0xFF] ^ (crc >> 8);
        }
    } while (nb > 0);

    if (m2mb_fs_close(fd) != 0) AZX_LOG_ERROR("Can't close %s\r\n", path_file);
    return crc ^ 0xFFFFFFFFUL;
}

static UINT16 save_file(char *data, char *buf, UINT16 buf_size){
    if (data == NULL || buf == NULL || buf_size == 0) return 0;
    // PATH_SIZE
    UINT16 path_file_size = (data[4] << 8) | data[5];
    if (path_file_size == 0) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    // PATH
    char path[path_file_size];
    memset(&path[0], 0x00, path_file_size);
    memcpy(&path[0], &data[6], path_file_size);

    if(is_file_exits(path)) {
        return negative_resp(buf, REQUEST_FILE_TRANSFER, CONDITIONS_NOT_CORRECT);
    }

    // FORMAT (text/byte)
    char format = data[6 + path_file_size];
    if (format > 0x01) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    UINT8 size = data[7 + path_file_size];
    if (size != 4) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    UINT32 file_size = 0;
    for (int i = 0; i < size; i++) {
        int shift = (4 - 1 - i) * 8;
        file_size += (data[8 + path_file_size + i] & 0x000000FF) << shift;
    }

    if(start_save_file(file_size, (format == 0x00) ? A_PLUS : AB, path, path_file_size) == TRUE) {
        buf[0] = REQUEST_FILE_TRANSFER | 0x40;
        buf[1] = ADD_FILE_38;
        buf[2] = size;
        memcpy(&buf[3], &data[8 + path_file_size], size);
        return 3 + size;
    } else{
        return negative_resp(buf, REQUEST_FILE_TRANSFER, CONDITIONS_NOT_CORRECT);
    }
}

static UINT16 read_file(char *data, char *buf, UINT16 buf_size){
    if(data == NULL || buf == NULL || buf_size == 0 || buf_size < 3){
        return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    UINT16 path_file_size = (data[4] << 8) | data[5];
    if (path_file_size == 0) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    // PATH
    char path[path_file_size];
    memset(&path[0], 0x00, path_file_size);
    memcpy(&path[0], &data[6], path_file_size);

    if(!is_file_exits(path)) { // Если файл не существует, то прочитать его нельзя
        return negative_resp(buf, REQUEST_FILE_TRANSFER, CONDITIONS_NOT_CORRECT);
    }

    buf[0] = REQUEST_FILE_TRANSFER | 0x40;
    buf[1] = READ_FILE_38;

    // Размер размера файла
    buf[2] = 0x04;

    // Какой размер у файла?
    UINT32 full_sz = get_size_file(path);
    buf[3] = (UINT8) full_sz >> 24;
    buf[4] = (UINT8) full_sz >> 16;
    buf[5] = (UINT8) full_sz >> 8;
    buf[6] = (UINT8) full_sz & 0x000000FF;

    // Какой hash у файла?
    unsigned long hash = get_crc32(path);
    char hs[9];
    sprintf(hs, "%08X", hash);
    memcpy(&buf[7], &hs[0], 8);
    if(!start_read_file(path, path_file_size)) return negative_resp(buf, REQUEST_FILE_TRANSFER, CONDITIONS_NOT_CORRECT);
    return 15;
}

static UINT16 replace_file(char *data, char *buf, UINT16 buf_size){
    if (data == NULL || buf == NULL || buf_size == 0 || buf_size < 3) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    UINT16 pack_sz = ((data[0] - 0x10) << 8) | data[1];
    pack_sz += 2;

    // Если размер пакета не может содержать размер pathFileLen исходного файла
    if (pack_sz < 6) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    UINT16 src_size = (data[4] << 8) | data[5];

    // Если в указанный размер пакета не может уместиться указанный pathFileLen исходного файла
    if(pack_sz < 6 + src_size) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    char src[src_size + 1];
    memset(&src[0], 0x00, src_size);
    memcpy(&src[0], &data[6], src_size);
    src[src_size] = '\0';

    UINT16 dest_index = 6 + src_size;
    // Если в данном пакете не может уместиться pathFileLen файла назначения
    if (pack_sz < (dest_index + 1)) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    UINT16 dest_size = (data[dest_index] << 8) | data[dest_index + 1];
    if (pack_sz < (dest_index + 4 + dest_size)) return negative_resp(buf, REQUEST_FILE_TRANSFER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);

    char dest[dest_size + 1];
    memset(&dest[0], 0x00, dest_size);
    memcpy(&dest[0], &data[dest_index + 2], dest_size);
    dest[dest_size] = '\0';

    UINT16 del_index = dest_index + 2 + dest_size;

    if ((data[del_index] > 0x01) || (data[del_index + 1] > 0x01)) return negative_resp(buf, REQUEST_FILE_TRANSFER, CONDITIONS_NOT_CORRECT);

    BOOLEAN del_src = (data[del_index] == 0x00) ? FALSE : TRUE;
    BOOLEAN del_dest = (data[del_index + 1] == 0x00) ? FALSE : TRUE;

    if (move_file(src, src_size, del_src, dest, dest_size, del_dest)){
        buf[0] = REQUEST_FILE_TRANSFER | 0x40;
        buf[1] = REPLACE_FILE_38;
        buf[2] = 0x00;
        return 3;
    }
    return negative_resp(buf, REQUEST_FILE_TRANSFER, CONDITIONS_NOT_CORRECT);
}