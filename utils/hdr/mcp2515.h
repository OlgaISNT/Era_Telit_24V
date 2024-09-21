//
// Created by klokov on 14.09.2022.
//

#ifndef MAIN_REP_MCP2515_H
#define MAIN_REP_MCP2515_H

#include <stddef.h>
#include "m2mb_types.h"

// Команды
#define RESET 0xC0
#define READ  0x03
#define WRITE 0x02
#define RTS_TX0 0x81
#define RTS_TX1 0x82
#define RTS_TX2 0x84
#define RTS_ALL 0x87
#define RX_STATUS 0xB0
#define READ_STATUS 0xA0

// Адреса фильтров буфера приема
#define    RXF0SIDH 0x00
#define    RXF0SIDL 0x01
#define    RXF0EID8 0x02
#define    RXF0EID0 0x03
#define    RXF1SIDH 0x04
#define    RXF1SIDL 0x05
#define    RXF1EID8 0x06
#define    RXF1EID0 0x07
#define    RXF2SIDH 0x08
#define    RXF2SIDL 0x09
#define    RXF2EID8 0x0A
#define    RXF2EID0 0x0B
#define    RXF3SIDH 0x10
#define    RXF3SIDL 0x11
#define    RXF3EID8 0x12
#define    RXF3EID0 0x13
#define    RXF4SIDH 0x14
#define    RXF4SIDL 0x15
#define    RXF4EID8 0x16
#define    RXF4EID0 0x17
#define    RXF5SIDH 0x18
#define    RXF5SIDL 0x19
#define    RXF5EID8 0x1A
#define    RXF5EID0 0x1B

// Адреса масок буфера приема
#define    RXM0SIDH 0x20
#define    RXM0SIDL 0x21
#define    RXM0EID8 0x22
#define    RXM0EID0 0x23
#define    RXM1SIDH 0x24
#define    RXM1SIDL 0x25
#define    RXM1EID8 0x26
#define    RXM1EID0 0x27

// Адреса регистров настройки скорости CAN
#define    CNF3 0x28
#define    CNF2 0x29
#define    CNF1 0x2A

// Адреса регистров управления буферами приема
#define RXB0CTRL 0x60
#define RXB1CTRL 0x70

// Адреса регистров управления буферами передачи
#define TXB0CTRL 0x30
#define TXB1CTRL 0x40
#define TXB2CTRL 0x50

#define CANCTRL 0x0F
#define CANSTAT 0x0E

// Адреса регистров настройки и чтения прерываний
#define CANINTE	0x2B
#define CANINTF	0x2C

// Адреса буфера приема RX0
#define	READ_RX0 0x90
#define	READ_RX1 0x94
#define	WRITE_TX0 0x40
#define	WRITE_TX1 0x42
#define	WRITE_TX2 0x44

#define	RXB0SIDH 0x61
#define	RXB0SIDL 0x62
#define	RXB0EID8 0x63
#define	RXB0EID0 0x64
#define	RXB0DLC 0x65
#define	RXB0D0 0x66
#define	RXB0D1 0x67
#define	RXB0D2 0x68
#define	RXB0D3 0x69
#define	RXB0D4 0x6A
#define	RXB0D5 0x6B
#define	RXB0D6 0x6C
#define	RXB0D7 0x6D

// Адреса буфера приема RX1
#define	RXB1SIDH 0x71
#define RXB1SIDL 0x72
#define	RXB1EID8 0x73
#define	RXB1EID0 0x74
#define	RXB1DLC	0x75
#define RXB1D0 0x76
#define	RXB1D1 0x77
#define	RXB1D2 0x78
#define	RXB1D3 0x79
#define	RXB1D4 0x7A
#define RXB1D5 0x7B
#define	RXB1D6 0x7C
#define	RXB1D7 0x7D

// Адреса буфера передачи TX0
#define	TXB0CTRL 0x30
#define	TXB0SIDH 0x31
#define	TXB0SIDL 0x32
#define	TXB0EID8 0x33
#define	TXB0EID0 0x34
#define	TXB0DLC	0x35
#define	TXB0D0 0x36
#define	TXB0D1 0x37
#define	TXB0D2 0x38
#define	TXB0D3 0x39
#define	TXB0D4 0x3A
#define	TXB0D5 0x3B
#define	TXB0D6 0x3C
#define	TXB0D7 0x3D

// Адреса буфера передачи TX1
#define	TXB1CTRL 0x40
#define	TXB1SIDH 0x41
#define	TXB1SIDL 0x42
#define	TXB1EID8 0x43
#define	TXB1EID0 0x44
#define	TXB1DLC	0x45
#define	TXB1D0 0x46
#define	TXB1D1 0x47
#define	TXB1D2 0x48
#define	TXB1D3 0x49
#define	TXB1D4 0x4A
#define	TXB1D5 0x4B
#define	TXB1D6 0x4C
#define	TXB1D7 0x4D

