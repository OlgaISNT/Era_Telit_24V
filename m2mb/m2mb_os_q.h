/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_os_q.h

  @brief
    OS queue prototypes

  @details
    basic functios for OS queue are provided

  @notes
    Dependencies:
             m2mb_types.h
             m2mb_os_types.h

  @author
    Alessandro Papagno

  @date
     03/03/2017
*/


#ifndef M2MB_OS_Q_H
#define M2MB_OS_Q_H



/* Global declarations ==========================================================================*/
#define  M2MB_OS_Q_INVALID           0
#define  M2MB_OS_Q_INVALID_ATTR      0
#define  M2MB_OS_Q_TX_PRIORITIZE     1

/* used to get how many word32 are needed for specified "type" struct : see M2MB_OS_Q_SEL_CMD_MSG_SIZE */
#define WORD32_FOR_MSG( type )     ( ( sizeof( type ) % 4 ) == 0 ? sizeof( type ) / 4 : sizeof( type ) / 4 + 1 )
/* then for decide how many bytes to reserve for any message in the queue : see M2MB_OS_Q_SEL_CMD_QSIZE */
#define BYTES_FOR_MSG( type )       ( 4 * WORD32_FOR_MSG( type ) )

/*-----------------------------------------------------------------------------------------------

    can be called directly without specify CMDS_ARGS
    osRes = m2mb_os_q_setAttrItem_( &qAttrHandle,
                                    M2MB_OS_Q_SEL_CMD_CREATE_ATTR, NULL,
                                    M2MB_OS_Q_SEL_CMD_NAME, "Myqueue_T",
                                    M2MB_OS_Q_SEL_CMD_QSTART, &QueueArea,
                                    M2MB_OS_Q_SEL_CMD_MSG_SIZE, WORD32_FOR_MSG( MY_MSG_T ),
                                    M2MB_OS_Q_SEL_CMD_QSIZE, sizeof(QueueArea)
                                  );
  -----------------------------------------------------------------------------------------------*/
#ifdef _WIN32
#define m2mb_os_q_setAttrItem_( h, ... )        m2mb_os_q_setAttrItem( h, CMDS_ARGS( __VA_ARGS__ ) )
#else
#define m2mb_os_q_setAttrItem_( h, args... )        m2mb_os_q_setAttrItem( h, CMDS_ARGS( args ) )
#endif

/* Global typedefs --------------------------------------------------------------- */

typedef enum
{
  M2MB_OS_Q_SEL_CMD_CREATE_ATTR,
  M2MB_OS_Q_SEL_CMD_DEL_ATTR,
  /* use as Os name: only available for creation */
  M2MB_OS_Q_SEL_CMD_NAME,
  /* if set, usr name could be different from Os used in creation  */
  M2MB_OS_Q_SEL_CMD_USRNAME,
  /* size of message in 32 bit words: 1 to 16 max */
  M2MB_OS_Q_SEL_CMD_MSG_SIZE,
  /* if not specified will be allocated by system: if specified align to MEM_W
     to avoid possible loosing space for 1 message
  */
  M2MB_OS_Q_SEL_CMD_QSTART,
  /* size in byte */
  M2MB_OS_Q_SEL_CMD_QSIZE,
  /* number of task suspended on the queue */
  M2MB_OS_Q_SEL_CMD_SUSPENDED_COUNT,
  /* first task (M2MB_OS_TASK_HANDLE) to be served when msg will be enqueued */
  M2MB_OS_Q_SEL_CMD_1ST_SUSP,
  /* number of messages enqueued on the queue*/
  M2MB_OS_Q_SEL_CMD_ENQUEUED,
  /* number of messages the queue currently has space for */
  M2MB_OS_Q_SEL_CMD_QROOM,

  /* ---- */
  M2MB_OS_Q_SEL_CMD_END,
  ENUM_TO_INT( M2MB_OS_Q_SEL_CMD_E )
} M2MB_OS_Q_SEL_CMD_E;


