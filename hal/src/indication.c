/**
 * @file indication.c
 * @version 1.0.0
 * @date 26.04.2022
 * @author: AKlokov
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "indication.h"
#include "failures.h"
#include "azx_utils.h"
#include "log.h"
#include "gpio_ev.h"
#include "tasks.h"
#include "factory_param.h"
#include "codec.h"

/* Local statics ================================================================================*/
static INT32 currentTask; // Текущая задача
static UINT32 stage;      // стадия для команд с разными таймингами
/* Static functions =============================================================================*/
static void revInd(void);
static void yellowBlink(void); // Мигать желтым (Cherry)
static BOOLEAN set_light(COLOUR colour);

static INT32 indCherryCan(INT32 type,INT32 param1,INT32 param2);     // Индикация для Cherry с CAN
static INT32 indCherryCanRev2(INT32 type,INT32 param1,INT32 param2); // Индикация для Cherry с CAN
/* Global functions =============================================================================*/

/***************************************************************************************************
   INDICATION_task Нельзя вызывать эту функцию напрямую!
**************************************************************************************************/
INT32 INDICATION_task(INT32 type, INT32 param1, INT32 param2) {
    (void) param1;
    (void) param2;

    // Если тип авто не указан
    if (get_model_id() == 0){
        LOG_WARN("INDICATION: Vehicle is undefined");
    }

    /**
     * >>>>>>>>>> Общие условия для индикации всех БЭГов
     */

    // Если поступила новая задача
    if (param1 == 0) {
        // Если требуемая задача отменить мигание индикаторов,
        // но при этом выключено зажигание, то меняем задачу на отключение зажигания
        if (((type == CANCEL_FLASHING) | (type == OFF_SOS)) & !isIgnition()){
            type = SHOW_IGN_OFF; // Замена CANCEL_FLASHING на SHOW_IGN_OFF
        }


        // Если требуемая задача показать наличие ошибок,
        // но при этом бэг не активирован, то меняем задачу
        // т.к. демонстрация отсутствия активации имеет более высокий приоритет
        if (((type == SHOW_FAILURE) | (type == SHOW_NO_FAILURE)) & needIndWhenIsNotActivated()){
            type = NO_ACTIVATED; // замена SHOW_FAILURE на NO_ACTIVATED
        }

        // Если новая задача требует показать отсутствие/наличие ошибок или отсутствие активации,
        // но индикатор состояния уже моргает, то эту задачу, к выполнению не допускаем
        if ((type == SHOW_NO_FAILURE || type == SHOW_FAILURE || type == NO_ACTIVATED) &&
            (currentTask == SHOW_ECALL || currentTask == SHOW_SENDING_MSD
             || currentTask == SHOW_VOICE_CHANNEL || currentTask == SHOW_ECALL_DIALING
             || currentTask == SHOW_ECALL_IMPOSSIBLE || currentTask == SHOW_TEST_STARTED)) {
            return M2MB_OS_SUCCESS;  // То задача пропускается
        }

        // Если требуемая задача показать наличие/отсутствие ошибок или отсутствие активации,
        // а текущая задача включение или выключение зажигания, то эту задачу, к выполнению не допускаем
        if ((type == SHOW_FAILURE || type == SHOW_NO_FAILURE || type == NO_ACTIVATED)
            && (currentTask == SHOW_IGN_ON || currentTask == SHOW_IGN_OFF)) {
            return M2MB_OS_SUCCESS; // То задача пропускается
        }

        // Только для Cherry
        // Если в данный момент проходит тестирование,
        // а требуемая задача не является показать включение или выключение зажигания
        if (get_model_id() == 10){ // Cherry
            if ((currentTask == SHOW_TEST_STARTED) & (type != SHOW_IGN_OFF || type != SHOW_IGN_ON)){
                return M2MB_OS_SUCCESS; // То задача пропускается
            }
        }

        // Иначе задача становится требуемой к выполнению и проходит дальше
        currentTask = type;
    }

    // Если задача была запущена ранее
    if(param1 == 1){
        if (currentTask != type){    // Если ранее запущенная задача более не является требуемой к выполнению
            return M2MB_OS_SUCCESS;  // Останавливаем задачу
        }
    }

    // Индикация
    if(is_two_wire_bip() && with_can()){
        return indCherryCanRev2(type, param1, param2);
    } else if(with_can()) {
        return indCherryCan(type, param1, param2);
    } else {
        LOG_ERROR("INDICATION: Unknown vehicle");
        return M2MB_OS_SUCCESS;
    }
}

