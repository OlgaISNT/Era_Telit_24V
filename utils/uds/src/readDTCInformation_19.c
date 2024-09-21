//
// Created by klokov on 21.10.2022.
//

#include <memory.h>
#include "../hdr/readDTCInformation_19.h"
#include "../hdr/uds.h"
#include "azx_log.h"
#include "log.h"
#include "gpio_ev.h"
#include "charger.h"
#include "sys_param.h"
#include "params.h"
#include "failures.h"
#include "../../hal/hdr/factory_param.h"
#include "../../accelerometer/hdr/accel_driver.h"
#include "../../accelerometer/hdr/KalmanFilter.h"
#include "sim.h"

#define DO_NOT_ADD (0x00)

static INT32 KL30_MAX_VOLTAGE = 25000;
static INT32 KL30_MAX_VOLTAGE_24 = 30000 + 1000;  // факт 28.4
static INT32 KL30_MIN_VOLTAGE_24 = 21600 + 1000;   // факт 20.3
static INT32 KL30_MIN_VOLTAGE = 6000;
static INT32 KL30_CIRC_OPEN = 2000;
static INT32 LOW_BATTERY_VOLTAGE = 3400;

static DTC_STATUS_MASK dtcStatusMask; // содержимое dtcStatusMask
static UINT16 confirmationTime = 0;
static UINT16 healingTime= 0;

static UINT16 pos_resp_19(char mask,char *buf, UINT16 buf_size);
static void parseMask(char mask);
static void readBoardVoltageError(char *buf);                // 9011
static void readGSMandUMTSmoduleError(char *buf);            // 9021
static void readGPSandModuleError(char *buf);                // 9031
static void readInternalBatteryError(char *buf);             // 9012
static void readVoltageSupplierError(char *buf);             // 9013
static void readLostCommunicationWithBCM(char *buf);         // C140
static void readInvalidDataReceivedFromBCM(char *buf);       // C422
static void readLostCommunicationWithABS(char *buf);         // C121
static void readInvalidDataReceivedFromABS(char *buf);       // C415
static void readLostCommunicationWithCluster(char *buf);     // C155
static void readInvalidDataReceivedFromCluster(char *buf);   // C423
static void readLostCommunicationWithAirbag(char *buf);      // D120
static void readInvalidDataReceivedFromAirbag(char *buf);    // D410
static void readLostCommunicationWithECM(char *buf);         // C100
static void readInvalidDataReceivedFromECM(char *buf);       // C401
static void readCANoff(char *buf);                           // C073
static void readMicrophoneError(char *buf);                  // 9041
static void readLoadSpeakerError(char *buf);                 // 9042
static void readVehicleIdentificationNumberError(char *buf); // 9062
static void readErrorActivation(char *buf);                  // 9063
static void readSOSButtonError(char *buf);                   // 9051
static void readAccelerometerError(char *buf);               // 9053
static void readSIMChipError(char *buf);                     // 9071
static void readFlashError(char *buf);                       // 9016

