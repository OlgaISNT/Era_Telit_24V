/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_common/m2mb_inc/m2mb_sim.h

  @brief
    Header file for m2mb_sim APIs

  @details
    m2mb_sim APIs provide actions and events for remote SIM management.

  @notes

  @author
    F. Sansa

  @date
    05/09/2017
*/

#ifndef  M2M_M2MB_SIM_API_H
#define  M2M_M2MB_SIM_API_H


/* Global declarations ==========================================================================*/

#define MAX_APDU_SIZE ( 300 )

/* Global typedefs ==============================================================================*/

typedef HANDLE M2MB_SIM_HANDLE;

typedef enum
{
  M2MB_SIM_SUCCESS,
  M2MB_SIM_GENERIC_FAILURE,
} M2MB_SIM_ERRNO_E;

typedef enum
{
  M2MB_SIM_REMOTE_EVENT_RESP,
  M2MB_SIM_REMOTE_APDU_RESP,
  M2MB_SIM_REMOTE_CONNECT_IND,
  M2MB_SIM_REMOTE_APDU_IND,
  M2MB_SIM_REMOTE_INIT_IND,
  /* indication of switching to VSIM */
  M2MB_SIM_PROF01_SET_IND,
  /* indication of switching back to physical SIM */
  M2MB_SIM_PROF00_SET_IND,
  /* indication of executing SIM personalization */
  M2MB_SIM_EXECUTE_CRYPTO_IND
} M2MB_SIM_IND_E;

typedef enum
{
  /* SIM type selection */
  M2MB_SIM_LOCAL_SIM,
  M2MB_SIM_REMOTE_SIM
} M2MB_SIM_SELECTION_E;

typedef enum
{
  M2MB_SIM_EUICC_EXECUTE_APDU_REQ = 0x01,
  M2MB_SIM_EUICC_EXECUTE_APDU_RESP = 0x02,
  M2MB_SIM_EUICC_INIT = 0x03,
  M2MB_SIM_EUICC_RESET = 0x04,
  M2MB_SIM_EUICC_EXECUTE_CRYPTO = 0x05
} M2MB_SIMOS_MSGTYPE_E;

typedef struct
{
  UINT32 pdu_size;
  UINT8 pdu[MAX_APDU_SIZE];
} M2MB_SIM_INCOMING_APDU_T;

/* used to provide personalization file: FFU */
typedef struct
{
  UINT32 perso_data_size;
  UINT8 *perso_data;
} M2MB_SIM_PERSO_DATA_T;

/* message to be sent to Sim OS; handle simHandle should be known by task */
typedef struct
{
  M2MB_SIMOS_MSGTYPE_E MsgType;
  M2MB_SIM_HANDLE simHandle;
  UINT32 pdu_size;
  UINT8 *pdu;
} M2MB_SIMOS_REQ_T;


