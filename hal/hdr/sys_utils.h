/**
 * @file sys_utils.h
 * @version 1.0.0
 * @date 08.04.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_SYS_UTILS_H_
#define HDR_SYS_UTILS_H_

#include "azx_timer.h"

typedef enum {
    MODE_ERA_OFF = 0,
    MODE_ERA = 1,
    MODE_TEST = 2,
    MODE_ECALL = 3,
    MODE_TELEMATIC = 4
} DEVICE_MODE;     // перечень режимов согласно п.7.1.4

//void finish_work_application(void);

/*!
  @brief
    Перезагрузка модема
  @details
    Перезагрузка модема
  @param[in] timeout_ms
    Интервал в мС, по истечении которого модем будет перезагружен

  @return

  @note

  @b
    Example
  @code
  @endcode
 */
void restart(UINT32 timeout_ms);

/*!
  @brief
    Инициализация и запуск программного таймера
  @details
  @param[in] timer_name - имя таймера, используемое в лог-сообщениях
  @param[in] timeout_ms - задаваемый таймаут таймера, мС
  @param[in] cb - указатель на функцию, которая будет вызвана по истечении работы таймера
  @return
	AZX_TIMER_ID - уникальный идентификатор таймера, используемый для его деинициализации или 0, если таймер не удалось инициализировать
  @note
  @code
  @endcode
 */
AZX_TIMER_ID init_timer(char *timer_name, UINT32 timeout_ms, azx_expiration_cb cb);

/*!
  @brief
    Деинициализация программного таймера
  @details
  @param[in] *id - указатель на идентификатор таймера, который будет сброшен к NO_AZX_TIMER_ID после деинициализации таймера
  @return
	BOOLEAN - TRUE - успех, FALSE - ошибка
 */
BOOLEAN deinit_timer(AZX_TIMER_ID *id);

char *get_mode_descr(DEVICE_MODE mode);

/*!
  @brief Текущий режим работы устройства согласно п.7.1.4 ГОСТ
  @return текущий режим работы устройства согласно п.7.1.4 ГОСТ
 */
DEVICE_MODE get_device_mode(void);

/*!
  @brief Установка режима работы устройства согласно п.7.1.4 ГОСТ
  @param[in] mode - устанавливаемый новый режим работы устройства
  @return
 */
void set_device_mode(DEVICE_MODE mode);

/*!
  @brief Чтение значения АЦП в мкВ
  @param[in] adc_number - номер АЦП в диапазоне от 1 до 3
  @return значение АЦП в мкВ или -1, если указан неверный номер АЦП или невозможно считать значение АЦП
  @example:  INT32 adc1 = get_adc(1);
 */
INT32 get_adc(UINT8 adc_number);

/*!
  @brief Чтение значения температуры модема в градусах цельсия
  @return значение температуры в в градусах цельсия
  @example:  INT32 temp = get_temp();
 */
INT32 get_temp(void);
#endif /* HDR_SYS_UTILS_H_ */

/*!
  @brief Менеджмент режима сна/выключения модема
  @param[in] ign_state состояние клеммы 15
  @param[in] t30_state состояние клеммы 30
 */
void manage_sleep(BOOLEAN ign_state, BOOLEAN t30_state);

void to_sleep(void);

UINT32 get_free_RAM(void);
