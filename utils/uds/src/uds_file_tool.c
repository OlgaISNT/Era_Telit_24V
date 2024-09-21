//
// Created by klokov on 18.10.2023.
//

#include "../hdr/uds_file_tool.h"
#include "log.h"
#include "tasks.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <m2mb_fs_posix.h>
#include "log.h"
#include <dirent.h>
#include "m2mb_types.h"
#include "at_command.h"
#include "log.h"
#include "sys_utils.h"
#include <fcntl.h>

#define TRANSMIT_TIMEOUT  2000
#define MAX_PATH_SIZE     1024

static FILE *file_stream = NULL;
static char path_name[MAX_PATH_SIZE];
static UINT32 file_exp_size = 0;
static UINT32 current_file_size = 0;
static UINT8 expect_seq = 1;
static BOOLEAN is_tool_busy = FALSE;
static BOOLEAN has_new_data = FALSE;
static UDS_FILE_CURRENT_PROC current_deal = IDLE;
static BOOLEAN copy_file(char *src, char *dest);

static BOOLEAN start_timeout_timer(void);

extern INT32 uds_file_tool_task(INT32 type, INT32 param1, INT32 param2){
    (void) param1;
    (void) param2;
    switch (type) {
        case START_TIMEOUT_TIMER:
            return start_timeout_timer();
        case STOP_TRANSMIT:
            return M2MB_RESULT_SUCCESS;
    }
    return M2MB_RESULT_SUCCESS;
}

extern BOOLEAN start_read_file(char *path_file, UINT32 path_size){
    if (path_file == NULL || path_size == 0 || is_tool_busy == TRUE || file_stream != NULL) return FALSE;

    // Если файл не существует
    if (!is_file_exits(path_file)) return FALSE;
    is_tool_busy = TRUE;
    // Запуск таймера отслеживающего таймаут передачи
    if(send_to_uds_file_tool_task(START_TIMEOUT_TIMER, 0, 0)) return FALSE;

    file_exp_size = get_size_file(path_file);
    if (file_exp_size == 0) return FALSE;
    memset(&path_name[0], 0x00, MAX_PATH_SIZE);
    memcpy(&path_name[0], &path_file[0], path_size);

    file_stream = fopen(path_file, "rb");
    // Открываем файл
    if (file_stream == NULL) {
        LOG_ERROR("Can't open file for read >%s<", path_file);
        return FALSE;
    }

    LOG_DEBUG("GET_FILE: Start read file %s. Expect size %d.", path_name, file_exp_size);
    current_deal = GET_FILE;
    return TRUE;
}

extern UINT32 get_size_file(char *path_file){
    if (path_file == NULL) return 0;
    UINT32 end_file_index = 0;
    FILE *stream = fopen(path_file, "rb");
    if (stream == NULL) {
        LOG_DEBUG("Unable to open file >%s<", path_file);
        return FALSE;
    }

    // Индекс окончания файла. Следующий индекс после индекса последнего символа в файле.
    if(fseek(stream, 0L, SEEK_END)) return FALSE;
    end_file_index = ftell(stream);

    fclose(stream);
    return end_file_index;
}


extern BOOLEAN start_save_file(UINT32 expect_size, UDS_FILE_MODE mode, char *path_file, UINT32 path_size){
    if (path_file == NULL || is_tool_busy == TRUE || path_size == 0 || file_stream != NULL) return FALSE;

    // Если файл существует
    if (is_file_exits(path_file)) return FALSE;

    is_tool_busy = TRUE;
    // Запуск таймера отслеживающего таймаут передачи
    if(send_to_uds_file_tool_task(START_TIMEOUT_TIMER, 0, 0)) return FALSE;

    file_exp_size = expect_size;
    memset(&path_name[0], 0x00, MAX_PATH_SIZE);
    memcpy(&path_name[0], &path_file[0], path_size);

    char *md = (mode == AB) ? "ab" : "a+";

    file_stream = fopen(path_file, md);
    if (file_stream == NULL) {
        LOG_DEBUG("Unable to create upload file >%s<. MODE %s ", path_file, md);
        return FALSE;
    }

    LOG_DEBUG("SAVE_FILE: Start save file %s. Expect size %d. Type %s", path_name, expect_size, md);
    current_deal = SAVE_FILE;
    return TRUE;
}

