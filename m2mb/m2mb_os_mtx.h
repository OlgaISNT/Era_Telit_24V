/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_os_mtxHandle.h

  @brief
    OS mutex prototypes

  @details
    basic functions for OS mutex are provided

  @notes
    Dependencies:
             m2mb_types.h
             m2mb_os_types.h

  @author
    <Alessandro Papagno>

  @date
     03/03/2017
*/


#ifndef M2MB_OS_MTX_H
#define M2MB_OS_MTX_H


/* GLOBAL DECLARATIONS ----------------------------------------------------------- */
#define  M2MB_OS_MTX_INVALID          0
#define  M2MB_OS_MTX_INVALID_ATTR     0

/* Global declarations ==========================================================================*/

/*-----------------------------------------------------------------------------------------------

    can be called directly without specify CMDS_ARGS
    osRes = m2mb_os_mtx_setAttrItem_( &mtxAttrHandle,
                                      M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
                                      M2MB_OS_MTX_SEL_CMD_NAME, "myMtx1",
                                      M2MB_OS_MTX_SEL_CMD_USRNAME, "myUserMtx1",
                                      M2MB_OS_MTX_SEL_CMD_INHERIT, inheritVal
                                    );
  -----------------------------------------------------------------------------------------------*/

#ifdef _WIN32
#define m2mb_os_mtx_setAttrItem_( h, ... )        m2mb_os_mtx_setAttrItem( h, CMDS_ARGS( __VA_ARGS__ ) )
#else
#define m2mb_os_mtx_setAttrItem_( h, args... )        m2mb_os_mtx_setAttrItem( h, CMDS_ARGS( args ) )
#endif


/* Global typedefs --------------------------------------------------------------- */

typedef enum
{
  M2MB_OS_MTX_SEL_CMD_CREATE_ATTR,
  M2MB_OS_MTX_SEL_CMD_DEL_ATTR,
  /* use as Os name during creation */
  M2MB_OS_MTX_SEL_CMD_NAME,
  /* if set, usr name could be different from Os during creation */
  M2MB_OS_MTX_SEL_CMD_USRNAME,
  /* select if priority inheritance will be active for it. To avoid priority inversion, priority of the task
     owning the mutex will be temporarily raised to the task of the highest priority waiting for the mutex */
  M2MB_OS_MTX_SEL_CMD_INHERIT,
  /* high priority task first option cmd not supported: here only for backward compatibility
     see function tx_hptf_com(...) for the desired functionality */
  M2MB_OS_MTX_SEL_CMD_HPTF_NO_SUPPORT,
  M2MB_OS_MTX_SEL_CMD_COUNT,
  /* number of task suspended on this resource */
  M2MB_OS_MTX_SEL_CMD_SUSPENDED_COUNT,
  /* first task (M2MB_OS_TASK_HANDLE) to be served when resource will be available */
  M2MB_OS_MTX_SEL_CMD_1ST_SUSP,
  /* owner (M2MB_OS_TASK_HANDLE) of the resource mtxHandle */
  M2MB_OS_MTX_SEL_CMD_OWNER,

  /* ---- */
  M2MB_OS_MTX_SEL_CMD_END,
  ENUM_TO_INT( M2MB_OS_MTX_SEL_CMD_E )
} M2MB_OS_MTX_SEL_CMD_E;

typedef struct M2MB_OS_MTX_HANDLE_TAG      *M2MB_OS_MTX_HANDLE;
typedef struct M2MB_OS_MTX_ATTR_HANDLE_TAG *M2MB_OS_MTX_ATTR_HANDLE;



