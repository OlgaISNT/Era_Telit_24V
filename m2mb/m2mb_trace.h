/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_trace.h

  @brief
    Base trace system for m2m application

  @notes
    Dependencies: stdarg.h, m2mb_types.h

  @author
    mb_
    dinoa

  @date
    17/02/2017
    05/04/2018
*/

#ifndef M2M_M2MB_TRACE_H
#define M2M_M2MB_TRACE_H

#include <string.h>

/* GCC has only the standard predefined MACRO __FILE__ and there is no __MODULE__ like in RVCT   dinoa*/


#if defined(unix) || defined(__unix__) || defined(__unix)
#define __FILENAMEI__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define __FILENAMER__  __FILENAMEI__
#else
#define __FILENAMEI__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define __FILENAMER__  (strrchr(__FILENAMEI__, '/') ? strrchr(__FILENAMEI__, '/') + 1 : __FILENAMEI__)

#endif


#if !defined(__MODULE__)
#define __MODULE__ __FILENAMER__
#endif



/* @FHB -------------------------------------------------------------------------------

  Info: Available ONLY for AZ user applications:
  PROTOTIPE OF THE log_az.ini FILE, to be inserted in the filesystem of the module
  in  <mod> directory with AT#M2MWRITE="/data/azc/mod/log_az.ini"
  The log_az.ini will be read with m2mb_trace_init
  if there is no log_az.ini, then the function m2mb_trace_init will set all the classes
  to a value of M2MB_TL_DEBUG (5) and there will be no filter for the print
  Here a copy of the file log_az.ini :
***********************************************************************<<<start here>>>
  ; conf file for appzone2 trace:

  [logport]       ; if PORT=  (empty) TRACE is the default value
  PORT=TRACE      ; TRACE, USB0, USB1, UART

  [class]   ; LOG SEVERITY for each class. Setting class M2MB_TL_DEBUG (5) will show
            ; also LOG, WARNING... Setting M2MB_TL_LOG will show WARNING, ERROR... and so on
            ; setting to 0 will not show any trace
  TC_GENERIC = 3
  TC_SOCKET = 0   ; 0=OFF 1=M2MB_TL_FATAL 2=M2MB_TL_ERROR 3=M2MB_TL_WARNING 4=M2MB_TL_LOG 5=M2MB_TL_DEBUG
  TC_CLOCK = 0
  TC_PDP = 0
  TC_GNSS = 0
  TC_M2M_USER = 4
  TC_FOTA = 0
  TC_FS = 0
  TC_QMI = 0
  TC_SMS = 0
  TC_INFO = 4
  TC_LWM2M = 0
  TC_NET = 0
  TC_SIM = 0
  TC_SPI = 0
  TC_USB = 0
  TC_NV = 1
  TC_RTC = 0
  TC_UART = 0
  TC_POWER = 0

*****************************************************************<<<end here>>>

  @FHE --------------------------------------------------------------------------------- */



/* Global declarations ==========================================================================*/
#define m2mb_trace_printf( _class, level, format, ... ) \
  m2mb_trace_print_##level( _class, level, format, ##__VA_ARGS__ )

