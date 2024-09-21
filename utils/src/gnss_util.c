/**
 * @file gnss_util.c
 * @version 1.0.0
 * @date 14.04.2022
 * @author: AKlokov
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include <time.h>
#include <math.h>
#include "char_service.h"
#include "gnss_rec.h"
#include "log.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
/* Local function prototypes ====================================================================*/
static UINT32 getUtcFromString(char *str);
static INT32 getLat(char *str);
static INT32 getLon(char *str);
static double degreeToMilSec(double degrees, double minutes);

#define DELIMITER ','
#define START_STR "$GPSACP:"
/* Static functions =============================================================================*/

static double degreeToMilSec(double degrees, double minutes){
    // Конвертация из представления широты/долготы градусы, минуты, доли минут в градусы и доли градусов
    double result = degrees + (minutes / 60);
    // Переводим градусы в секунды
    // 1 угловой градус - 3600 угловых секунд или (3600 * 1000) милисекунд
    result = result * 3600 * 1000;
    return result;
}


//5332.2471N
//ddmm.mmmm N/S
//N/S => +/-
INT32 getLat(char *str){
    if (strlen(str) != 10){
        // Некорректная строка
        return 0;
    }
    // Получаем градусы
    char *y;
    char deg[3];
    memcpy(deg, &str[0], 2);
    double degrees = strtod(deg, &y);


    // Получаем минуты
    char *x;
    char min[8];
    memcpy(min, &str[2], 7);
    double minutes = strtod(min,&x);
    double result = degreeToMilSec(degrees, minutes);
    if(str[9] == 'S'){
        result *= (-1);
    }
    return result;
}

//04918.9815E
//dddmm.mmmm E/W
//E/W => +/-
INT32 getLon(char *str){
    if (strlen(str) != 11){
        // Некорректная строка
        return 0;
    }

    // Получаем градусы
    char *y;
    char deg[4];
    memcpy(deg, &str[0], 3);
    double degrees = strtod(deg, &y);

    // Получаем минуты
    char *x;
    char min[8];
    memcpy(min, &str[3], 7);
    double minutes = strtod(min,&x);
    double result = degreeToMilSec(degrees, minutes);

    if(str[10] == 'W'){
        result *= (-1);
    }
    return result;
}

// 1656922855
// 1649308855
//      hhmmss.sssddmmyy
//     "150006.004140422"
//<UTC>[082055.000070422]
static UINT32 getUtcFromString(char *str){
    if (strlen(str) != 16){
        // Неверная длина строки
        return 0;
    }
    struct tm m_time;

    // int часы (отсчет с 0);
    char hours[3];
    hours[2] = '\0';
    memcpy(hours, &str[0], 2);
    m_time.tm_hour = atoi(hours);

    // int минуты (отсчет с 0);
    char minutes[3];
    minutes[2] = '\0';
    memcpy(minutes, &str[2], 2);
    m_time.tm_min = atoi(minutes);

    // int секунды (отсчет с 0);
    char seconds[3];
    seconds[2] = '\0';
    memcpy(seconds, &str[4], 2);
    m_time.tm_sec = atoi(seconds);

    // int день месяца (отсчет с 1);
    char day[3];
    day[2] = '\0';
    memcpy(day, &str[10], 2);
    m_time.tm_mday = atoi(day);

    // int месяц (отсчет с 0);
    char month[3];
    month[2] = '\0';
    memcpy(month, &str[12], 2);
    m_time.tm_mon = atoi(month) - 1;

    // int год (за начала отсчета принят 1900 год);
    char year[3];
    year[2] = '\0';
    memcpy(year, &str[14], 2);
    int y = atoi(year);
    m_time.tm_year = y + 2000 - 1900;

    m_time.tm_wday = 0;   // день недели (воскресенье - 0);
    m_time.tm_yday = 0;   // день в году (отсчет с 0);
    m_time.tm_isdst = -1; // признак "летнее время"
    return mktime (&m_time);
}

