/*
 * u_time.h
 * Created on: 24.03.2022, Author: DKozenkov     
 */

#ifndef TIME_U_TIME_H_
#define TIME_U_TIME_H_

#define STR_SIZE 40 // размер строки, которая будет заполнена символами текущей даты/временем

int updateRTC_ms(UINT64 timestamp_ms, UINT32 delta_sec);
int updateRTC(UINT32 timestamp_sec, UINT32 delta_sec);
int set_time_RTC(UINT32 timestamp_sec);
int set_time_RTC_ms(UINT64 timestamp_ms);

/*
 * Заполняет переданную строку (не менее 40 байт) текущей датой/временем
 */
void get_sys_date_time(char *str);
/*
 * Заполняет переданную строку (не менее 40 байт) текущей датой
 */
void get_sys_date(char *str);
/*
 *  Заполняет переданную строку (не менее 40 байт) текущим временем
 */
void get_sys_time(char *str);

/** Возвращает текущий UTC из RTC модема*/
UINT32 get_rtc_timestamp(void);

void get_time_for_logs(char *buffer, UINT32 buffer_size);

#endif /* TIME_U_TIME_H_ */
