/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_sys.h

  @brief
    Base system functions for m2m application, following operations can be performed:
    -get/set operating modes:
      -Online
       Indicates that the device can make data traffic and make calls.

      -Low Power
       Lowest power consumption state from which the device can return to Online mode;
       indicates that the device has temporarily disabled RF and detached from the netwrok
       If persistent flag is set, this mode persists even if the device is reset.
       persistent flag is reset, automatically if the device return to Online mode, again
       Currently, persistent flag applies only to Low Power mode.

      -Factory Test Mode
       Special mode for manufacturer use. The device detached from the network

      -Offline
       Phone has deactivated RF and shutdwon all network protocol stack;
       the device must be power cycled before it can reacquire service from this mode.

  @notes
    Dependencies: m2m/m2m_generic/common/m2mb_inc/m2mb_types.h

  @author
    mb_

  @date
    29/08/2017
*/

#ifndef M2M_M2MB_SYS_H
#define M2M_M2MB_SYS_H

/* Global declarations ==========================================================================*/

/* Global typedefs ==============================================================================*/
typedef HANDLE M2MB_SYS_HANDLE;

typedef enum
{
  M2MB_SYS_OP_MODE_ONLINE,                       /**<  Online */
  M2MB_SYS_OP_MODE_LOW_POWER,                    /**<  Low power */
  M2MB_SYS_OP_MODE_FACTORY_TEST_MODE,            /**<  Factory Test mode */
  M2MB_SYS_OP_MODE_OFFLINE,                      /**<  Offline */
  M2MB_SYS_OP_MODE_UNKNOWN,
  M2MB_SYS_OP_MODE_MAX_VAL
} M2MB_SYS_OP_MODE_E;

typedef enum
{
  M2MB_SYS_NULL_RESP,
  M2MB_SYS_GET_OPERATING_MODE_RESP,
  M2MB_SYS_SET_OPERATING_MODE_RESP,
  M2MB_SYS_OPERATING_MODE_CHANGED_IND
} M2MB_SYS_IND_E;

typedef enum
{
  M2MB_SYS_ERR_OK,
  M2MB_SYS_ERR_UNKNOWN
} M2MB_SYS_ERR_CAUSE_E;

typedef enum
{
  M2MB_SYS_REBOOT,
  M2MB_SYS_REBOOT_RECOVERY
} M2MB_SYS_REBOOT_TYPE_E;

typedef struct
{
  M2MB_SYS_OP_MODE_E operatingMode;
  BOOLEAN persistent;
} M2MB_SYS_OPERATING_MODE_T;

typedef struct
{
  M2MB_SYS_ERR_CAUSE_E err;
  M2MB_SYS_OPERATING_MODE_T operatingMode_s;
} M2MB_SYS_GET_OPERATING_MODE_RESP_T;

typedef struct
{
  M2MB_SYS_ERR_CAUSE_E err;
  M2MB_RESULT_E response;
} M2MB_SYS_GENERIC_RESP_T;

