/*
 * files.h
 * Created on: 31.03.2022, Author: DKozenkov     
 */

#ifndef FILES_FILES_H_
#define FILES_FILES_H_

#include "m2mb_types.h"
/*!
  @brief
    Определение размера файла

  @details
    Функция определяет размер указанного файла
  @param[in] file_path
    Имя файла, размер которого нужно определить

  @return
    Размер файла в байтах или -1, если определить размер не удалось

  @note
    The file and directory update(creating/deleting/writing) is available only on "/data" and its-subdirectories.

  @b
    Example
  @code
    <C code example>
  @endcode
 */
int get_file_size(char *file_path);

BOOLEAN delete_file(char *file_path);

/**
 * @brief Проверяет размер файла
 *
 * @param[in] file_path путь к файлу. (Например: "/data/aplay/md_test.wav")
 * @param[in] expectSize ожидаемый размер файла в байтах. (Например: 56398)
 *
 * @return TRUE если размер файла соответствует ожидаемому. Иначе - FALSE.
 */
BOOLEAN check_file_size(char *file_path, UINT32 expectSize);

#endif /* FILES_FILES_H_ */
