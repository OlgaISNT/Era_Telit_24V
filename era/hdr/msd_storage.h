/**
 * @file msd_storage.h
 * @version 1.0.0
 * @date 05.05.2022
 * @author: DKozenkov
 * @brief Менеджер хранилища МНД (записей) в оперативной памяти и в файловой системе в файле MSD_DAT_FILE_NAME (app_cfg.h).
 * Максимальное кол-во сохраняемых записей в хранилище определяется константой MAX_NUMBER_MSD (app_cfg.h).
 * После достижения максимального кол-ва при добавлении новой записи из хранилища удаляется самая старая запись по принципу FIFO.
 * При любых манипуляциях, изменяющих содержимое данных в хранилище, оно сохраняется в файле MSD_DAT_FILE_NAME
 */

#ifndef HDR_MSD_STORAGE_H_
#define HDR_MSD_STORAGE_H_
#include "m2mb_types.h"
#include "msd.h"

typedef struct {
	UINT8 *data;	// указатель на массив байт
	UINT32 size;    // кол-во байт в массиве data
} MSD_DATA;

/*!
  @brief
    Инициализация хранилища МНД
  @details
    Считывание МНД из файла MSD_DAT_FILE_NAME в буфер в RAM (при наличии файла)
    @param[in] record_size - кол-во байт в каждом сохраняемом в хранилище МНД
  @return TRUE - файл успешно считан или файл отсутствует;
  	    FALSE - файл MSD_DAT_FILE_NAME существует, но считывание данных из него закончилось ошибкой
 */
BOOLEAN init_MSD(UINT16 msd_size);

/*!
  @brief
    Сохранение МНД в буфере RAM и в файле MSD_DAT_FILE_NAME
  @details
    Функция сохраняет передаваемую запись в конец промежуточного буфера и сразу же в файле MSD_DAT_FILE_NAME
    При достижении максимального кол-ва сохранённых МНД (MAX_NUMBER_MSD) из буфера и файла удаляется самый старый МНД (FIFO)
    @param[in] msd - сохраняемое МНД (кол-во байт которого было указано в функции init_MSD)
  @return BOOLEAN TRUE / FALSE - успех/неуспех
 */
BOOLEAN save_MSD(T_msd *msd);

/*!
  @brief
    Чтение указанного МНД из хранилища
  @details
    @param[in] msd_number - порядковый номер МНД, считываемого из хранилища
    @param[in] msd - массив, в который необходимо сохранить считанный МНД
    @return результат операции: TRUE / FALSE - МНД считан / нет МНД с указанным номером
 */
BOOLEAN read_MSD(UINT8 msd_number, T_msd *msd);

/*!
  @brief
    Обновление содержимого указанного МНД
    @param[in] msd_number - порядковый номер обновляемого МНД
    @param[in] msd - новое содержимое обновляемого МНД (кол-во байт которого было указано в функции init_MSD)
  @return TRUE / FALSE - МНД обновлён / сбой обновления или в хранилище нет МНД с указанным номером
 */
BOOLEAN update_MSD(UINT8 msd_number, T_msd *msd);

/*!
  @brief
    Удаление указанного МНД из хранилища
  @return TRUE / FALSE - успех / сбой или в хранилище нет МНД с указанным номером
 */
BOOLEAN delete_MSD(UINT16 msd_number);

/*!
  @brief
    Удаление всего содержимого хранилища МНД
  @return TRUE / FALSE - успех (даже в случае, если хранилище уже было пусто) / сбой
 */
BOOLEAN delete_all_MSDs(void);

/*!
  @brief
    Запрос кол-ва сохранённых в хранилище МНД
  @return кол-во хранящихся МНД в хранилище
 */
UINT16 get_number_MSDs(void);
/*!
  @brief
    Поиск МНД с указанным флагом
    @param[in] *msd - указатель на МНД, содержимое которого будет обновлено в случае, если МНД был найден в хранилище
    @param[in] msd_flag - флаг, по которому в хранилище ищется МНД
    @return номер найденного МНД или -1, если МНД не найден
 */
INT32 find_msd_n(T_msd *msd, T_MSDFlag msd_flag);
BOOLEAN find_msd_inband(T_msd *msd);
BOOLEAN find_msd_sms(T_msd *msd);
BOOLEAN find_msd_saved(T_msd *msd);

BOOLEAN delete_inband_msd(void);
BOOLEAN move_inband_to_sms(void);
BOOLEAN move_sms_to_saved(void);
void print_msd_storage_status(void);

#endif /* HDR_MSD_STORAGE_H_ */