typedef void ( *m2mb_sys_ind_callback )( M2MB_SYS_HANDLE h, M2MB_SYS_IND_E sys_event, UINT16 resp_size, void *resp_struct, void *userdata );
/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sys_init initializes SYS service for current client.

  @details
    m2mb_sys_init initialize SYS service for current client returning the handle that must be passed as first parameter
    for all SYS actions. Calling the m2mb_sys_init is mandatory before using the SYS service.

  @param[in] *h
    first parameter is the handle to the SYS service, that will be initialized by the function.
    It has to be preallocated and initialized to NULL value (or M2MB_ID_HANDLE_INVALID).
  @param[in] callback
    second parameter is the callback that will be called if an event or a response happens.
  @param[in] userdata
    third parameter is a pointer to generic user data that will be returned as it is in the callback.

  @return
    M2MB_RESULT_SUCCESS on success.
  @return
    refer to M2MB_RESULT_E for different values.

  @b
    m2mb_sys_init(&h, myCallback, myUserdata);
  @code
    void myCallback(M2MB_SYS_HANDLE h, M2MB_SYS_IND_E sys_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      //my code
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SYS_HANDLE h = M2MB_ID_HANDLE_INVALID;
      struct myStruct myUserdata;
      retVal = m2mb_sys_init(&h, myCallback, (void*)myUserdata);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sys_init succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sys_init( M2MB_SYS_HANDLE *h, m2mb_sys_ind_callback callback, void *userdata );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sys_deinit deinitializes SYS service for current client.

  @details
    m2mb_sys_deinit deinitialize SYS service for current client freeing the handle passed as first parameter.
    Call to m2mb_sys_deinit should be performed in order to release memory resources when handle is no more used.

  @param[in] h
    first parameter is the handle to the SYS service, that will be deinitialized by the function.

  @return
    M2MB_RESULT_SUCCESS on success.
  @return
    refer to M2MB_RESULT_E for different values.

  @note

  @b
    m2mb_sys_deinit(&h);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_SYS_HANDLE h = M2MB_ID_HANDLE_INVALID;
    struct myStruct myUserdata;
    retVal = m2mb_sys_init(&h, myCallback, (void*)myUserdata);

    //... use SYS APIs ...

    retVal = m2mb_sys_deinit(h);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "m2mb_sys_deinit succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sys_deinit( M2MB_SYS_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sys_get_operating_mode gets information about the current operating mode.

  @details
    The function is asynchronous. This means that the function returns immediately, but the signal information is received
    in the callback previously set by the m2mb_sys_init function.
    The response event received in the callback will be the M2MB_SYS_GET_OPERATING_MODE_RESP. The resp_struct must be casted to
    the M2MB_SYS_GET_OPERATING_MODE_RESP_T type.

  @param[in] h
    the handle to the SYS service, previously initialized by the m2mb_sys_init function.

  @return
    M2MB_RESULT_SUCCESS on success.
  @return
    refer to M2MB_RESULT_E for different values.

  @note

  @b
    m2mb_sys_get_operating_mode(h);
  @code
    void myCallback(M2MB_SYS_HANDLE h, M2MB_SYS_IND_E sys_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sys_event)
      {
        //...

        case M2MB_SYS_GET_OPERATING_MODE_RESP:
        {
          M2MB_SYS_GET_OPERATING_MODE_RESP_T *resp = (M2MB_SYS_GET_OPERATING_MODE_RESP_T*)resp_struct;
          printf("GET OPERATING MODE resp is err %d, operatingMode %d, persistent %d", resp->err, resp->operatingMode_s.operatingMode, resp->operatingMode_s.persistent);
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SYS_HANDLE h = M2MB_ID_HANDLE_INVALID;
      struct myStruct myUserdata;
      retVal = m2mb_sys_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sys_get_operating_mode(h);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sys_get_operating_mode request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sys_get_operating_mode( M2MB_SYS_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sys_set_operating_mode sets the operating mode.

  @details
    The function is asynchronous. This means that the function returns immediately, but the signal information is received
    in the callback previously set by the m2mb_sys_init function.
    The indication event received in the callback will be the M2MB_SYS_OPERATING_MODE_CHANGED_IND. The resp_struct must be casted to
    the M2MB_SYS_OPERATING_MODE_T type.

  @param[in] h
    the handle to the SYS service, previously initialized by the m2mb_sys_init function.
  @param[in] operatingMode
    operating mode to set
  @param[in] persistent
    whether the operating mode must be persistent or not

  @return
    M2MB_RESULT_SUCCESS on success.
  @return
    refer to M2MB_RESULT_E for different values.

  @note

  @b
    m2mb_sys_set_operating_mode(h, operatingMode, persistent);
  @code
    void myCallback(M2MB_SYS_HANDLE h, M2MB_SYS_IND_E sys_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sys_event)
      {
        //...

        case M2MB_SYS_OPERATING_MODE_CHANGED_IND:
        {
          M2MB_SYS_OPERATING_MODE_T *resp = (M2MB_SYS_OPERATING_MODE_T*)resp_struct;
          printf("SET OPERATING MODE is operatingMode %d, persistent %d", resp->operatingMode, resp->persistent);
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SYS_HANDLE h = M2MB_ID_HANDLE_INVALID;
      struct myStruct myUserdata;
      retVal = m2mb_sys_init(&h, myCallback, (void*)myUserdata);
      //...
      // setting non persistent M2MB_SYS_OP_MODE_LOW_POWER
      retVal = m2mb_sys_set_operating_mode(h, M2MB_SYS_OP_MODE_LOW_POWER, FALSE);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sys_set_operating_mode request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sys_set_operating_mode( M2MB_SYS_HANDLE h, M2MB_SYS_OP_MODE_E operatingMode, BOOLEAN persistent );




/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sys_reboot restarts all the system

  @details
    This function performs a system reboot

  @param[in] Type
    Unused, usually must be set to 0

  @return
    returns M2MB_RESULT_SUCCESS on success, refers to M2MB_RESULT_E for different values.

  @b
    m2mb_sys_reboot(0)
  @code
    M2MB_RESULT_E Ret = m2mb_sys_reboot(0);
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sys_reboot
(
  int Type
);




#endif /* M2M_M2MB_SYS_H */