extern UINT16 handler_service19(char *data, char *buf, UINT16 buf_size){
    (void) buf_size;
    // Проверка фрейма
    if (data == NULL || data[1] != READ_DTC_INFORMATION){
        return negative_resp(buf, READ_DTC_INFORMATION, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Проверка длины фрейма (фрейм с запросом всегда имеет длину 0х03)
    if (data[0] != 0x03){
        return negative_resp(buf, READ_DTC_INFORMATION, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Проверка пустых байтов фрейма
//    for (UINT8 i = 4; i < 8; i++){
//        if (data[i] != 0xAA){
//            return negative_resp(buf, READ_DTC_INFORMATION, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
//        }
//    }

    // Проверка субфункции
    if(data[2] != REPORT_DTC_BY_STATUS_MASK){
        return negative_resp(buf, READ_DTC_INFORMATION, SUBFUNCTION_NOT_SUPPORTED);
    }

    // Собираем ответ
    return pos_resp_19(data[3], buf, buf_size);
}

static UINT16 pos_resp_19(char mask,char *buf, UINT16 buf_size){
    (void) buf_size;
    // >> Маска
    // DTCStatusMask содержит восемь (8) бит состояния DTC.
    // Этот байт используется в сообщении запроса, чтобы позволить клиенту запрашивать информацию о DTC
    // для тех DTC, статус которых соответствует DTCStatusMask.

    // Состояние DTC соответствует DTCStatusMask, если любой из битов фактического состояния DTC
    // установлен в 1 и соответствующий бит состояния в
    // DTCStatusMask также установлен в 1 (т. результат отличен от нуля, то совпадение произошло).

    // Если клиент указывает маску состояния, содержащую биты,
    // которые сервер не поддерживает, то сервер должен обрабатывать информацию DTC,
    // используя только те биты, которые он поддерживает.
    parseMask(mask);
    buf[0] = READ_DTC_INFORMATION | 0x40;
    buf[1] = REPORT_DTC_BY_STATUS_MASK & 0x7F; // This parameter is an echo of bits 6 - 0 of the sub-function parameter provided in the request message from the client.
    buf[2] = mask;

    char temp[4];
    UINT8 index = 3;
    UINT8 size = 3;

    for (UINT8 i = 0; i < 24; i++){
        switch (i) {
            case 0:  // 9011 +
                readBoardVoltageError(temp);
                break;
            case 1: // 9021 +
                readGSMandUMTSmoduleError(temp);
                break;
            case 2: // 9031 +
                readGPSandModuleError(temp);
                break;
            case 3: // 9012 +
                readInternalBatteryError(temp);
                break;
            case 4: // 9013 +
                readVoltageSupplierError(temp);
                break;
            case 5:// C140 - (описание не найдено)
                readLostCommunicationWithBCM(temp); // OFF
                break;
            case 6: // C422 - (описание не найдено)
                readInvalidDataReceivedFromBCM(temp); // OFF
                break;
            case 7: // C121 +
                readLostCommunicationWithABS(temp);
                break;
            case 8: // C415 +
                readInvalidDataReceivedFromABS(temp);
                break;
            case 9: // C155 - (описание не найдено)
                readLostCommunicationWithCluster(temp); // OFF
                break;
            case 10: // C423 - (описание не найдено)
                readInvalidDataReceivedFromCluster(temp); // OFF
                break;
            case 11: // D120 +
                readLostCommunicationWithAirbag(temp);
                break;
            case 12: // D410 +
                readInvalidDataReceivedFromAirbag(temp);
                break;
            case 13: // C100 +
                readLostCommunicationWithECM(temp);
                break;
            case 14: // C401 +
                readInvalidDataReceivedFromECM(temp);
                break;
            case 15: // C073 +
                readCANoff(temp);
                break;
            case 16:  // 9041 +
                readMicrophoneError(temp);
                break;
            case 17: // 9042 +
                readLoadSpeakerError(temp);
                break;
            case 18: // 9062 +
                readVehicleIdentificationNumberError(temp);
                break;
            case 19: // 9063 +
                readErrorActivation(temp);
                break;
            case 20:  // 9051 +
                readSOSButtonError(temp);
                break;
            case 21: // 9053 +
                readAccelerometerError(temp);
                break;
            case 22: // 9071 +
                readSIMChipError(temp);
                break;
            case 23: // 9016 +
                readFlashError(temp);
                break;
            default:
                break;
        }
        if (temp[2] != DO_NOT_ADD){
            for (UINT8 j = index; j <= index + 4;j ++){
                buf[j] = temp[j - index];
                temp[j - index] = DO_NOT_ADD;
            }
            index += 4;
            size = index + 1;
        }
    }
    size--;
    return size;
}

static void parseMask(char mask){
    dtcStatusMask.WARNING_INDICATOR_REQUESTED = mask & 0x80;
    dtcStatusMask.TEST_NOT_COMPLETED_THIS_MONITORING_CYCLE = mask & 0x40;
    dtcStatusMask.TEST_FAILED_SINCE_LAST_CLEAR = mask & 0x20;
    dtcStatusMask.TEST_NOT_COMPLETED_SINCE_LAST_CLEAR = mask & 0x10;
    dtcStatusMask.CONFIRMED_DTC = mask & 0x08;
    dtcStatusMask.PENDING_DTC = mask & 0x04;
    dtcStatusMask.TEST_FAILED_THIS_MONITORING_CYCLE = mask & 0x02;
    dtcStatusMask.TEST_FAILED = mask & 0x01;
}

// 9011

static void readBoardVoltageError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x11;
    confirmationTime = 14000;
    healingTime = 5500;

    INT32 volt = getKL30voltage();
    if(get_current_rev() == 2)
		{
			if (volt > KL30_MAX_VOLTAGE_24)
				  buf[2] = 0x17; // Circuit voltage above threshold (high voltage);
			 else if ((volt > KL30_CIRC_OPEN) & (volt < KL30_MIN_VOLTAGE_24))
				    buf[2] = 0x16; // Circuit voltage below threshold (low voltage);
			 else if (volt < KL30_CIRC_OPEN){
			         buf[2] = 0x13; // Circuit open.
			 }
		}
    else{
    if (volt > KL30_MAX_VOLTAGE ){
        buf[2] = 0x17; // Circuit voltage above threshold (high voltage);
    } else if ((volt > KL30_CIRC_OPEN) & (volt < KL30_MIN_VOLTAGE)){
        buf[2] = 0x16; // Circuit voltage below threshold (low voltage);
    } else if (volt < KL30_CIRC_OPEN){
        buf[2] = 0x13; // Circuit open.
    } else{
        buf[2] = DO_NOT_ADD; // не добавляем
    }
    }
    buf[3] = 0x01;
}

// 9021
// 0x46 Audiofiles error (missing or invalide).
// 0x47 Audiocodec error.
// 0x48 Invalid software version of GSM module.
// 0x81 Invalid serial data received (wrong frame format or data);
// 0x87 Missing message (no signals from module).
static void readGSMandUMTSmoduleError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x21;
    confirmationTime = 3000;
    healingTime = 5500;
    if (get_audio_files_failure() == F_ACTIVE){
        buf[2] = 0x46; // Audiofiles error (missing or invalide).
    } else if (get_codec_failure() == F_ACTIVE){
        buf[2] = 0x47; // Audiocodec error.
    } else {
        buf[2] = DO_NOT_ADD; // не добавляем
    }
    buf[3] = 0x01;
}

// 9031
// 0x81 Invalid serial data received (wrong frame format or data)
// 0x87 Missing message (no signals from module)
static void readGPSandModuleError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x31;
    buf[2] = DO_NOT_ADD;
//    if (low_prior_failures()){
//        buf[2] = 0x87; // Missing message (no signals from module).
//    } else {
//        buf[2] = DO_NOT_ADD; // не добавляем
//    }
    buf[3] = 0x01;
}

// 9012
// Ошибки которые не задействованы:
// buf[2] = 0x92;  Performance or incorrect operation (battery is old);
// buf[2] = 0x97;  Component obstructed or blocked (battery unchargeable)
// buf[2] = 0x98; // Component over temperature (battery temperature too high);
static void readInternalBatteryError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x12;
    confirmationTime = 5500;
    healingTime = 5500;
    if (getBatteryVolatage() < LOW_BATTERY_VOLTAGE){
        buf[2] = 0x91; // Parametric (battery charge too low);
    } else{
        buf[2] = DO_NOT_ADD; // не добавляем
    }
    buf[3] = 0x01;
}

