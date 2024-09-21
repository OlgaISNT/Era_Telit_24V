/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_os_tmr.h

  @brief
    OS timer prototypes

  @details
    basic functios for OS timer are provided

  @notes
    Dependencies:
             m2mb_types.h
             m2mb_os_types.h

  @author
    <Alessandro Papagno>

  @date
     03/03/2017
*/


#ifndef M2MB_OS_TMR_H
#define M2MB_OS_TMR_H




/* GLOBAL DECLARATIONS ----------------------------------------------------------- */
#define  M2MB_OS_TMR_INVALID            0
#define  M2MB_OS_TMR_INVALID_ATTR       0
#define  M2MB_OS_TMR_PERIODIC_TMR       1
#define  M2MB_OS_TMR_ONESHOT_TMR        0


/* Global declarations ==========================================================================*/


/*-----------------------------------------------------------------------------------------------

    can be called directly without specify CMDS_ARGS
    osRes = m2mb_os_tmr_setAttrItem_( &tmrAttrHandle,
                                      M2MB_OS_TMR_SEL_CMD_CREATE_ATTR,  NULL,
                                      M2MB_OS_TMR_SEL_CMD_NAME, "mytmr",
                                      M2MB_OS_TMR_SEL_CMD_USRNAME, "myUsrtmr",
                                      M2MB_OS_TMR_SEL_CMD_CB_FUNC, &timerCb,
                                      M2MB_OS_TMR_SEL_CMD_ARG_CB, &tmrHandle,
                                      M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, M2MB_OS_MS2TICKS(4000)
                                    );
  -----------------------------------------------------------------------------------------------*/
#ifdef _WIN32
#define m2mb_os_tmr_setAttrItem_( h, ... )        m2mb_os_tmr_setAttrItem( h, CMDS_ARGS( __VA_ARGS__ ) )
#else
#define m2mb_os_tmr_setAttrItem_( h, args... )        m2mb_os_tmr_setAttrItem( h, CMDS_ARGS( args ) )
#endif

/* Global typedefs --------------------------------------------------------------- */

/* available timer states */
typedef enum
{
  M2MB_TMR_STATE_TOSTART,
  M2MB_TMR_STATE_RUN,
  M2MB_TMR_STATE_STOP,
  M2MB_TMR_STATE_DELETED
} M2MB_TMR_STATE_E;

/* set or get commands ; not always all availbale */
typedef enum
{
  M2MB_OS_TMR_SEL_CMD_CREATE_ATTR,
  M2MB_OS_TMR_SEL_CMD_DEL_ATTR,
  M2MB_OS_TMR_SEL_CMD_NAME,
  M2MB_OS_TMR_SEL_CMD_USRNAME,
  /* the callback for timer elapsed */
  M2MB_OS_TMR_SEL_CMD_CB_FUNC,
  /* argument to be passed to callback */
  M2MB_OS_TMR_SEL_CMD_ARG_CB,
  /* timeout period in ticks */
  M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD,
  /* period of elapsing in ticks : max is 0xFFFF FFFF ticks */
  M2MB_OS_TMR_SEL_CMD_TICKS2ELAPSE,
  /* define if it is periodic or not */
  M2MB_OS_TMR_SEL_CMD_PERIODIC,
  /* autostart, not need to call m2mb_os_tmr_start */
  M2MB_OS_TMR_SEL_CMD_AUTOSTART,
  M2MB_OS_TMR_SEL_CMD_STATE,

  /* ---- */
  M2MB_OS_TMR_SEL_CMD_END,
  ENUM_TO_INT( M2MB_OS_TMR_SEL_CMD_E )
} M2MB_OS_TMR_SEL_CMD_E;


typedef struct M2MB_OS_TMR_HANDLE_TAG      *M2MB_OS_TMR_HANDLE;
typedef struct M2MB_OS_TMR_ATTR_HANDLE_TAG *M2MB_OS_TMR_ATTR_HANDLE;


