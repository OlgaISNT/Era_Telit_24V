//
// Created by klokov on 30.11.2022.
//

#include "../hdr/airbags.h"
#include "log.h"
#include "azx_utils.h"
#include "failures.h"
#include "tasks.h"
#include "sys_param.h"
#include "factory_param.h"

#define AIRBAGS_CHECK_TIMER_INTERVAL 5000 // Проверка наличия сигнала от подушек безопасности каждые 10сек
#define AIRBAGS_DETECT_INTERVAL 2000       // Период определения сигнала от подушек

static void checkAirbagsSignal(void);    // Проверка сигнала от подушек безопасности
static BOOLEAN hasAirbags = FALSE;       // Наличие сигнала от подушек безопасности за последние 5 сек
static CHERRY_CRASH cherryCrash;
static BOOLEAN currentCrashState = FALSE; // Текущее состояние подушек безопасности
static BOOLEAN prevCrashState = FALSE;    // Последнее состояние подушек безопасности
static BOOLEAN checkAirbags = FALSE;

static void airbags_id_switcher(void);
static BOOLEAN check_airbags(CAN_FILTERING_MODE filter);

static void airbags_id_switcher(void) {
    LOG_DEBUG("There is no airbags signal.");
    hasAirbags = FALSE;
    char *ID;
    LOG_DEBUG("Searching airbags messages....");
    while (!hasAirbags) {
        set_airbags_failure(F_ACTIVE); // Ошибка подушек
        // Проверяем UDS_ONLY_TYPE_0, // 0x31C
        if (check_airbags(UDS_ONLY_TYPE_0)) {
            ID = "0x31C";
            goto end;
        }

        // Проверяем UDS_ONLY_TYPE_1, // 0x320
        if (check_airbags(UDS_ONLY_TYPE_1)) {
            ID = "0x320";
            goto end;
        }

        // Проверяем UDS_ONLY_TYPE_2, // 0x021
        if (check_airbags(UDS_ONLY_TYPE_2)) {
            ID = "0x021";
            goto end;
        }

        // Проверяем UDS_ONLY_TYPE_3, // 0xAB
        if (check_airbags(UDS_ONLY_TYPE_3)) {
            ID = "0xAB";
            goto end;
        }
    }

    end:
    hasAirbags = TRUE;
    LOG_DEBUG("Airbags with ID %s was detected", ID);
    set_airbags_failure(F_INACTIVE); // Нет ошибки подушек
}

static BOOLEAN check_airbags(CAN_FILTERING_MODE filter){
    hasAirbags = FALSE;
    set_can_filter(filter);
    azx_sleep_ms(AIRBAGS_DETECT_INTERVAL);
    return hasAirbags == TRUE ? TRUE : FALSE;
}

extern INT32 AIRBAGS_task(INT32 type, INT32 param1, INT32 param2){
    (void) param1;
    (void) param2;
    switch (type) {
        case CHECK_AIRBAGS:
            if(is_airbags_disable() == TRUE) break; // Если использование подушек для данного авто не предусмотренно
            checkAirbags = TRUE;
            checkAirbagsSignal();
            break;
        default:
            LOG_DEBUG("AIRBAGS: undefined task");
            break;
    }
    return M2MB_OS_SUCCESS;
}


static void checkAirbagsSignal(void){
    set_airbags_failure(F_ACTIVE); // Ошибка подушек
    while(checkAirbags){
        while (!is_init()) {azx_sleep_ms(1000);}
        azx_sleep_ms(AIRBAGS_CHECK_TIMER_INTERVAL);
        if (!hasAirbags){
            airbags_id_switcher();  // Если сигнала от подушек не поступило
        } else {
            set_airbags_failure(F_INACTIVE);
        }
        hasAirbags = FALSE;
    }
    hasAirbags = FALSE; // При остановке проверки появляется ошибка подушек безопасности
}

extern void stopAirbagsDetect(void){
    checkAirbags = FALSE;
}

extern void airbagsCrashDetect(char D3){
    prevCrashState = currentCrashState;
    cherryCrash.NO_CRASH = (D3 & 0x00) == 0x00 ? TRUE : FALSE;
    cherryCrash.BELT_CRASH = (D3 & 0x01) == 0x01 ? TRUE : FALSE;
    cherryCrash.FRONT_CRASH = (D3 & 0x02) == 0x02 ? TRUE : FALSE;
    cherryCrash.LEFT_SIDE_CRASH = (D3 & 0x04) == 0x04 ? TRUE : FALSE;
    cherryCrash.RIGHT_SIDE_CRASH = (D3 & 0x08) == 0x08 ? TRUE : FALSE;
    cherryCrash.REAR_CRASH = (D3 & 0x10) == 0x10 ? TRUE : FALSE;

    hasAirbags = TRUE;
    if ((UINT8) D3 > 0) {
//        if (cherryCrash.NO_CRASH){LOG_DEBUG("CAN: CHERRY NO_CRASH");}
//        if (cherryCrash.BELT_CRASH) {LOG_DEBUG("CAN: CHERRY BELT_CRASH"); }
//        if (cherryCrash.FRONT_CRASH) {LOG_DEBUG("CAN: CHERRY FRONT_CRASH"); }
//        if (cherryCrash.LEFT_SIDE_CRASH) {LOG_DEBUG("CAN: CHERRY LEFT_SIDE_CRASH");}
//        if (cherryCrash.RIGHT_SIDE_CRASH) {LOG_DEBUG("CAN: CHERRY RIGHT_SIDE_CRASH");}
//        if (cherryCrash.REAR_CRASH) {LOG_DEBUG("CAN: CHERRY REAR_CRASH");}

        currentCrashState = TRUE;
        if ((prevCrashState == FALSE) & (currentCrashState == TRUE)){ // Если подушки безопасности сработали
            if(get_no_auto_trig() == 1){
                send_to_ecall_task(ECALL_AIRBAGS, ECALL_NO_REPEAT_REASON, 0);
                disableClearLog();
            }
        }
    } else {
        currentCrashState = FALSE;
    }
}
