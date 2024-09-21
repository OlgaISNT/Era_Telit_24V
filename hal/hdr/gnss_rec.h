/**
 * @file gnss.h
 * @version 1.0.0
 * @date 28.03.2022
 * @author: DKozenkov
 * @brief
 */

#ifndef HDR_GNSS_REC_H_
#define HDR_GNSS_REC_H_

#define UNKNOWN_COORDINATE 0x7FFFFFFF

typedef enum {
    START_GNSS,			// запуск GNSS-приёмника
    //STOP_GNSS,			// останов GNSS-приёмника  // осторожно, отправка команды AT$GPSP=0 почему-то блокирует обмен данными по UART0!
    RESTART_GNSS_SESSION, // программный перезапуск GNSS-приёмника
    RESET_GNSS,			// сброс GNSS-приёмника
    PRINT_GNSS_DATA,	// вывод в лог последних данных от GNSS-приёмника
    GNSS_TIMER,
    CLEAR_LAST_RELIABLE_COORD	// стирание последних известных координат
} GNSS_COMMAND;

typedef struct {
    UINT32 utc;			// timestamp UTC, секунды
    INT32 latitude;
    INT32 longitude;
} T_Location;

typedef struct {
    UINT32 utc;			// timestamp UTC, секунды
    char latitude_i[11];// значение широты непосредственно от приёмника
    char longitude_i[12];// значение долготы непосредственно от приёмника
    INT32 latitude;		// широта в угловых мС
    INT32 longitude;	// долгота в угловых мС
    T_Location rec_locations[2]; // предыдущие координаты в моменты времени N-1 и N-2
    float hdop;			// горизонтальная точность HDOP
    float altitude;		// высота над уровнем моря, метры
    UINT8 fix;			// признак достоверности геоданных
    float course;		// курс (угол), градусы.минуты
    UINT16 speed;		// скорость, км/ч
    UINT8 gps_sat;		// кол-во спутников GPS
    UINT8 glonass_sat;	// кол-во спутников GPS
} T_GnssData;


INT32 GNSS_task(INT32 type, INT32 param1, INT32 param2);
INT32 GNSS_TT_task(INT32 type, INT32 param1, INT32 param2);

BOOLEAN is_gnss_valid(void);

T_GnssData get_gnss_data(void);

void print_gnss_data(T_GnssData gd);

INT32 get_m_last_latitude(void);
INT32 get_m_last_longitude(void);

void deinit_gnss(void);

/**
 * Временно приостановить отправку АТ-команд запроса геоданных
 */
void pause_gnss_request(BOOLEAN value);

/**
 * Проверка, остановлено ли чтение геоданных из ГНСС-приёмника
 * @return TRUE - остановлено (АТ-запросы не отправляются в ГНСС)
 */
BOOLEAN is_gnss_req_stopped(void);

BOOLEAN is_gnss_resetted(void);

T_GnssData get_gnss_data_fast(void);
#endif /* HDR_GNSS_REC_H_ */