// Адреса буфера передачи TX2
#define	TXB2CTRL 0x50
#define	TXB2SIDH 0x51
#define	TXB2SIDL 0x52
#define	TXB2EID8 0x53
#define	TXB2EID0 0x54
#define	TXB2DLC	0x55
#define	TXB2D0 0x56
#define	TXB2D1 0x57
#define	TXB2D2 0x58
#define	TXB2D3 0x59
#define	TXB2D4 0x5A
#define	TXB2D5 0x5B
#define	TXB2D6 0x5C
#define	TXB2D7 0x5D

// Адреса других регистров
#define EFLG 0x2D

// Адреса счетчиков ошибок приема и передачи
#define	TEC_ADR	0x1C
#define	REC_ADR	0x1D

#define MCP_16MHz_1000kBPS_CFG1 (0x00)
#define MCP_16MHz_1000kBPS_CFG2 (0xD0)
#define MCP_16MHz_1000kBPS_CFG3 (0x82)

#define MCP_16MHz_500kBPS_CFG1 (0x80)
#define MCP_16MHz_500kBPS_CFG2 (0xA4)
#define MCP_16MHz_500kBPS_CFG3 (0x04)

#define MCP_16MHz_250kBPS_CFG1 (0x41)
#define MCP_16MHz_250kBPS_CFG2 (0xF1)
#define MCP_16MHz_250kBPS_CFG3 (0x85)

#define MCP_16MHz_200kBPS_CFG1 (0x01)
#define MCP_16MHz_200kBPS_CFG2 (0xFA)
#define MCP_16MHz_200kBPS_CFG3 (0x87)

#define MCP_16MHz_125kBPS_CFG1 (0x03)
#define MCP_16MHz_125kBPS_CFG2 (0xF0)
#define MCP_16MHz_125kBPS_CFG3 (0x86)

#define MCP_16MHz_100kBPS_CFG1 (0x03)
#define MCP_16MHz_100kBPS_CFG2 (0xFA)
#define MCP_16MHz_100kBPS_CFG3 (0x87)

#define MCP_16MHz_83kBPS_CFG1 (0x03)
#define MCP_16MHz_83kBPS_CFG2 (0xBE)
#define MCP_16MHz_83kBPS_CFG3 (0x07)

#define MCP_16MHz_50kBPS_CFG1 (0x07)
#define MCP_16MHz_50kBPS_CFG2 (0xFA)
#define MCP_16MHz_50kBPS_CFG3 (0x87)

#define MCP_16MHz_40kBPS_CFG1 (0x07)
#define MCP_16MHz_40kBPS_CFG2 (0xFF)
#define MCP_16MHz_40kBPS_CFG3 (0x87)

#define MCP_16MHz_33kBPS_CFG1 (0x4E)
#define MCP_16MHz_33kBPS_CFG2 (0xF1)
#define MCP_16MHz_33kBPS_CFG3 (0x85)

#define MCP_16MHz_20kBPS_CFG1 (0x0F)
#define MCP_16MHz_20kBPS_CFG2 (0xFF)
#define MCP_16MHz_20kBPS_CFG3 (0x87)

#define MCP_16MHz_10kBPS_CFG1 (0x1F)
#define MCP_16MHz_10kBPS_CFG2 (0xFF)
#define MCP_16MHz_10kBPS_CFG3 (0x87)

#define MCP_16MHz_5kBPS_CFG1 (0x3F)
#define MCP_16MHz_5kBPS_CFG2 (0xFF)
#define MCP_16MHz_5kBPS_CFG3 (0x87)

typedef enum {
    START_CAN_SERV,   // запуск сервиса CHARGER
    STOP_CAN_SERV,     // останов сервиса
} CAN_TASK_COMMAND;

typedef enum {
    READ_MESSAGE,   // запуск сервиса CHARGER
    WRITE_MESSAGE,  // останов сервиса
} CAN_EVENT;

typedef enum {
    NORMAL_MODE,
    SLEEP_MODE,
    LOOPBACK_MODE,
    LISTEN_ONLY_MODE,
    CONFIGURATION_MODE,
    UNDEFINED_MODE
} OPMOD;

// Настроки масок и фильтров для фильтрации только
#define UDS_MASK_SIDH (0xFF)
#define UDS_MASK_SIDL (0xE0)

