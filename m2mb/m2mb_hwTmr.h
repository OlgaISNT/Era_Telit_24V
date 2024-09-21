/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_hwTmr.h

  @brief
   HW timer prototypes

  @details
    Basic functions for HW timer are provided

  @notes
    Dependencies:
        m2mb_types.h

  @author
    Goran Paic

  @date
    22/01/2018
*/

#ifndef M2MB_HW_TMR_H
#define M2MB_HW_TMR_H

/* Include ======================================================================================*/

/* Global declarations ==========================================================================*/
#define  M2MB_HWTMR_INVALID            0
#define  M2MB_HWTMR_INVALID_ATTR       0

#define  M2MB_HWTMR_PERIODIC_TMR       1
#define  M2MB_HWTMR_ONESHOT_TMR        0


#define  M2MB_HWTMR_AUTOSTART         1
#define  M2MB_HWTMR_NOT_START         0

/* for backward compatibility : use M2MB_HWTMR_AUTOSTART */
#define  M2MB_HW_TASK_AUTOSTART       1  /* use M2MB_HWTMR_AUTOSTART instead */
#define  M2MB_HW_TASK_NOT_START       0  /* use M2MB_HWTMR_NOT_START instead */

/* minimum period allowed: less than this values is changed in the minimum to avoid
   system issue */
#define M2MB_HWTMR_MIN_TIMEOUT ( 100 )      /* minimum allowed timeout in us */

/* maximum allowed timeout in us is related to MEmory Wide bus : e.g 32 bit -> 0xFFFFFFFF */
#define M2MB_HWTMR_MAX_TIMEOUT ( ( 1<<(sizeof(MEM_W)*4 - 1) ) | ~( 1<<(sizeof(MEM_W)*4 - 1) )  )


#define M2MB_HWTMR_UNIT_FACTOR        ( 1000 )  /* is ms to us: current base time in us */
#define M2MB_HWTMR_TIME_MS(timems)    ( timems * M2MB_HWTMR_UNIT_FACTOR )
#define M2MB_HWTMR_TIME_S(times)      M2MB_HWTMR_TIME_MS(times * 1000 )


#ifdef _WIN32
#define m2mb_hwTmr_setAttrItem_( h, ... )    m2mb_hwTmr_setAttrItem( h, CMDS_ARGS( __VA_ARGS__ ) )
#else
#define m2mb_hwTmr_setAttrItem_( h, args... )    m2mb_hwTmr_setAttrItem( h, CMDS_ARGS( args ) )
#endif

/* Global typedefs ==============================================================================*/

/* every enum has been rename to M2MB_HWTMR_xxx in bacward compatibility mode
   so M2MB_HW_xxx  or M2MB_HWTMR_xxx can be used in the enum M2MB_HWTMR_RESULT_E
*/
#define enum_rename( a )           M2MB_HW_##a, M2MB_HWTMR_##a = M2MB_HW_##a
#define enum_rename_set( a, val )  M2MB_HW_##a = val, M2MB_HWTMR_##a = M2MB_HW_##a

