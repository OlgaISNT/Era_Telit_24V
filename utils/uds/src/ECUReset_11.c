//
// Created by klokov on 02.02.2023.
//

#include "../hdr/ECUReset_11.h"
#include "m2mb_types.h"
#include "../hdr/uds.h"
#include "log.h"
#include "atc_era.h"
#include "sys_param.h"
#include "tasks.h"
#include "sys_utils.h"
#include "msd_storage.h"

#define HARD_RESET 0x60
#define GSM_RESET 0x61
#define GPS_RESET 0x62

static BOOLEAN hardReset(void);
static BOOLEAN gsmReset(void);
static BOOLEAN gpsReset(void);

static UINT16 pos_resp_11(char data_id, char *buf, UINT16 buf_size);
extern UINT16 handler_service11(char *data, char *buf, UINT16 buf_size){
    (void) buf_size;
    if (data == NULL || data[1] != ECU_RESET_SERVICE){
        return negative_resp(buf, ECU_RESET_SERVICE, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Проверка длины фрейма (фрейм с запросом всегда имеет длину 0х02)
    if (data[0] != 0x02){
        return negative_resp(buf, ECU_RESET_SERVICE, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Проверка пустых байтов фрейма
    for (UINT8 i = 3; i < 8; i++){
        if (data[i] != 0xAA){
            return negative_resp(buf, ECU_RESET_SERVICE, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
        }
    }

    // Собираем ответ
    return pos_resp_11(data[2], buf, buf_size);
}

static UINT16 pos_resp_11(char data_id, char *buf, UINT16 buf_size){
    (void) buf_size;
    switch (data_id) {
        case HARD_RESET:
            if (!hardReset()){
                return negative_resp(buf, ECU_RESET_SERVICE, CONDITIONS_NOT_CORRECT);
            }
            break;
        case GSM_RESET:
            if (!gsmReset()){
                return negative_resp(buf, ECU_RESET_SERVICE, CONDITIONS_NOT_CORRECT);
            }
            break;
        case GPS_RESET:
            if (!gpsReset()){
                return negative_resp(buf, ECU_RESET_SERVICE, CONDITIONS_NOT_CORRECT);
            }
            break;
        default:
            return negative_resp(buf, ECU_RESET_SERVICE, SUBFUNCTION_NOT_SUPPORTED);
    }
    buf[0] = 0x51;
    buf[1] = data_id;
    return 2;
}

static BOOLEAN hardReset(void){
    return FALSE;
}

static BOOLEAN gsmReset(void){
    delete_all_MSDs();
    restart(1000);
    return TRUE;
}

static BOOLEAN gpsReset(void){
    send_to_gnss_task(RESET_GNSS, 1, 0);
    send_to_gnss_task(CLEAR_LAST_RELIABLE_COORD,  0, 0);
    return TRUE;
}