typedef void ( *USR_TMR_CB )( M2MB_OS_TMR_HANDLE handle, void *arg );


/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Application timer Creation

  @details
    This service creates an application timer with the specified Timer attributes

  @param [out] pTmrHandle
    pointer to timer handle
  @param [in] pTmrAttrHandle
    pointer to timer attribute handle

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h )
  @return
    M2MB_OS_TICK_ERROR
     Invalid value (a zero) supplied for period in ticks in attribute
    M2MB_OS_TIMER_ERROR
     Invalid application timer handle: either the handle is invalid or the timer is already created
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    After successful timer creation, timer attribute will be deleted, their resources free
    and pointed by pTmrAttrHandle set to M2MB_OS_TMR_INVALID_ATTR
    In case of error return, it is application's responsibility to release resources of attributes
    by calling m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_DEL_ATTR,  NULL );
    Allowed From
      Initialization and tasks
    Preemption Possible
      No

  @b
    Example
    Complete use of Timer with its callback; callback stop the timer and restarts it with different
    period.
    Arg for callback is a context as pointer to timer handle, so in the callback the related timer
    can be managed

  @code
    M2MB_OS_TMR_ATTR_HANDLE tmrAttrHandle;
    M2MB_OS_TMR_HANDLE      tmrHandle;
    M2MB_OS_RESULT_E        osRes;

    void timerCb( M2MB_OS_TMR_HANDLE tmrHandle, void *ctx)
    {
      M2MB_OS_TMR_HANDLE handle;
      MEM_W currDuration;
      //here in this case ctx is address of  M2MB_OS_TMR_HANDLE tmrHandle, so it is a (M2MB_OS_TMR_HANDLE *)
      handle = *((M2MB_OS_TMR_HANDLE *)ctx);
      //or very simply, here just as an example. tmrHandle = ctx
      //  handle = tmrHandle;
      m2mb_os_tmr_stop(handle);
      m2mb_os_tmr_getItem( handle, M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, &currDuration, NULL );
      //changing duration
      currDuration += 100;
      if ( currDuration > 600 )
        currDuration = 10;
      //update duration: here use m2mb_os_tmr_setItem since tmr already created
      m2mb_os_tmr_setItem( handle, M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, (void*)currDuration );
      //restart timer with new value of ticks: if new value is not provided, it will
      // restart with previous ones
      m2mb_os_tmr_start(handle);
    }

    [...]
    if ( m2mb_os_tmr_setAttrItem( M2MB_OS_TMR_SEL_CMD_CREATE_ATTR, &tmrAttrHandle, NULL ) != M2MB_OS_SUCCESS )
             exit(error_creating attribute timer);

     // insert 1 cmd for call
     osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_CREATE_ATTR,  NULL);
     osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_NAME,  "mytmr");
     osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_USRNAME, "myUsrtmr");
     osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_CB_FUNC, &tmr_cb );
     osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_ARG_CB, handle );
     osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, M2MB_OS_MS2TICKS(4000) );
     osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_PERIODIC, M2MB_OS_TMR_PERIODIC_TMR );
     // or use for example CMDS_ARGS(...) macro
     // osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle,
     //                                  CMDS_ARGS
     //                                  (
     //                                    M2MB_OS_TMR_SEL_CMD_CREATE_ATTR,  NULL,
     //                                    M2MB_OS_TMR_SEL_CMD_NAME, "mytmr",
     //                                    M2MB_OS_TMR_SEL_CMD_USRNAME, "myUsrtmr",
     //                                    M2MB_OS_TMR_SEL_CMD_CB_FUNC, &timerCb,
     //                                    //use handle of the tsak as arg for callback
     //                                    M2MB_OS_TMR_SEL_CMD_ARG_CB, &tmrHandle,
     //                                    M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, M2MB_OS_MS2TICKS(4000)
     //                                  )
     //                                );


    if ( osRes != M2MB_OS_SUCCESS )
       exit( error setting or creating tmrAttrHandle)


    if ( m2mb_os_tmr_init( &tmrHandle, &tmrAttrHandle ) != M2MB_OS_SUCCESS )
    {
      //in case of error remember to manually remove attribute and release resources
      m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_DEL_ATTR, NULL );
    }
    m2mb_os_tmr_start(tmrHandle);


  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_tmr_init(
  M2MB_OS_TMR_HANDLE *pTmrHandle,
  M2MB_OS_TMR_ATTR_HANDLE *pTmrAttr
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Timer deinit => deletion

  @details
    This service deletes the specified os timer.
    The resources will be released.
    It is the application's responsibility to prevent use of a deleted
    timer.

  @param [in] tmrHandle
    handle of the timer

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_TIMER_ERROR
     Invalid timer handle
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

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
M2MB_OS_RESULT_E m2mb_os_tmr_deinit( M2MB_OS_TMR_HANDLE tmrHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Start application timer

  @details
    This service starts the specified application timer. The expiration
    routines of timers that expire at the same time are executed in the order
    they were activated.


  @param [in] tmrHandle
    handle of timer

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_TIMER_ERROR
     Invalid timer handle.
    M2MB_OS_ACTIVATE_ERROR
     Timer was already started
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.
    M2MB_OS_TICK_ERROR
      Invalid TICKS_PERIOD

  @note

    Allowed From
      Initialization, tasks, timers and ISRs
    Preemption Possible
      No



  @b
    Example
    For a complete example of use, see m2mb_os_tmr_init section
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_tmr_start( M2MB_OS_TMR_HANDLE tmrHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Stop an application timer

  @details
    This service stops the specified application timer. If the timer is
    already stopped, this service has no effect


  @param [in] tmrHandle
    handle of timer

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_TIMER_ERROR
     Invalid timer handle

  @note
    Allowed From
     Initialization, tasks, timers, and ISRs
    Preemption Possible
     No

  @b
    Example
    For a complete example of use, see m2mb_os_tmr_init section
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_tmr_stop( M2MB_OS_TMR_HANDLE tmrHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific attribute for timer creation

  @details
    This service set the attribute (or create it) to be used for timer creation

  @param [in] pTmrAttrHandle
   pointer to timer attribute handle
  @param [in] nCmds
    number of commands to be issued at once
  @param [in] ...
    variable inputs, N cmds (see M2MB_OS_TMR_SEL_CMD_E) followed by N arguments
    for their respective issued commands. CMDS(...) and CMDS_ARGS(...) macro can be
    used to better insert cmd and parameters and to better control right insertion

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_SUCCESS
     Successful timer operation.
    M2MB_OS_TIMER_ERROR
     Invalid timer handle
    M2MB_OS_INVALID_ARG
      invalid pTmrAttr or pIn for set operation

  @note
    The attribute set are useful for timer creation.
    Once created, use instead m2mb_os_tmr_setItem() to set only available timer properties
    It is under application responsibility the right use of m2mb_os_sem_setAttrItem.
    For instance, multi creation of attrHandle withouth free of previous resources
    ( M2MB_OS_TMR_SEL_CMD_CREATE_ATTR without M2MB_OS_TMR_SEL_CMD_DEL_ATTR in case
      no m2mb_os_tmr_init is used in the between ) will waste memory.

  @b
    Example
    See m2mb_os_tmr_init for complete example of usage
  @code
    M2MB_OS_RESULT_E osRes;
    M2MB_OS_TMR_ATTR_HANDLE  tmrAttrHandle;
    M2MB_OS_TMR_HANDLE       tmrHandle;

    // this is the callback for timer
    void tmr_cb( M2MB_OS_TMR_HANDLE tmrHandle, void *argv)
    {
      M2MB_OS_TASK handle = (M2MB_OS_TASK)(argv);
      //or simply handle = tmrHandle;
      m2mb_os_taskResume(handle);
      m2mb_os_tmr_stop(tmrHandle);
    }

    void entry( void *arg )
    {
      // for example it is an entry of already created task
      M2MB_OS_TASK handle = m2mb_os_taskGetId();
      handle = m2mb_os_taskGetId();
      if ( m2mb_os_tmr_setAttrItem( M2MB_OS_TMR_SEL_CMD_CREATE_ATTR, &tmrAttrHandle, NULL ) != M2MB_OS_SUCCESS )
          exit(error_creating attribute timer);

      // insert 1 cmd for call
      osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_CREATE_ATTR,  NULL);
      osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_NAME,  "mytmr");
      osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_USRNAME, "myUsrtmr");
      osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_CB_FUNC, &tmr_cb );
      osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_ARG_CB, handle );
      osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, M2MB_OS_MS2TICKS(4000) );
      osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_PERIODIC, M2MB_OS_TMR_PERIODIC_TMR );
      // or use for example CMDS_ARGS(...) macro
      // osRes = m2mb_os_tmr_setAttrItem( &tmrAttrHandle,
      //                                  CMDS_ARGS
      //                                  (
      //                                    M2MB_OS_TMR_SEL_CMD_CREATE_ATTR,  NULL,
      //                                    M2MB_OS_TMR_SEL_CMD_NAME, "mytmr",
      //                                    M2MB_OS_TMR_SEL_CMD_USRNAME, "myUsrtmr",
      //                                    M2MB_OS_TMR_SEL_CMD_CB_FUNC, &tmr_cb,
      //                                    //use handle of the tsak as arg for callback
      //                                    M2MB_OS_TMR_SEL_CMD_ARG_CB, handle,
      //                                    M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, M2MB_OS_MS2TICKS(4000),
      //                                  )
      //                                );

      if( osRes != M2MB_OS_SUCCESS )
        exit(error creating or setting attribute timer);
      if ( m2mb_os_tmr_init( &tmrHandle, &tmrAttrHandle ) != M2MB_OS_SUCCESS )
      {
        //in case of error remember to manually remove attribute and release resources
        m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_DEL_ATTR,  NULL );
      }
      while(1)
      {
        osRes = m2mb_os_tmr_start( tmrHandle );
        //suspend the task till timer expire
        printf( "\r\n task [ 0x%x ] suspended ", handle );
        m2mb_os_taskSuspend( handle );
        printf( "\r\n task [ 0x%x ] resumed by timer ", handle );
      }

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_tmr_setAttrItem(
  M2MB_OS_TMR_ATTR_HANDLE *pTmrAttrHandle,
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
    To be used when timer is already created: Right usage is under application responsibility

  @param [in] tmrHandle
    handle of timer
  @param [in] selCmd
   Select parameter to be set (see M2MB_OS_TMR_SEL_CMD_E)
  param [out] pOut
   pointer Output parameter got: its meaning depend on selCmd used See examples
   as general pointer it is set to MEM_W
  @param [in] pIn
   pointer to void, general Parameter to be specified, if any, to get right output See examples
   If parameter is not needed, use NULL

  @return
   M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_TIMER_ERROR
     Invalid timer handle
    M2MB_OS_INVALID_ARG
      invalid arguments

  @note
    Not all the M2MB_OS_TMR_SEL_CMD_E are always available.
    Of course M2MB_OS_TMR_SEL_CMD_CREATE_ATTR is not right for this function.
    Allowed:
    M2MB_OS_TMR_SEL_CMD_TICKS2ELAPSE
    M2MB_OS_TMR_SEL_CMD_NAME
    M2MB_OS_TMR_SEL_CMD_USRNAME
    M2MB_OS_TMR_SEL_CMD_AUTOSTART
    M2MB_OS_TMR_SEL_CMD_PERIODIC
    M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD
    M2MB_OS_TMR_SEL_CMD_ARG_CB
    M2MB_OS_TMR_SEL_CMD_CB_FUNC
    M2MB_OS_TMR_SEL_CMD_STATE



  @b
    Example
  @code
    M2MB_OS_TMR_ATTR_HANDLE tmrAttrHandle;
    M2MB_OS_TMR_HANDLE      tmrHandle;
    CHAR                    MyTmrName[10];
    CHAR                    *pTmrName;
    MEM_W                   currTickDuration;

    if ( m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1, M2MB_OS_TMR_SEL_CMD_CREATE_ATTR,  NULL ) != M2MB_OS_SUCCESS )
        exit(error_creating attribute timer);
    //set th inherit
    m2mb_os_tmr_setAttrItem( &tmrAttrHandle, 1 M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, 100 );
    // ...

    //create
    m2mb_os_tmr_init( &tmrHandle, &tmrAttrHandle );

    //

    m2mb_os_tmr_getItem( tmrHandle, M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, &currTickDuration, NULL );

    //copy only the first 10 chars to MyTaskName array of chars
    m2mb_os_taskGetItem(tmrHandle, M2MB_OS_TMR_SEL_CMD_USRNAME, &MyTmrName, 10);
    // or simply get the pointer of name, with NULL
    m2mb_os_taskGetItem(tmrHandle, M2MB_OS_TMR_SEL_CMD_USRNAME,&pTmrName, NULL);

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_tmr_getItem(
  M2MB_OS_TMR_HANDLE tmrHandle,
  M2MB_OS_TMR_SEL_CMD_E selCmd,
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

  @param [in] tmrHandle
    handle of timer
  @param [in] selCmd
   Select parameter to be set (see M2MB_OS_TMR_SEL_CMD_E)
  @param [in] pIn
    pointer to value to be set. If a simple value, can be easily casted to (void*)
    e.g. int value -3 has to be passed, use (void*)-3
    NULL if no value is needed

  @return
   M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_TIMER_ERROR
     Invalid timer handle
    M2MB_OS_INVALID_ARG
      invalid arguments

  @note
    Not all the M2MB_OS_TMR_SEL_CMD_E are always available.
    Of course M2MB_OS_TMR_SEL_CMD_CREATE_ATTR is not right for this function
    Allowed:
    M2MB_OS_TMR_SEL_CMD_USRNAME
    and ( if timer is not running ) the following:
    M2MB_OS_TMR_SEL_CMD_CB_FUNC
    M2MB_OS_TMR_SEL_CMD_ARG_CB
    M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD
    M2MB_OS_TMR_SEL_CMD_PERIODIC

    It is the application's responsibility to set the values in right context, for example
    the change of callback or periodicity should better be done inside expiration callback,
    or by stopping involved timer first

  @b
    Example
    Usage of the function inside expiration callback
  @code
    void timerCb( M2MB_OS_TMR_HANDLE tmrHandle, void *ctx)
    {
      M2MB_OS_TMR_HANDLE handle;
      MEM_W currDuration;
      handle = *((M2MB_OS_TMR_HANDLE*)ctx);
      // or simply handle = tmrHandle;
      m2mb_os_tmr_stop(handle);
      m2mb_os_tmr_getItem( handle, M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, &currDuration, NULL );
      currDuration += 100;
      if ( currDuration > 600 )
        currDuration = 10;
      //update duration through m2mb_os_tmr_setItem use
      m2mb_os_tmr_setItem( handle, M2MB_OS_TMR_SEL_CMD_TICKS_PERIOD, (void*)currDuration );
      //restart timer with new value of ticks: if new value is not provided, it will
      // restart with previous ones
      m2mb_os_tmr_start(handle);
    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_tmr_setItem(
  M2MB_OS_TMR_HANDLE tmrHandle,
  M2MB_OS_TMR_SEL_CMD_E selCmd,
  void *pIn
);

#endif /* M2MB_OS_TMR_H */