/* error for HW TIMER module */
typedef enum M2MB_HWTMR_RESULT
{
  /* M2MB_HWTMR_xxx is the same of M2MB_HWxx_ and they can be interchanged */

  /* M2MB_HWTMR_SUCCESS                       */ enum_rename_set( SUCCESS, 0 ),
  /* M2MB_HWTMR_DELETED,                      */ enum_rename( DELETED ),
  /* M2MB_HWTMR_PTR_ERROR,                    */ enum_rename( PTR_ERROR ),
  /* M2MB_HWTMR_NO_MEMORY,                    */ enum_rename( NO_MEMORY ),
  /* M2MB_HWTMR_TIMER_ERROR,                  */ enum_rename( TIMER_ERROR ),
  /* M2MB_HWTMR_TICK_ERROR,                   */ enum_rename( TICK_ERROR ),
  /* M2MB_HWTMR_START_ERROR,                  */ enum_rename( START_ERROR ),
  /* M2MB_HWTMR_STOP_ERROR,                   */ enum_rename( STOP_ERROR ),
  /* M2MB_HWTMR_TICK_ERROR_TIMER_RUN,         */ enum_rename( TICK_ERROR_TIMER_RUN ),
  /* M2MB_HWTMR_PERIOD_ERROR_TIMER_RUN,       */ enum_rename( PERIOD_ERROR_TIMER_RUN ),
  /* M2MB_HWTMR_CB_ERROR_TIMER_RUN,           */ enum_rename( CB_ERROR_TIMER_RUN ),
  /* M2MB_HWTMR_ARG_ERROR_TIMER_RUN,          */ enum_rename( ARG_ERROR_TIMER_RUN ),
  /* M2MB_HWTMR_TIME_DURATION_ERROR_TIMER_RUN,*/ enum_rename( TIME_DURATION_ERROR_TIMER_RUN ),
  /* M2MB_HWTMR_GENERIC_ERROR,                */ enum_rename( GENERIC_ERROR ),
  /* M2MB_HWTMR_INVALID_ARG,                  */ enum_rename( INVALID_ARG ),
  /* M2MB_HWTMR_ERROR,                        */ enum_rename( ERROR ),
  /* M2MB_HWTMR_ERROR_INVALID_ARG,            */ enum_rename( ERROR_INVALID_ARG ),
  /* M2MB_HWTMR_ERR_INVALID_PARAM,            */ enum_rename( ERR_INVALID_PARAM ),
  /* M2MB_HWTMR_ERR_NO_MEMORY,                */ enum_rename( ERR_NO_MEMORY ),
  /* M2MB_HWTMR_ERR_NO_RESOURCE,              */ enum_rename( ERR_NO_RESOURCE ),
  /* M2MB_HWTMR_ERR_BUSY,                     */ enum_rename( ERR_BUSY ),
  /* M2MB_HWTMR_ERR_NO_ENTRY,                 */ enum_rename( ERR_NO_ENTRY ),
  /* M2MB_HWTMR_ERR_NOT_SUPPORTED,            */ enum_rename( ERR_NOT_SUPPORTED ),
  /* M2MB_HWTMR_ERR_TIMEOUT,                  */ enum_rename( ERR_TIMEOUT ),
  /* M2MB_HWTMR_ERR_BOUNDS,                   */ enum_rename( ERR_BOUNDS ),
  /* M2MB_HWTMR_ERR_BAD_PAYLOAD,              */ enum_rename( ERR_BAD_PAYLOAD ),
  /* M2MB_HWTMR_ERR_EXISTS,                   */ enum_rename( ERR_EXISTS ),
  /* M2MB_HWTMR_ERR_NOT_STARTED,              */ enum_rename( ERR_NOT_STARTED ),
  /* M2MB_HWTMR_ERR_ALREADY_EXPIRED,          */ enum_rename( ERR_ALREADY_EXPIRED ),


  /* M2MB_HWTMR_ERR_UNKNOWN = 255,            */ enum_rename_set( ERR_UNKNOWN, 255 ),
  /* M2MB_HWTMR_END,                          */ enum_rename( END ),

  ENUM_TO_INT( M2MB_HWTMR_RESULT_E )
} M2MB_HWTMR_RESULT_E;


/* availbale timer states */
typedef enum
{
  M2MB_HWTMR_STATE_TOSTART,
  M2MB_HWTMR_STATE_RUN,
  M2MB_HWTMR_STATE_STOP,
  M2MB_HWTMR_STATE_DELETED
} M2MB_HWTMR_STATE_E;

