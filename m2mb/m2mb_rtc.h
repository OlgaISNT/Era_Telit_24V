/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_rtc.h

  @brief
    m2m base api for rtc functionalities

  @details
    m2m base rtc api prototype:
    m2mb_rtc_open
    m2mb_rtc_close
    m2mb_rtc_ioctl

  @notes

  @author
    Davide Cubeddu

  @date
    21/09/2017
*/


#ifndef  M2M_M2MB_RTC_API_H
#define  M2M_M2MB_RTC_API_H


/* Global declarations ==========================================================================*/


/* Global typedefs ==============================================================================*/
typedef struct
{
  UINT8  sec;
  UINT8  min;
  UINT8  hour;
  UINT8  day;   /* the range for day depends either on the month and on the year it refers to.
                   Available ranges are: (01..28) (01..29) (01..30) (01..31) */
  UINT8  mon;
  UINT16 year;  /* complete year, range is: ( 2000..2099) */
  INT16  tz;    /* Time Zone */
  UINT8  dlst;  /* Daylight Saving Time */
} M2MB_RTC_TIME_T;

typedef struct
{
  UINT32  sec;
  UINT32  msec;
} M2MB_RTC_TIMEVAL_T;

typedef enum
{
  M2MB_RTC_IOCTL_GET_SYSTEM_TIME,  /* Read system time */
  M2MB_RTC_IOCTL_SET_SYSTEM_TIME,  /* Set system time */
  M2MB_RTC_IOCTL_SET_ALARM_TIME,   /* Set time for a specific alarm */
  M2MB_RTC_IOCTL_CLEAR_ALARM,      /* Clear a specific alarm */
  M2MB_RTC_IOCTL_GET_TIMEVAL,      /* Get current time in timeval format. Epoch: 1970/01/01-00:00:00 */
  M2MB_RTC_IOCTL_SET_TIMEVAL       /* Set current time in timeval format. Epoch: 1970/01/01-00:00:00 */
} M2MB_RTC_IOCTL_REQUEST_E;


/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Open a RTC device

  @details

  @param[in] path
    /dev/rtc#, where # is in decimal format
  @param[in] flags
    currently unused

  @return
    file descriptor on SUCCESS
    -1 on FAILURE

  @note

  @b
    fd = m2mb_rtc_open( "/dev/rtc0", 0 );
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_rtc_open( const CHAR *path, INT32 flags, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Close a RTC device

  @details

  @param[in] fd
    file descriptor returned by m2mb_rtc_open

  @return
    0 on SUCCESS
    -1 on FAILURE

  @note
    In case of Failure result memory will be not deallocated,
    so check the return value before send another m2mb_rtc_open.

  @b
    m2mb_rtc_close( fd );
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_rtc_close( INT32 fd );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Configure a RTC device

  @details

  @param[in] fd
    file descriptor returned by m2mb_rtc_open
  @param[in] request
    required operation (see M2MB_RTC_IOCTL_REQUEST_E)

  @return
    0 on SUCCESS
    -1 on FAILURE

  @note

  @b
    M2MB_RTC_TIME_T time;
    M2MB_RTC_TIMEVAL_T timeval;
    UINT8 alarmId;

    time = ...
    alarmId =         only 0x01

    m2mb_rtc_ioctl( fd, M2MB_RTC_IOCTL_GET_SYSTEM_TIME, &time );
    m2mb_rtc_ioctl( fd, M2MB_RTC_IOCTL_SET_SYSTEM_TIME, &time );
    m2mb_rtc_ioctl( fd, M2MB_RTC_IOCTL_SET_ALARM_TIME, &time, alarmId );
    m2mb_rtc_ioctl( fd, M2MB_RTC_IOCTL_CLEAR_ALARM, alarmId );
    m2mb_rtc_ioctl( fd, M2MB_RTC_IOCTL_GET_TIMEVAL, &timeval );
    m2mb_rtc_ioctl( fd, M2MB_RTC_IOCTL_SET_TIMEVAL, &timeval );
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_rtc_ioctl( INT32 fd, INT32 request, ... );


#endif  /* M2M_M2MB_GNSS_API_H */