// 9013
static void readVoltageSupplierError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x13;
//    UINT32 val = getBatteryVolatage();
    confirmationTime = 14000;
    healingTime = 3000;
//    if (val < 200){
//        buf[2] = 0x01; // General Electrical Failure (supplier is defective).
//    } else {
        buf[2] = DO_NOT_ADD; // не добавляем
//    }
    buf[3] = 0x01;
}

// C140
static void readLostCommunicationWithBCM(char *buf){
    buf[0] = 0xC1;
    buf[1] = 0x40;
    UINT8 val = 1; // (!!!!!!ЗАГЛУШКА!!!!!!)
    confirmationTime = 3000;
    healingTime = 10000;
    switch (val) {
        case 0:
            buf[2] = 0x08; // Bus Signal/Message Failures.
            break;
        case 1:
        default:
            buf[2] = DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// C422
static void readInvalidDataReceivedFromBCM(char *buf){
    buf[0] = 0xC4;
    buf[1] = 0x22;
    UINT8 val = 1; // (!!!!!!ЗАГЛУШКА!!!!!!)
    confirmationTime = 3000;
    healingTime = 10000;
    switch (val) {
        case 0:
            buf[2] = 0x08; // Bus Signal/Message Failures.
            break;
        case 1:
        default:
            buf[2] = DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// C121
static void readLostCommunicationWithABS(char *buf){
    buf[0] = 0xC1;
    buf[1] = 0x21;
    UINT8 val = 1; // (!!!!!!ЗАГЛУШКА!!!!!!)
    confirmationTime = 3000;
    healingTime = 10000;
    switch (val) {
        case 0:
            buf[2] = 0x08; // Bus Signal/Message Failures.
            break;
        case 1:
        default:
            buf[2] = DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// C415
static void readInvalidDataReceivedFromABS(char *buf){
    buf[0] = 0xC4;
    buf[1] = 0x15;
    UINT8 val = 1; // (!!!!!!ЗАГЛУШКА!!!!!!)
    confirmationTime = 3000;
    healingTime = 10000;
    switch (val) {
        case 0:
            buf[2] = 0x08; // Bus Signal/Message Failures.
            break;
        case 1:
        default:
            buf[2] = DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// C155
static void readLostCommunicationWithCluster(char *buf){
    buf[0] = 0xC1;
    buf[1] = 0x55;
    UINT8 val = 1; // (!!!!!!ЗАГЛУШКА!!!!!!)
    confirmationTime = 3000;
    healingTime = 10000;
    switch (val) {
        case 0:
            buf[2] = 0x08; // Bus Signal/Message Failures.
            break;
        case 1:
        default:
            buf[2] = DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// C423
static void readInvalidDataReceivedFromCluster(char *buf){
    buf[0] = 0xC4;
    buf[1] = 0x23;
    UINT8 val = 1; // (!!!!!!ЗАГЛУШКА!!!!!!)
    confirmationTime = 3000;
    healingTime = 10000;
    switch (val) {
        case 0:
            buf[2] = 0x08; // Bus Signal/Message Failures.
            break;
        case 1:
        default:
            buf[2] = DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// D120
static void readLostCommunicationWithAirbag(char *buf){
    buf[0] = 0xD1;
    buf[1] = 0x20;
    confirmationTime = 3000;
    healingTime = 10000;
    if (get_airbags_failure() == F_ACTIVE){
        buf[2] = 0x08; // Bus Signal/Message Failures.
    } else {
        buf[2] = DO_NOT_ADD; // не добавляем
    }
    buf[3] = 0x01;
}

// D410
static void readInvalidDataReceivedFromAirbag(char *buf){
    buf[0] = 0xD4;
    buf[1] = 0x10;
    confirmationTime = 3000;
    healingTime = 10000;
    // (заглушка)
    buf[2] =  DO_NOT_ADD; // не добавляем
    buf[3] = 0x01;
}

// C100
static void readLostCommunicationWithECM(char *buf){
    buf[0] = 0xC1;
    buf[1] = 0x00;
    UINT8 val = 1; // (!!!!!!ЗАГЛУШКА!!!!!!)
    confirmationTime = 3000;
    healingTime = 10000;
    switch (val) {
        case 0:
            buf[2] =  0x08; // Bus Signal/Message Failures.
            break;
        case 1:
        default:
            buf[2] =  DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// C401
static void readInvalidDataReceivedFromECM(char *buf){
    buf[0] = 0xC4;
    buf[1] = 0x01;
    UINT8 val = 1; // (!!!!!!ЗАГЛУШКА!!!!!!)
    confirmationTime = 3000;
    healingTime = 10000;
    switch (val) {
        case 0:
            buf[2] =  0x08; // Bus Signal/Message Failures.
            break;
        case 1:
        default:
            buf[2] =  DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// C073
static void readCANoff(char *buf){
    buf[0] = 0xC0;
    buf[1] = 0x73;
    UINT8 val = 1; // (!!!!!!ЗАГЛУШКА!!!!!!)
    confirmationTime = 3000;
    healingTime = 10000;
    switch (val) {
        case 0:
            buf[2] =  0x08; // Bus Signal/Message Failures.
            break;
        case 1:
        default:
            buf[2] =  DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// 9041
// Ошибки которые не задействованы:
// buf[2] =  0x09; // Component Failures.
static void readMicrophoneError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x41;
    confirmationTime = 1000;
    healingTime = 1000;
    switch (get_microphone_failure()) {
        case F_ACTIVE:
            buf[2] = 0x13; // Break
            break;
        case F_INACTIVE:
        default:
            buf[2] = DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// 9042
// Ошибки которые не задействованы:
// buf[2] =  0x09; // Component Failures.
static void readLoadSpeakerError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x42;
    confirmationTime = 1000;
    healingTime = 1000;
    switch (get_speaker_failure()) {
        case F_ACTIVE:
            buf[2] = 0x13; // Break
            break;
        case F_INACTIVE:
        default:
            buf[2] = DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}

// 9062
static void readVehicleIdentificationNumberError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x62;
    confirmationTime = 1000;
    healingTime = 1000;
    char vin[17 + 1];
    get_vin(vin, sizeof(vin));
    if (strcmp(vin, "00000000000000000\0") == 0){
        buf[2] =  0x02; // General Signal Failures.
    } else {
        buf[2] =  DO_NOT_ADD; // не добавляем
    }
    buf[3] = 0x01;
}

// 9063
static void readErrorActivation(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x63;
    confirmationTime = 1000;
    healingTime = 2000;
    buf[2] =  DO_NOT_ADD; // не добавляем
    buf[3] = 0x01;
}

// 9051
// Ошибки которые не задействованы:
// buf[2] =  0x23; // Signal stuck low (SOS button is stuck)
static void readSOSButtonError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x51;
    confirmationTime = 10000;
    healingTime = 1000;
    if (get_sos_break_failure() == F_ACTIVE){
        buf[2] =  0x13; // Break
    } else {
        buf[2] =  DO_NOT_ADD; // не добавляем
    }
    buf[3] = 0x01;
}

// 9053
// 0x02; // no signal from module
// 0x09; // initialisation error
// 0x15; // calibration error
static void readAccelerometerError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x53;
    confirmationTime = 10000;
    healingTime = 10000;
    if (get_crash_sensor_failure() == F_ACTIVE){
        buf[2] =  0x09; // initialisation error
    } else {
        buf[2] =  DO_NOT_ADD; // не добавляем
    }
    buf[3] = 0x01;
}

// 9071
static void readSIMChipError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x71;
    confirmationTime = 1000;
    healingTime = 1000;
    if(hasIccid() == FALSE){
        buf[2] =  DO_NOT_ADD; // No response from SIM chip
    } else {
        buf[2] =  DO_NOT_ADD; // не добавляем
    }
    buf[3] = 0x01;
}

// 9016
static void readFlashError(char *buf){
    buf[0] = 0x90;
    buf[1] = 0x16;
    UINT8 val = 1; // (!!!!!!ЗАГЛУШКА!!!!!!)
    confirmationTime = 1000;
    healingTime = 1000;
    switch (val) {
        case 0:
            buf[2] =  0x01; // Incorrect CRC for flash application
            break;
        case 1:
        default:
            buf[2] =  DO_NOT_ADD; // не добавляем
            break;
    }
    buf[3] = 0x01;
}
