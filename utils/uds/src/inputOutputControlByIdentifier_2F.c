//
// Created by klokov on 24.10.2022.
//

#include <iwlib.h>
#include "log.h"
#include "../hdr/inputOutputControlByIdentifier_2F.h"
#include "../hdr/uds.h"
#include "gpio_ev.h"

#define ON     0xFF
#define OFF    0x00

#define RCTECU 0x00
#define RTD    0x01
#define STA    0x03

static UINT16 pos_resp_2f(UINT16 dataIdentifier, char *option, char *buf, UINT16 buf_size);
static char setOutputLockDoor(char data);     // 0001
static char setOutputServiseLED(char data);   // 0002
static char setOutputSOSLED(char data);       // 0003
static char setOutputMute(char data);         // 0004
static char setOutputKL15A(char data);        // 0005
static char setOutputPreheater(char data);    // 0006
static char setOutputKlakson(char data);      // 0007
static char setOutputASButton(char data);     // 0008
static char setOutputBacklight(char data);    // 0009
static char setOutputLEDGreenMode(char data); // 000A
static char setOutputLEDRedMode(char data);   // 000B
static char setOutputONCharge(char data);     // 000C
static char setOutputONOFF_Mode(char data);   // 000D

extern UINT16 handler_service2F(char *data, char *buf, UINT16 buf_size){
    // Проверка фрейма
    if (data == NULL || data[1] != INPUT_OUTPUT_CONTROL_BY_IDENTIFIER){
        return negative_resp(buf, INPUT_OUTPUT_CONTROL_BY_IDENTIFIER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    UINT8 size = (UINT8) data[0]; // Размер полезных данных в кадре

    // Размер полезных данных минимум 4 байта
    if (size <= 3){
        return negative_resp(buf, INPUT_OUTPUT_CONTROL_BY_IDENTIFIER, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Проверка InputOutputControlParameter
    if ((data[4] != RCTECU) | (data[4] != RTD) | (data[4] != STA)){
        return negative_resp(buf, INPUT_OUTPUT_CONTROL_BY_IDENTIFIER, CONDITIONS_NOT_CORRECT);
    }

    UINT16 dataIdentifier = (data[2] << 8) | data[3];  // Идентификатор данных

    // Проверка идентификатора данных
    if (dataIdentifier > 13){
        return negative_resp(buf, INPUT_OUTPUT_CONTROL_BY_IDENTIFIER, REQUEST_OUT_OF_RANGE);
    }

    char optionRecord[size - 3];                       // Данные для записи
    memcpy(optionRecord, &optionRecord[4], size - 3);
    return pos_resp_2f(dataIdentifier, optionRecord, buf, buf_size);
}

static UINT16 pos_resp_2f(UINT16 dataIdentifier, char *option, char *buf, UINT16 buf_size){
    (void) buf_size;
    UINT16 size = 0;
    buf[0] = 0x6F;
    buf[1] = dataIdentifier >> 8;
    buf[2] = dataIdentifier;
    buf[3] = option[0];
    switch (dataIdentifier) {
        case 0x0001: // Output_LockDoor (1 byte)
            buf[4] = setOutputLockDoor(option[1]);
            break;
        case 0x0002: // Output_ServiseLED (1 byte)
            buf[4] = setOutputServiseLED(option[1]);
            break;
        case 0x0003: // Output_SOS_LED (1 byte)
            buf[4] = setOutputSOSLED(option[1]);
            break;
        case 0x0004: // Output_Mute (1 byte)
            buf[4] = setOutputMute(option[1]);
            break;
        case 0x0005: // Output_KL15A (1 byte)
            buf[4] = setOutputKL15A(option[1]);
            break;
        case 0x0006: // Output_Preheater (1 byte)
            buf[4] = setOutputPreheater(option[1]);
            break;
        case 0x0007: // Output_Klakson (1 byte)
            buf[4] = setOutputKlakson(option[1]);
            break;
        case 0x0008: // Output_AS_Button (1 byte)
            buf[4] = setOutputASButton(option[1]);
            break;
        case 0x0009: // Output_Backlight (1 byte)
            buf[4] = setOutputBacklight(option[1]);
            break;
        case 0x000A: // Output_LED_GreenMode (1 byte)
            buf[4] = setOutputLEDGreenMode(option[1]);
            break;
        case 0x000B: // Output_LED_RedMode (1 byte)
            buf[4] = setOutputLEDRedMode(option[1]);
            break;
        case 0x000C: // Output_ON_Charge (1 byte)
            buf[4] = setOutputONCharge(option[1]);
            break;
        case 0x000D: // Output_ON_OFF_Mode (1 byte)
            buf[4] = setOutputONOFF_Mode(option[1]);
            break;
    }
    size = 5;
    return size;
}

static char setOutputLockDoor(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_LockDoor ON");
            return ON; // (!!!!!ЗАГЛУШКА!!!!!!!)
        case OFF:
            LOG_DEBUG("Output_LockDoor OFF");
            return OFF; // (!!!!!ЗАГЛУШКА!!!!!!!)
        default:
            LOG_DEBUG("Output_LockDoor NOT_USED");
            return data; // (!!!!!ЗАГЛУШКА!!!!!!!)
    }
}

static char setOutputServiseLED(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_ServiseLED ON");
            serviceControl(TRUE);
            return getServiceCtrl() ? ON : OFF;
        case OFF:
            LOG_DEBUG("Output_ServiseLED OFF");
            serviceControl(FALSE);
            return getServiceCtrl() ? ON : OFF;
        default:
            LOG_DEBUG("Output_ServiseLED NOT_USED");
            return data;
    }
}

static char setOutputSOSLED(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_SOS_LED ON");
            sosCtrl(TRUE);
            return getSosCtrl() ? ON : OFF;
        case OFF:
            LOG_DEBUG("Output_SOS_LED OFF");
            sosCtrl(FALSE);
            return getSosCtrl() ? ON : OFF;
        default:
            LOG_DEBUG("Output_SOS_LED NOT_USED");
            return data;
    }
}

static char setOutputMute(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_Mute ON");
            enAudio(TRUE);
            return getEnAudio() ? ON : OFF;
        case OFF:
            LOG_DEBUG("Output_Mute OFF");
            enAudio(FALSE);
            return getEnAudio() ? ON : OFF;
        default:
            LOG_DEBUG("Output_Mute NOT_USED");
            return data;
    }
}

