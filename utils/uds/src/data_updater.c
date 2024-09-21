//
// Created by klokov on 27.02.2023.
//


#include "../hdr/data_updater.h"
#include "log.h"
#include "azx_utils.h"
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "../hdr/readDataByLocalIdentifier_21.h"

static UINT32 UPDATE_TIME = 400; // Обновление данных UDS каждые 900 мСек

static BOOLEAN isStart = FALSE;
static void updateCycle(void);

INT32 UPDATER_task(INT32 type, INT32 param1, INT32 param2){
    (void) param1;
    (void) param2;
    switch (type) {
        case UPDATE_DATA:
            LOG_DEBUG("DATA_UPDATER: start");
            updateCycle();
            return M2MB_OS_SUCCESS;
        default:
            return M2MB_OS_SUCCESS;
    }
}

static void updateCycle(void){
    azx_sleep_ms(3000);
    isStart = TRUE;
    while (isStart == TRUE){
        azx_sleep_ms(UPDATE_TIME);

        // >>>>>>>>>>>>>>>>>>>>>>> readDataByLocalIdentifier_21
        update2101();
        update2102();
        update2103();
        update2180();
        update2181();
        update2184();
    }
}

extern void finishUpdate(void){
    isStart = FALSE;
}