typedef struct M2MB_OS_Q_HANDLE_TAG      *M2MB_OS_Q_HANDLE;
typedef struct M2MB_OS_Q_ATTR_HANDLE_TAG *M2MB_OS_Q_ATTR_HANDLE;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Message Queue creation

  @details
    This service creates a message queue that is typically used for inter-
    task communication. The total number of messages is calculated from
    the specified message size and the total number of bytes in the queue.
    Specifies the size of each message in the queue.
    Message sizes range from 1 32-bit word to 16 32-bit words. Valid message size options are
    numerical values from 1 through 16, inclusive.

  @param [out] pQHandle
    pointer to queue handle
  @param [in] pQAttrHandle
    pointer to queue attribute handle

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_QUEUE_ERROR
     Invalid counting queue handle
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_SIZE_ERROR
      Size of message queue is invalid: for example no space for at least 1 msg
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    If the total number of bytes specified in the queue’s memory area is not
    evenly divisible by the specified message size, the remaining bytes in the
    memory area are not used.
    After successful queue creation, queue attribute will be deleted, their resources free
    and pQAttr set to M2MB_OS_Q_INVALID_ATTR
    Pay attention if start of queue is provided: since message size is multiple of word32 ( 32 bit word )
    better to specify q area as UINT32 array, whose size has to be multiple of message size in word32.
    In case start of q area is not aligned on word32, a space for 1 message can be lost due to internal
    alignement


    Allowed From
      Initialization and tasks
    Preemption Possible
      No

  @b
    Example
  @code
    #define N_MSG          100
    M2MB_OS_RESULT_E       osRes;
    M2MB_OS_Q_ATTR_HANDLE  qAttrHandle;
    M2MB_OS_Q_HANDLE       queueHandle;
    UINT32                 msgSize = 5;           // 5 * 4Byte => 20 bytes; message sizes range from 1 32-bit word to 16 32-bit words. Valid message size options are numerical values from 1 through 16, inclusive.
    //or
    // in case user struct
    // typedef struct
    // {
    // WORD32_FOR_MSG( MY_MSG_T )  can be used to get msgSize for MY_MSG_T struct used to transfer infos in queue
    // suggest to use MEM_W to align : see M2MB_OS_Q_SEL_CMD_QSTART description
    // in this way
    static UINT32  QueueArea[ (N_MSG * 5*4) / sizeof( UINT32) + 1];  // just to contain  100 of  5 word32
                                                                   // message size =>20 byte each msg

    or just simply

    static UINT32 QueueArea[ (N_MSG * 5) ]; // since already aligned to UINT32

    //or
    // in case user struct: suggested to use little struct with pointers inside to avoid excessive RAM usage
    // typedef struct
    // {
    //   void a;
    //   UINT32 b;
    //   UINT16 c[2];
    //   ...
    // }MY_MSG_T;
    //
    // static UINT8 QueueArea[ ( N_MSG * BYTES_FOR_MSG( MY_MSG_T ) ) / sizeof( UINT32) + 1  ];
    // or better
    // static UINT32 QueueArea[ N_MSG * WORD32_FOR_MSG( MY_MSG_T ) ];
    // WORD32_FOR_MSG( MY_MSG_T )  can be used to get msgSize for MY_MSG_T struct used to transfer infos in queue
    // m2mb_os_q_setAttrItem( &qAttrHandle,
    //                        CMDS_ARGS
    //                        (
    //                          M2MB_OS_Q_SEL_CMD_CREATE_ATTR, NULL,
    //                          M2MB_OS_Q_SEL_CMD_NAME, "Myqueue_T",
    //                          M2MB_OS_Q_SEL_CMD_QSTART, &QueueArea,
    //                          M2MB_OS_Q_SEL_CMD_MSG_SIZE, WORD32_FOR_MSG( MY_MSG_T ),
    //                          M2MB_OS_Q_SEL_CMD_QSIZE, sizeof(QueueArea)
    //                        )
    //                       );

    static UINT32 QueueArea[N_MSG * 5 ];  // just to contain  100 of  5 word32
                                          // message size =>20 byte each msg

    // or static UINT8 QueueArea[ N_MSG * BYTES_FOR_MSG( MY_MSG_T ) ]; if usr  MY_MSG_T is used
    if ( m2mb_os_q_setAttrItem(  &qAttrHandle, 1, M2MB_OS_Q_SEL_CMD_CREATE_ATTR,  NULL  ) != M2MB_OS_SUCCESS )
        exit(error_creating attribute queueHandle);

    osRes = m2mb_os_q_setAttrItem( &qAttrHandle, 4,
                                   M2MB_OS_Q_SEL_CMD_NAME, M2MB_OS_Q_SEL_CMD_MSG_SIZE,
                                   M2MB_OS_Q_SEL_CMD_QSTART, M2MB_OS_Q_SEL_CMD_QSIZE,
                                   "Myqueue", msgSize, &QueueArea, sizeof(QueueArea)
                                 );
    if ( m2mb_os_q_setAttrItem(  &qAttrHandle, 1, M2MB_OS_Q_SEL_CMD_CREATE_ATTR,  NULL  ) != M2MB_OS_SUCCESS )
        exit(error_creating attribute queueHandle);

    osRes = m2mb_os_q_setAttrItem( &qAttrHandle, 4,
                                   M2MB_OS_Q_SEL_CMD_NAME, M2MB_OS_Q_SEL_CMD_MSG_SIZE,
                                   M2MB_OS_Q_SEL_CMD_QSTART, M2MB_OS_Q_SEL_CMD_QSIZE,
                                   "Myqueue", msgSize, &QueueArea, sizeof(QueueArea)
                                 );


    // or simply using CMDS_ARGS(...) macro
    // m2mb_os_q_setAttrItem( &qAttrHandle,
    //                        CMDS_ARGS
    //                        (
    //                          M2MB_OS_Q_SEL_CMD_NAME, "Myqueue",
    //                          M2MB_OS_Q_SEL_CMD_QSTART, &QueueArea,
    //                          M2MB_OS_Q_SEL_CMD_MSG_SIZE, msgSize,
    //                          M2MB_OS_Q_SEL_CMD_QSIZE, sizeof(QueueArea)
    //                        )
    //                       );

   if ( osRes != M2MB_OS_SUCCESS )
       exit(...)

    m2mb_os_q_init( &queueHandle, &qAttrHandle );

    // by not specifying QueueArea, or setting to NULL
    // OS will provide allocation for qsize: the number of messages that queueHandle can keep depend
    // on the size specified in M2MB_OS_Q_SEL_CMD_MSG_SIZE
    // e.g queuesize = 2000 and msgSize = 5 (20 bytes) => max msgs = 2000/20 = 100

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_q_init( M2MB_OS_Q_HANDLE *pQHandle, M2MB_OS_Q_ATTR_HANDLE *pQAttrHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Message Queue Deinit or Deletion

  @details
    This service deletes the specified message queue. All tasks suspended
    waiting for a message from this queue are resumed with a return value egual to M2MB_OS_DELETED
    The resources will be released
    It is the application's responsibility to manage the memory area associated with the queue,
    which is available after this service completes, if qStart with M2MB_OS_Q_SEL_CMD_QSTART
    was specified.
    It is the application's responsibility to prevent use of a deleted queue.

  @param [in] qHandle
     handle of the queue

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_QUEUE_ERROR
     Invalid counting queue handle
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    It is the application's responsibility to prevent use of a deleted
    queue.
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
M2MB_OS_RESULT_E m2mb_os_q_deinit( M2MB_OS_Q_HANDLE qHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Receive message from the queue

  @details
    This service retrieves a message from the specified message queue identified by its handle.
    The retrieved message is copied from the queue into the memory area
    specified by the destination pointer. That message is then removed from
    the queue.

  @param [in] qHandle
    handle of queue
  @param [in] pDest
    pointer to destination where to copy the received message
  @param [in] timeoutTicks
    If timeout is specified and messages are not available on queue, task will be suspended for a maximum
    time of timeout ticks

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_SUCCESS
     Successful retrieval of a queue instance.
    M2MB_OS_DELETED
     queue was deleted while task was suspended.
    M2MB_OS_WAIT_ABORTED
     Suspension was aborted by another task, timer, or ISR.
    M2MB_OS_QUEUE_ERROR
     Invalid counting queue handle.
    M2MB_OS_WAIT_ERROR
     A wait option other than M2MB_OS_NO_WAIT was specified on a call from a non-task.
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    The specified destination memory area must be large enough to hold the
    message; i.e., the message destination pointed to by pDest
    must be at least as large as the message size for this queue. Otherwise,
    if the destination is not large enough, memory corruption occurs in the
    following memory area.
    Allowed From
      Initialization, tasks, timers and ISRs
    Preemption Possible
      Yes

  @b
    Example
  @code
    static UINT32 myMsg[5];
    m2mb_os_q_rx( myQHandle, myMsg, M2MB_OS_WAIT_FOREVER );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_q_rx( M2MB_OS_Q_HANDLE qHandle, void *pDest, UINT32 timeoutTicks );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Send message to the queue

  @details
    This service sends a message to the specified message queue identified by its handler. The sent
    message is copied to the queue from the memory area specified by the
    pSource pointer. If priority M2MB_OS_Q_TX_PRIORITIZE or a value higher then 0 is selected,
    the message is placed at the top of the queue, therefore first to be received

  @param [in] qHandle
    handle of queue
  @param [in] pSource
    pointer of Source message to be sent in queue
  @param [in] timeoutTicks
    If timeout is specified and there is not room available on queue, task will be suspended for a maximum
    numbers of timeout ticks
  @param [in] priority
    with priority > 0 or M2MB_OS_Q_TX_PRIORITIZE the message is copied to the front of the queue from
    the memory area specified by the pSource pointer

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_SUCCESS
     Successful queue put.
    M2MB_OS_QUEUE_ERROR
     Invalid queue handle
    M2MB_OS_CALLER_ERROR
     Invalid caller of this service.
    M2MB_OS_QUEUE_FULL
     Service was unable to send message because the queue was full for the duration of the specified time to wait.
    M2MB_OS_WAIT_ERROR
     A wait option other than M2MB_OS_NO_WAIT was specified on a call from a non-task.
    M2MB_OS_WAIT_ABORTED
     Suspension was aborted by another task, timer, or ISR.

  @note
    Allowed From
      Initialization, tasks timers and ISRs
    Preemption Possible
     Yes

  @b
    Example
  @code
    //Send a message to "my_queue." Return immediately regardless of success.
    //This wait option is used for calls from initialization, timers, and ISRs.
    // make use of prioritization; put highest task waiting on the queue at the top of priority list
    m2mb_os_q_tx( myQHandle, myMsg, M2MB_OS_NO_WAIT, M2MB_OS_Q_TX_PRIORITIZE);
    // or simply send message with no prioritization
    m2mb_os_q_tx( myQHandle, myMsg, M2MB_OS_NO_WAIT, 0);
    //If status equals M2MB_OS_SUCCESS, the message is in the queue
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_q_tx(
  M2MB_OS_Q_HANDLE qHandle,
  void *pSource,
  UINT32 timeoutTicks,
  UINT8 priority
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Prioritize queue suspension list
    Set HPTF: Highest Priority Task First to receive the message from queue (one shot)

  @details
    This service set the task waiting for this queue which has currently highest priority,
    on the top of FIFO suspension list

  @param [in] queueHandle
    handle of queue

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
    Example:
    Set highest priority task waiting on the queue, at the top of FIFO suspension list
  @code
    M2MB_OS_Q_HANDLE queueHandle;
    //...
    //already created... used queue
    //just put highest priority task on top of suspension list
    m2mb_os_q_hptf( queueHandle);

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_q_hptf( M2MB_OS_Q_HANDLE queueHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Clear all messages from the queue

  @details
    This service deletes all messages stored in the specified message queue identified by its handle.
    If the queue is full, messages of all suspended tasks are discarded.
    Each suspended task is then resumed with a return status that
    indicates the message send was successful. If the queue is empty, this
    service does nothing.

  @param [in] qHandle
     handle of queue to be clear

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_SUCCESS
     Successful queue operation.
    M2MB_OS_QUEUE_ERROR
     Invalid counting queue handle

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
M2MB_OS_RESULT_E m2mb_os_q_clear( M2MB_OS_Q_HANDLE qHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific attribute for queue creation

  @details
    This service set the attribute (or create it) to be used for queue creation
    Selecting cmd, a parameter to be set can be selected


  @param [in] pQAttrHandle
   pointer to event attribute of event
  @param [in] nCmds
    number of commands to be issued at once
  @param [in] ...
    variable inputs, N cmds (see M2MB_OS_Q_SEL_CMD_E) followed by N arguments
    for their respective issued commands. CMDS(...) and CMDS_ARGS(...) macro can be
    used to better insert cmd and parameters and to better control right insertion

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_SUCCESS
     Successful queue operation.
    M2MB_OS_QUEUE_ERROR
     Invalid counting queue handle
    M2MB_OS_INVALID_ARG
      invalid pQAttr or pIn

  @note
    The attribute set are useful for queue creation.
    Once created, use instead m2mb_os_q_setItem() to set only available queue properties
    After m2mb_os_q_init, resources of M2MB_OS_Q_ATTR_HANDLE will be released and handle set to
    M2MB_OS_Q_INVALID_ATTR
    It is under application responsibility the right use of m2mb_os_q_setAttrItem.
    For instance, multi creation of attrHandle withouth free of previous resources
    ( M2MB_OS_Q_SEL_CMD_CREATE_ATTR without M2MB_OS_Q_SEL_CMD_DEL_ATTR in case
      no m2mb_os_q_init is used in the between ) will waste memory.

  @b
    Example
  @code

    M2MB_OS_RESULT_E       osRes;
    M2MB_OS_Q_ATTR_HANDLE  qAttrHandle;
    M2MB_OS_Q_HANDLE       queueHandle;
    UINT32                 msgSize = 5 ; //  Message sizes range from 1 32-bit word to 16 32-bit words. Valid message size options are numerical values from 1 through 16, inclusive.
    static UINT32          QueueArea[100 * 5 ];  // just to contain  100 of
                                                 // 5 word32 message size =>20 byte each msg

    if ( m2mb_os_q_setAttrItem( &qAttrHandle, 1, M2MB_OS_Q_SEL_CMD_CREATE_ATTR,  NULL ) != M2MB_OS_SUCCESS )
        exit(error_creating attribute queueHandle);

    osRes = m2mb_os_q_setAttrItem( &qAttrHandle, 4,
                                   M2MB_OS_Q_SEL_CMD_NAME, M2MB_OS_Q_SEL_CMD_QSTART,
                                   M2MB_OS_Q_SEL_CMD_MSG_SIZE, M2MB_OS_Q_SEL_CMD_QSIZE,
                                   Myqueue", &QueueArea, msgSize, sizeof(QueueArea)
                                 )

    // or using CMDS_ARGS(...) macro
    // m2mb_os_q_setAttrItem( &qAttrHandle,
    //                        CMDS_ARGS
    //                        (
    //                          M2MB_OS_Q_SEL_CMD_NAME, "Myqueue",
    //                          M2MB_OS_Q_SEL_CMD_QSTART, &QueueArea,
    //                          M2MB_OS_Q_SEL_CMD_MSG_SIZE, msgSize,
    //                          M2MB_OS_Q_SEL_CMD_QSIZE, sizeof(QueueArea)
    //                        )
    //                       );

   if ( osRes != M2MB_OS_SUCCESS )
       exit(...)

    m2mb_os_q_init( &queueHandle, &qAttrHandle );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_q_setAttrItem( M2MB_OS_Q_ATTR_HANDLE *pQAttrHandle, UINT8 nCmds, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get specific properties of the queue

  @details
    This service retrieve value for specified parameter of the specified queue
    Selecting cmd, a parameter to be retrieved can be set
    To be used when queue is already created: its right use is under application responsibility

  @param [in] qHandle
    handle of queue
  @param [in] selCmd
    Select parameter to be retrieved (see M2MB_OS_Q_SEL_CMD_E)
  @param [out] pOut
     pointer Output parameter got: its meaning depend on selCmd used See examples
     as general pointer it is set to UINT32
  @param [in] pIn
     pointer to void general Parameter to be specified, if any, to get right output See examples
     If parameter is not needed, use NULL

  @return
   M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_QUEUE_ERROR
     Invalid counting queue handle
    M2MB_OS_INVALID_ARG
      invalid arguments

  @note
    Not all the M2MB_OS_Q_SEL_CMD_E are always available.
    Of course M2MB_OS_Q_SEL_CMD_CREATE_ATTR is not right for this function
    Allowed:
    M2MB_OS_Q_SEL_CMD_NAME
    M2MB_OS_Q_SEL_CMD_USRNAME
    M2MB_OS_Q_SEL_CMD_MSG_SIZE
    M2MB_OS_Q_SEL_CMD_QSTART
    M2MB_OS_Q_SEL_CMD_QSIZE
    M2MB_OS_Q_SEL_CMD_1ST_SUSP
    M2MB_OS_Q_SEL_CMD_SUSPENDED_COUNT
    M2MB_OS_Q_SEL_CMD_ENQUEUED
    M2MB_OS_Q_SEL_CMD_QROOM


  @b
    Example
  @code
    M2MB_OS_Q_ATTR_HANDLE       qAttrHandle;
    M2MB_OS_Q_HANDLE            queueHandle;
    MEM_W                       qRoom;
    MEM_W                       enqueueHandled;
    MEM_W                       suspended, qsize;
    M2MB_OS_TASK                fisrtEnq;


    m2mb_os_q_getItem( queueHandle, M2MB_OS_Q_SEL_CMD_SUSPENDED_COUNT,  &suspended, NULL );
    m2mb_os_q_getItem( queueHandle, M2MB_OS_Q_SEL_CMD_ENQUEUED, &enqueueHandled, NULL );
    m2mb_os_q_getItem( queueHandle, M2MB_OS_Q_SEL_CMD_QROOM, &qRoom, NULL );
    m2mb_os_q_getItem( queueHandle, M2MB_OS_Q_SEL_CMD_QSIZE, &qsize, NULL );

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_q_getItem(
  M2MB_OS_Q_HANDLE qHandle,
  M2MB_OS_Q_SEL_CMD_E selCmd,
  MEM_W *pOut,
  void *pIn
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific properties of the queue

  @details
    This service set value for specified parameter of the specified queue if available
    Selecting cmd, a parameter to be set can be selected
    To be used when queue is already created: its right use is under application responsibility


  @param [in] qHandle
    handle of queue
  @param [in] selCmd
   Select parameter to be set (see M2MB_OS_Q_SEL_CMD_E)
  @param [in] pIn
    pointer to value to be set. If a simple value, can be easily casted to (void*)
    e.g. int value -3 has to be passed, use (void*)-3
    NULL if no value is needed

  @return
   M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_QUEUE_ERROR
     Invalid counting queue handle
    M2MB_OS_INVALID_ARG
      invalid arguments

  @note
    Not all the M2MB_OS_Q_SEL_CMD_E are always available.
    Of course M2MB_OS_Q_SEL_CMD_CREATE_ATTR is not right for this function
    Only
    M2MB_OS_Q_SEL_CMD_USRNAME
    is allowed


  @b
    Example
  @code
    m2mb_os_q_setItem(queueHandle, M2MB_OS_Q_SEL_CMD_USRNAME, (void*)"ThisIsUserName");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_q_setItem(
  M2MB_OS_Q_HANDLE qHandle,
  M2MB_OS_Q_SEL_CMD_E selCmd,
  void *pIn
);



#endif /* M2MB_OS_H */

