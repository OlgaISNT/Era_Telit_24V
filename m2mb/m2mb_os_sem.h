/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_os_sem.h

  @brief
    OS Samaphore prototypes

  @details
    basic functios for OS semaphores are provided

  @notes
    Dependencies:
             m2mb_types.h
             m2mb_os_types.h

  @author
    Alessandro Papagno

  @date
     03/03/2017
*/


#ifndef M2MB_OS_SEM_H
#define M2MB_OS_SEM_H


/* Global declarations ==========================================================================*/
#define  M2MB_OS_SEM_INVALID          0
#define  M2MB_OS_SEM_INVALID_ATTR     0

/*-----------------------------------------------------------------------------------------------

    can be called directly without specify CMDS_ARGS
    osRes = m2mb_os_sem_setAttrItem_( &semAttrHandle,
                                      M2MB_OS_SEM_SEL_CMD_CREATE_ATTR,  NULL,
                                      M2MB_OS_SEM_SEL_CMD_COUNT, initVal,
                                      M2MB_OS_SEM_SEL_CMD_TYPE, M2MB_OS_SEM_GEN,
                                      M2MB_OS_SEM_SEL_CMD_NAME, "MySem"
                                    );
  -----------------------------------------------------------------------------------------------*/
#ifdef _WIN32
#define m2mb_os_sem_setAttrItem_( h, ... )        m2mb_os_sem_setAttrItem( h, CMDS_ARGS( __VA_ARGS__ ) )
#else
#define m2mb_os_sem_setAttrItem_( h, args... )        m2mb_os_sem_setAttrItem( h, CMDS_ARGS( args ) )
#endif

/* Global typedefs --------------------------------------------------------------- */
typedef enum
{
  /* not limited counter value: could overflow and restart with 0 */
  M2MB_OS_SEM_GEN,
  /* counter limited to 1 */
  M2MB_OS_SEM_BINARY,
  /* counter limited to defined value */
  M2MB_OS_SEM_COUNTING,

  M2MB_OS_SEM_MAX,
  ENUM_TO_INT( M2MB_OS_SEM_TYPE_E )
} M2MB_OS_SEM_TYPE_E;

typedef enum
{
  M2MB_OS_SEM_SEL_CMD_CREATE_ATTR,
  M2MB_OS_SEM_SEL_CMD_DEL_ATTR,
  /* use as Os name, available during creation */
  M2MB_OS_SEM_SEL_CMD_NAME,
  /* if set, usr name could be different from Os during creation */
  M2MB_OS_SEM_SEL_CMD_USRNAME,
  /* type of semaphore: see M2MB_OS_SEM_TYPE_E */
  M2MB_OS_SEM_SEL_CMD_TYPE,
  /* max counter value has effect for M2MB_OS_SEM_COUNTING type only */
  M2MB_OS_SEM_SEL_CMD_MAX_COUNT,
  /* High Priority Task First: every put will move highest priority task on top of FIFO */
  M2MB_OS_SEM_SEL_CMD_HPTF,
  /* curren semaphore counting value */
  M2MB_OS_SEM_SEL_CMD_COUNT,
  /* number of task suspended on this resource */
  M2MB_OS_SEM_SEL_CMD_SUSPENDED_COUNT,
  /* first task (M2MB_OS_TASK_HANDLE) to be served when resource will be available */
  M2MB_OS_SEM_SEL_CMD_1ST_SUSP,

  /* ---- */
  M2MB_OS_SEM_SEL_CMD_END,
  ENUM_TO_INT( M2MB_OS_SEM_SEL_CMD_E )
} M2MB_OS_SEM_SEL_CMD_E;

typedef struct M2MB_OS_SEM_HANDLE_TAG      *M2MB_OS_SEM_HANDLE;
typedef struct M2MB_OS_SEM_ATTR_HANDLE_TAG *M2MB_OS_SEM_ATTR_HANDLE;