extern UDS_FILE_CURRENT_PROC what_i_do_now(void){
    return current_deal;
}

extern UDS_FILE_STATUS read_data(char *data, UINT32 data_size, UINT8 *read_bytes, UINT8 sequence){
    if (file_stream == NULL || is_tool_busy == FALSE || current_deal != GET_FILE) {
        return NOT_STARTED;
    }
    if (expect_seq != sequence) {
        LOG_DEBUG("SEQ expect/current %02X/%02X", expect_seq, sequence);
        return WRONG_SEQ;
    }


    char temp[data_size];
    memset(&temp[0], 0x00, data_size);
    UINT8 read = fread(&temp[0], sizeof(char), data_size, file_stream);
    if (read == 0){
        finish_all_deals();
        return INVALID_SIZE;
    }
    print_hex("READ: ", temp, read);
    current_file_size += read;


    // Читаем в data количество которое указано в data_size в качестве sequence устанавливаем переданный
    data[0] = sequence;
    memcpy(&data[1], &temp[0], read);

    *read_bytes = read + 1;
    expect_seq++; // Ожидаем следующий блок
    has_new_data = TRUE;

    if (current_file_size == file_exp_size){
        finish_all_deals();
        LOG_DEBUG("File transferred successfully");
    }
    return SUCCESS_PACK_READ;
}

extern UDS_FILE_STATUS add_data(char *data, UINT32 data_size, UINT8 sequence){
    if (file_stream == NULL || is_tool_busy == FALSE || current_deal != SAVE_FILE) return NOT_STARTED;

    if(expect_seq != sequence) {
        LOG_DEBUG("Block counter error. Expect/actual - %d/%d", expect_seq, sequence);
        return WRONG_SEQ;
    }

    // Дописываем данные в файл
    if (fwrite(data, sizeof(char), data_size, file_stream) != data_size) {
        print_hex("Error writing data to file: ", data, data_size);
        return INTERNAL_ERROR;
    }
    has_new_data = TRUE;
    current_file_size += data_size;

    print_hex("SAVED: ", data, data_size);

    // Если все байты файла были получены
    if(current_file_size == file_exp_size){
        LOG_DEBUG("File %s successfully downloaded.");
        finish_all_deals();
        return SUCCESS;
    }

    if (current_file_size > file_exp_size){
        LOG_DEBUG("An attempt was made to store more bytes of data in a file than intended. current/expect = %d/%d", current_file_size, file_exp_size);
        // Удаляем файл
        delete_file_if_exist(path_name);
        finish_all_deals();
        return INVALID_SIZE;
    }

    expect_seq++; // Ожидаем следующий блок
    return SUCCESS_PACK_SAVE;
}

extern BOOLEAN delete_file_if_exist(char *path_file){
    if (path_file == NULL) return FALSE;

    if (m2mb_fs_unlink((void *) path_file) == 0) {
        LOG_DEBUG("File >%s< deleted successfully ", path_file);
        return TRUE;
    } else {
        LOG_DEBUG("Could not delete the file >%s<", path_file);
        return FALSE;
    }
}

extern BOOLEAN finish_all_deals(void){
    // Освобождение ресурсов, sync
    if (file_stream != NULL){
        fflush(file_stream);
        int fd = fileno(file_stream);
        fsync(fd);
        fclose(file_stream);
        file_stream = NULL;
    }

    if (current_deal == SAVE_FILE){
        LOG_DEBUG("The client stopped save file operation");
        if ((file_exp_size != 0) & (current_file_size != file_exp_size)){
            LOG_DEBUG("The file was not downloaded completely current/expect = %d/%d", current_file_size, file_exp_size);
            delete_file_if_exist(path_name);
        }
    } else if (current_deal == GET_FILE){
        LOG_DEBUG("The client stopped getting file operation");
    } else {
        LOG_DEBUG("There is no operation with files right now");
    }


    current_deal = IDLE;
    expect_seq = 1;
    current_file_size = 0;
    file_exp_size = 0;
    memset(&path_name[0], 0x00, MAX_PATH_SIZE);
    is_tool_busy = FALSE;
    return TRUE;
}

