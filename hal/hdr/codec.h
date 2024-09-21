#ifndef HDR_CODEC_H_
#define HDR_CODEC_H_

#include "m2mb_types.h"

#define ERR_CODEC_NO_ERROR 0
#define ERR_CODEC_I2C_UNABLE_OPEN 1
#define ERR_CODEC_I2C_UNABLE_SET_SLAVE 2
#define ERR_CODEC_I2C_UNABLE_WRITE 3
#define ERR_CODEC_I2C_UNABLE_PARSE_HEX 4
#define ERR_CODEC_UNABLE_PLAY_FILE 5
#define ERR_CODEC_UNABLE_START_RECORD_FILE 6
#define ERR_CODEC_UNABLE_STOP_RECORD_FILE 7
#define ERR_CODEC_UNABLE_START_CODEC 8

//! @brief Включение кодека через I2C
//! @return 0/err_code - успех/код ошибки
UINT8 on_codec(void);

//! @brief осуществляется ли сейчас запись микрофона
//! @return TRUE/FALSE > да/нет
BOOLEAN is_rec(void);

//! @brief Выключение кодека через I2C
//! @return 0/err_code - успех/код ошибки
UINT8 off_codec(void);

//! @brief Отправка в кодек указанной строки конфигурации через I2C
//! @param[in] str – отправляемая в кодек строка в виде HEX-символов
//! @details Имя файла `aud_file` максимум 64 байта.
//! @return 0/err_code - успех/код ошибки
UINT8 send_to_codec(char *str);

//! @brief Воспроизведение аудиофайла
//! @param[in] aud_file – имя аудио-файла
//! @return 0/err_code - успех/код ошибки
UINT8 play_audio(char *aud_file);

//! @brief запись указываемого файла в течение указанного времени
//! @param[in] aud_file – имя записываемого аудио-файла (если файл уже есть, то он перезаписывается)
//! @param[in] duration_ms – длительность записи, мС
//! @details Имя файла `aud_file` максимум 64 байта.
//! Функция блокирующая, т.е. ждет указанное время `duration_ms` пока не завершится запись.
//! @return 0/err_code - успех/код ошибки
UINT8 record_audio(char *aud_file, UINT32 duration_ms);

extern char getWhoAmICodec(void);

#endif /* HDR_CODEC_H_ */