/* Global functions =============================================================================*/


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Semaphore creation

  @details
    This service creates a counting semaphore for inter-intra task
    synchronization. The initial semaphore attribute are set inside M2MB_OS_SEM_ATTR_HANDLE
    as parameters.

  @param [out] pSemHandle
    pointer to semaphore handle
  @param [in] pSemAttr
    pointer to semaphore attribute handle

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_SEMAPHORE_ERROR
     Invalid counting semaphore handle
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    After successfull semaphore creation, semaphore attribute will be deleted, their resources free
    and pSemAttr set to M2MB_OS_SEM_INVALID_ATTR
    Allowed From
      Initialization and threads
    Preemption Possible
      No

  @b
    Example
    Semaphore creation using m2mb_os_sem_init
  @code
    M2MB_OS_SEM_ATTR_HANDLE semAttrHandle;
    M2MB_OS_SEM_HANDLE      semHandle, semCountingHandle;
    UINT32                  initVal = 1;
    UINT32                  max_count=5;

    if ( m2mb_os_sem_setAttrItem( &semAttrHandle, 1, M2MB_OS_SEM_SEL_CMD_CREATE_ATTR,  NULL ) != M2MB_OS_SUCCESS )
        exit(error_creating attribute semaphore);

    m2mb_os_sem_setAttrItem( &semAttrHandle, M2MB_OS_SEM_SEL_CMD_COUNT, initVal );
    m2mb_os_sem_setAttrItem( &semAttrHandle, M2MB_OS_SEM_SEL_CMD_TYPE, M2MB_OS_SEM_GEN );
    m2mb_os_sem_setAttrItem( &semAttrHandle, M2MB_OS_SEM_SEL_CMD_NAME, "MySem" );

    // or simply all at once using macro CMDS_ARGS
    // arithmetic operation MUST BE INCLUDED in brackets: e.g  M2MB_OS_SEM_SEL_CMD_COUNT, ( initVal + 50 )
    // m2mb_os_sem_setAttrItem( &semAttrHandle,
    //                          CMDS_ARGS
    //                          (
    //                            M2MB_OS_SEM_SEL_CMD_CREATE_ATTR,  NULL,
    //                            M2MB_OS_SEM_SEL_CMD_COUNT, initVal,
    //                            M2MB_OS_SEM_SEL_CMD_TYPE, M2MB_OS_SEM_GEN,
    //                            M2MB_OS_SEM_SEL_CMD_NAME, "MySem"
    //                          )
    //                        );

    m2mb_os_sem_init( &semHandle, &semAttrHandle );

    //now example of creation of semaphore counting...

      m2mb_os_sem_setAttrItem( &semAttrHandle,
                           CMDS_ARGS
                           (
                             M2MB_OS_SEM_SEL_CMD_CREATE_ATTR,  NULL,
                             M2MB_OS_SEM_SEL_CMD_TYPE, M2MB_OS_SEM_COUNTING,
                             M2MB_OS_SEM_SEL_CMD_MAX_COUNT, max_count,
                             M2MB_OS_SEM_SEL_CMD_COUNT, initVal,
                             M2MB_OS_SEM_SEL_CMD_NAME, "SemContingExample"
                           )
                         );

      m2mb_os_sem_init( &semCountingHandle, &semAttrHandle );
      // now the start count is 1 and maximum numbers of put will be limited to
      // what needed to reach count=5
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_sem_init(
  M2MB_OS_SEM_HANDLE *pSemHandle,
  M2MB_OS_SEM_ATTR_HANDLE *pSemAttr
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Semaphore deinit or deletion

  @details
    This service deletes the specified os semaphore. All tasks
    suspended waiting for a this semaphore are resumed with a @return value egual to M2MB_OS_DELETED
    The resources will be released.
    It is the application's responsibility to prevent use of a deleted
    semaphore.

  @param [in] semHandle
    handle of the semaphore

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_SEMAPHORE_ERROR
     Invalid counting semaphore handle
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    It is the application's responsibility to prevent use of a deleted semaphore.

    semaphore.
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
M2MB_OS_RESULT_E m2mb_os_sem_deinit( M2MB_OS_SEM_HANDLE semHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get an instance of semaphore

  @details
    This service retrieves an instance (a single count) from the specified
    semaphore if its count is grater than 0. As a result, the specified semaphore’s count is
    decreased by one.
    If count is not availbale parameter timeoutTicks specifies how to behave: wait forever or
    set a limit to wait and then exit

  @param [in] semHandle
    handle of semaphore
  @param [in] timeoutTicks
    If timeout is specified and semaphore count is zero, task will be suspended for a maximum
    time of timeout ticks

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_SUCCESS
     Successful retrieval of a semaphore instance.
    M2MB_OS_DELETED
     Counting semaphore was deleted while task was suspended.
    M2MB_OS_NO_INSTANCE
     Service was unable to retrieve an instance of the
     counting semaphore (semaphore count is zero within the specified time to
     wait).
    M2MB_OS_WAIT_ABORTED
     Suspension was aborted by another task, timer, or ISR.
    M2MB_OS_SEMAPHORE_ERROR
     Invalid counting semaphore handle.
    M2MB_OS_WAIT_ERROR
     A wait option other than M2MB_OS_NO_WAIT was specified on a call from a non-task.

  @note
    Allowed From
      Initialization, tasks, timers, and ISRs
    Preemption Possible
      Yes

  @b
    Example
  @code
    #include "m2mb_os.h"

    // set timeout as 2000 ms
    m2mb_os_sem_get( semHandle, M2MB_OS_MS2TICKS(2000) );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_sem_get( M2MB_OS_SEM_HANDLE semHandle, UINT32 timeoutTicks );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Place (put) an instance in semaphore

  @details
    This service puts an instance into the specified counting semaphore,
    which in reality increments the counting semaphore by one.
    In case of general counting semaphore (M2MB_OS_SEM_GEN),
    if this service is called when the semaphore count is all ones (OxFFFFFFFF),
    the new put operation will cause the semaphore to be reset to zero.
    When M2MB_OS_SEM_BINARY or M2MB_OS_SEM_COUNTING are the type of OS semaphore,
    count will be limited to 1 or maxCount as specified by M2MB_OS_SEM_SEL_CMD_MAX_COUNT attribute


  @param [in] semHandle
    handle of semaphore

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_SUCCESS
     Successful semaphore put.
    M2MB_OS_SEMAPHORE_ERROR
     Invalid counting semaphore handle
    M2MB_OS_CEILING_EXCEEDED
      Put request exceeds maximum maxCount.
    M2MB_OS_INVALID_CEILING
      An invalid value of zero was as maxCount

  @note
    Allowed From
      Initialization, tasks, timers, and ISRs
    Preemption Possible
      Yes

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_sem_put( M2MB_OS_SEM_HANDLE semHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Prioritize semaphore suspension list
    HPTF: Highest Priority Task First

  @details
    This service set the task waiting for this semaphore which has currently highest priority,
    on the top of FIFO task suspension list
    It is one shot operation.
    To have highest priority task served first for every put, see m2m_os_sem_setItem(..)
    or m2mb_os_sem_setAttrItem.

  @param [in] semHandle
    handle of semaphore

  @return
    M2MB_OS_FEATURE_NOT_SUPPORTED always

  @note
    This function is implemented as dummy function and always return M2MB_OS_FEATURE_NOT_SUPPORTED
    for the compatibility with other products

    Allowed From
      Initialization, tasks, timers, and ISRs
    Preemption Possible
      No

  @b
    Example
    usage of m2mb_os_sem_hptf
  @code
    M2MB_OS_SEM_HANDLE semHandle;
    //...
    //already created... used semaphore
    //just put highest priority task on top of suspension list
    m2mb_os_sem_hptf( semHandle );

    m2mb_os_sem_put(semHandle);
    //will give control to highest priority task waiting...

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_sem_hptf( M2MB_OS_SEM_HANDLE semHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set semaphore attribute for creation

  @details
    This service set the attribute (or create it) to be used for semaphore creation

  @param [in] pSemAttrHandle
   pointer to semaphore attribute of semaphore
  @param [in] nCmds
    number of commands to be issued at once
  @param [in] ...
    variable inputs, N cmds (see M2MB_OS_SEM_SEL_CMD_E) followed by N arguments
    for their respective issued commands. CMDS(...) and CMDS_ARGS(...) macro can be
    used to better insert cmd and parameters and to better control right insertion

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_SUCCESS
     Successful semaphore operation.
    M2MB_OS_SEMAPHORE_ERROR
     Invalid counting semaphore handle
    M2MB_OS_INVALID_ARG
      invalid pSemAttr or pIn

  @note
    The attribute set are useful for semaphore creation.
    Once created, use instead m2mb_os_sem_setItem() to set semaphore properties
    It is under application responsibility the right use of m2mb_os_sem_setAttrItem.
    For instance, multi creation of attrHandle withouth free of previous resources
    ( M2MB_OS_SEM_SEL_CMD_CREATE_ATTR without M2MB_OS_SEM_SEL_CMD_DEL_ATTR in case
      no m2mb_os_sem_init is used in the between ) will waste memory.

  @b
    Example
  @code
    M2MB_OS_RESULT_E        osRes;
    M2MB_OS_SEM_ATTR_HANDLE semAttrHandle;
    M2MB_OS_SEM_HANDLE      semHandle;
    UINT32                  initVal = 1;

    if ( m2mb_os_sem_setAttrItem( &semAttrHandle, 1, M2MB_OS_SEM_SEL_CMD_CREATE_ATTR, NULL ) != M2MB_OS_SUCCESS )
        exit(error_creating attribute semaphore);

    //set init value,max_count to 5, type as counting semaphore and set its name:  using 4 as nCmds
    osRes = m2mb_os_sem_setAttrItem( &semAttrHandle, 4,
                                     M2MB_OS_SEM_SEL_CMD_COUNT, M2MB_OS_SEM_SEL_CMD_MAX_COUNT,
                                     M2MB_OS_SEM_SEL_CMD_TYPE,  M2MB_OS_SEM_SEL_CMD_NAME,
                                     initVal, 5, M2MB_OS_SEM_COUNTING, "MyCountingSem"
                                   );
    //or simply use macro CMDS_ARGS(...)
    // osRes = m2mb_os_sem_setAttrItem( &semAttrHandle,
    //                                  CMDS_ARGS
    //                                  (
    //                                    M2MB_OS_SEM_SEL_CMD_COUNT, initVal,
    //                                    M2MB_OS_SEM_SEL_CMD_TYPE, M2MB_OS_SEM_COUNTING,
    //                                    M2MB_OS_SEM_SEL_CMD_MAX_COUNT, 5,
    //                                    M2MB_OS_SEM_SEL_CMD_NAME, "MyCountingSem"
    //                                  )
    //                                );

    if( osRes != M2MB_OS_SUCCESS )
      exit(...)
    m2mb_os_sem_init( &semHandle, &semAttrHandle );
    //...

    m2mb_os_sem_deinit(&semHandle);

    //now create it as binary: for error set M2MB_OS_SEM_SEL_CMD_MAX_COUNT to 5: never reached as binary
    osRes = m2mb_os_sem_setAttrItem( &semAttrHandle,
                                     CMDS_ARGS
                                     (
                                       M2MB_OS_SEM_SEL_CMD_COUNT, 1,
                                       M2MB_OS_SEM_SEL_CMD_TYPE, M2MB_OS_SEM_BINARY,
                                       M2MB_OS_SEM_SEL_CMD_MAX_COUNT, 5,
                                       M2MB_OS_SEM_SEL_CMD_NAME, "MyCountingSem"
                                     )
                                   );
    if( osRes != M2MB_OS_SUCCESS )
      exit(...)

    //create as binary
    m2mb_os_sem_init( &semHandle, &semAttrHandle );

    m2mb_os_sem_put( semHandle );
    m2mb_os_sem_put( semHandle );
    m2mb_os_sem_put( semHandle );
    m2mb_os_sem_put( semHandle );

    //value of count will always be no more than 1

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_sem_setAttrItem(
  M2MB_OS_SEM_ATTR_HANDLE *pSemAttrHandle,
  UINT8 nCmds,
  ...
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get specific semaphore properties

  @details
    This service retrieve value for specified parameter of the specified semaphore
    Selecting cmd, a parameter to be retrieved can be set
    To be used when semaphore is already created: its right usage is under application responsibility

  @param [in] semHandle
    handle of semaphore
  @param [in] selCmd
   Select parameter to be retrieved (see M2MB_OS_SEM_SEL_CMD_E)
  @param [out] pOut
   pointer Output parameter got: its meaning depend on selCmd used. See examples.
   As general pointer it is set to MEM_W
  @param [in] pIn
   pointer to void general Parameter to be specified, if any, to get right output. See examples.
   If parameter is not needed, use NULL

  @return
   M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_SEMAPHORE_ERROR
     Invalid counting semaphore handle
    M2MB_OS_INVALID_ARG
      invalid arguments

  @note
    Not all the M2MB_OS_SEM_SEL_CMD_E are always available.
    Of course M2MB_OS_SEM_SEL_CMD_CREATE_ATTR is not right for this function
    Allowed:
    M2MB_OS_SEM_SEL_CMD_NAME
    M2MB_OS_SEM_SEL_CMD_USRNAME
    M2MB_OS_SEM_SEL_CMD_1ST_SUSP
    M2MB_OS_SEM_SEL_CMD_SUSPENDED_COUNT
    M2MB_OS_SEM_SEL_CMD_COUNT
    M2MB_OS_SEM_SEL_CMD_TYPE
    M2MB_OS_SEM_SEL_CMD_MAX_COUNT ( has effect only for M2MB_OS_SEM_COUNTING type )
    M2MB_OS_SEM_SEL_CMD_HPTF


  @b
    Example
  @code
    M2MB_OS_RESULT_E        osRes;
    M2MB_OS_SEM_ATTR_HANDLE semAttrHandle;
    M2MB_OS_SEM_HANDLE      semHandle;
    UINT32                  initVal = 1;
    MEM_W                   currCount;

   //create it as binary:
    osRes = m2mb_os_sem_setAttrItem( &semAttrHandle,
                                     CMDS_ARGS
                                     (
                                       M2MB_OS_SEM_SEL_CMD_COUNT, 1,
                                       M2MB_OS_SEM_SEL_CMD_MAX_COUNT, 5,
                                       M2MB_OS_SEM_SEL_CMD_TYPE, M2MB_OS_SEM_BINARY,
                                       M2MB_OS_SEM_SEL_CMD_NAME, "MyCountingSem"
                                     )
                                   );
    if( osRes != M2MB_OS_SUCCESS )
      exit(...)

    //create as binary
    m2mb_os_sem_init( &semHandle, &semAttrHandle );

    m2mb_os_sem_put( semHandle );
    m2mb_os_sem_put( semHandle );
    m2mb_os_sem_put( semHandle );
    m2mb_os_sem_put( semHandle );

    m2mb_os_sem_getItem( semHandle, M2MB_OS_SEM_SEL_CMD_COUNT, &currCount, NULL );
    //you get currCount =1
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_sem_getItem( M2MB_OS_SEM_HANDLE semHandle,
                                      M2MB_OS_SEM_SEL_CMD_E selCmd,
                                      MEM_W *pOut,
                                      void *pIn
                                    );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific semaphore properties

  @details
    This service set value for specified parameter of the specified semaphore if available
    Selecting cmd, a parameter to be set can be selected
    To be used when semaphore is already created: its right usage is under application responsibility

  @param [in] semHandle
    handle of semaphore
  @param [in] selCmd
   Select parameter to be set (see M2MB_OS_SEM_SEL_CMD_E)
  @param [in] pIn
    pointer to value to be set. If a simple value, can be easily casted to (void*)
    e.g. int value -3 has to be passed, use (void*)-3
    NULL if no value is needed

  @return
   M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_SEMAPHORE_ERROR
     Invalid counting semaphore handle
    M2MB_OS_INVALID_ARG
      invalid arguments

  @note
    Not all the M2MB_OS_SEM_SEL_CMD_E are always available.
    Of course M2MB_OS_SEM_SEL_CMD_CREATE_ATTR is not right for this function.
    Allowed:
    M2MB_OS_SEM_SEL_CMD_USRNAME
    M2MB_OS_SEM_SEL_CMD_MAX_COUNT
    M2MB_OS_SEM_SEL_CMD_HPTF


  @b
    Example
  @code
    m2mb_os_sem_setItem(semHandle, M2MB_OS_SEM_SEL_CMD_HPTF, (void*)1 );
    // this will force every put to release the highest priority task waiting for semaphore
    m2mb_os_sem_setItem(semHandle, M2MB_OS_SEM_SEL_CMD_HPTF, (void*)0 );
    //will set standard FIFO rule again
    m2mb_os_sem_setItem(semHandle, M2MB_OS_SEM_SEL_CMD_USRNAME, (void*)"ThisIsUserName");
    m2mb_os_sem_setItem(semHandle, M2MB_OS_SEM_SEL_CMD_MAX_COUNT, (void*)3);
    //Has effect only for semaphore type M2MB_OS_SEM_COUNTING , new maxCount will be set at run time
    // for other type odf semaphore has not effect
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_sem_setItem(
  M2MB_OS_SEM_HANDLE semHandle,
  M2MB_OS_SEM_SEL_CMD_E selCmd,
  void *pIn
);

#endif /* M2MB_OS_SEM_H */
