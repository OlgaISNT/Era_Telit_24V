/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_os.h


  @brief
    Basic functionality of Operating systems

  @details
    Majors task related functionalities are defined, plus some systems info and settings
    e.g. system timer tick

  @notes
    Dependencies:
             m2mb_types.h
             m2mb_os_types.h
             m2mb_platform_conf.h

  @author
    Alessandro Papagno

  @date
    01/03/2017
*/

#ifndef M2MB_OS_H
#define M2MB_OS_H

/* Include ======================================================================================*/

/* Global declarations ==========================================================================*/
#define  M2MB_OS_TASK_INVALID       0
#define  M2MB_OS_TASK_INVALID_ATTR  0

#define  M2MB_OS_TASK_AUTOSTART     1
#define  M2MB_OS_TASK_NOT_START     0

#define  M2MB_OS_ISR_ON     1
#define  M2MB_OS_ISR_OFF    0

/* in bytes */
#define M2MB_OS_MIN_STACK_SIZE  1024

/* category for errno */
#define M2MB_FS_CAT    ( (UINT32)0 )
#define M2MB_SMS_CAT   ( (UINT32)1 )
#define M2MB_GNSS_CAT  ( (UINT32)2 )
#define M2MB_OS_CAT    ( (UINT32)3 )

/*-----------------------------------------------------------------------------------------------

    can be called directly without specify CMDS_ARGS
    m2mb_os_taskSetAttrItem_( &taskAttrHandle,
                              M2MB_OS_TASK_SEL_CMD_CREATE_ATTR, NULL,
                              M2MB_OS_TASK_SEL_CMD_NAME, "myTask",
                              M2MB_OS_TASK_SEL_CMD_PRIORITY, 201,
                              M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH, 201,
                              M2MB_OS_TASK_SEL_CMD_AUTOSTART, M2MB_OS_TASK_AUTOSTART,
                              M2MB_OS_TASK_SEL_CMD_USRNAME, "myUserTask"
                            );
  -----------------------------------------------------------------------------------------------*/
#ifdef _WIN32
#define m2mb_os_taskSetAttrItem_( h, ... )        m2mb_os_taskSetAttrItem( h, CMDS_ARGS( __VA_ARGS__ ) )
#else
#define m2mb_os_taskSetAttrItem_( h, args... )        m2mb_os_taskSetAttrItem( h, CMDS_ARGS( args ) )
#endif

/*-----------------------------------------------------------------------------------------------*/

/* Global typedefs ==============================================================================*/
typedef struct M2MB_OS_TASK_ATTR_HANDLE_TAG *M2MB_OS_TASK_ATTR_HANDLE;
typedef struct M2MB_OS_TASK_HANDLE_TAG      *M2MB_OS_TASK_HANDLE;

/* specified type for entry function of task */
typedef void( *ENTRY_FN )( void * );

typedef enum
{
  M2MB_OS_TASK_SEL_CMD_CREATE_ATTR,
  M2MB_OS_TASK_SEL_CMD_DEL_ATTR,
  /* use as Os name : the name during task creation */
  M2MB_OS_TASK_SEL_CMD_NAME,
  /* if set, usr name could be different from initial CMD NAME */
  M2MB_OS_TASK_SEL_CMD_USRNAME,
  M2MB_OS_TASK_SEL_CMD_STACK_START,
  M2MB_OS_TASK_SEL_CMD_STACK_SIZE,
  M2MB_OS_TASK_SEL_CMD_PRIORITY,
  M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH,
  /* time slice: use max ticks for task to run when other tasks with same priority are ready */
  M2MB_OS_TASK_SEL_CMD_TSLICE,
  /* start task soon after m2mb_os_taskCreate is called: no need to call m2mb_os_taskResume */
  M2MB_OS_TASK_SEL_CMD_AUTOSTART,

  /* state of the task: see M2MB_OS_TASK_STATE_E */
  M2MB_OS_TASK_SEL_CMD_STATE,
  /* task running count */
  M2MB_OS_TASK_SEL_CMD_RUN_CNT,

  /* ---- */
  M2MB_OS_TASK_SEL_CMD_END,
  ENUM_TO_INT( M2MB_OS_TASK_SEL_CMD_E )
} M2MB_OS_TASK_SEL_CMD_E;

typedef enum
{
  M2MB_OS_TASK_STATE_READY,
  M2MB_OS_TASK_STATE_COMPLETED,
  M2MB_OS_TASK_STATE_TERMINATED,
  M2MB_OS_TASK_STATE_SUSP,
  M2MB_OS_TASK_STATE_SLEEP,
  M2MB_OS_TASK_STATE_QUEUE_SUSP,
  M2MB_OS_TASK_STATE_SEMAPHORE_SUSP,
  M2MB_OS_TASK_STATE_SIG_SUSP,
  M2MB_OS_TASK_STATE_BLOCK_MEM_SUSP,
  M2MB_OS_TASK_STATE_BYTE_MEM_SUSP,
  M2MB_OS_TASK_STATE_MUTEX_SUSP = 0x0D,

  /* ---- */
  M2MB_OS_TASK_STATE_END,
  ENUM_TO_INT( M2MB_OS_TASK_STATE_E )
} M2MB_OS_TASK_STATE_E;

