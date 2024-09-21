//
// Created by klokov on 18.10.2023.
//

#ifndef ERA_TELIT_UDS_FILE_TOOL_H
#define ERA_TELIT_UDS_FILE_TOOL_H
#include "m2mb_types.h"

typedef enum {
    START_TIMEOUT_TIMER,  // Запуск таймера отслеживания процесса загрузки/выгрузки файла
    STOP_TRANSMIT         // Полный останов всех процессов, освобождение занятых ресурсов
} UDS_FILE_TOOL_COMMAND;

typedef enum {
    NOT_STARTED,       // Запуск таймера отслеживания процесса загрузки/выгрузки файла
    SUCCESS_PACK_SAVE, // Текущий пакет успешно сохранен
    SUCCESS_PACK_READ, // Текущий пакет успешно прочитан
    REPEAT_LAST,       // Повторить последний пакет
    SUCCESS,           // Файл успешно обработан
    INTERNAL_ERROR,    // Внутренняя ошибка при сохранении
    INVALID_SIZE,      // Файл весит больше чем планировалось
    WRONG_SEQ,         // Ошибка счетчика
} UDS_FILE_STATUS;


typedef enum{
    SAVE_FILE = 1,         // Сохраняем файл
    GET_FILE = 2,          // Выгружаем файл
    IDLE = 3              // Простаивание
} UDS_FILE_CURRENT_PROC;

typedef enum {
    AB,     // ab - Дописывает информацию к концу двоичного файла. Если файла нет, то создаем его
    A_PLUS  // а+ - Дописывает информацию к концу файла или создает файл для чтения/записи (по умолчанию открывается как текстовый файл).
} UDS_FILE_MODE;

extern UINT32 get_size_file(char *path_file);
extern INT32 uds_file_tool_task(INT32 type, INT32 param1, INT32 param2);
extern BOOLEAN start_read_file(char *path_file, UINT32 path_size);
extern BOOLEAN start_save_file(UINT32 expect_size, UDS_FILE_MODE mode, char *path_file, UINT32 path_size);
extern UDS_FILE_STATUS add_data(char *data, UINT32 data_size, UINT8 sequence);
extern UDS_FILE_STATUS read_data(char *data, UINT32 data_size, UINT8 *read_bytes, UINT8 sequence);
extern BOOLEAN finish_all_deals(void);
extern BOOLEAN delete_file_if_exist(char *path_file);
extern BOOLEAN is_file_exits(char *path_file);
extern UDS_FILE_CURRENT_PROC what_i_do_now(void);
extern BOOLEAN get_list_files(char *path, UINT32 path_size, char *buffer, UINT32 buffer_size);
extern void stop_action(void);
extern BOOLEAN move_file(char *src, UINT16 src_size, BOOLEAN src_del, char *dest, UINT16 dest_size, BOOLEAN dest_del);

#endif //ERA_TELIT_UDS_FILE_TOOL_H