#if !defined(M2MB_TRACE_LEVEL_FATAL_DISABLED)
#define m2mb_trace_print_M2MB_TL_FATAL( _class, level, format, ... ) \
  m2mb_trace_file_line_printf(__MODULE__, __LINE__, _class, level, format, ##__VA_ARGS__)
#else
#define m2mb_trace_print_M2MB_TL_FATAL( _class, level, format, ... )
#endif

#if !defined(M2MB_TRACE_LEVEL_ERROR_DISABLED)
#define m2mb_trace_print_M2MB_TL_ERROR( _class, level, format, ... ) \
  m2mb_trace_file_line_printf(__MODULE__, __LINE__, _class, level, format, ##__VA_ARGS__)
#else
#define m2mb_trace_print_M2MB_TL_ERROR( _class, level, format, ... )
#endif

#if !defined(M2MB_TRACE_LEVEL_WARNING_DISABLED)
#define m2mb_trace_print_M2MB_TL_WARNING( _class, level, format, ... ) \
  m2mb_trace_file_line_printf(__MODULE__, __LINE__, _class, level, format, ##__VA_ARGS__)
#else
#define m2mb_trace_print_M2MB_TL_WARNING( _class, level, format, ... )
#endif

#if !defined(M2MB_TRACE_LEVEL_LOG_DISABLED)
#define m2mb_trace_print_M2MB_TL_LOG( _class, level, format, ... ) \
  m2mb_trace_file_line_printf(__MODULE__, __LINE__, _class, level, format, ##__VA_ARGS__)
#else
#define m2mb_trace_print_M2MB_TL_LOG( _class, level, format, ... )
#endif

#if !defined(M2MB_TRACE_LEVEL_DEBUG_DISABLED)
#define m2mb_trace_print_M2MB_TL_DEBUG( _class, level, format, ... ) \
  m2mb_trace_file_line_printf(__MODULE__, __LINE__, _class, level, format, ##__VA_ARGS__)
#else
#define m2mb_trace_print_M2MB_TL_DEBUG( _class, level, format, ... )
#endif

/* Global typedefs ==============================================================================*/

/*
  typedef enum
  {
  M2MB_TC_GENERIC = 0,
  M2MB_TC_SOCKET = 1,
  M2MB_TC_CLOCK = 2,
  M2MB_TC_PDP = 3,
  M2MB_TC_GNSS = 4,
  M2MB_TC_M2M_USER = 5,
  M2MB_TC_FOTA = 6,
  M2MB_TC_FS = 7,
  M2MB_TC_QMI = 8,
  M2MB_TC_SMS = 9,
  M2MB_TC_INFO = 10,
  M2MB_TC_LWM2M = 11,
  M2MB_TC_NET = 12,
  M2MB_TC_SIM = 13,
  M2MB_TC_SPI = 14,
  M2MB_TC_USB = 15,
  M2MB_TC_NV = 16,
  M2MB_TC_RTC = 17,
  M2MB_TC_UART = 18,
  M2MB_TC_POWER = 19,
  M2MB_TC_FTPC = 20,
  M2MB_TC_ATI = 21,
  M2MB_TC_BACKUP = 22,
  M2MB_TC_NIPD = 23,
  M2MB_TC_SYS = 24,
  M2MB_TC_PSM = 25,
  M2MB_TC_SSL = 26
  M2MB_TC_FWAUTOSIM = 27,
  NUM_M2MB_TC
  } M2MB_TRACE_CLASS;

*/

/* Parse SUBSYS_TRACE_TABLE into a struct using X-Macros
   the result will be an enum like the commented one above
*/

#define SUBSYS_TRACE_TABLE \
    SUB_TR_AZ(M2MB_TC_GENERIC, "TC_GENERIC")     \
    SUB_TR_AZ(M2MB_TC_SOCKET, "TC_SOCKET")    \
    SUB_TR_AZ(M2MB_TC_CLOCK, "TC_CLOCK")    \
    SUB_TR_AZ(M2MB_TC_PDP, "TC_PDP")    \
    SUB_TR_AZ(M2MB_TC_GNSS, "TC_GNSS")    \
    SUB_TR_AZ(M2MB_TC_M2M_USER, "TC_M2M_USER")    \
    SUB_TR_AZ(M2MB_TC_FOTA, "TC_FOTA")      \
    SUB_TR_AZ(M2MB_TC_FS, "TC_FS")      \
    SUB_TR_AZ(M2MB_TC_QMI, "TC_QMI")    \
    SUB_TR_AZ(M2MB_TC_SMS, "TC_SMS")    \
    SUB_TR_AZ(M2MB_TC_INFO, "TC_INFO")    \
    SUB_TR_AZ(M2MB_TC_LWM2M, "TC_LWM2M")    \
    SUB_TR_AZ(M2MB_TC_NET, "TC_NET")    \
    SUB_TR_AZ(M2MB_TC_SIM, "TC_SIM")    \
    SUB_TR_AZ(M2MB_TC_SPI, "TC_SPI")    \
    SUB_TR_AZ(M2MB_TC_USB, "TC_USB")    \
    SUB_TR_AZ(M2MB_TC_NV, "TC_NV")    \
    SUB_TR_AZ(M2MB_TC_RTC, "TC_RTC")    \
    SUB_TR_AZ(M2MB_TC_UART, "TC_UART")   \
    SUB_TR_AZ(M2MB_TC_POWER, "TC_POWER") \
    SUB_TR_AZ(M2MB_TC_FTPC, "TC_FTPC") \
    SUB_TR_AZ(M2MB_TC_ATI, "TC_ATI") \
    SUB_TR_AZ(M2MB_TC_BACKUP, "TC_BACKUP")  \
    SUB_TR_AZ(M2MB_TC_NIPD, "TC_NIPD")  \
    SUB_TR_AZ(M2MB_TC_SYS, "TC_SYS")  \
    SUB_TR_AZ(M2MB_TC_PSM, "TC_PSM")  \
    SUB_TR_AZ(M2MB_TC_SSL, "TC_SSL") \
    SUB_TR_AZ(M2MB_TC_FWAUTOSIM, "TC_FWAUTOSIM")

#define SUB_TR_AZ(a, b) a,
typedef enum
{
  SUBSYS_TRACE_TABLE
  NUM_M2MB_TC,
  ENUM_TO_INT( M2MB_TRACE_CLASS )
} M2MB_TRACE_CLASS;
#undef SUB_TR_AZ


/*
   LOG SEVERITY  enum for the trace.
   if in the ini file the log severity is set for example to  M2MB_TL_LOG (4), then all
   the log with inferior severity will be shown, but not M2MB_TL_DEBUG (5)
*/

typedef enum
{
  M2MB_TL_FATAL = 1,
  M2MB_TL_ERROR = 2,
  M2MB_TL_WARNING = 3,
  M2MB_TL_LOG = 4,
  M2MB_TL_DEBUG = 5,
  NUM_M2MB_TL,
  ENUM_TO_INT( M2MB_TRACE_LEVEL )
} M2MB_TRACE_LEVEL;

/*
   Type of output for logs, to be set only in log_az.ini.
   Without the ini file the log will always be on trace
*/
typedef enum
{
  LOG_TRACE, /* TRACE in the log_az.ini file */
  LOG_USB0,  /* USB0 in the log_az.ini file */
  LOG_USB1,  /* USB1 in the log_az.ini file */
  LOG_UART,  /* UART in the log_az.ini file */
  ENUM_TO_INT( M2MB_TRACE_OUTPUT_TYPE )
} M2MB_TRACE_OUTPUT_TYPE;

typedef HANDLE M2MB_TRACE_HANDLE;

/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Base trace system initializazion

  @details
    This function collects all necessary initializatione for the base trace system
    If there is no log_az.ini, all the classes to a M2MB_TRACE_LEVEL of M2MB_TL_DEBUG (5), that is
    all traces will be shown. ( user case )

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @b
    Example
  @code
    M2MB_RESULT_E Ret = m2mb_trace_init();
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_trace_init
(
  void
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Base trace system deinitializazion

  @details
    This function deinitialize and free the structure needed for the trace
    If there are no other log connections from other Apps close also the ports USB/UART open (if any)

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @b
    Example
  @code
    M2MB_RESULT_E Ret = m2mb_trace_deinit();
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_trace_deinit
(
  void
);


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Base trace abilitation

  @details
    This function enables a specific trace class

  @param[in] traceClass
    Class of the trace to be enabled at a M2MB_TRACE_LEVEL of M2MB_TL_DEBUG (5)
    If it is used after m2mb_trace_init, it activates the selected trace class.
    If it is used before or instead of m2mb_trace_init, it bypasses any reading of log_az.ini
    and activates the trace as LOG_TRACE with only the selected trace class activated

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @b
    Example
  @code
    M2MB_RESULT_E Ret = m2mb_trace_enable(M2MB_TC_GENERIC);
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_trace_enable
(
  M2MB_TRACE_CLASS traceClass
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Base trace disabilitation

  @details
    This function disaables a specific trace class

  @param[in] traceClass
    Class of the trace to be disabled
    If the log is not active, doesn't do anything.
    If the traceClass deactivated is the only active, it deactivates the trace

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @b
    Example
  @code
    M2MB_RESULT_E Ret = m2mb_trace_disable(M2MB_TC_GENERIC);
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_trace_disable
(
  M2MB_TRACE_CLASS traceClass
);

/*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Base trace printf with file and line info.

  @details
    Print formatted data to to trace system including file and line information.

  @param[in] file
    String containing the name of the file from where the trace is called
  @param[in] line
    String containing the line in the file from where the trace is called
  @param[in] traceClass
    Enum from M2MB_TRACE_CLASS that specifies class of the trace
  @param[in] level
    Enum from M2MB_TRACE_LEVEL that specifies level of the trace
  @param[in] fmt
    Format string for printf
  @param[in] ...
    Unnamed arguments

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @b
    Example
  @code
    #include <stdarg.h>

    M2MB_RESULT_E ret;

    ret = m2mb_trace_file_line_printf(__MODULE__, __LINE__, M2MB_TC_GENERIC, "%s", "Test");

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_trace_file_line_printf
(
  const char *file,
  int line,
  M2MB_TRACE_CLASS _class,
  M2MB_TRACE_LEVEL level,
  char *fmt,
  ...
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_trace_all disable/enable all traces

  @details
    m2mb_trace_all allows to disable/enable all class of traces

  @param[in] val
   is the value to enable/disable all trace

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_trace_all(int val);

#endif /* M2M_M2MB_TRACE_H */