static BOOLEAN start_timeout_timer(void){
    while(is_tool_busy == TRUE){
        azx_sleep_ms(TRANSMIT_TIMEOUT);
            if(!has_new_data){
                finish_all_deals();
            }
        has_new_data = FALSE;
    }
    LOG_DEBUG("Timeout timer was finish");
    return TRUE;
}

extern BOOLEAN is_file_exits(char *path_file){
    if (path_file == NULL) return FALSE;
    FILE *file_stream = fopen(path_file, "r");
    // Открываем файл
    if (file_stream == NULL) {
        LOG_DEBUG("File does not exist >%s<", path_file);
        return FALSE;
    } else {
        fclose(file_stream);
        return TRUE;
    }
}

extern void stop_action(void){
    finish_all_deals();
}

extern BOOLEAN get_list_files(char *path, UINT32 path_size, char *buffer, UINT32 buffer_size){
    if (path == NULL || path_size == 0 || buffer == NULL || buffer_size == 0) return FALSE;

    LOG_DEBUG("Get list of files in >%s<", path);

    DIR *d = opendir(path);
    if (d == NULL) return FALSE;

    memset(&buffer[0], 0x00 ,buffer_size);
    struct dirent *dir;
    while ((dir = readdir(d)) != NULL) {
        if(dir->d_type==DT_REG){
            strcat(buffer,dir->d_name);
            strcat(buffer, "\r\n");
        }
    }

    closedir(d);
    return TRUE;
}

extern BOOLEAN move_file(char *src, UINT16 src_size, BOOLEAN src_del, char *dest, UINT16 dest_size, BOOLEAN dest_del){
    if (src == NULL || src_size == 0 || dest == NULL || dest_size == 0) return FALSE;
    if (strcmp(src, dest) == 0) return FALSE;
    LOG_DEBUG("SRC: %02X %d >%s<", src_del, src_size, src);
    LOG_DEBUG("DST: %02X %d >%s<", dest_del, dest_size, dest);

    if (!is_file_exits(src)) {  // Если исходный файл не существует
        LOG_DEBUG("Source file >%s< does not exist", src);
        // Перемещать нечего
        return FALSE;
    } else { // Если исходный файл существует
        BOOLEAN dest_exist = is_file_exits(dest);
        if ((dest_exist == TRUE) & (dest_del == FALSE)){
            // Если файл назначения существует и установлен запрет на его удаление
            LOG_DEBUG("Destenation file is exist and it can't delete him");
            return FALSE;
        } else if ((dest_exist == TRUE) & (dest_del == TRUE)) {
            // Если файл назначения существует и требуется его удалить
            delete_file_if_exist(dest);
            if(!copy_file(src, dest)) return FALSE;
            // Если требуется удалить исходный файл
            if (src_del == TRUE) delete_file_if_exist(src);
        } else {
            // Если файл назначения не существует
            if(!copy_file(src, dest)) return FALSE;
            // Если требуется удалить исходный файл
            if (src_del == TRUE) delete_file_if_exist(src);
        }
        return TRUE;
    }
}

static BOOLEAN copy_file(char *src, char *dest){
    int fd_in = 0, fd_out = 0;
    ssize_t ret_in = 0,ret_out = 0;

    fd_in = open(src, O_RDONLY);
    if(fd_in == -1) {
        LOG_ERROR("Error with open file >%s< for source", src);
        return FALSE;
    }

    fd_out = open(dest, O_WRONLY | O_CREAT, 0644);
    if (fd_out == -1) {
        LOG_ERROR("Error with open file >%s< for destenation", dest);
        return FALSE;
    }

    char buffer[256];

    while ((ret_in = read(fd_in, &buffer, 256)) > 0)
    {
        ret_out = write(fd_out,  &buffer, (ssize_t) ret_in);
        if (ret_out != ret_in){
            LOG_ERROR("Error with write >%s< to dest %s.", src, dest);
            return FALSE;
        }
    }

    LOG_INFO("Copy file successfully. Src: %s Dest: %s ", src, dest);

    close(fd_in);
    close(fd_out);
    return TRUE;
}