/* set or get commands ; not always all availbale */
typedef enum
{
  M2MB_HWTMR_SEL_CMD_CREATE_ATTR,
  M2MB_HWTMR_SEL_CMD_DEL_ATTR,
  /* name and usrname attribute NOT supported: backward compatibility */
  M2MB_HWTMR_SEL_CMD_NAME_ATTR_NO_SUPPORTED,
  M2MB_HWTMR_SEL_CMD_USRNAME_ATTR_NO_SUPPORTED,
  M2MB_HWTMR_SEL_CMD_CB_FUNC,
  /* argument to be passed to callback */
  M2MB_HWTMR_SEL_CMD_ARG_CB,
  /* period of the timer duration expressed in system ticks
     In any case the corresponding limit is M2MB_HWTMR_MAX_TIMEOUT in us
     So upper limit is M2MB_HWTMR_MAX_TIMEOUT[us]/SystemTimeTick[us]
  */
  M2MB_HWTMR_SEL_CMD_TICKS_PERIOD,
  /* FFU: not supported period of elapsing in ticks */
  M2MB_HWTMR_SEL_CMD_TICKS2ELAPSE,
  /* period of the timer duration in us units */
  M2MB_HWTMR_SEL_CMD_TIME_DURATION,
  /* define if it is periodic or not */
  M2MB_HWTMR_SEL_CMD_PERIODIC,
  /* not need to call m2mb_hwTmr_start */
  M2MB_HWTMR_SEL_CMD_AUTOSTART,
  M2MB_HWTMR_SEL_CMD_STATE,

  /* ---- */
  M2MB_HWTMR_SEL_CMD_END,
  ENUM_TO_INT( M2MB_HWTMR_SEL_CMD_E )
} M2MB_HWTMR_SEL_CMD_E;


typedef struct M2MB_HWTMR_HANDLE_TAG *M2MB_HWTMR_HANDLE;
typedef struct M2MB_HWTMR_ATTR_HANDLE_TAG *M2MB_HWTMR_ATTR_HANDLE;

typedef void ( *USR_HWTMR_CB )( M2MB_HWTMR_HANDLE handle, void *arg );