typedef void ( *m2mb_sim_ind_callback )( M2MB_SIM_HANDLE h, M2MB_SIM_IND_E sim_event, UINT16 resp_size, void *resp_struct, void *userdata );

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sim_init initializes SIM service for current client.

  @details
    m2mb_sim_init initialize SIM service for current client returning the handle that must be passed as first parameter
    for all SIM actions. Calling the m2mb_sim_init is mandatory before using the SIM service.

  @param[in] *h
    first parameter is the handle to the SIM service, that will be initialized by the function.
  @param[in] callback
    second parameter is the callback that will be called if an event or a response happens.
  @param[in] userdata
    third parameter is a pointer to generic user data that will be returned as it is in the callback.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sim_init(&h, myCallback, myUserdata);
  @code
    void myCallback( M2MB_SIM_HANDLE h, M2MB_SIM_IND_E sim_event, UINT16 resp_size, void *resp_struct, void *userdata )
    {
      //my code
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SIM_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_sim_init(&h, myCallback, (void*)myUserdata);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sim_init succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sim_init( M2MB_SIM_HANDLE *h, m2mb_sim_ind_callback callback, void *userdata );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sim_send_remote_apdu_req sends a PDU to remote SIM.

  @details
    m2mb_sim_send_remote_apdu_req sends a PDU to the remote SIM. The function is TBD. The response of the request is
  received in the callback defined by the user, using the m2mb_sim_send_remote_apdu_resp template.

  @param[in] h
    first parameter is the handle to the remote SIM service, previously initialized by the m2mb_sim_init function.
  @param[in] apdu_command
    second parameter is an array containing the PDU to be sent.
  @param[in] cmdApduLen
    third parameter is PDU length in byte.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sim_send_remote_apdu_req(h, apdu_command, cmdApduLen);
  @code
    void myCallback( M2MB_SIM_HANDLE h, UINT8 *SW1, UINT8 *SW2, UINT8 *apdu_response, UINT16 respLen, UINT8 SIMStatus, void *userdata )
    {
      //my code
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SIM_HANDLE h;
      struct myStruct myUserdata;
      UINT8 apdu[] = {0x00, 0x11, 0x00, 0x0A, 0x91, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0xAA, 0x0C, 0xC8, 0xF7, 0x1D, 0x14, 0x96, 0x97, 0x41, 0xF9, 0x77, 0xFD, 0x07};
      retVal = m2mb_sim_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sim_send_remote_apdu_req(h, pdu, 25);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sim_send_remote_apdu_req request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sim_send_remote_apdu_req( M2MB_SIM_HANDLE h, UINT8 *apdu_command, UINT16 cmdApduLen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sim_select_sim switches between local (physical) and remote (virtual) SIM.

  @details
    m2mb_sim_select_sim switches between local (physical) and remote (virtual) SIM.

  @param[in] h
    first parameter is the handle to the remote SIM service, previously initialized by the m2mb_sim_init function.
  @param[in] simSelection
    second parameter is an enum indicating which SIM should be selected.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sim_select_sim(h, simSelection);
  @code
    void myCallback( M2MB_SIM_HANDLE h, M2MB_SIM_IND_E sim_event, UINT16 resp_size, void *resp_struct, void *userdata )
    {
      //my code
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SIM_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_sim_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sim_select_sim(h, 1);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sim_select_sim request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sim_select_sim( M2MB_SIM_HANDLE h, M2MB_SIM_SELECTION_E simSelection );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sim_send_remote_apdu_resp sends a remote SIM response APDU to UIM.

  @details
     m2mb_sim_send_remote_apdu_resp sends a remote SIM response APDU to UIM task (UIM (local SIM) manager).

  @param[in] h
    first parameter is the handle to the remote SIM service, previously initialized by the m2mb_sim_init function.
  @param[in] SW1
    second parameter is SW1 of remote SIM response APDU.
  @param[in] SW2
    third parameter is SW2 of remote SIM response APDU.
  @param[in] apdu_response
    fourth parameter is remote SIM response APDU.
  @param[in] respLen
    fifth parameter is remote SIM response APDU length.
  @param[in] respLen
    sixth parameter is remote SIM status.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sim_send_remote_apdu_resp(h, SW1, SW2, apdu_response, respLen, SIMStatus);
  @code
    void myCallback( M2MB_SIM_HANDLE h, M2MB_SIM_IND_E sim_event, UINT16 resp_size, void *resp_struct, void *userdata )
    {
      //my code
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SIM_HANDLE h;
      struct myStruct myUserdata;
      UINT8 apdu[] = {0x00, 0x11, 0x00, 0x0A, 0x91, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0xAA, 0x0C, 0xC8, 0xF7, 0x1D, 0x14, 0x96, 0x97, 0x41, 0xF9, 0x77, 0xFD, 0x07};
      retVal = m2mb_sim_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sim_send_remote_apdu_resp(h, 0x90, 0x00, pdu, 25, 0);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sim_send_remote_apdu_resp request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_sim_send_remote_apdu_resp( M2MB_SIM_HANDLE h, UINT8 SW1, UINT8 SW2, UINT8 *apdu_response, UINT16 respLen, UINT8 SIMStatus );


/*-----------------------------------------------------------------------------------------------*/
 /*!  
 
   @brief
     Provide response after 
 
   @details
 
   @param [in] h
     first parameter is the handle to the remote SIM service, previously initialized by the m2mb_sim_init function.
   @param [in] pData
     pointer to data provided as response
   @param [in] datalen
     lenght of data
    @param [in] cryptoResult
     result from Crypto operation
     
   @return
     returns M2MB_RESULT_SUCCESS on success, a different value on error.
     
   @note
 
   @b
 
   @code
 
   @endcode
 
 */
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sim_send_crypto_resp( M2MB_SIM_HANDLE h, UINT8 *pData, UINT16 datalen, M2MB_RESULT_E cryptoResult );


/*-----------------------------------------------------------------------------------------------*/
 /*!  
 
   @brief
     Call reset of SIM service to the modem
 
   @details
 
   @param [in] h
     first parameter is the handle to the remote SIM service, previously initialized by the m2mb_sim_init function.
 
   @return
 
   @note
 
   @b
 
   @code
 
   @endcode
 
 */
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sim_send_remote_reset_req( M2MB_SIM_HANDLE h );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sim_send_remote_watchdog_kick_req can both kick watchdog of uim task on modem and set r
      its timeout multiplie (something similar to clock divider).
    

  @details
    when heavy code is performed, using for example crypto libs or TrustZone,
    kicking of watchdog related to uim management in modem, can avoid crash for starvation

  @param[in] h
    first parameter is the handle to the remote SIM service, previously initialized by the m2mb_sim_init function.

  @param[in] wdTimeoutX
    Watchdog timeout multiplier. If a value greater than 127 is set, WD will be disabled. If 0, 1 is set instead.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_RESULT_E m2mb_sim_send_remote_watchdog_kick_req( M2MB_SIM_HANDLE h, UINT32 wdTimeoutX );



#endif
