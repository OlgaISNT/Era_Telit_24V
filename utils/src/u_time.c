/*
 * u_time.c
 * Created on: 24.03.2022, Author: DKozenkov     
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <m2mb_os_api.h>
#include <m2mb_rtc.h>
#include <utmpx.h>
#include "u_time.h"
#include "log.h"
#include <sys/time.h>

/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
typedef enum {
    DATE_TIME, DATE, TIME
} DT_FORMAT;

/* Local statics ================================================================================*/
/* Local function prototypes ====================================================================*/
void str_format(struct tm *u, char *str, DT_FORMAT format);
/* Static functions =============================================================================*/
static struct tm *get_current_time(void);
/* Global functions =============================================================================*/

int updateRTC_ms(UINT64 timestamp_ms, UINT32 delta_sec) {
    UINT32 t = (UINT32) (timestamp_ms / 1000);
    return updateRTC(t, delta_sec);
}

int updateRTC(UINT32 timestamp_sec, UINT32 delta_sec) {
    const time_t rtc = time((void*)0);
    //LOG_INFO("----now time: %i", rtc); // TODO разобраться, почему после запуска опроса GNSS время в NMEA отстаёт
    //LOG_INFO("----new time: %i", timestamp_sec);
    int d = ((int)rtc) - ((int)timestamp_sec);
    if (fabs(d) > delta_sec) {
        return set_time_RTC(timestamp_sec);
    } else return M2MB_OS_SUCCESS;
}

UINT32 get_rtc_timestamp(void) {
    const time_t rtc = time((void*)0);
    return ((UINT32)rtc);
}

int set_time_RTC(UINT32 timestamp_sec) {
    UINT64 ms = timestamp_sec; ms *= 1000;
    return set_time_RTC_ms(ms);
}

int set_time_RTC_ms(UINT64 timestamp_ms) {
    (void) timestamp_ms;
    int ret = M2MB_OS_SUCCESS;
    int fd = m2mb_rtc_open("/dev/rtc0", 0);
    if (fd == -1) {
        LOG_CRITICAL("Can't open RTC!!");
        return M2MB_OS_SUCCESS;
    }
    UINT32 sec = timestamp_ms / 1000;
    M2MB_RTC_TIMEVAL_T timeval = {sec, timestamp_ms};
    if (m2mb_rtc_ioctl(fd, M2MB_RTC_IOCTL_SET_TIMEVAL, &timeval) == -1) {
        LOG_CRITICAL("Can't set RTC");
        ret = M2MB_OS_APP_INVALID;
    } else {
        LOG_INFO("RTC was setted to %i sec", sec);
    }
    if (m2mb_rtc_close(fd) == -1) {
        ret = M2MB_OS_NOT_DONE;
    }
    return ret;
}

void str_format(struct tm *u, char *str,  DT_FORMAT format) {
    char *f;
    switch (format) {
        default:
        case DATE_TIME: f = "%d.%m.%Y %H:%M:%S "; break; //strftime(s, 40, "%d.%m.%Y %H:%M:%S, %A", u);
        case DATE:		f = "%d.%m.%Y "; break;
        case TIME:		f = "%H:%M:%S "; break;
    }
    strftime(str, STR_SIZE, f, u);
}
static struct tm *get_current_time(void) {
    const time_t timer = time((void*)0);
    return localtime(&timer);
}

void get_sys_date_time(char *str) {
    struct tm *u = get_current_time();
    str_format(u, str, DATE_TIME);
}

void get_sys_date(char *str) {
    struct tm *u = get_current_time();
    str_format(u, str, DATE);
}

void get_sys_time(char *str) {
    struct tm *u = get_current_time();
    str_format(u, str, TIME);
}

void get_time_for_logs(char *buffer, UINT32 buffer_size){
    memset(buffer, '\0', buffer_size);


    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;

//    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
    strftime(buffer, buffer_size, "%d.%m.%Y %H:%M:%S", localtime(&curTime.tv_sec));
    sprintf(buffer, "%s:%03d", buffer, milli);
}