extern IND_TASK_COMMAND getCurrenTask(void){
    return currentTask;
}

static void revInd(void) {
    if (with_can()) {
        if (getRedMode()) {
            set_light(GREEN);
        } else {
            set_light(RED);
        }
    }
}

static void yellowBlink(void){
    if (with_can()){
        if (getRedMode()){
            set_light(NO_LIGHT);
        } else{
            set_light(YELLOW);
        }
    }
}

static INT32 indCherryCanRev2(INT32 type,INT32 param1,INT32 param2){
    (void) param1;
    (void) param2;

    if(isTestMode()){
        currentTask = NO_TASK;
        return M2MB_OS_SUCCESS;
    }

    if(with_can()){
        onLedPwrCtrl(TRUE);
    }

    if(is_rec() == TRUE) {
        set_light(GREEN);
        azx_sleep_ms(200);
        send_to_ind_task(currentTask, 1, 0);
        return M2MB_OS_SUCCESS;
    }

    if(param1 == 0){
        stage = 0;
        set_light(GREEN);
    }

    switch (type) {
        case ACTIVATED:
            send_to_ind_task(SHOW_FAILURE, 0, 0);
            currentTask = NO_TASK;
            return M2MB_OS_SUCCESS;
        case NO_ACTIVATED:
            if (needIndWhenIsNotActivated()){
                greenModeCtrl(FALSE);
                switch (stage) {
                    case 0:
                        stage = 1;
                        set_light(RED);
                        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(500));
                        break;
                    case 1:
                        stage = 0;
                        set_light(GREEN);
                        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(500));
                        break;
                }
                send_to_ind_task(currentTask, 1, 0);
            }
            return M2MB_OS_SUCCESS;
        case SHOW_IGN_OFF:
            if (stage < 5) {
                set_light(GREEN);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(50));
                send_to_ind_task(currentTask, 1, 0);
            } else {
                set_light(RED);
                return M2MB_OS_SUCCESS;
            }
            stage++;
            return M2MB_OS_SUCCESS;
        case SHOW_IGN_ON:
            if(stage == 0){
                set_light(GREEN);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(50));
            } else if(stage <= 50){
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(50));
                set_light(RED);
            } else if(stage > 50 && stage <= 80){
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(50));
                set_light(GREEN);
            }
            else{
                currentTask = NO_TASK;
                send_to_ind_task(SHOW_FAILURE,0,0);
                return M2MB_OS_SUCCESS;
            }
            stage++;
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case OFF_SOS:
            set_light(GREEN);
            return M2MB_OS_SUCCESS;
        case CANCEL_FLASHING:
        case SHOW_FAILURE:
        case SHOW_NO_FAILURE:
            if (needIndWhenIsNotActivated()){
                send_to_ind_task(NO_ACTIVATED,0,0);
                return M2MB_OS_SUCCESS;
            }
            azx_sleep_ms(500);
            if (high_prior_failures()){
                set_light(RED);
                send_to_ind_task(currentTask,1,0);
                return M2MB_OS_SUCCESS;
            }

            else if(low_prior_failures() || medium_prior_failures()){
                set_light(GREEN);
                send_to_ind_task(currentTask,1,0);
                return M2MB_OS_SUCCESS;
            }

            else if(!failures_exist()){
                set_light(GREEN);
                return M2MB_OS_SUCCESS;
            }
        case SHOW_ECALL:
            if(stage == 0){
                stage = 1;
                set_light(RED);
            } else {
                stage = 0;
                set_light(GREEN);
            }
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(950));
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_RING_SIGNAL:
//            Отобразить входящий сигнал RING ("красный индикатор" согласно требованию А.Архипова от 21.02.24)
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
            set_light(RED);
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case CANCEL_SHOW_RING_SIGNAL:
//            Прекратить отображать входящий сигнал RING и вернуться к отображению предыдущего состояния ("зелёный индикатор" согласно требованию А.Архипова от 21.02.24)
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
            set_light(GREEN);
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_SENDING_MSD:
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
            set_light(RED);
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_VOICE_CHANNEL:
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
            set_light(GREEN);
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_ECALL_DIALING:
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
            set_light(GREEN);
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_ECALL_IMPOSSIBLE:
            if (stage == 0){
                stage = 1;
                set_light(RED);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(1950));
            } else{
                stage = 0;
                set_light(GREEN);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(250));
            }
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_TEST_STARTED:
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(450));
            if(is_testing_working()){
                switch (stage) {
                    case 0:
                        set_light(RED);
                        stage++;
                        break;
                    default:
                        set_light(GREEN);
                        stage = 0;
                        break;
                }
                send_to_ind_task(currentTask,1,0);
            }else{
                currentTask = NO_TASK;
                send_to_ind_task(CANCEL_FLASHING,0,0);
            }
            return M2MB_OS_SUCCESS;
        case ENABLE_SPEC_FUNC:
            for (unsigned int i = 0; i < 10; i++){
                set_light(RED);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(450));
                set_light(GREEN);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(450));
            }
            currentTask = NO_TASK;
            send_to_ind_task(CANCEL_FLASHING,0,0);
            return M2MB_OS_SUCCESS;
        case DISABLE_SPEC_FUNC:
            for (unsigned int i = 0; i < 6; i++){
                set_light(GREEN);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(1000));
                set_light(RED);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(1000));
            }
            currentTask = NO_TASK;
            send_to_ind_task(CANCEL_FLASHING,0,0);
            return M2MB_OS_SUCCESS;
        default:
            LOG_ERROR("INDICATION: unknown command %d", type);
            currentTask = NO_TASK;
            send_to_ind_task(CANCEL_FLASHING,0,0);
            return M2MB_OS_SUCCESS;
    }
}