#define UDS_FILT_SIDH_TERM (0xED) // ID сообщений от терминала
#define UDS_FILT_SIDL_TERM (0x00)
#define UDS_FILT_SIDH_AIRBAGS_0 (0x63) // ID сигнала подушек безопасности 0x31C
#define UDS_FILT_SIDL_AIRBAGS_0 (0x80)
#define UDS_FILT_SIDH_AIRBAGS_1 (0x64) // ID сигнала подушек безопасности 0x320
#define UDS_FILT_SIDL_AIRBAGS_1 (0x00)
#define UDS_FILT_SIDH_AIRBAGS_2 (0x04) // ID сигнала подушек безопасности 0x021
#define UDS_FILT_SIDL_AIRBAGS_2 (0x20)
#define UDS_FILT_SIDH_AIRBAGS_3 (0x15) // ID сигнала подушек безопасности 0x0AB
#define UDS_FILT_SIDL_AIRBAGS_3 (0x60)

#define UDS_FILT_SIDH_THR (0x72)
#define UDS_FILT_SIDL_THR (0x20)

typedef enum {
    UDS_ONLY_TYPE_0, // 0x31C
    UDS_ONLY_TYPE_1, // 0x320
    UDS_ONLY_TYPE_2, // 0x021
    UDS_ONLY_TYPE_3, // 0xAB
    SEARCH_AIRBAGS,  // Поиск подушек
    UDS_WITHOUT_AIRBAGS,
    ANY_FRAME
} CAN_FILTERING_MODE;


// Скорости работы CAN шины для кварца 16MHz
typedef enum {
    MCP_16MHz_1000kBPS,
    MCP_16MHz_500kBPS,
    MCP_16MHz_250kBPS,
    MCP_16MHz_200kBPS,
    MCP_16MHz_125kBPS,
    MCP_16MHz_100kBPS,
    MCP_16MHz_83kBPS,
    MCP_16MHz_50kBPS,
    MCP_16MHz_40kBPS,
    MCP_16MHz_33kBPS,
    MCP_16MHz_20kBPS,
    MCP_16MHz_10kBPS,
    MCP_16MHz_5kBPS
} SPEED;

typedef enum {
    STANDARD,
    EXTENDED
} TYPE_ID;

typedef enum {
    REMOTE,
    NOT_REMOTE
} TYPE_FRAME;

typedef struct {
    BOOLEAN MERRF;
    BOOLEAN WAKIF;
    BOOLEAN ERRIF;
    BOOLEAN TX2IF;
    BOOLEAN TX1IF;
    BOOLEAN TX0IF;
    BOOLEAN RX1IF;
    BOOLEAN RX0IF;
} CANINTF_CONT;

typedef struct {
    BOOLEAN bit0;
    BOOLEAN bit1;
    BOOLEAN bit2;
    BOOLEAN bit3;
    BOOLEAN bit4;
    BOOLEAN bit5;
    BOOLEAN bit6;
    BOOLEAN bit7;
}RX_STATUS_CONT;

typedef struct {
    BOOLEAN RX1OVR;
    BOOLEAN RX0OVR;
    BOOLEAN TXBO;
    BOOLEAN TXEP;
    BOOLEAN RXEP;
    BOOLEAN TXWAR;
    BOOLEAN RXWAR;
    BOOLEAN EWARN;
}EFLG_CONT;

typedef struct {
    BOOLEAN RX0IF;
    BOOLEAN RX1IF;
    BOOLEAN TX0REQ;
    BOOLEAN TX0IF;
    BOOLEAN TX1REQ;
    BOOLEAN TX1IF;
    BOOLEAN TX2REQ;
    BOOLEAN TX2IF;
} STATUS;

typedef struct {
    UINT32 id;
    TYPE_ID typeId;
    TYPE_FRAME rtr;
    char SIDH;
    char SIDL;
    char EID8;
    char EID0;
    char DLC;
    char data[8];
} MESSAGE;

extern INT32 SPI_CAN_task(INT32 type, INT32 param1, INT32 param2);

extern INT32 SPI_CAN_event_task(INT32 type, INT32 param1, INT32 param2);


extern void finishCAN(void);    // Останов сервиса взаимодействия с CAN шиной
extern void printMessage(MESSAGE mess, BOOLEAN isRead); // Вывести содержимое сообщения в консоль
extern BOOLEAN message_to_string(MESSAGE mess, char *buffer, UINT32 buffer_size, char *name);
extern void sendMessage(UINT32 id, TYPE_FRAME typeFrame, const char *data, size_t size);// Отправить сообщение
extern BOOLEAN set_can_filter(CAN_FILTERING_MODE mode);
extern BOOLEAN is_init(void);

extern CAN_FILTERING_MODE get_current_filter(void);
#endif //MAIN_REP_MCP2515_H