typedef enum
{
  M2MB_OS_MEMINFO_FRAGMENTS,                /* the total number of memory fragments */
  M2MB_OS_MEMINFO_BYTES_AVAILABLE,          /* the total number of available bytes */
  M2MB_OS_MEMINFO_SIZE,                     /* the total number of memory size */

  M2MB_OS_MEMINFO_BYTES_MAX_OCCUPATION = 6, /* the max occupation of bytes that have been allocated */
  /* ---- */
  M2MB_OS_MEMINFO_END,
  ENUM_TO_INT( M2MB_OS_MEMINFO_E )
} M2MB_OS_MEMINFO_E;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!

  @brief  Convert ms to ticks

  @details
     This functions converts input expressed as millisecond in number of ticks
     Can be used in functions which require number of ticks as arguments, providing ticks got from ms

  @param [in] ms
     Input value expressed in ms

  @return
     Number of Ticks to cover requested ms

  @note
     Usually maximum number of ticks which can be used in a function ( e.g see m2mb_os_taskSleep )
     is defined by the function itself: e.g. for UINT32 it is 0xFFFF FFFF
     M2MB_OS_MS2TICKS is instead limited in ms input to UINT32, so in principle cannot cover all
     range of ticks. If needed calculate the ticks and place directly the value in dedicated function.
     E.g.
     if tick is 16 ms wide, 0xFFFF FFFF ms will provide maximum 0xFFF FFFF ticks instead of
     0xFFFF FFFF available in functions requiring ticks as argument.
     By the way 0xFFFF FFFF ms is something like 1193 hours

  @b

  @code
     // make the task wait for 10 s
     m2mb_os_taskSleep( M2MB_OS_MS2TICKS( 10000 ) );

  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/

