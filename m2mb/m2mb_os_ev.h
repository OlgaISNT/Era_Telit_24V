/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_os_ev.h

  @brief
    OS event prototypes

  @details
    basic functions for OS event are provided

  @notes
    Dependencies:
             m2mb_types.h
             m2mb_os_types.h

  @author
    <Alessandro Papagno>

  @date
     03/03/2017
*/


#ifndef M2MB_OS_EV_H
#define M2MB_OS_EV_H



/* GLOBAL DECLARATIONS ----------------------------------------------------------- */
#define  M2MB_OS_EV_INVALID      0
#define  M2MB_OS_EV_INVALID_ATTR 0



/* Global declarations ==========================================================================*/
/*-----------------------------------------------------------------------------------------------

    can be called directly without specify CMDS_ARGS
    osRes = m2mb_os_ev_setAttrItem_( &evAttrHandle,
                                     M2MB_OS_EV_SEL_CMD_CREATE_ATTR, NULL,
                                     M2MB_OS_EV_SEL_CMD_NAME, "event_1",
                                     M2MB_OS_EV_SEL_CMD_USRNAME, "myevent1"
                                   );
  -----------------------------------------------------------------------------------------------*/
#ifdef _WIN32
#define m2mb_os_ev_setAttrItem_( h, ... )        m2mb_os_ev_setAttrItem( h, CMDS_ARGS( __VA_ARGS__ ) )
#else
#define m2mb_os_ev_setAttrItem_( h, args... )        m2mb_os_ev_setAttrItem( h, CMDS_ARGS( args ) )
#endif



/* Global typedefs --------------------------------------------------------------- */

typedef enum
{
  M2MB_OS_EV_SEL_CMD_CREATE_ATTR,
  M2MB_OS_EV_SEL_CMD_DEL_ATTR,
  /* use as Os name , can be used during creation */
  M2MB_OS_EV_SEL_CMD_NAME,
  /* if set, usr name could be different from Os used in creation  */
  M2MB_OS_EV_SEL_CMD_USRNAME,
  /* number of task suspended on this resource */
  M2MB_OS_EV_SEL_CMD_SUSPENDED_COUNT,
  /* first task (M2MB_OS_TASK_HANDLE) to be served when its required resources will be available */
  M2MB_OS_EV_SEL_CMD_1ST_SUSP,
  /* to get the status of current bits event */
  M2MB_OS_EV_SEL_CMD_CURRENT_BITS_EVENT,

  /* ---- */
  M2MB_OS_EV_SEL_CMD_END,
  ENUM_TO_INT( M2MB_OS_EV_SEL_CMD_E )
} M2MB_OS_EV_SEL_CMD_E;

/* options for setting event bits: set or clear specified ones */
typedef enum
{
  M2MB_OS_EV_SET = 0,
  M2MB_OS_EV_CLEAR = 2,

  /* ---- */
  M2MB_OS_EV_SET_OPT_END,
  ENUM_TO_INT( M2MB_OS_EV_SET_OPT_E )
} M2MB_OS_EV_SET_OPT_E;

/* options for getting event bits: get all selected (and clear) or get any of selected (and clear matches) */
typedef enum
{
  /* get the event if any of the selected event bits are set ...*/
  M2MB_OS_EV_GET_ANY = 0,
  /* ... then clear all the set bit in selected event bits  */
  M2MB_OS_EV_GET_ANY_AND_CLEAR,
  /* get the event if all the selected event bits are set ...*/
  M2MB_OS_EV_GET_ALL,
  /* ... then clear all the set bit in selected event bits  */
  M2MB_OS_EV_GET_ALL_AND_CLEAR,

  /* ---- */
  M2MB_OS_EV_GET_OPT_END,
  ENUM_TO_INT( M2MB_OS_EV_GET_OPT_E )
} M2MB_OS_EV_GET_OPT_E;

typedef struct M2MB_OS_EV_HANDLE_TAG      *M2MB_OS_EV_HANDLE;
typedef struct M2MB_OS_EV_ATTR_HANDLE_TAG *M2MB_OS_EV_ATTR_HANDLE;