static char setOutputKL15A(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_KL15A ON");
            return ON; // (!!!!!ЗАГЛУШКА!!!!!!!)
        case OFF:
            LOG_DEBUG("Output_KL15A OFF");
            return OFF; // (!!!!!ЗАГЛУШКА!!!!!!!)
        default:
            LOG_DEBUG("Output_KL15A NOT_USED");
            return data; // (!!!!!ЗАГЛУШКА!!!!!!!)
    }
}

static char setOutputPreheater(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_Preheater ON");
            return ON; // (!!!!!ЗАГЛУШКА!!!!!!!)
        case OFF:
            LOG_DEBUG("Output_Preheater OFF");
            return OFF; // (!!!!!ЗАГЛУШКА!!!!!!!)
        default:
            LOG_DEBUG("Output_Preheater NOT_USED");
            return data; // (!!!!!ЗАГЛУШКА!!!!!!!)
    }
}

static char setOutputKlakson(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_Klakson ON");
            return ON; // (!!!!!ЗАГЛУШКА!!!!!!!)
        case OFF:
            LOG_DEBUG("Output_Klakson OFF");
            return OFF; // (!!!!!ЗАГЛУШКА!!!!!!!)
        default:
            LOG_DEBUG("Output_Klakson NOT_USED");
            return data; // (!!!!!ЗАГЛУШКА!!!!!!!)
    }
}

static char setOutputASButton(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_AS_Button ON");
            return ON; // (!!!!!ЗАГЛУШКА!!!!!!!)
        case OFF:
            LOG_DEBUG("Output_AS_Button OFF");
            return OFF; // (!!!!!ЗАГЛУШКА!!!!!!!)
        default:
            LOG_DEBUG("Output_AS_Button NOT_USED");
            return data; // (!!!!!ЗАГЛУШКА!!!!!!!)
    }
}

static char setOutputBacklight(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_Backlight ON");
            return ON; // (!!!!!ЗАГЛУШКА!!!!!!!)
        case OFF:
            LOG_DEBUG("Output_Backlight OFF");
            return OFF; // (!!!!!ЗАГЛУШКА!!!!!!!)
        default:
            LOG_DEBUG("Output_Backlight NOT_USED");
            return data; // (!!!!!ЗАГЛУШКА!!!!!!!)
    }
}

static char setOutputLEDGreenMode(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_LED_GreenMode ON");
            greenModeCtrl(TRUE);
            return getGreenModeCtrl() ? ON : OFF;
        case OFF:
            LOG_DEBUG("Output_LED_GreenMode OFF");
            greenModeCtrl(FALSE);
            return getGreenModeCtrl() ? ON : OFF;
        default:
            LOG_DEBUG("Output_LED_GreenMode NOT_USED");
            return data;
    }
}

static char setOutputLEDRedMode(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_LED_RedMode ON");
            redMode(TRUE);
            return getRedMode() ? ON : OFF;
        case OFF:
            LOG_DEBUG("Output_LED_RedMode OFF");
            redMode(FALSE);
            return getRedMode() ? ON : OFF;
        default:
            LOG_DEBUG("Output_LED_RedMode NOT_USED");
            return data;
    }
}

static char setOutputONCharge(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_ON_Charge ON");
            onCharge(TRUE);
            return getOnCharge() ? ON : OFF;
        case OFF:
            LOG_DEBUG("Output_ON_Charge OFF");
            onCharge(FALSE);
            return getOnCharge() ? ON : OFF;
        default:
            LOG_DEBUG("Output_ON_Charge NOT_USED");
            return data;
    }
}

static char setOutputONOFF_Mode(char data){
    switch (data) {
        case ON:
            LOG_DEBUG("Output_ON_OFF_Mode ON");
            onOffMode(TRUE);
            return getOnOffMode() ? ON : OFF;
        case OFF:
            LOG_DEBUG("Output_ON_OFF_Mode OFF");
            onOffMode(FALSE);
            return getOnOffMode() ? ON : OFF;
        default:
            LOG_DEBUG("Output_ON_OFF_Mode NOT_USED");
            return data;
    }
}