static INT32 indCherryCan(INT32 type,INT32 param1,INT32 param2){
    (void) param2;

    if(isTestMode()){
        currentTask = NO_TASK;
        return M2MB_OS_SUCCESS;
    }

    if(with_can()){
        onLedPwrCtrl(TRUE);
    }

    if(param1 == 0){
        stage = 0;
        set_light(GREEN);
    }

    switch (type) {
        case ACTIVATED:
            send_to_ind_task(SHOW_FAILURE, 0, 0);
            currentTask = NO_TASK;
            return M2MB_OS_SUCCESS;
        case NO_ACTIVATED:
            if (needIndWhenIsNotActivated()){
                greenModeCtrl( FALSE);
                switch (stage) {
                    case 0:
                        stage = 1;
                        set_light(RED);
                        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(500));
                        break;
                    case 1:
                        stage = 0;
                        set_light(NO_LIGHT);
                        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(500));
                        break;
                }
                send_to_ind_task(currentTask, 1, 0);
            }
            return M2MB_OS_SUCCESS;
        case SHOW_IGN_OFF:
            if (stage < 5) {
                set_light(GREEN);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
                send_to_ind_task(currentTask, 1, 0);
            } else {
                set_light(RED);
                return M2MB_OS_SUCCESS;
            }
            stage++;
            return M2MB_OS_SUCCESS;
        case SHOW_IGN_ON:
            if(stage == 0){
                set_light(GREEN);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
            } else if(stage <= 50){
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
                set_light(RED);
            } else if(stage > 50 && stage <= 80){
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
                set_light(GREEN);
            }
            else{
                currentTask = NO_TASK;
                send_to_ind_task(SHOW_FAILURE,0,0);
                return M2MB_OS_SUCCESS;
            }
            stage++;
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case OFF_SOS:
            set_light(GREEN);
            return M2MB_OS_SUCCESS;
        case CANCEL_FLASHING:
        case SHOW_FAILURE:
        case SHOW_NO_FAILURE:
            if (needIndWhenIsNotActivated()){
                send_to_ind_task(NO_ACTIVATED,0,0);
                return M2MB_OS_SUCCESS;
            }
            azx_sleep_ms(500);
            if (high_prior_failures()){
                set_light(RED);
                send_to_ind_task(currentTask,1,0);
                return M2MB_OS_SUCCESS;
            }

            else if (medium_prior_failures()){
                yellowBlink();
                send_to_ind_task(currentTask,1,0);
                return M2MB_OS_SUCCESS;
            }

            else if(low_prior_failures()){
                set_light(YELLOW);
                send_to_ind_task(currentTask,1,0);
                return M2MB_OS_SUCCESS;
            }

            else if(!failures_exist()){
                set_light(GREEN);
                return M2MB_OS_SUCCESS;
            }
        case SHOW_ECALL:
            revInd();
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(1000));
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_SENDING_MSD:
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
            revInd();
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_VOICE_CHANNEL:
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(500));
            revInd();
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_ECALL_DIALING:
            revInd();
            if(stage == 0){
                stage = 1;
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(100));
            } else{
                stage = 0;
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(1000));
            }
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_ECALL_IMPOSSIBLE:
            revInd();
            if (stage == 0){
                stage = 1;
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(2000));
            } else{
                stage = 0;
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(300));
            }
            send_to_ind_task(currentTask,1,0);
            return M2MB_OS_SUCCESS;
        case SHOW_TEST_STARTED:
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(500));
            if(is_testing_working()){
                switch (stage) {
                    case 0:
                        set_light(RED);
                        stage++;
                        break;
                    case 1:
                        set_light(YELLOW);
                        stage++;
                        break;
                    default:
                        set_light(GREEN);
                        stage = 0;
                        break;
                }
                send_to_ind_task(currentTask,1,0);
            }else{
                currentTask = NO_TASK;
                send_to_ind_task(CANCEL_FLASHING,0,0);
            }
            return M2MB_OS_SUCCESS;
        case ENABLE_SPEC_FUNC:
            for (unsigned int i = 0; i < 10; i++){
                set_light(YELLOW);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(150));
                set_light(GREEN);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(150));
            }
            currentTask = NO_TASK;
            send_to_ind_task(CANCEL_FLASHING,0,0);
            return M2MB_OS_SUCCESS;
        case DISABLE_SPEC_FUNC:
            for (unsigned int i = 0; i < 10; i++){
                set_light(GREEN);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(150));
                set_light(YELLOW);
                m2mb_os_taskSleep(M2MB_OS_MS2TICKS(150));
            }
            currentTask = NO_TASK;
            send_to_ind_task(CANCEL_FLASHING,0,0);
            return M2MB_OS_SUCCESS;
        default:
            LOG_ERROR("INDICATION: unknown command %d", type);
            currentTask = NO_TASK;
            send_to_ind_task(CANCEL_FLASHING,0,0);
            return M2MB_OS_SUCCESS;
    }
}