UINT32 M2MB_OS_MS2TICKS( UINT32 ms );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates bytes of memory

  @details
    This function provides service to reserve memory space to the caller

  @param [in] size
    size in byte of memory to be allocated

  @return
    valid pointer in case of success

  @return
    NULL in case of error


  @note
    The performance of this service is a function of the requested block size and the
    amount of fragmentation in the heap. Hence, this service should not be
    used during time-critical task of execution.
    Allowed From
     Initialization and tasks
    Preemption Possible
      Yes

  @b
    //pointer to 10 UINT32
    UINT32 *pUint;
    pUint = ( UINT32 * )m2mb_os_malloc( 10 * sizeof(UINT32) );
    if ( pUint == NULL )
      exit(...)
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
void *m2mb_os_malloc( UINT32 size );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates bytes of memory and init space with 0

  @details
    This function provides service to reserve memory space to the caller and initialize it to 0

  @param [in] size
    size in byte of memory to be allocated and initialize

  @return
    valid pointer in case of success

  @return
    NULL in case of error

  @note
    The performance of this service is a function of the requested block size and the
    amount of fragmentation in the heap. Hence, this service should not be
    used during time-critical task of execution.
    Allowed From
     Initialization and tasks
    Preemption Possible
      Yes

  @b
    Example
  @code
    //pointer to 10 UINT32
    typedf struct
    {
      INT32 a;
      INT8  b;
      void *ptr;
    }GEN_T;

    GEN_T *pStruct;
    pStruct = ( GEN_T * )m2mb_os_calloc( sizeof(GEN_T) );
    if ( pStruct == NULL )
      exit(...)
    //all pStruct initialized to 0: pStruct->a = 0; pStruct->b = 0; pStruct->ptr = 0;
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
void *m2mb_os_calloc( UINT32 size );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Free allocated memory

  @details
    This function provides service to free already allocated memory space

  @param [in] pMem
    pointer to memory where to release previous allocation

  @return
    M2MB_OS_SUCCESS in case of success

  @return
    M2MB_OS_PTR_ERROR
      Invalid memory area pointer
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service

  @note
    The application must prevent using the memory area after it is released.
    Allowed From
     Initialization and tasks
    Preemption Possible
      Yes

  @b
    Example
  @code
    M2MB_OS_RESULT_E osRes;
    osRes = m2mb_os_free( pStruct );
    if ( osRes != M2MB_OS_SUCCESS )
      //...

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_free( void *pMem );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Initialization of OS

  @details
    Init of operating systems. Should be called as initialization process, before calling any
    other function of OS
    Simple init function which will setup the OS environment for next usage

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    others in case of error : see m2mb_os_types.h

  @note
    <Notes>

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_init( void );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Create application task

  @details
    This service creates an application task that starts execution at the
    specified task entry function. The stack, priority, preemption-threshold,
    time-slice, and initial execution state of the task are among the attributes specified by
    the input parameters in pTaskAttr.
    If stack is not provided, it will be allocated by the system and removed after task deletion.
    In case stack is provided, some space will be lost due to internal management structures,
    so the area available will be a bit less than the area provided
    If stack size is not provided, minimum stack size M2MB_OS_MIN_STACK_SIZE will be allocated.

  @param [out] pTaskHandle
     pointer to Handle of the task to be used for future calling the specific task
  @param [in] pTaskAttr
     handle Attribute and options of the task to be created
  @param [in] entryFn
     Function which will be the start for the task
  @param [in] pArg
     pointer - argument of the entry function of the task


  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    Lot of errors can be revealed using m2mb_os_setTaskAttr (set attribute),
    since they are related to the attribute setting mechanism
    M2MB_OS_TASK_ERROR
     Invalid task handler, or the task is already created
    M2MB_OS_PTR_ERROR
     Invalid starting address of the entry point or the stack area in Attr is invalid, usually NULL
    M2MB_OS_SIZE_ERROR
     Size of stack area in Attr is invalid.
    M2MB_OS_PRIORITY_ERROR
     Priority set in Attr is out of range or invalid
    M2MB_OS_THRESH_ERROR
     Invalid preemption- threshold specified in Attr. The value should be less or equal to priority
    M2MB_OS_START_ERROR
     Invalid auto-start selection in Attr
    M2MB_OS_CALLER_ERROR
     Invalid caller of this service

  @note
    After successfull creation of task, M2MB_OS_TASK_ATTR_HANDLE will be deleted and relative pointer
    set to M2MB_OS_TASK_INVALID_ATTR
    Up to the user to remove the resources of attribute in case of failure during task creation
    Allowed From
      Initialization and tasks
    Preemption Possible
      Yes
  @b
    Example
  @code

    M2MB_OS_RESULT_E osRes;
    M2MB_OS_TASK_ATTR_HANDLE taskAttrHandle;
    M2MB_OS_TASK_HANDLE taskHandle = M2MB_OS_TASK_INVALID;
    UINT16 OFFSET = 1;
    //3 methods are shown:
    // ######  single attribute set n times
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_CREATE_ATTR, NULL);
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_NAME, "myTask" );
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_PRIORITY, 201 );
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH, 201 );
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_AUTOSTART, M2MB_OS_TASK_AUTOSTART );
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_USRNAME, "myUserTask" );

    // ######  CMD macro usage: multi commands, followed by their relative associated parameter
    m2mb_os_taskSetAttrItem( &taskAttrHandle,
                             CMDS(
                               M2MB_OS_TASK_SEL_CMD_CREATE_ATTR,
                               M2MB_OS_TASK_SEL_CMD_NAME,
                               M2MB_OS_TASK_SEL_CMD_PRIORITY,
                               M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH,
                               M2MB_OS_TASK_SEL_CMD_AUTOSTART,
                               M2MB_OS_TASK_SEL_CMD_USRNAME
                             ),
                             NULL, "myTask", 201, 201, M2MB_OS_TASK_AUTOSTART, "myUserTask"
                           );


    // ######  CMDS_ARGS macro usage: multi commands, each followed by its own parameter
    // arithmetic operation MUST BE INCLUDED in brackets: e.g  M2MB_OS_TASK_SEL_CMD_PRIORITY, ( 200 + OFFSET ),
      m2mb_os_taskSetAttrItem( &taskAttrHandle,
                                CMDS_ARGS(
                                           M2MB_OS_TASK_SEL_CMD_CREATE_ATTR, NULL,
                                           M2MB_OS_TASK_SEL_CMD_NAME, "myTask",
                                           M2MB_OS_TASK_SEL_CMD_PRIORITY, ( 200 + OFFSET ),
                                           M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH, ( 200 + OFFSET ),
                                           M2MB_OS_TASK_SEL_CMD_AUTOSTART, M2MB_OS_TASK_AUTOSTART,
                                           M2MB_OS_TASK_SEL_CMD_USRNAME, "myUserTask"
                                         )
                             );

    osRes = m2mb_os_taskCreate( &taskHandle, &taskAttrHandle, entry, (void*)1 );
    if ( osRes != M2MB_OS_SUCCESS )
    {
      m2mb_os_taskSetAttrItem( &taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_DEL_ATTR, NULL );
    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskCreate(
  M2MB_OS_TASK_HANDLE *pTaskHandle,
  M2MB_OS_TASK_ATTR_HANDLE *pTaskAttr,
  ENTRY_FN entryFn,
  void *pArg
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get specific task properties

  @details
    This function retrieve special information of the specified task
    Selecting cmd, a parameter to be retrieved can be set
    To be used when task is already created: its right usage is under application responsibility

  @param [in] taskHandle
    Task handle identify the task to get parameter from
  @param [in] selCmd
    Select parameter to be retrieved (see M2MB_OS_TASK_SEL_CMD_E)
  @param [out] pOut
    pointer Output parameter got: its meaning depend on selCmd used See examples
  @param [in] pIn
    pointer to Parameter to be specified, if any, to get right output See examples
    If parameter is not needed, use NULL

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    others in case of error : see m2mb_os_types.h

  @note
   Not all M2MB_OS_TASK_SEL_CMD_E are allowed.
   Of course not Allowed:
   M2MB_OS_TASK_SEL_CMD_CREATE_ATTR
   M2MB_OS_TASK_SEL_CMD_DEL_ATTR


  @b
    Example
  @code

    M2MB_OS_TASK_HANDLE myTask;
    CHAR MyTaskName[10];
    CHAR *getName;
    M2MB_OS_TASK_STATE_E taskState;
    MEM_W stackStart;

    memset(MyTaskName,0,10);
    m2mb_os_taskGetItem(myTask, M2MB_OS_TASK_SEL_CMD_NAME, (MEM_W*)&getName, NULL);
    memcpy(MyTaskName, getName, strlen(getName)>10 ? 10: strlen(getName));

    or

    m2mb_os_taskGetItem(myTask, M2MB_OS_TASK_SEL_CMD_NAME, (MEM_W*)&MyTaskName, 10);


    m2mb_os_taskGetItem(myTask, M2MB_OS_TASK_SEL_CMD_STACK_START, (MEM_W*)&stackStart, NULL);
    //copy only the first 10 chars to MyTaskName array of chars
    m2mb_os_taskGetItem(myTask, M2MB_OS_TASK_SEL_CMD_USRNAME, (MEM_W*)&MyTaskName, 10);
    // or simply get the pointer of name, with NULL
    m2mb_os_taskGetItem(myTask, M2MB_OS_TASK_SEL_CMD_USRNAME, (MEM_W*)&getName, NULL);

    //get the currrent state of the task
    m2mb_os_taskGetItem(myTask, M2MB_OS_TASK_SEL_CMD_STATE, (MEM_W*)&taskState, NULL);
    // and so on

    M2MB_OS_TASK_ATTR_HANDLE taskAttrHandle;
    M2MB_OS_TASK_HANDLE taskHandle = M2MB_OS_TASK_INVALID;

    // option to set single attribute: see below to set all at once
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_CREATE_ATTR,  NULL);
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_STACK_SIZE, 2048);
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_NAME, "MyTask" );
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_PRIORITY, 201 );
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH, 201 );
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_AUTOSTART, M2MB_OS_TASK_AUTOSTART );
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_USRNAME, "myUserTask" );

    //or simply all at once
    // m2mb_os_taskSetAttrItem( &taskAttrHandle,
    //                          CMDS(
    //                                M2MB_OS_TASK_SEL_CMD_CREATE_ATTR,
    //                                M2MB_OS_TASK_SEL_CMD_STACK_SIZE,
    //                                M2MB_OS_TASK_SEL_CMD_NAME,
    //                                M2MB_OS_TASK_SEL_CMD_PRIORITY,
    //                                M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH,
    //                                M2MB_OS_TASK_SEL_CMD_AUTOSTART,
    //                                M2MB_OS_TASK_SEL_CMD_USRNAME
    //                               ),
    //                            NULL, (void*)2048, "MyTask", (void*)201, (void*)201,
    //                            (void*)M2MB_OS_TASK_AUTOSTART, "myUserTask"
    //                         )

    // or using CMDS_ARGS...
    // NB: // arithmetic operation MUST BE INCLUDED in brackets: e.g  M2MB_OS_TASK_SEL_CMD_PRIORITY, ( 200 + OFFSET ),
    //
    // m2mb_os_taskSetAttrItem( &taskAttrHandle,
    //                           CMDS_ARGS (
    //                                        M2MB_OS_TASK_SEL_CMD_CREATE_ATTR, NULL,
    //                                        M2MB_OS_TASK_SEL_CMD_STACK_SIZE,  2048,
    //                                        M2MB_OS_TASK_SEL_CMD_NAME, "MyTask",
    //                                        M2MB_OS_TASK_SEL_CMD_PRIORITY, 201,
    //                                        M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH, 201,
    //                                        M2MB_OS_TASK_SEL_CMD_AUTOSTART, M2MB_OS_TASK_AUTOSTART,
    //                                        M2MB_OS_TASK_SEL_CMD_USRNAME, "myUserTask"
    //                                      )
    //                         );
    m2mb_os_taskCreate( &taskHandle, &taskAttrHandle, entry, (void*)1,);

    // ....

    void entry (void *arg)
    {
      MEM_W  out = 0;
      UINT32 id = (UINT32)arg;
      M2MB_OS_TASK_HANDLE handle = m2mb_os_taskGetId();

      printf("\r\n Hello from Task argv [%d] ", id);

      m2mb_os_taskGetItem( handle, M2MB_OS_TASK_SEL_CMD_NAME, &out, NULL );
      printf ( "\r\n name %s ", (char*)out );
      m2mb_os_taskGetItem( handle, M2MB_OS_TASK_SEL_CMD_STACK_START, &out, NULL );
      printf ( "\r\n stack start 0x%x ", out );
      m2mb_os_taskGetItem( handle, M2MB_OS_TASK_SEL_CMD_STACK_SIZE, &out, NULL );
      printf ( "\r\n stack size %xd ", out );
      m2mb_os_taskGetItem( handle, M2MB_OS_TASK_SEL_CMD_PRIORITY, &out, NULL );
      m2mb_os_taskGetItem( handle, M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH, &out, NULL );
      m2mb_os_taskGetItem( handle, M2MB_OS_TASK_SEL_CMD_TSLICE, &out, NULL );
      m2mb_os_taskGetItem( handle, M2MB_OS_TASK_SEL_CMD_USRNAME, &out, NULL );
      m2mb_os_taskGetItem( handle, M2MB_OS_TASK_SEL_CMD_STATE, &out, NULL );
      m2mb_os_taskGetItem( handle, M2MB_OS_TASK_SEL_CMD_RUN_CNT, &out, NULL );
      m2mb_os_taskGetItem( handle, M2MB_OS_TASK_SEL_CMD_AUTOSTART, &out, NULL );

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskGetItem(
  M2MB_OS_TASK_HANDLE taskHandle,
  M2MB_OS_TASK_SEL_CMD_E selCmd,
  MEM_W *pOut,
  void *pIn
);


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific task properties

  @details
    This function set value for specified parameter of the specified task
    Selecting cmd, a parameter to be set can be selected
    To be used when task is already created: it is under application responsibility

  @param [in] taskHandle
    Task handle identify the task to set parameter to
  @param [in] selCmd
    Select parameter to be set (see M2MB_OS_TASK_SEL_CMD_E)
  @param [in] pIn
    pointer as value of parameter to be set, to the specified parameter

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    others in case of error : see m2mb_os_types.h

  @note
    Not all M2MB_OS_TASK_SEL_CMD_E are allowed.
    Allowed:
    M2MB_OS_TASK_SEL_CMD_USRNAME
    M2MB_OS_TASK_SEL_CMD_TSLICE
    M2MB_OS_TASK_SEL_CMD_PRIORITY
    M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH

  @b
    Example
  @code

    M2MB_OS_TASK_HANDLE myTask;
    //used myTask; already created
    CHAR MyTaskName[] = "myModule";
    MEM_W stackStart = 0x52000100;
    MEM_W stackSize = 0x1000;

    m2mb_os_taskSetItem(myTask,  M2MB_OS_TASK_SEL_CMD_USRNAME, (void*)&MyTaskName);


    // and so on
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskSetItem(
  M2MB_OS_TASK_HANDLE taskHandle,
  M2MB_OS_TASK_SEL_CMD_E selCmd,
  void *pIn
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific attribute for task creation

  @details
    Attribute defined can be used to create the task
    In case stack is provided, ( M2MB_OS_TASK_SEL_CMD_STACK_START) some space will be lost due
    to internal management structures, so the area available will be a bit less than the area provided
    ( M2MB_OS_TASK_SEL_CMD_STACK_SIZE )

  @param [in] selCmd
    Select parameter to be set (see M2MB_OS_TASK_SEL_CMD_E)
  @param [in\out] pTaskAttr
    pointer to M2MB_OS_TASK_ATTR_HANDLE handle to be set
  @param [in] ...
    variable inputs, N cmds (see M2MB_OS_TASK_SEL_CMD_E) followed by N arguments
    for their respective issued commands. CMDS(...) and CMDS_ARGS(...) macro can be
    used to better insert cmd and parameters and to better control right insertion

  @return
   M2MB_OS_SUCCESS in case of success
  @return
    others in case of error : see m2mb_os_types.h

  @note
    The attribute set are useful for task creation.
    Once created, use instead m2mb_os_taskSetItem() to set only available task properties
    After m2mb_os_taskCreate call, M2MB_OS_TASK_ATTR_HANDLE handle will be automatically free and
    its value set to M2MB_OS_TASK_INVALID_ATTR
    To set multi parameters at once, make use of macro CMDS(...) or CMDS_ARGS:
    see m2mb_os_taskCreate section
    It is under application responsibility the right use of m2mb_os_taskSetAttrItem.
    For instance, multi creation of attrHandle withouth free of previous resources
    ( M2MB_OS_TASK_SEL_CMD_CREATE_ATTR without M2MB_OS_TASK_SEL_CMD_DEL_ATTR in case
      no m2mb_os_taskCreate is used in the between ) will waste memory.

    Allowed From
      Initialization, tasks, timers, and ISRs
    Preemption Possible
     Yes

  @b
    Example
  @code
    M2MB_OS_TASK_ATTR_HANDLE taskAttrHandle;
    if ( m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_CREATE_ATTR,  NULL) != M2MB_OS_SUCCESS )
      exit(ERROR_CREATE_ATTR);
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_NAME, "myTask" );
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_PRIORITY, 101 );

    //...
    // In case
    m2mb_os_taskSetAttrItem(&taskAttrHandle, 1, M2MB_OS_TASK_SEL_CMD_DEL_ATTR, NULL);

    //or using macro CMDS(...)
    //    m2mb_os_taskSetAttrItem( &taskAttrHandle,
    //                             CMDS(
    //                                   M2MB_OS_TASK_SEL_CMD_CREATE_ATTR,
    //                                   M2MB_OS_TASK_SEL_CMD_NAME,
    //                                   M2MB_OS_TASK_SEL_CMD_PRIORITY
    //                                 ),
    //                             NULL, "myTask", 101
    //                           );

    // or using macro CMDS_ARGS(...)
    // arithmetic operation MUST BE INCLUDED in brackets: e.g  M2MB_OS_TASK_SEL_CMD_PRIORITY, ( 100 + OFFSET ),
    //
    //    m2mb_os_taskSetAttrItem( &taskAttrHandle,
    //                             CMDS_ARGS(
    //                                        M2MB_OS_TASK_SEL_CMD_CREATE_ATTR, NULL,
    //                                        M2MB_OS_TASK_SEL_CMD_NAME, "myTask",
    //                                        M2MB_OS_TASK_SEL_CMD_PRIORITY, 101
    //                                      ),
    //                           );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskSetAttrItem(
  M2MB_OS_TASK_ATTR_HANDLE *pTaskAttrHandle,
  UINT8 nCmds,
  ...
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Suspend current task for specified time in ticks

  @details
    This functions allows current task to be suspended for the amount of "ticks" timer ticks
    Suspend calling task for the specified ticks

  @param [in] ticks
    Number of timer ticks the task will be suspended. Duration of timer ticks depend on
    HW implementation and platform

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    others in case of error : see m2mb_os_types.h .

  @note
   Allowed From
    tasks
   Preemption Possible
     Yes

  @b
    Example
  @code
    // Suppose duration of system tick is 100ms
    m2mb_os_taskSleep(10);
    //will suspend current task for 1 s (1000 ms)
    //Could be easier to use the macro
    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1000) );
    //which automatically convert 1000 ms to Ticks

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskSleep( UINT32 ticks );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
     Retrieves handle to the currently executing task

  @details
    The function return the handle of the currently executing task
    This Function is used to get task handle of the currently executing task


  @return
    M2MB_OS_TASK_HANDLE of the current task
  @return
    M2MB_OS_TASK_INVALID in case of error

  @note
    Allowed From
      Threads and ISRs
    Preemption Possible
      No

  @b
    Example
  @code
    M2MB_OS_TASK_HANDLE taskHandle;
    taskHandle = m2mb_os_taskGetId();

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_TASK_HANDLE m2mb_os_taskGetId( void );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Suspend application task

  @details
    This service suspends the calling application task.
    Once it suspended, the task must be resumed by
    m2mb_os_taskResume to execute again and then it retrun M2MB_OS_SUCCESS

  @param [in] taskHandle
    handle of the task to suspend

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_INVALID_ARG
      in case of invalid taskHandle
    M2MB_OS_SUSPEND_ERROR
      task is already in suspended or terminated state
    M2MB_OS_CALLER_ERROR
      invalid caller

  @note
    It's not allow for one task to suspend another task.
    taskHandle must be the calling task's handle, otherwise it return M2MB_OS_CALLER_ERROR

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskSuspend( M2MB_OS_TASK_HANDLE taskHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
   Resume suspended application task

  @details
    This service resumes or prepares for execution a task that was
    previously suspended by a m2mb_os_taskSuspend call. In addition, this
    service resumes tasks that were created without an automatic start.

  @param [in] taskHandle
    handle of the task to resume

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_INVALID_ARG
      in case of invalid taskHandle
    M2MB_OS_SUSPEND_LIFTED
      Previously set delayed suspension was lifted
    M2MB_OS_RESUME_ERROR
      task is not suspended or it is suspended for reasons other than m2mb_os_taskSuspend call
    M2MB_OS_CALLER_ERROR
      invalid caller

  @note
    Allowed From
      Initialization, tasks, timers, and ISRs

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskResume( M2MB_OS_TASK_HANDLE taskHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
   Terminates application task

  @details
    This service terminates the specified application task regardless of
    whether the task is suspended or not. A task may call this service to
    terminate itself.
    After being terminated, the task must be restarted through m2mb_os_taskRestart
    for it to execute again.

  @param [in] taskHandle
    handle of the task to terminate

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_INVALID_ARG
      invalid taskHandle
    M2MB_OS_CALLER_ERROR
      invalid caller
    M2MB_OS_CALLER_ERROR
      invalid caller

  @note
    Allowed From
     tasks and timers
    Preemption Possible
     Yes

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskTerminate( M2MB_OS_TASK_HANDLE taskHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
   Task restart

  @details
    This service resets the specified task to execute at the entry point
    defined during task creation. The task must be in either a
    M2MB_OS_TASK_STATE_COMPLETED or M2MB_OS_TASK_STATE_TERMINATED state for it to be restarded
    It is in:
    M2MB_OS_TASK_STATE_COMPLETED when return of its entry function has been reached
    M2MB_OS_TASK_STATE_TERMINATED when it has been terminated by m2mb_os_taskTerminate

  @param [in] taskHandle
    handle of the task to be restarted

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_INVALID_ARG
      invalid taskHandle
    M2MB_OS_NOT_DONE
     specified task is not in compliant state to be restarted
    M2MB_OS_CALLER_ERROR
      invalid caller


  @note
    Allowed From
      Tasks

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskRestart( M2MB_OS_TASK_HANDLE taskHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
   Task delete

  @details
   This service deletes the specified application task. Since the specified
   task must be in a terminated or completed state, this service cannot be
   called from a task attempting to delete itself.


  @param [in\out] taskHandle
    handle of the task to be deleted

  @return
    M2MB_OS_SUCCESS in case of success

  @return
    M2MB_OS_TASK_ERROR
      Invalid application task pointer
    M2MB_OS_DELETE_ERROR
      Specified thread is not in a terminated or completed state
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service

  @note
  It is the application's responsibility to manage the memory area associated with the tasks’s stack,
  which is available after this service completes. All the resources will be released and the
  taskHandle set to M2MB_OS_TASK_INVALID
  In addition, the application must prevent use of a deleted task.

    Allowed From
      Tasks and timers
    Preemption Possible
     No

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskDelete( M2MB_OS_TASK_HANDLE taskHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Make task to coperate by releasing scheduler control

  @details
    This service give control to scheduler if other tasks are ready-to-run
    at the same or higher priority.
    This is a method to allow other tasks at the same priority to be executed.
    Think about while1 tasks...

  @return
    M2MB_OS_FEATURE_NOT_SUPPORTED always

  @note
    This function is implemented as dummy function and always return M2MB_OS_FEATURE_NOT_SUPPORTED
    for the compatibility with other products
    
    Allowed From
     Tasks
    Preemption Possible
     Yes

  @b
   Example
  @code
    //suppose to have 2 tasks at the same priority
    void entryTask1 (void *arg)
    {
      while(1)
      {
        // operation of task1
        // only task1 can run: processor dedicated to it
        m2mb_os_taskCooperate(); //let task2 to be executed
      }
    }

    void entryTask2 (void *arg)
    {
      while(1)
      {
        // operation of task2
        // only task2 can run: processor dedicated to it
        m2mb_os_taskCooperate(); //let task1 to be executed
      }
    }

    //so after every cicle, tasks will give room to others to be executed
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskCooperate( void );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Abort suspension of specified task

  @details
   This service aborts sleep or any other object suspension of the specified
   task. If the wait is aborted, a M2MB_OS_WAIT_ABORTED value is returned
   from the service that the task was waiting on.
   This service does not release explicit suspension that is made by the
   m2mb_os_taskSuspend service.

  @param [in] taskHandle
    handle of the task

  @return
    M2MB_OS_FEATURE_NOT_SUPPORTED always

  @note
    This function is implemented as dummy function and always return M2MB_OS_FEATURE_NOT_SUPPORTED
    for the compatibility with other products
    Allowed From
     Initialization, tasks, timers, and ISRs
    Preemption Possible
     Yes

  @b
    Example of use:
  @code
    void entry2( void *arg );
    void entry3( void *arg );

    M2MB_OS_TASK_HANDLE taskHandle2 = M2MB_OS_TASK_INVALID;
    M2MB_OS_TASK_HANDLE taskHandle3 = M2MB_OS_TASK_INVALID;
    M2MB_OS_SEM_HANDLE sem;
    M2MB_OS_TASK_ATTR_HANDLE taskAttrHandle;
    M2MB_OS_SEM_ATTR_HANDLE semAttr;

    {

      //create a semaphore
      m2mb_os_sem_setAttrItem( &semAttr,
                             CMDS_ARGS(
                                        M2MB_OS_SEM_SEL_CMD_CREATE_ATTR,NULL,
                                        M2MB_OS_SEM_SEL_CMD_NAME,"MySem",
                                        M2MB_OS_SEM_SEL_CMD_TYPE,0,
                                        M2MB_OS_SEM_SEL_CMD_COUNT, 0
                                      )
                           );
      m2mb_os_sem_init( &sem, &semAttr );


      m2mb_os_taskSetAttrItem( &taskAttrHandle,
                               CMDS_ARGS(
                                 M2MB_OS_TASK_SEL_CMD_CREATE_ATTR, NULL,
                                 M2MB_OS_TASK_SEL_CMD_STACK_SIZE, 2048,
                                 M2MB_OS_TASK_SEL_CMD_NAME, "myTask2",
                                 M2MB_OS_TASK_SEL_CMD_PRIORITY, 204,
                                 M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH, 204,
                                 M2MB_OS_TASK_SEL_CMD_AUTOSTART, M2MB_OS_TASK_AUTOSTART,
                                 M2MB_OS_TASK_SEL_CMD_USRNAME, "myUserTask2"
                               )
                             );
      m2mb_os_taskCreate( &taskHandle2, &taskAttrHandle, entry2, ( void * )2 );

      // arithmetic operation MUST BE INCLUDED in brackets: e.g  M2MB_OS_TASK_SEL_CMD_PRIORITY, ( 200 + OFFSET ),
      m2mb_os_taskSetAttrItem( &taskAttrHandle,
                               CMDS_ARGS(
                                 M2MB_OS_TASK_SEL_CMD_CREATE_ATTR, NULL,
                                 M2MB_OS_TASK_SEL_CMD_STACK_SIZE, 2048,
                                 M2MB_OS_TASK_SEL_CMD_NAME, "myTask3",
                                 M2MB_OS_TASK_SEL_CMD_PRIORITY, 205,
                                 M2MB_OS_TASK_SEL_CMD_PREEMPTIONTH, 205,
                                 M2MB_OS_TASK_SEL_CMD_AUTOSTART, M2MB_OS_TASK_AUTOSTART,
                                 M2MB_OS_TASK_SEL_CMD_USRNAME, "myUserTask3"
                               )
                             );
      m2mb_os_taskCreate( &taskHandle3, &taskAttrHandle, entry3, ( void * )3 );

      ...
    }

    void entry2( void *arg )
    {
      UINT32 id = ( UINT32 )arg;
      M2MB_OS_RESULT_E osRes;
      osRes = m2mb_os_sem_get( sem, M2MB_OS_WAIT_FOREVER );
      ... printf( "\r\n Task [%d], Result: %d ", id, osRes);
    }

    void entry3( void *arg )
    {
      UINT32 id = ( UINT32 )arg;
      M2MB_OS_RESULT_E osRes;
      osRes = m2mb_os_taskWaitAbort( taskHandle2 );
      ... printf( "\r\n Task [%d], Result: %d ", id, osRes);
    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_taskWaitAbort( M2MB_OS_TASK_HANDLE taskHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get system time in ticks

  @details
    This service returns the contents of the internal system clock. Each timer-
    tick increases the internal system clock by one. The system clock is set to
    zero during initialization and can be changed to a specific value by the
    service m2mb_os_setSysTicks.

  @return
   M2MB_OS_FEATURE_NOT_SUPPORTED always

  @note    
    This function is implemented as dummy function and always return M2MB_OS_FEATURE_NOT_SUPPORTED
    for the compatibility with other products
    
    The actual time each timer-tick represents is application specific.
    Please check your system
    Allowed From
      Initialization, tasks, timers, and ISRs
    Preemption Possible
      No

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
MEM_W m2mb_os_getSysTicks( void );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set start system time in ticks

  @details
    This service sets the internal system clock to the specified value. Each
    timer-tick increases the internal system clock by one.


  @param [in] newTick
    new value to be set in system clock

  @return
   M2MB_OS_SUCCESS only

  @note
    The actual time each timer-tick represents is application specific.
    Please check your system to verify if this feature is even supported
    Allowed From
      Threads, timers, and ISRs
    Preemption Possible
      No

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_setSysTicks( UINT32 newTick );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get duration of system tick in ms

  @details
    This service return the value of the duration in ms of tick used inside system

  @param [out] pTickDuration
    pointer to address of variable to be updated

  @return
    duration of a system tick in case of success
  @return
    0 in case of errors or service not available

  @note
    Please check your system to verify if this feature is even supported

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
FLOAT32 m2mb_os_getSysTickDuration_ms( void );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
   Allow to get Errno saved in relative task environment

  @details
    This service allow to get last error number task safe, with the specified type
    of category for the error.
    It checks the error in the environment task in which the function is called,
    so to check the error, call it with this in mind

  @param [out] pCategory
    pointer for environment category the error is relative

  @param [out] pErrno
    pointer to the variable where to set the error

  @return
    M2MB_OS_INVALID_ARG if a not valid or recognized task calls the service
  @return
   M2MB_OS_SUCCESS
     for success

  @note

  @b
    Example
  @code
    INT32 pErrno;
    UINT32 pCategory;
    M2MB_OS_RESULT_E res;

    //get last error if any
    res = m2mb_os_get_errno( &pCategory, &pErrno );
    // the error will be in the form: category - error
    // e.g M2MB_OS_CAT - M2MB_OS_QUEUE_ERROR if res = M2MB_OS_SUCCESS
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_get_errno( UINT32 *pCategory, INT32 *pErrno );

/*-----------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Returns dynamic memory (HEAP) information

  @details
    This function returns dynamic memory (HEAP) information:
          -the total number of memory fragments
          -the total number of available bytes
          -the total memory size

  @param [in] *memInfoSelect
    pointer of information required type: M2MB_OS_MEMINFO_E

  @param [out] *pOut
    pointer of information required value

  @return
    M2MB_OS_SUCCESS
    for success
  @return
    others in case of error : see m2mb_os_types.h

  @note
    Heap memory information change whenever querying HEAP usage with this function
    because there are other applications running in the background and HEAP shared by other applications
    
  @b
    Example
  @code

    M2MB_OS_RESULT_E res;
    MEM_W fragments, bytes_available, size;

    res = m2mb_os_memInfo( M2MB_OS_MEMINFO_FRAGMENTS, &fragments );
    if ( res != M2MB_OS_SUCCESS )
    {
      PrintToUart("\n Memory Information fragments error %d", res);
      return;
    }
    res = m2mb_os_memInfo( M2MB_OS_MEMINFO_BYTES_AVAILABLE, &bytes_available );
    if ( res != M2MB_OS_SUCCESS )
    {
      PrintToUart("\n Memory Information bytes available error %d", res);
      return;
    }
    res = m2mb_os_memInfo( M2MB_OS_MEMINFO_SIZE, &size );
    if ( res != M2MB_OS_SUCCESS )
    {
      PrintToUart("\n Memory Information size error %d", res);
      return;
    }

    PrintToUart("\n Memory Informations: \n MEMINFO_FRAGMENTS: %d \n BYTES_AVAILABLE: %d \n MEMORY_SIZE: %d \n", fragments, bytes_available, size);

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_memInfo( M2MB_OS_MEMINFO_E memInfoSelect, MEM_W *pOut );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Dynamic memory reallocation

  @details
    Changes the size of the memory block pointed to by ptr.

    The function may move the memory block to a new location (whose address is returned by the function).

    The content of the memory block is preserved up to the lesser of the new and old sizes, even if the block is moved to a new location. If the new size is larger, the value of the newly allocated portion is indeterminate.

    In case that ptr is a null pointer, the function behaves like malloc, assigning a new block of size bytes and returning a pointer to its beginning.

  @param [in] *ptr
    Pointer to a memory block previously allocated with malloc, calloc or realloc.

  @param [in] size
    New size for the memory block, in bytes.

  @return
    A pointer to the reallocated memory block, which may be either the same as ptr or a new location.
    In case of error, return null pointer

  @note

  @b
    Example
  @code

    UINT32 *ptr;
    UINT32 *new_ptr;
    UINT32 size;
    UINT32 new_size;

    ptr = ( UINT32* ) m2mb_os_calloc ( size * sizef( UINT32 ));

    new_ptr = ( UINT32* ) m2mb_os_realloc (( void *) ptr, new_size * sizeof ( UINT32 ))
    if ( new_ptr == NULL )
    {
      PrintToUart("\n Dynamic memory reallocation error \n");
      return;
    }
    PrintToUart("\n [%d UINT32] dynamic memories were reallocated with the new pointer %d",new_size, new_ptr);

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
void *m2mb_os_realloc( void *ptr, UINT32 size );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    total and current RAM available for applications

  @param [out] *p_total
    pointer to total RAM for application

  @param [out] *p_available
    pointer to currently available memory

  @return
    M2MB_OS_SUCCESS for success
    others in case of error : see m2mb_os_types.h
    in case of failure the total and available RAM is set to 0
    
  @note
    RAM information change whenever querying RAM usage with this function.
    because there are other applications running in the background and RAM shared by other applications
    
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_RAMAvail( MEM_W *p_total, MEM_W *p_available );




#endif /* M2MB_OS_H */