/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Application Event Set Creation

  @details
    This service creates a group of 32 event bits. All 32 event bits in the
    event are initialized to zero.

  @param [out] pEvHandle
    pointer to event handle
  @param [in] pEvAttrHandle
    pointer to event attribute handle

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_EVENT_ERROR
     Invalid application event handle: either the handle is invalid or the event is already created
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    After successfull event creation, event attribute will be deleted, their resources free
    and pEvAttr set to M2MB_OS_EV_INVALID_ATTR
    In case of error return, it is application's responsibility to release resources of attributes
    by calling m2mb_os_ev_setAttrItem( &evAttr, M2MB_OS_EV_SEL_CMD_DEL_ATTR,  NULL );
    Allowed From
      Initialization and tasks
    Preemption Possible
      No

  @b
    Example
  @code
    M2MB_OS_RESULT_E        osRes;
    M2MB_OS_EV_ATTR_HANDLE  evAttrHandle;
    M2MB_OS_EV_HANDLE       evHandle;

    osRes  = m2mb_os_ev_setAttrItem( &evAttrHandle, 2,
                                     M2MB_OS_EV_SEL_CMD_CREATE_ATTR,  M2MB_OS_EV_SEL_CMD_NAME,
                                     NULL, "MyEv"
                                   );
    // or using CMDS_ARGS(...) macro
    osRes  = m2mb_os_ev_setAttrItem( &evAttrHandle,
                                     CMDS_ARGS
                                     (
                                       M2MB_OS_EV_SEL_CMD_CREATE_ATTR, NULL,
                                       M2MB_OS_EV_SEL_CMD_NAME, "MyEv"
                                     )
                                   );

    osRes = m2mb_os_ev_init( &evHandle, &evAttrHandle );
    if ( osRes != M2MB_OS_SUCCESS )
    {
      m2mb_os_ev_setAttrItem( &evAttrHandle, M2MB_OS_EV_SEL_CMD_DEL_ATTR, NULL );
    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_ev_init( M2MB_OS_EV_HANDLE *pEvHandle, M2MB_OS_EV_ATTR_HANDLE *pEvAttrHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Application Event deinit or deletion

  @details
    This service deletes the specified os event. All tasks
    suspended waiting for this event are resumed with a return value egual to M2MB_OS_DELETED
    The resources will be released.
    It is the application’s responsibility to prevent use of a deleted
    event.

  @param [in] evHandle
    handle of the event

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_EVENT_ERROR
     Invalid event handle
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    It is the application’s responsibility to prevent use of a deleted
    event.
    Allowed From
     tasks
    Preemption Possible
     Yes

  @b
    Example
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_ev_deinit( M2MB_OS_EV_HANDLE evHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set bits of event in a specified event

  @details
    This service sets or clears event bits in an event, depending
    upon the specified set-option. All suspended tasks whose event bits
    request is now satisfied are resumed.


  @param [in] evHandle
    handle of event
  @param [in] evBits
    event bits to set\clear
  @param [in] optSet
    option for set or clear specified bit (see M2MB_OS_EV_SET_OPT_E)

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_EVENT_ERROR
     Invalid event handle.


  @note

    Allowed From
      Initialization, tasks, events and ISRs
    Preemption Possible
      Yes

  @b
    Example
  @code
     M2MB_OS_EV_HANDLE evHandle;
     M2MB_OS_RESULT_E  osRes;

    // Set event bit 0, 4, and 8.
    osRes = m2mb_os_ev_set(evHandle, (UINT32)0x111, M2MB_OS_EV_SET);

    // If osRes equals M2MB_OS_SUCCESS, the event bits have been
    // set and any suspended task whose request was satisfied
    // will be resumed.

    osRes = m2mb_os_ev_set(evHandle, (UINT32)0x100, M2MB_OS_EV_CLEAR);
    // If osRes equals M2MB_OS_SUCCESS, the event bit 8 has been
    // cleared

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_ev_set(
  M2MB_OS_EV_HANDLE evHandle,
  UINT32 evBits,
  M2MB_OS_EV_SET_OPT_E optSet
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get an application event

  @details
    This service retrieves bits event from the specified event.
    Each event contains 32 event bits.
    This service can retrieve a variety of event bits
    combinations, as selected by the input reqEvBits.


  @param [in] evHandle
    handle of event
  @param [in] reqEvBits
   requested bits event
  @param [in] optGet
    option for get the event (see M2MB_OS_EV_GET_OPT_E)
  @param [out] pCurEvBits
    pointer to where current bits events are written to. It contains all the currents bits as found before
    any actions are taken (e.g cleared as requested)
    Can be set to NULL if information is not needed
  @param [in] timeout
    wait option in case of not success in getting requested bit event

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_EVENT_ERROR
     Invalid event handle
    M2MB_OS_DELETED
      event was deleted while task was suspended
    M2MB_OS_NO_EVENTS
      Service was unable to get the specified events within the specified timeout
    M2MB_OS_WAIT_ABORTED
      Suspension was aborted by another task, timer, or ISR
    M2MB_OS_PTR_ERROR
      Invalid pointer for current event bits container
    M2MB_OS_WAIT_ERROR
      A wait option other than M2MB_OS_NO_WAIT was specified on a call from a non task

  @note
    Allowed From
     Initialization, tasks, events, and ISRs
    Preemption Possible
     Yes

  @b
    Example
  @code
    M2MB_OS_EV_HANDLE evHandle;
    UINT32            CurEvBits;
    M2MB_OS_RESULT_E  osRes;

    //Request that any of the event bits 0, 4, and 8 are set. Also, if any are set
    // the set bits in the selected events bit will be cleared.
    //If the event bits are not set, this service suspends for a maximum of 50 timer-ticks.

    osRes = m2mb_os_ev_get(evHandle, (UINT32)0x111, M2MB_OS_EV_GET_ANY_AND_CLEAR, &CurEvBits, 50 );

    // or for example 2000 ms using M2MB_OS_MS2TICKS : "m2mb_os.h" has to be included
    osRes = m2mb_os_ev_get(evHandle, (UINT32)0x111, M2MB_OS_EV_GET_ANY_AND_CLEAR, &CurEvBits,
                           M2MB_OS_MS2TICKS(2000) );

    //If status equals M2MB_OS_SUCCESS, CurEvBits contains the actual bits event obtained, before any
    //operation on them are taken, e.g clear. It means CurEvBits in the example will contain
    // any of the bits 0,4,8
    // plus in case, additional not requested. It is just previous event bits state


  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_ev_get(
  M2MB_OS_EV_HANDLE evHandle,
  UINT32 reqEvBits,
  M2MB_OS_EV_GET_OPT_E optGet,
  UINT32 *pCurEvBits,
  UINT32 timeout
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific attribute for event init or creation

  @details
    This service set the attribute (or create it) to be used for event creation


  @param [in] pEvAttrHandle
   pointer to event attribute handle of event
  @param [in] nCmds
    number of commands to be issued at once
  @param [in] ...
    variable inputs, N cmds (see M2MB_OS_EV_SEL_CMD_E) followed by N arguments
    for their respective issued commands. CMDS(...) and CMDS_ARGS(...) macro can be
    used to better insert cmd and parameters and to better control right insertion

  @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_SUCCESS
     Successful event operation.
    M2MB_OS_EVENT_ERROR
     Invalid counting event handle
    M2MB_OS_INVALID_ARG
      invalid pEvAttr or pIn for set operation

  @note
    The attribute set are useful for event creation.
    Once created, use instead m2mb_os_ev_setItem() to set only available event properties
    Typical uses is only for M2MB_OS_EV_SEL_CMD_USRNAME
    It is under application responsibility the right use of m2mb_os_ev_setAttrItem.
    For instance, multi creation of attrHandle withouth free of previous resources
    ( M2MB_OS_EV_SEL_CMD_CREATE_ATTR without M2MB_OS_EV_SEL_CMD_DEL_ATTR in case
      no m2mb_os_ev_init is used in the between ) will waste memory.


  @b
    Example
  @code
    M2MB_OS_EV_ATTR_HANDLE evAttrHandle;
    M2MB_OS_EV_HANDLE      evHandle;
    UINT32                 CurEvBits;

    if ( m2mb_os_ev_setAttrItem( &evAttrHandle, 1, M2MB_OS_EV_SEL_CMD_CREATE_ATTR,  NULL ) != M2MB_OS_SUCCESS )
        exit(error_creating attribute event);

    m2mb_os_ev_setAttrItem( &evAttrHandle, 2, M2MB_OS_EV_SEL_CMD_NAME, M2MB_OS_EV_SEL_CMD_USRNAME,
                            "MyEv", "MyEvUser"
                          );

    //or using CMDS_ARGS(...) macro
    m2mb_os_ev_setAttrItem( &evAttrHandle,
                            CMDS_ARGS
                            (
                               M2MB_OS_EV_SEL_CMD_CREATE_ATTR, NULL,
                               M2MB_OS_EV_SEL_CMD_NAME, "MyEv",
                               M2MB_OS_EV_SEL_CMD_USRNAME, "MyEvUser"
                            )
                          );


    if ( m2mb_os_ev_init( &evHandle, &evAttrHandle ) != M2MB_OS_SUCCESS )
    {
      //in case of error remember to manually remove attribute and release resources
      m2mb_os_ev_setAttrItem( &evAttrHandle, 1, M2MB_OS_EV_SEL_CMD_DEL_ATTR, NULL );
    }
    //...

    osRes = m2mb_os_ev_get(evHandle, (UINT32)0x111, M2MB_OS_EV_GET_ANY_AND_CLEAR, &CurEvBits, 50 );

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_ev_setAttrItem( M2MB_OS_EV_ATTR_HANDLE *pEvAttrHandle, UINT8 nCmds, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get specific properties of the event

  @details
    This service retrieve value for specified parameter of the specified event
    Selecting cmd, a parameter to be retrieved can be set.
    To be used when event is already created: it is under application responsibility

  @param [in] selCmd
   Select parameter to be retrieved (see M2MB_OS_EV_SEL_CMD_E)
  @param [in] evHandle
    handle of event
  param [out] pOut
   pointer Output parameter got: its meaning depend on selCmd used See examples
   as general pointer it is set to UINT32
  @param [in] pIn
   pointer to void general Parameter to be specified, if any, to get right output See examples
   If parameter is not needed, use NULL

  @return
   M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_EVENT_ERROR
     Invalid counting event handle
    M2MB_OS_INVALID_ARG
      invalid arguments

  @note
    Not all the M2MB_OS_EV_SEL_CMD_E are always available.
    Of course M2MB_OS_EV_SEL_CMD_CREATE_ATTR is not right for this function.
    Allowed:
    M2MB_OS_EV_SEL_CMD_NAME
    M2MB_OS_EV_SEL_CMD_USRNAME ( see the code example for use of pIn )
    M2MB_OS_EV_SEL_CMD_SUSPENDED_COUNT
    M2MB_OS_EV_SEL_CMD_1ST_SUSP
    M2MB_OS_EV_SEL_CMD_CURRENT_BITS_EVENT


  @b
    Example
  @code
    M2MB_OS_EV_ATTR_HANDLE evAttrHandle;
    M2MB_OS_EV_HANDLE      evHandle;
    CHAR                   MyEvName[10];
    CHAR                   *pEvName;
    MEM_W                  currBitEvent;
    M2MB_OS_TASK           whoIs;
    M2MB_OS_RESULT_E       osRes;

    osRes = m2mb_os_ev_setAttrItem( &evAttrHandle,
                                    CMDS_ARGS
                                    (
                                      M2MB_OS_EV_SEL_CMD_CREATE_ATTR, NULL,
                                      M2MB_OS_EV_SEL_CMD_NAME, "event_1",
                                      M2MB_OS_EV_SEL_CMD_USRNAME, "myevent1"
                                    )
                                  );

    if( osRes != M2MB_OS_SUCCESS )
      exit(...)

    //create
    m2mb_os_ev_init( &evHandle, &evAttrHandle );

    //get the first task suspended on the list on that event handle
    m2mb_os_ev_getItem( evHandle, M2MB_OS_EV_SEL_CMD_1ST_SUSP, (MEM_W *)&whoIs, NULL );
    //get the current bits in the event
    m2mb_os_ev_getItem( M2MB_OS_EV_SEL_CMD_CURRENT_EVENTS, evHandle, &currBitEvent, NULL );

    //copy only the first 10 chars to MyTaskName array of chars
    m2mb_os_ev_getItem( evHandle, M2MB_OS_EV_SEL_CMD_USRNAME,(MEM_W *)&MyEvName, (void*)10);
    // or simply get the pointer of name, with NULL
    m2mb_os_ev_getItem(evHandle, M2MB_OS_EV_SEL_CMD_USRNAME, (MEM_W *)&pEvName, NULL);

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_ev_getItem(
  M2MB_OS_EV_HANDLE evHandle,
  M2MB_OS_EV_SEL_CMD_E selCmd,
  MEM_W *pOut,
  void *pIn
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific properties of the event

  @details
    This service set value for specified parameter of the specified event if available
    To be used when event is already created: its right use is under application responsibility
    By Selecting cmd, a specified parameter to be set can be selected

  @param [in] evHandle
    handle of event
  @param [in] selCmd
   Select parameter to be set (see M2MB_OS_EV_SEL_CMD_E)
  @param [in] pIn
    pointer to value to be set. If a simple value, can be easily casted to (void*)
    e.g. int value -3 has to be passed, use (void*)-3
    NULL if no value is needed

  @return
   M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_EVENT_ERROR
     Invalid event handle
    M2MB_OS_INVALID_ARG
      invalid arguments
    see M2MB_OS_RESULT_E in m2mb_os_types.h for others
  @note
    Not all the M2MB_OS_EV_SEL_CMD_E are always available.
    Of course M2MB_OS_EV_SEL_CMD_CREATE_ATTR is not right for this function
    Only M2MB_OS_EV_SEL_CMD_USRNAME is currently allowed
    are ok


  @b
    Example
  @code
    m2mb_os_ev_setItem( evHandle, M2MB_OS_EV_SEL_CMD_USRNAME, ( void* )"MyNewName" );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_ev_setItem(
  M2MB_OS_EV_HANDLE evHandle,
  M2MB_OS_EV_SEL_CMD_E selCmd,
  void *pIn
);


#endif /* M2MB_OS_EV_H */