static BOOLEAN set_light(COLOUR colour){
    if(with_can()){
        switch (colour) {
            case RED:
                if(get_current_rev() == 0){ // REV = 0
                    redMode(TRUE);
                    sosCtrl(TRUE);
                    greenModeCtrl(FALSE);
                } else if(is_two_wire_bip()){ // REV = 1
                    redMode(FALSE);
                    sosCtrl(FALSE);
                    greenModeCtrl(FALSE);
                }
                break;
            case YELLOW:
                if(get_current_rev() == 0){ // REV = 0
                    redMode(TRUE);
                    sosCtrl(TRUE);
                    greenModeCtrl(TRUE);
                } else if(is_two_wire_bip()){ // REV = 1
                    LOG_ERROR("YELLOW ligth doesn't supports on Chery rev 2");
                }
                break;
            case GREEN:
                if(get_current_rev() == 0){ // REV = 0
                    redMode(FALSE);
                    sosCtrl(FALSE);
                    greenModeCtrl(TRUE);
                } else if(is_two_wire_bip()){ // REV = 1
                    redMode(TRUE);
                    sosCtrl(TRUE);
                    greenModeCtrl(FALSE);
                }
                break;
            case NO_LIGHT:
                if(get_current_rev() == 0){ // REV = 0
                    redMode(FALSE);
                    sosCtrl(FALSE);
                    greenModeCtrl(FALSE);
                } else if(is_two_wire_bip()){ // REV = 1
                    LOG_ERROR("NO_LIGHT doesn't supports on Chery rev 2");
                }
                break;
        }
    } else {
        LOG_ERROR("UNKNOWN IND TYPE");
    }
    return TRUE;
}
