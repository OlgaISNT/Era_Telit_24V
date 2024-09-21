/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2nb_inc/m2mb_ati.h

  @brief
    Header file for m2mb_ati APIs

  @details
    m2mb_ati APIs provide actions and events for AT interface.

  @notes

  @author
    Morgan Deidda

  @date
    09/07/2018
*/

#ifndef  M2M_M2MB_ATI_API_H
#define  M2M_M2MB_ATI_API_H


/* Global declarations ==========================================================================*/


/* Global typedefs ==============================================================================*/

typedef HANDLE M2MB_ATI_HANDLE;

/*!
   @brief ATI event type.

   This type defines the indications which can be passed to the callback.
*/
typedef enum
{
  M2MB_RX_DATA_EVT           = 0x01,    /**< Received data Event */
  M2MB_STATE_IDLE_EVT        = 0x02,    /**< AT parser entered in IDLE state */
  M2MB_STATE_RUNNING_EVT     = 0x04,    /**< AT parser entered in RUNNING state */
  M2MB_STATE_CMD_MODE_EVT    = 0x08,    /**< AT parser entered in COMMAND mode */
  M2MB_STATE_ONLINE_MODE_EVT = 0x10     /**< AT parser entered in ONLINE mode */
} M2MB_ATI_EVENTS_E;

/*!
   @brief Transmit and receive operation callback type.

   This type defines the callback functions that the client can set in the
   m2mb_ati_init API.
*/
typedef void ( *m2mb_ati_callback )( M2MB_ATI_HANDLE h, M2MB_ATI_EVENTS_E ati_event, UINT16 resp_size, void *resp_struct, void *userdata );

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_ati_init initializes a ATI interface for current client.

  @details
    m2mb_ati_init initializes a ATI interface for current client returning the handle that must
    be passed as first parameter for all ATI actions.
    Every ATI interface is linked to a AT parser instance.
    Calling the m2mb_ati_init is mandatory before using the ATI interface.

  @param[in,out] *pHandle
    first parameter is the handle to the ATI interface, that will be initialized by the function.
  @param[in] atInstance
    second parameter is the index of the AT parser instance that will be reserved to the current
    ATI interface. If the At instance is already reserved to another ATI interface, an error is returned.
  @param[in] callback
    third parameter is the callback that will be called if an event or a response happens.
  @param[in] userdata
    fourth parameter is a pointer to generic user data that will be returned as it is in the callback.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
    if atInstance is already reserved to a physical port, the physical port will not work until atInstance is deinitialized.
    
  @b
    m2mb_ati_init(&h, instanceID, myCallback, myUserdata);
  @code
    static void ati_callback( M2MB_ATI_HANDLE h, M2MB_ATI_EVENTS_E ati_event, UINT16 resp_size, void *resp_struct, void *userdata )
    {
      //my code
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_ATI_HANDLE h;
      INT16 instanceID = 2;
      struct myStruct myUserdata;
      retVal = m2mb_ati_init(&h, instanceID, myCallback, (void*)myUserdata);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_ati_init succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_ati_init( M2MB_ATI_HANDLE *pHandle, INT16 atInstance, m2mb_ati_callback callback, void *userdata );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_ati_deinit deinitializes ATI interface for current client.

  @details
    m2mb_ati_deinit deinitialize ATI interface for current client freeing the handle passed
    as first parameter.
    Calling the m2mb_ati_deinit is useful after using the ATI interface in order to free memory
    space and to restore the previous scenario.

  @param[in] handle
    first parameter is the handle to the ATI interface, that will be deinitialized by the function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_ati_deinit(h);
  @code
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_ATI_HANDLE h;
      INT16 instanceID = 2;
      struct myStruct myUserdata;
      retVal = m2mb_ati_init(&h, instanceID, myCallback, (void*)myUserdata);

      //... use ATI APIs ...

      retVal = m2mb_ati_deinit(h);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_ati_deinit succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_ati_deinit( M2MB_ATI_HANDLE handle );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_ati_send_cmd sends a command to an initialized ATI interface.

  @details
    m2mb_ati_send_cmd writes a command of nbyte Bytes from the source buffer buf to a previously
    initialized ATI interface.

  @param[in] handle
    first parameter is the handle to the ATI interface, previously initialized by the m2mb_ati_init function.
  @param[in] buf
    second parameter is the source buffer, previously allocated, containing the command string.
  @param[in] nbyte
    third parameter is the length of the command string.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_ati_send_cmd(h, cmd_str, cmd_len);
  @code
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_ATI_HANDLE h;
      INT16 instanceID = 2;
      struct myStruct myUserdata;
      CHAR cmd_str[ATI_CMD_LEN];
      SSIZE_T cmd_len;
      retVal = m2mb_ati_init(&h, instanceID, myCallback, (void*)myUserdata);
      ...
      strncpy(cmd_str, "at+cfun?", ATI_CMD_LEN);
      cmd_len = strlen(cmd_str);
      cmd_str[cmd_len++] = '\r';
      retVal = m2mb_ati_send_cmd(h, cmd_str, cmd_len);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_ati_send_cmd succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_ati_send_cmd( M2MB_ATI_HANDLE handle, void *buf, SIZE_T nbyte );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_ati_rcv_resp receives data from an initialized ATI interface.

  @details
    m2mb_ati_rcv_resp reads a message (response or indication) of maximum nbyte Bytes from an
    previously initialized ATI interface into the destination buffer buf.

  @param[in] handle
    first parameter is the handle to the ATI interface, previously initialized by the m2mb_ati_init function.
  @param[in] buf
    second parameter is the destination buffer, previously allocated.
  @param[in] nbyte
    third parameter is the length of the destination buffer in Bytes.

  @return
    returns the number of read Bytes on SUCCESS, -1 on FAILURE.

  @note

  @b
    m2mb_ati_rcv_resp(h, resp_str, max_rsp_len);
  @code
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_ATI_HANDLE h;
      INT16 instanceID = 2;
      struct myStruct myUserdata;
      CHAR resp_str[ATI_RSP_LEN];
      SIZE_T max_rsp_len = ATI_RSP_LEN;
      SSIZE_T rsp_len;
      retVal = m2mb_ati_init(&h, instanceID, myCallback, (void*)myUserdata);
      ...
      rsp_len = m2mb_ati_rcv_resp(atiHandle, resp_str, max_rsp_len);
      if ( rsp_len > 0 )
        printf( "m2mb_ati_rcv_resp succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
SSIZE_T m2mb_ati_rcv_resp( M2MB_ATI_HANDLE handle, void *buf, SIZE_T nbyte );

#endif

