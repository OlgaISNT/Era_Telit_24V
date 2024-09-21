#ifndef HDR_SOFTDOG_H_
#define HDR_SOFTDOG_H_

#include "m2mb_types.h"


#define ERR_SOFTDOG_NO_ERROR 0
#define ERR_SOFTDOG_UNABLE_CREATE_PIPE 1
#define ERR_SOFTDOG_UNABLE_FORK 2
#define ERR_SOFTDOG_UNABLE_OPEN_PIPE 3
#define ERR_NOT_SOFTDOG_FILE 4
/**
 * Комментарий Андрея:
 * В отдельной папке softdog лежат исходники и бинарник программы softdog. Её необходимо скопировать в модем в папку /data/azc/mod/.
 * И не забыть дать права на исполнение командой chmod +x /data/azc/mod/softdog.
 */

//! @brief Включение softwate watchdog
//! @param[in] timeout_ms – таймаут в миллисекундах, после которого устройство будет перезагружено
//! @return 0/err_code - успех/код ошибки
UINT8 initSoftdog(int timeout_ms);


//! @brief Сброс softwate watchdog, необходимо вызывать чаще чем заданый timeout_ms
void resetSoftdog(void);


#endif // HDR_SOFTDOG_H_