/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Mutex Creation

  @details
    This service creates a mutex for inter-task mutual exclusion for
    resource protection

  @param [out] pMtxHandle
    pointer to mutex handle
  @param [in] pMtxAttr
    pointer to mutex attribute handle

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h )
  @return
    M2MB_OS_MUTEX_ERROR
     Invalid counting mutex handle
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.
    M2MB_OS_INHERIT_ERROR
      Invalid Inherit parameter in attribute

  @note
    After successfull mutex creation, mutex attribute will be deleted, their resources free
    and pMtxAttr set to NULL
    Allowed From
      Initialization and tasks
    Preemption Possible
      No

  @b
    Example
  @code
    M2MB_OS_RESULT_E        osRes;
    M2MB_OS_MTX_ATTR_HANDLE mtxAttrHandle;
    M2MB_OS_MTX_HANDLE      mtxHandle;
    UINT32                  inheritVal = 1;

   // mutex will support inheritance, while with inheritVal=0, it will not
   osRes = m2mb_os_mtx_setAttrItem( &mtxAttrHandle,
                                        CMDS_ARGS(
                                          M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
                                          M2MB_OS_MTX_SEL_CMD_NAME, "myMtx1",
                                          M2MB_OS_MTX_SEL_CMD_USRNAME, "myUserMtx1",
                                          M2MB_OS_MTX_SEL_CMD_INHERIT, inheritVal
                                        )
                                  );
  if( osRes != M2MB_OS_SUCCESS )
    exit( ... )

  m2mb_os_mtx_init( &mtxHandle, &mtxAttrHandle);
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_mtx_init(
  M2MB_OS_MTX_HANDLE *pMtxHandle,
  M2MB_OS_MTX_ATTR_HANDLE *pMtxAttr
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Mutex deinit or deletion

  @details
    This service deletes the specified os mutex. All tasks
    suspended waiting for this mutex are resumed with a @return value egual to M2MB_OS_DELETED
    The resources will be released.
    It is the application's responsibility to prevent use of a deleted
    mutex.

  @param [in] pmTX
    pointer handle of the mutex

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_MUTEX_ERROR
     Invalid counting mutex handle
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    It is the application's responsibility to prevent use of a deleted
    mutex.
    Allowed From
     tasks
    Preemption Possible
     Yes

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_mtx_deinit( M2MB_OS_MTX_HANDLE mtxHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
     Get the ownership of the mutex (critical session enter)

  @details
    This service attempts to obtain exclusive ownership of the specified
    mutex. If the calling task already owns the mutex, an internal counter is
    incremented and a successful status is returned.
    If the mutex is owned by another task and this task has higher priority
    and priority inheritance was specified at mutex create, the lower
    task'priority of the task owning the mutex will be temporarily raised
    to that of the calling task.


  @param [in] mtxHandle
    handle of mutex
  @param [in] timeoutTicks
    If timeout is specified and mutex count is zero, task will be suspended for a maximum
    time of timeout ticks

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_SUCCESS
     Successful retrieval of a mutex instance.
    M2MB_OS_DELETED
     Counting mutex was deleted while task was suspended.
    M2MB_OS_NOT_AVAILABLE
     Service was unable to get ownership of the mutex within the specified time to wait
    M2MB_OS_WAIT_ABORTED
     Suspension was aborted by another task, timer, or ISR.
    M2MB_OS_MUTEX_ERROR
     Invalid counting mutex handle.
    M2MB_OS_WAIT_ERROR
     A wait option other than M2MB_OS_NO_WAIT was specified on a call from a non-task.
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    The priority of the lower priority task owning a mutex with priority-
    inheritance should never be modified by an external task during mutex
    ownership.
    Allowed From
      Initialization, tasks, timers
    Preemption Possible
      Yes

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_mtx_get( M2MB_OS_MTX_HANDLE mtxHandle, UINT32 timeoutTicks );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Release ownership of the mutex (if counter = 0: exit critical section)

  @details
    This service decrements the ownership count of the specified mutex. If
    the ownership count is zero, the mutex is made available.


  @param [in] mtxHandle
    handle of mutex

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_SUCCESS
     Successful mutex put.
    M2MB_OS_MUTEX_ERROR
     Invalid mutex handle
    M2MB_OS_CALLER_ERROR
     Invalid caller of this service.
    M2MB_OS_NOT_OWNED
     Mutex is not owned by caller.

  @note
    If priority inheritance was selected during mutex creation, the priority of
    the releasing task will be restored to the priority it had when it originally
    obtained ownership of the mutex. Any other priority changes made to the
    releasing task during ownership of the mutex may be undone
    Allowed From
     Initialization, tasks
    Preemption Possible
     Yes

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_mtx_put( M2MB_OS_MTX_HANDLE mtxHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Prioritize mutex suspension list
    HPTF: Highest Priority Task First

  @details
    This service set the task waiting for this mutex which has currently highest priority,
    on the top of FIFO task suspension list
    It is one shot operation.
    To have highest priority task served first for every put, see m2m_os_mtx_setItem(..)

  @param [in] mtxHandle
    handle of mutex

  @return
    M2MB_OS_FEATURE_NOT_SUPPORTED always

  @note    
    This function is implemented as dummy function and always return M2MB_OS_FEATURE_NOT_SUPPORTED
    for the compatibility with other products

  @b
    Example
  @code
    M2MB_OS_MTX_HANDLE mtxHandle;
    //...
    //already created... used mutex
    //just put highest priority task on top of suspension list
    m2mb_os_mtx_hptf( mtxHandle );

    m2mb_os_mtx_put( mtxHandle );
    //will give control to highest priority task waiting...

    //...

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_mtx_hptf( M2MB_OS_MTX_HANDLE mtxHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific attribute for mutex creation

  @details
    This service set the attribute (or create it) to be used for mutex creation

  @param [in] selCmd
   Select parameter to be set (see M2MB_OS_MTX_SEL_CMD_E)
  @param [in] pMtxAttr
   pointer to mutex attribute of mutex
  @param [in] ...
    variable inputs, N cmds (see M2MB_OS_MTX_SEL_CMD_E) followed by N arguments
    for their respective issued commands. CMDS(...) and CMDS_ARGS(...) macro can be
    used to better insert cmd and parameters and to better control right insertion

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_SUCCESS
     Successful mutex operation.
    M2MB_OS_MUTEX_ERROR
     Invalid counting mutex handle
    M2MB_OS_INVALID_ARG
      invalid pMtxAttr or pIn

  @note
    The attribute set are useful for mutex creation.
    Once created, use instead m2mb_os_mtx_setItem() to set only available mutex properties
    After m2mb_os_mtx_init, resources of M2MB_OS_MTX_ATTR_HANDLE will be released and handle set to
    M2MB_OS_MTX_INVALID_ATTR
    It is under application responsibility the right use of m2mb_os_mtx_setAttrItem.
    For instance, multi creation of attrHandle withouth free of previous resources
    ( M2MB_OS_MTX_SEL_CMD_CREATE_ATTR without M2MB_OS_MTX_SEL_CMD_DEL_ATTR in case
      no m2mb_os_mtx_init is used in the between ) will waste memory.

  @b
    Example
  @code
    M2MB_OS_RESULT_E        osRes;
    M2MB_OS_MTX_ATTR_HANDLE mtxAttrHandle;
    M2MB_OS_MTX_HANDLE      mtxHandle;
    UINT32                  inheritVal = 1;

    osRes = m2mb_os_mtx_setAttrItem( &mtxAttrHandle,
                                         CMDS_ARGS(
                                           M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
                                           M2MB_OS_Q_SEL_CMD_NAME, "myMtx1",
                                           M2MB_OS_Q_SEL_CMD_USRNAME, "myUserMtx1",
                                           M2MB_OS_MTX_SEL_CMD_INHERIT, inheritVal
                                         )
                                   );
    if( osRes != M2MB_OS_SUCCESS )
      exit( ... )

    m2mb_os_mtx_init( &mtxHandle, &mtxAttrHandle);
    [...]
    m2mb_os_mtx_deinit( &mtxHandle );


  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_mtx_setAttrItem(
  M2MB_OS_MTX_ATTR_HANDLE *pMtxAttrHandle,
  UINT8 nCmds,
  ...
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get specific properties of the mutex

  @details
    This service retrieve value for specified parameter of the specified mutex
    Selecting cmd, a parameter to be retrieved can be set
    To be used when mutex is already created: it is under application responsibility

  @param [in] mtxHandle
    handle of mutex
  @param [in] selCmd
   Select parameter to be retrieved (see M2MB_OS_MTX_SEL_CMD_E)
  @param [out] pOut
   pointer Output parameter got: its meaning depend on selCmd used See examples
   as general pointer it is set to MEM_W
  @param [in] pIn
   pointer to void general Parameter to be specified, if any, to get right output See examples
   If parameter is not needed, use NULL

  @return
   M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_MUTEX_ERROR
     Invalid counting mutex handle
    M2MB_OS_INVALID_ARG
      invalid arguments

  @note
    Not all the M2MB_OS_MTX_SEL_CMD_E are always available.
    Of course M2MB_OS_MTX_SEL_CMD_CREATE_ATTR is not right for this function.
    Allowed
    M2MB_OS_MTX_SEL_CMD_NAME
    M2MB_OS_MTX_SEL_CMD_USRNAME
    M2MB_OS_MTX_SEL_CMD_1ST_SUSP
    M2MB_OS_MTX_SEL_CMD_OWNER
    M2MB_OS_MTX_SEL_CMD_SUSPENDED_COUNT
    M2MB_OS_MTX_SEL_CMD_COUNT
    M2MB_OS_MTX_SEL_CMD_INHERIT

  @b
    Example
  @code
    M2MB_OS_RESULT_E         osRes;
    M2MB_OS_MTX_ATTR_HANDLE  mtxAttrHandle;
    M2MB_OS_MTX_HANDLE       mtxHandle;
    UINT32                   inheritVal = 0;
    MEM_W                    currCount;
    osRes = m2mb_os_mtx_setAttrItem( &mtxAttrHandle,
                                         CMDS_ARGS(
                                           M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
                                           M2MB_OS_Q_SEL_CMD_NAME, "myMtx1",
                                           M2MB_OS_Q_SEL_CMD_USRNAME, "myUserMtx1",
                                           M2MB_OS_MTX_SEL_CMD_INHERIT, inheritVal
                                         )
                                   );
    if( osRes != M2MB_OS_SUCCESS )
      exit( ... )

    m2mb_os_mtx_getItem( mtxHandle, M2MB_OS_MTX_SEL_CMD_COUNT, &currCount, NULL );
    //you get currCount...  the same for
    // M2MB_OS_MTX_SEL_CMD_SUSPENDED_COUNT
    // and M2MB_OS_MTX_SEL_CMD_OWNER, M2MB_OS_MTX_SEL_CMD_1ST_SUSP
    // where the outputs will be task handle and so on
    //
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_mtx_getItem(
  M2MB_OS_MTX_HANDLE mtxHandle,
  M2MB_OS_MTX_SEL_CMD_E selCmd,
  MEM_W *pOut,
  void *pIn
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific properties of the mutex

  @details
    This service set value for specified parameter of the specified mutex if available
    Selecting cmd, a parameter to be set can be selected
    To be used when mutex is already created: its right use is under application responsibility

  @param [in] mtxHandle
    handle of mutex
  @param [in] selCmd
   Select parameter to be set (see M2MB_OS_MTX_SEL_CMD_E)
  @param [in] pIn
    pointer to value to be set. If a simple value, can be easily casted to (void*)
    e.g. int value -3 has to be passed, use (void*)-3
    NULL if no value is needed

  @return
   M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_MUTEX_ERROR
     Invalid counting mutex handle
    M2MB_OS_INVALID_ARG
      invalid arguments

  @note
    Not all the M2MB_OS_MTX_SEL_CMD_E are always available.
    Of course M2MB_OS_MTX_SEL_CMD_CREATE_ATTR is not right for this function
    Allowed
    M2MB_OS_MTX_SEL_CMD_USRNAME
    only

  @b
    Example
  @code
    m2mb_os_mtx_setItem( mtxHandle, M2MB_OS_MTX_SEL_CMD_USRNAME, (void*)"ThisIsUserName");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_mtx_setItem( M2MB_OS_MTX_HANDLE mtxHandle,
                                      M2MB_OS_MTX_SEL_CMD_E selCmd,
                                      void *pIn
                                    );



#endif /* M2MB_OS_MTX_H */