/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Application timer Creation

  @details
    This service creates an application timer with the specified Timer attributes

  @param [out] pTmrHwHandle
    pointer to timer handle
  @param [in] pTmrHwAttr
    pointer to timer attribute handle

  @return
    M2MB_HWTMR_SUCCESS in case of success
    see M2MB_HWTMR_RESULT_E in case of error

  @note
    After successful timer creation, timer attributes will be deleted, their resources free
    and pointed by pTmrHwAttr set to M2MB_HWTMR_INVALID_ATTR
    Also in case of error return, timer attribute will be deleted, their resources free
    and pointed by pTmrHwAttr set to M2MB_HWTMR_INVALID_ATTR
    Allowed From
      Initialization and tasks
    Preemption Possible
      No

  @b
    Example
    Complete use of Timer with its callback, the time unit in USEC.
    Also ticks period M2MB_HWTMR_SEL_CMD_TICKS_PERIOD can be select,
    which is the period expressed in number of system ticks
    Period MUST BE not 0

  @code
    static void TimerCb( M2MB_HWTMR_HANDLE hwTmrHandle, void *argCb )
    {
      static unsigned int Count = 0;
      // here just as example for debug: using print inside callback
      // can drive to system crash or hung : is not guaranteed
      // avoid using print
      PrintToUart("\r\nCallback Count: [%u]", Count++);
    }

    void M2MB_main( int argc, char **argv )
    {
      M2MB_HWTMR_RESULT_E        hwRes;
      M2MB_HWTMR_ATTR_HANDLE     tmrHwAttrHandle;
      M2MB_HWTMR_HANDLE          tmrHwHandle;
      UINT32                     timeDuration;
      MEM_W                      argCb;

      if( m2mb_hwTmr_setAttrItem( &tmrHwAttrHandle, 1, M2MB_HWTMR_SEL_CMD_CREATE_ATTR, NULL ) != M2MB_HWTMR_SUCCESS )
      {
        PrintToUart("\r\nCreate the timer attributes, error %d", hwRes);
      }
      hwRes = m2mb_hwTmr_setAttrItem( &tmrHwAttrHandle,
                                     CMDS_ARGS(
                                               M2MB_HWTMR_SEL_CMD_CB_FUNC, &TimerCb,
                                               M2MB_HWTMR_SEL_CMD_ARG_CB, argCb,
                                               M2MB_HWTMR_SEL_CMD_TIME_DURATION, 1000, //or M2MB_HWTMR_TIME_MS(1)
                                               M2MB_HWTMR_SEL_CMD_PERIODIC, M2MB_HWTMR_PERIODIC_TMR,
                                               M2MB_HWTMR_SEL_CMD_AUTOSTART, M2MB_HWTMR_AUTOSTART
                                            )
                                   );

      // if M2MB_HWTMR_SEL_CMD_TICKS_PERIOD is used instead of M2MB_HWTMR_SEL_CMD_TIME_DURATION
      // at the end no more than M2MB_HWTMR_MAX_TIMEOUT in us can be get as period
      // so the limit is M2MB_HWTMR_MAX_TIMEOUT[us]/SystemTimeTick[us]

      if( hwRes != M2MB_HWTMR_SUCCESS )
      {
        PrintToUart("\r\nCreate the timer attributes, error %d", hwRes);
      }
      else
      {
        PrintToUart( "\r\nCreate the timer attributes" );
      }
      //Define the timer
      hwRes = m2mb_hwTmr_init( &tmrHwHandle, &tmrHwAttrHandle );

      if( hwRes != M2MB_HWTMR_SUCCESS )
      {
        m2mb_hwTmr_setAttrItem( &tmrHwAttrHandle, 1, M2MB_HWTMR_SEL_CMD_DEL_ATTR, NULL );
        PrintToUart("\r\nDefine the timer, error %d", hwRes);
      }
      else
      {
        PrintToUart( "\r\nDefine the timer" );
      }

      //Start the timer in case no M2MB_HWTMR_AUTOSTART attribute was used
      hwRes = m2mb_hwTmr_start( tmrHwHandle );

      if( hwRes != M2MB_HWTMR_SUCCESS )
      {
        PrintToUart("\r\nStart the timer, error %d", hwRes);
      }
      else
      {
        PrintToUart( "\r\nStart the timer" );
      }

      //Stop a running the timer
      hwRes = m2mb_hwTmr_stop( tmrHwHandle );
      if( hwRes != M2MB_HWTMR_SUCCESS )
      {
        PrintToUart("\r\nStopa running the timer, error %d", hwRes);
      }
      else
      {
        PrintToUart("\r\nStopa running the timer" );
      }

      timeDuration++;

      //Set attribute [time duration] of the timer
      hwRes = m2mb_hwTmr_setItem( tmrHwHandle, M2MB_HWTMR_SEL_CMD_TIME_DURATION, ( void * )timeDuration );
      if( hwRes != M2MB_HWTMR_SUCCESS )
      {
        PrintToUart("\r\nSet attribute [time duration] of the timer, error %d", hwRes);
      }
      else
      {
        PrintToUart("\r\nSet attribute [time duration] of the timer" );
      }

      //Undef the timer
      m2mb_hwTmr_deinit( tmrHwHandle );
    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_HWTMR_RESULT_E m2mb_hwTmr_init( M2MB_HWTMR_HANDLE *pTmrHwHandle,
                                     M2MB_HWTMR_ATTR_HANDLE *pTmrHwAttr
                                   );
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Timer deinit => deletion

  @details
    This service deletes the specified hw timer.
    The resources will be released.
    It is the application responsibility to prevent use of a deleted
    timer.

  @param [in] tmrHwHandle
    handle of the timer

  @return
    M2MB_HWTMR_SUCCESS in case of success

  @note
    It is the application's responsibility to prevent use of a deleted
    timer.
    Allowed From
     tasks
    Preemption Possible
     No

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_HWTMR_RESULT_E m2mb_hwTmr_deinit( M2MB_HWTMR_HANDLE tmrHwHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Start application timer

  @details
    This service starts the specified application timer. The expiration
    routines of timers that expire at the same time are executed in the order
    they were activated.


  @param [in] tmrHwHandle
    handle of timer

  @return
    M2MB_HWTMR_SUCCESS in case of success

  @note
    Start is allowed only if timeout is grater than 0

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_HWTMR_RESULT_E m2mb_hwTmr_start( M2MB_HWTMR_HANDLE tmrHwHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Stop an application timer

  @details
    This service stops the specified application timer. If the timer is
    already stopped, this service has no effect


  @param [in] tmrHwHandle
    handle of timer

  @return
    M2MB_HWTMR_SUCCESS in case of success

  @note


  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_HWTMR_RESULT_E m2mb_hwTmr_stop( M2MB_HWTMR_HANDLE tmrHwHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific attribute for timer creation

  @details
    This service set the attribute (or create it) to be used for timer creation

  @param [in] pTmrHwAttrHandle
   pointer to timer attribute
  @param [in] nCmds
    number of commands to be issued at once
  @param [in] ...
    variable inputs, N cmds (see M2MB_HWTMR_SEL_CMD_E) followed by N arguments
    for their respective issued commands. CMDS(...) and CMDS_ARGS(...) macro can be
    used to better insert cmd and parameters and to better control right insertion

  @return
    M2MB_HWTMR_SUCCESS in case of success


  @note
    The attribute set are useful for timer creation.
    Once created, use instead m2mb_hwTmr_setItem() to set only available timer properties
    It is under application responsibility the right use of m2mb_hwTmr_setAttrItem.

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_HWTMR_RESULT_E m2mb_hwTmr_setAttrItem( M2MB_HWTMR_ATTR_HANDLE *pTmrHwAttrHandle,
    UINT8 nCmds,
    ...
                                          );
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get specific properties of the timer

  @details
    This service retrieve value for specified parameter of the specified timer
    Selecting cmd, a parameter to be retrieved can be set
    To be used when timer is already created: Right usage is under application's responsibility

  @param [in] tmrHwHandle
    handle of timer
  @param [in] selCmd
   Select parameter to be set (see M2MB_HWTMR_SEL_CMD_E)
  param [out] pOut
   pointer Output parameter got: its meaning depend on selCmd used See examples
   as general pointer it is set to MEM_W
  @param [in] pIn
   pointer to void, general Parameter to be specified, if any, to get right output See examples
   If parameter is not needed, use NULL

  @return
   M2MB_OS_SUCCESS in case of success

  @note
    Not all M2MB_HWTMR_SEL_CMD_E are allowed.
    Allowed:
    M2MB_HWTMR_SEL_CMD_AUTOSTART
    M2MB_HWTMR_SEL_CMD_PERIODIC
    M2MB_HWTMR_SEL_CMD_TIME_DURATION
    M2MB_HWTMR_SEL_CMD_TICKS_PERIOD
    M2MB_HWTMR_SEL_CMD_ARG_CB
    M2MB_HWTMR_SEL_CMD_CB_FUNC
    M2MB_HWTMR_SEL_CMD_STATE

  @b
    Example
  @code


  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_HWTMR_RESULT_E m2mb_hwTmr_getItem( M2MB_HWTMR_HANDLE tmrHwHandle,
                                        M2MB_HWTMR_SEL_CMD_E selCmd,
                                        MEM_W *pOut,
                                        void *pIn
                                      );
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific properties of the timer

  @details
    This service set value for specified parameter of the specified timer if available
    Selecting cmd, a parameter to be set can be selected
    To be used when timer is already created: its right usage is under application responsibility

  @param [in] tmrHwHandle
    handle of timer
  @param [in] selCmd
   Select parameter to be set (see M2MB_HWTMR_SEL_CMD_E)
  @param [in] pIn

  @return
   M2MB_OS_SUCCESS in case of success


  @note
    It is the application's responsibility to set the values in right context, for example
    the change of callback or periodicity should better be done inside expiration callback,
    or by stopping involved timer first
    Not all M2MB_HWTMR_SEL_CMD_E are allowed: none if timer is not running
    e.g
    Allowed only if timer is not running:
    M2MB_HWTMR_SEL_CMD_CB_FUNC
    M2MB_HWTMR_SEL_CMD_ARG_CB
    M2MB_HWTMR_SEL_CMD_TICKS_PERIOD
    M2MB_HWTMR_SEL_CMD_TIME_DURATION
    M2MB_HWTMR_SEL_CMD_PERIODIC

  @b
    Example

  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_HWTMR_RESULT_E m2mb_hwTmr_setItem( M2MB_HWTMR_HANDLE tmrHwHandle,
                                        M2MB_HWTMR_SEL_CMD_E selCmd,
                                        void *pIn
                                      );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    get time in ms

  @details
    Get a timestamp from System Time in millisecond units from 6 Jan 1980
    00:00:00.

  @return
   M2MB_OS_SUCCESS in case of success

  @b
    Example

  @code
  @endcode
*/


M2MB_HWTMR_RESULT_E m2mb_hwTmr_timeGet_ms( UINT64 *ms_time );


/* Global define ================================================================================*/

#endif /* M2M_HW_TMR_H */