/* Global functions =============================================================================*/
T_GnssData parse_gnss_mess(char *message) {
    T_GnssData gnssData;
    if (message == NULL || strstr (message, "ERROR") != NULL || strstr (message, "OK") == NULL){
        //LOG_DEBUG("GNSS message is not valid: %s", (message == NULL ? "NULL" : message));
        goto down;
    }

    for(unsigned int j = 0; j < strlen(message); j++){
        if(message[j] == '\n' || message[j] == '\r' || message[j] == ' ' || message[j] == '\t'){
            deleteChar(message,j);
            j--;
        }
    }
        LOG_DEBUG("len = %d content = >%s<", strlen(message), message);
    // В этом месте можно увидеть следующие строки:
    // $GPSACP:074818.000,5540.7049N,03737.9245E,0.6,196.9,3,0.0,1.1,0.6,231022,10,05OK
    // $GPSACP:,,,,,1,,,,,,OK
    // Нужно убрать из строки: "OK" и "$GPSACP:".

    // Удаление лишних символов перед >$GPSACP:<
    // Пример поломанной строки:
    // #ECALLEV:5#ECALLEV:12$GPSACP:143941.000,5540.7076N,03737.9276E,0.7,198.2,3,358.8,0.0,0.0,280123,09,07OK
    // #APLAYEV:0$GPSACP:154828.000,5540.7045N,03737.9337E,0.6,201.1,3,0.0,0.0,0.0,280123,10,05OK
    char *st = strstr (message,START_STR);
    size_t start_ind = st - message;
    size_t lnt = strlen(message) - start_ind;
    char newMess[200];
    if (st == NULL) {
        goto down;
    } else {
        if(start_ind > 0){
            LOG_DEBUG("GNSS_P: bad message\r\n>%s<", message);
            memcpy(&newMess[0], &message[start_ind], strlen(message) - start_ind);
            newMess[lnt] = '\0';
            message = &newMess[0];
            LOG_DEBUG("GNSS_P: recov message\r\n>%s<", message);
        }
    }

    // Убираем "OK" (всегда последние 2 символа)
    message[strlen(message) - 2] = '\0';
    // LOG_DEBUG("after erase OK len = %d content = >%s<", strlen(message), message);
    // В этом месте можно увидеть следующие строки:
    // $GPSACP:081104.000,5540.6859N,03737.8896E,1.9,44.7,3,0.0,0.0,0.0,231022,03,02
    // $GPSACP:,,,,,1,,,,,,

    // Убираем "$GPSACP:" (всегда первые 8 символов)
    for (UINT16 i = 8; i < strlen(message); i++){
        message[i - 8] = message[i];
    }
    message[strlen(message) - 8] = '\0';
//     LOG_DEBUG("after erase $GPSACP: len = %d content = >%s<", strlen(message), message);
    // В этом месте можно увидеть следующие строки:
    // 082757.000,5540.7014N,03737.9114E,1.7,251.0,3,158.0,0.0,0.0,231022,04,02
    // ,,,,,1,,,,,,

    // Теперь нужно пересчитать количество разделителей ',' (всегда 11 штук)
    // Также нужно проверить отсутствуют ли несколько подряд идущих разделителей
    // Также первый и последний символы не могут быть разделителями
    UINT8 delimCnt = 0;
    if ((message[0] == DELIMITER) | (message[strlen(message) - 1] == DELIMITER)){
        goto down;
    }
    for (UINT32 i = 0; i < strlen(message); ++i) {
        if (message[i] == DELIMITER){
            delimCnt++;
        }
        if (i < strlen(message) - 1){
            if ((message[i] == DELIMITER) & (message[i] == message[i + 1])){
                goto down;
            }
        }
    }
    if (delimCnt != 11){
        goto down;
    }
    // LOG_DEBUG("delim cnt %d", delimCnt);

    // Собираем контент до каждого следующего разделителя
    char token[30];
    char dateTime[11];
    char dateTimeAddon[6];
    char fullTd[17];
    UINT8 index = 0;
    UINT8 stage = 0;
    for (UINT32 i = 0; i < strlen(message); i++){
        if (message[i] == DELIMITER){ // Если обнаружен разделитель
            up:
            token[index] = '\0';
//            LOG_DEBUG(">%s< stage = %d", token, stage);
            index = 0;
            switch (stage) {
                case 0:  // <UTC> part1
                    memcpy(dateTime, token, 11);
//                    LOG_DEBUG("<UTC> part1 >%s<",dateTime);
//                    LOG_DEBUG("<UTC> token >%s<",token);
                    // Без выделения памяти
                    break;
                case 1:  // <latitude>
                    gnssData.latitude = getLat(token);
                    if (strlen(token) == sizeof(gnssData.latitude_i) - 1){
                        memcpy(gnssData.latitude_i, token, sizeof(gnssData.latitude_i) - 1);
                    }
//                    LOG_DEBUG("latitude %d",   gnssData.latitude);
                    break;
                case 2:  // <longitude>
                    gnssData.longitude = getLon(token);
                    if (strlen(token) == sizeof(gnssData.longitude_i) - 1) {
                        memcpy(gnssData.longitude_i, token,sizeof(gnssData.longitude_i) - 1);
                    }
//                    LOG_DEBUG("longitude %d",   gnssData.longitude);
                    break;
                case 3:  // <hdop>
                    gnssData.hdop = atof(token);
//                    LOG_DEBUG("hdop %f",   gnssData.hdop);
                    break;
                case 4:  // <altitude>
                    gnssData.altitude = atof(token);
//                    LOG_DEBUG("altitude %f",   gnssData.altitude);
                    break;
                case 5:  // <fix>
                    gnssData.fix = atoi(token);
//                    LOG_DEBUG("fix %d",   gnssData.fix);
                    break;
                case 6:  // <cog>
                    gnssData.course = atof(token);
//                    LOG_DEBUG("course %f",   gnssData.course);
                    break;
                case 7:  // <spkm>
                    gnssData.speed = atoi(token);
//                    LOG_DEBUG("speed %d",   gnssData.speed);
                    break;
                case 8:  // <spkn> not use
                    break;
                case 9:  // <date> part2
                    memcpy(dateTimeAddon, token, 6);
                    memcpy(fullTd, dateTime, 10);          // 10 символов из dateTime без '\0' [0,9]
                    memcpy(&fullTd[10], dateTimeAddon, 6); // 6 символов из dateTimeAddon [10,15]
                    gnssData.utc = getUtcFromString(fullTd);
//                    LOG_DEBUG("utc %d",   gnssData.utc);
                    break;
                case 10: // <nsat_gps>
                    gnssData.gps_sat = atoi(token);
//                    LOG_DEBUG("gps_sat %d",   gnssData.gps_sat);
                    break;
                case 11: // <nsat_glonass>
                    gnssData.glonass_sat = atoi(token);
//                    LOG_DEBUG("glonass_sat %d",   gnssData.glonass_sat);
                    break;
                default:
                    break;
            }
            stage++;
        } else { // Если любой другой символ
            token[index] = message[i];
            index++;
        }
    }
    if (stage == 11){
        goto up;
    }


    return gnssData;

    down:
    gnssData.utc = 0;
    gnssData.latitude = 0;
    gnssData.longitude = 0;
    gnssData.fix = 0;
    gnssData.course = 0;
    gnssData.speed = 0;
    gnssData.hdop = 0;
    gnssData.glonass_sat = 0;
    gnssData.gps_sat = 0;
    gnssData.altitude = 0;
    return gnssData;
}
