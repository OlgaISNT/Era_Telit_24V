/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_sms.h

  @brief
    Header file for m2mb_sms APIs

  @details
    m2mb_sms APIs provide actions and events for SMS management.

  @notes

  @author
    S. Passiu

  @date
    21/04/2017
*/

#ifndef  M2M_M2MB_SMS_API_H
#define  M2M_M2MB_SMS_API_H


/* Global declarations ==========================================================================*/

#define M2MB_SMS_PDU_LENGTH_MAX 255
#define M2MB_SMS_STORAGE_INDEXES_MAX 255
#define M2MB_SMS_ADDRESS_TYPE_MAX 3
#define M2MB_SMS_SCA_DIGIT_MAX 21

/* Global typedefs ==============================================================================*/

typedef HANDLE M2MB_SMS_HANDLE;

typedef enum
{
  M2MB_SMS_SUCCESS,
  M2MB_SMS_GENERIC_FAILURE,
  M2MB_SMS_RADIO_NOT_AVAILABLE,
  M2MB_SMS_REQUEST_RAW_SEND_UNKNOWN_ERROR,
  M2MB_SMS_REQUEST_RAW_SEND_NETWORK_TIMEOUT,
  M2MB_SMS_SEND_FAIL_RETRY,
  M2MB_SMS_FDN_CHECK_FAILURE
} M2MB_SMS_ERRNO_E;

typedef enum
{
  M2MB_SMS_STORAGE_NONE,
  M2MB_SMS_STORAGE_SM,
  M2MB_SMS_STORAGE_ME
} M2MB_SMS_STORAGE_E;

typedef enum
{
  M2MB_SMS_DISCARD,
  M2MB_SMS_STORE_AND_ACK,
  M2MB_SMS_FORWARD_ONLY,
  M2MB_SMS_FORWARD_AND_ACK
} M2MB_SMS_INC_ACTION_E;

typedef enum
{
  M2MB_SMS_CLASS_0,
  M2MB_SMS_CLASS_1,
  M2MB_SMS_CLASS_2,
  M2MB_SMS_CLASS_3,
  M2MB_SMS_CLASS_NONE
} M2MB_SMS_CLASS_E;

typedef enum
{
  M2MB_SMS_NULL_RESP,
  M2MB_SMS_SEND_RESP,
  M2MB_SMS_WRITE_RESP,
  M2MB_SMS_SEND_FROM_MEM_RESP,
  M2MB_SMS_ACK_RESP,
  M2MB_SMS_READ_RESP,
  M2MB_SMS_DELETE_RESP,
  M2MB_SMS_SET_TAG_RESP,
  M2MB_SMS_SET_SCA_RESP,
  M2MB_SMS_GET_SCA_RESP,
  M2MB_SMS_GET_STORAGE_STAT_RESP,
  M2MB_SMS_GET_STORAGE_INDEXES_RESP,
  M2MB_SMS_INCOMING_IND,
  M2MB_SMS_MEMORY_FULL_IND, /* event sent when every time the m2mb_sms_write fails due to memory full */
  M2MB_SMS_MEMORY_FULL_REACHED_IND /* event sent when SMS memory is full (only once) */
} M2MB_SMS_IND_E;

typedef enum
{
  M2MB_SMS_SET_FORMAT,
  M2MB_SMS_GET_FORMAT,
  M2MB_SMS_SET_IMS_EN,
  M2MB_SMS_GET_IMS_EN,
  M2MB_SMS_SET_WRITE_TAG,
  M2MB_SMS_GET_WRITE_TAG,
  M2MB_SMS_SET_MORE_MSG_TO_SEND,
  M2MB_SMS_GET_MORE_MSG_TO_SEND
} M2MB_SMS_ACTION_E;

typedef enum
{
  M2MB_SMS_FORMAT_3GPP,
  M2MB_SMS_FORMAT_3GPP2
} M2MB_SMS_FORMAT_E;

typedef enum
{
  M2MB_SMS_MMTS_DISABLED,
  M2MB_SMS_MMTS_ENABLED
} M2MB_SMS_MMTS_E;

typedef enum
{
  M2MB_SMS_TAG_MT_READ,
  M2MB_SMS_TAG_MT_NOT_READ,
  M2MB_SMS_TAG_MO_SENT,
  M2MB_SMS_TAG_MO_NOT_SENT
} M2MB_SMS_TAG_E;

typedef enum
{
  M2MB_SMS_OVER_CS,
  M2MB_SMS_OVER_IMS
} M2MB_SMS_IMS_E;

typedef enum
{
  M2MB_SMS_ACK_FAIL,
  M2MB_SMS_ACK_SUCCESS
} M2MB_SMS_ACK_RES_E;

typedef enum
{
  M2MB_SMS_ACK_OK,
  M2MB_SMS_ACK_FAIL_NO_NETWORK_RESPONSE,
  M2MB_SMS_ACK_FAIL_NETWORK_RELEASED_LINK,
  M2MB_SMS_ACK_FAIL_ACK_NOT_SENT
} M2MB_SMS_ACK_FAILURE_CAUSE_E;

typedef enum
{
  M2MB_SMS_ERR_OK,
  M2MB_SMS_ERR_UNKNOWN,
  M2MB_SMS_ERR_API_NOT_AVAILABLE
} M2MB_SMS_ERR_CAUSE_E;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
} M2MB_SMS_ERR_CAUSE_T;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
  UINT32 index;
  M2MB_SMS_FORMAT_E format;
  M2MB_SMS_TAG_E tag;
  UINT16 pdu_size;
  UINT8 *pdu;
} M2MB_SMS_READ_RESP_T;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
  UINT32 index;
} M2MB_SMS_WRITE_RESP_T;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
  UINT16 messageRef;
} M2MB_SMS_SEND_RESP_T;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
  M2MB_SMS_ACK_FAILURE_CAUSE_E failCause;
} M2MB_SMS_ACK_RESP_T;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
  UINT8 addr_type[M2MB_SMS_ADDRESS_TYPE_MAX + 1];
  UINT16 sca_size;
  UINT8 *sca;
} M2MB_SMS_GET_SCA_RESP_T;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
  M2MB_SMS_STORAGE_E stType;
  UINT32 smsCounter;
  UINT32 maxStorageSize;
} M2MB_SMS_GET_STORAGE_STAT_RESP_T;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
  M2MB_SMS_STORAGE_E stType;
  UINT16 indexes_size;
  UINT8 *indexes;
} M2MB_SMS_GET_STORAGE_INDEXES_RESP_T;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
  M2MB_RESULT_E response;
} M2MB_SMS_GENERIC_RESP_T;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
  M2MB_SMS_FORMAT_E format;
  M2MB_SMS_IMS_E mode;
  M2MB_SMS_STORAGE_E storage;
  INT32 transactionID; //transactionID starts from 0 if the incoming SMS must be acked, if index < 0 the incoming SMS has been already acked
  INT32 index; //storage indexes start from 0, if index < 0 the message is not stored and it is forwarded in the pdu field
  UINT16 pdu_size;
  UINT8 *pdu;
} M2MB_SMS_INCOMING_T;

typedef struct
{
  M2MB_SMS_ERR_CAUSE_E err;
  M2MB_SMS_FORMAT_E format;
  M2MB_SMS_STORAGE_E storage;
} M2MB_SMS_MEMORY_FULL_T;

typedef void ( *m2mb_sms_ind_callback )( M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *userdata );

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_init initializes SMS service for current client.

  @details
    m2mb_sms_init initialize SMS service for current client returning the handle that must be passed as first parameter
    for all SMS actions. Calling the m2mb_sms_init is mandatory before using the SMS service.

  @param[in] *h
    first parameter is the handle to the SMS service, that will be initialized by the function.
  @param[in] callback
    second parameter is the callback that will be called if an event or a response happens.
  @param[in] userdata
    third parameter is a pointer to generic user data that will be returned as it is in the callback.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_init(&h, myCallback, myUserdata);
  @code
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      //my code
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_init succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_init( M2MB_SMS_HANDLE *h, m2mb_sms_ind_callback callback, void *userdata );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_deinit deinitializes SMS service for current client.

  @details
    m2mb_sms_deinit deinitialize SMS service for current client freeing the handle passed
    as first parameter. Calling the m2mb_sms_deinit is useful after using the SMS service in order to free memory space.

  @param[in] h
    first parameter is the handle to the SMS service, that will be deinitialized by the function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_deinit(&h);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_SMS_HANDLE h;
    struct myStruct myUserdata;
    retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);

    //... use SMS APIs ...

    retVal = m2mb_sms_deinit(h);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "m2mb_sms_deinit succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_deinit( M2MB_SMS_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_conf sets/gets some SMS parameters.

  @details
    m2mb_sms_conf sets/gets SMS parameters listed in the M2MB_SMS_ACTION_E enum.

  @param[in] h
    Handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] argc
    It determines the number of parameters that follows, plus one (e.g.: if two parameters follow, argc is 3)
  @param[in]
    It determines the action (among the ones listed in M2MB_SMS_ACTION_E), while the parameters
    that follow depend on the action (see following note for further details)

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
    Action: M2MB_SMS_SET_FORMAT

    Description: set the format (3GPP or 3GPP2) used to send, ack and write the SMS. Default is 3GPP.

    Params(in): h(M2MB_SMS_HANDLE), action(M2MB_SMS_ACTION_E), format(M2MB_SMS_FORMAT_E)

    - example: set the 3GPP format

    m2mb_sms_conf(h, 3, M2MB_SMS_SET_FORMAT, M2MB_SMS_FORMAT_3GPP);


    Action: M2MB_SMS_GET_FORMAT

    Description: get the current SMS format used to send, ack and write the SMS.

    Params(in): h(M2MB_SMS_HANDLE), action(M2MB_SMS_ACTION_E), format(M2MB_SMS_FORMAT_E*)

    - example: get current SMS format

    M2MB_SMS_FORMAT_E currentFormat;
    m2mb_sms_conf(h, 3, M2MB_SMS_GET_FORMAT, &currentFormat);


    Action: M2MB_SMS_SET_IMS_EN

    Description: set the mode used to send the SMS (over CS or over IMS). Default is "over CS".

    Params(in): h(M2MB_SMS_HANDLE), action(M2MB_SMS_ACTION_E), format(M2MB_SMS_IMS_E)

    - example: set the "over CS" mode

    m2mb_sms_conf(h, 3, M2MB_SMS_SET_IMS_EN, M2MB_SMS_OVER_CS);


    Action: M2MB_SMS_GET_IMS_EN

    Description: get the current mode used to send the SMS (over CS or over IMS).

    Params(in): h(M2MB_SMS_HANDLE), action(M2MB_SMS_ACTION_E), format(M2MB_SMS_IMS_E*)

    - example: get current SMS mode

    M2MB_SMS_IMS_E currentMode;
    m2mb_sms_conf(h, 3, M2MB_SMS_GET_IMS_EN, &currentMode);


    Action: M2MB_SMS_SET_WRITE_TAG

    Description: set the tag used when a SMS is written through m2mb_sms_write funcion. Default is "not sent".

    Params(in): h(M2MB_SMS_HANDLE), action(M2MB_SMS_ACTION_E), format(M2MB_SMS_TAG_E)

    - example: set the "not sent" tag

    m2mb_sms_conf(h, 3, M2MB_SMS_SET_WRITE_TAG, M2MB_SMS_TAG_MO_NOT_SENT);


    Action: M2MB_SMS_GET_WRITE_TAG

    Description: get the current m2mb_sms_write tag.

    Params(in): h(M2MB_SMS_HANDLE), action(M2MB_SMS_ACTION_E), format(M2MB_SMS_TAG_E*)

    - example: get current m2mb_sms_write tag

    M2MB_SMS_TAG_E currentTag;
    m2mb_sms_conf(h, 3, M2MB_SMS_GET_WRITE_TAG, &currentTag);


    Action: M2MB_SMS_SET_MORE_MSG_TO_SEND

    Description: enable/disable the "more message to send" functionality. Default is "disabled".
      The "more message to send" functionality controls the continuity of SMS relay protocol link.
      When feature is enabled (and supported by network) multiple messages can be sent much faster as
      link is kept open. If the time between the response of the latest message send command and the
      next send command exceeds 5 seconds, the link is closed but the functionality remains enabled.

    Params(in): h(M2MB_SMS_HANDLE), action(M2MB_SMS_ACTION_E), format(M2MB_SMS_MMTS_E)

    - example: disable the "more message to send" functionality

    m2mb_sms_conf(h, 3, M2MB_SMS_SET_MORE_MSG_TO_SEND, M2MB_SMS_MMTS_DISABLED);


    Action: M2MB_SMS_GET_MORE_MSG_TO_SEND

    Description: get the current status of "more message to send" functionality.

    Params(in): h(M2MB_SMS_HANDLE), action(M2MB_SMS_ACTION_E), format(M2MB_SMS_MMTS_E*)

    - example: get current status of "more message to send" functionality

    M2MB_SMS_MMTS_E currentMmts;
    m2mb_sms_conf(h, 3, M2MB_SMS_GET_MORE_MSG_TO_SEND, &currentMmts);

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_conf( M2MB_SMS_HANDLE h, UINT8 argc, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_enable_ind enables SMS events.

  @details
    m2mb_sms_enable_ind enables the signaling of SMS events through the callback function previously registerd with
    the m2mb_sms_init.

  @param[in] h
    Handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] ind
    Determines the event (among the ones ending with the _IND suffix, listed in M2MB_SMS_IND_E) to be
    enabled/disabled.
  @param[in] enable
    Integer to enable/disable the event "ind" (0 to disable or 1 to enable).

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
    ind = M2MB_SMS_MEMORY_FULL_IND enables/disables both M2MB_SMS_MEMORY_FULL_IND and M2MB_SMS_MEMORY_FULL_REACHED_IND events.
    ind = M2MB_SMS_MEMORY_FULL_REACHED_IND enables/disables both M2MB_SMS_MEMORY_FULL_IND and M2MB_SMS_MEMORY_FULL_REACHED_IND events.

  @b
    m2mb_sms_enable_ind(h, M2MB_SMS_INCOMING_IND, 1);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_SMS_HANDLE h;
    struct myStruct myUserdata;
    retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);

    retVal = m2mb_sms_enable_ind(h, M2MB_SMS_INCOMING_IND, 1);

    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "M2MB_SMS_INCOMING_IND enabled");
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_enable_ind( M2MB_SMS_HANDLE h, M2MB_SMS_IND_E ind, UINT8 enable );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_send sends a SMS PDU.

  @details
    m2mb_sms_send sends a SMS PDU to the network. The function is asynchronous. This means that the function returns
    immediately, but the confirmation of the SMS shipment is received in the callback previously set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_SEND_RESP. The resp_struct must be casted to
    the M2MB_SMS_SEND_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] ind
    second parameter is the PDU length in byte.
  @param[in] enable
    third parameter is an array containing the PDU to be sent.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_send(h, pdu_len, pdu);
  @code
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...

        case M2MB_SMS_SEND_RESP:
        {
          M2MB_SMS_SEND_RESP_T *resp = (M2MB_SMS_SEND_RESP_T*)resp_struct;
          printf("Send resp msg ID %d", resp->messageRef);
          break;
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      UINT8 pdu[] = {0x00, 0x11, 0x00, 0x0A, 0x91, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0xAA, 0x0C, 0xC8, 0xF7, 0x1D, 0x14, 0x96, 0x97, 0x41, 0xF9, 0x77, 0xFD, 0x07};
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_send(h, 25, pdu);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_send request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_send( M2MB_SMS_HANDLE h, UINT32 pdu_len, UINT8 *pdu );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_write writes a SMS PDU into the current memory.

  @details
    m2mb_sms_write writes a SMS PDU into the current memory (see m2mb_sms_set_storage function). The function is asynchronous.
    This means that the function returns immediately, but the confirmation of the SMS writing is received in the callback previously
    set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_WRITE_RESP. The resp_struct must be casted to
    the M2MB_SMS_WRITE_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] ind
    second parameter is the PDU length in byte.
  @param[in] enable
    third parameter is an array containing the PDU to be written.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_write(h, pdu_len, pdu);
  @code
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...

        case M2MB_SMS_WRITE_RESP:
        {
          M2MB_SMS_WRITE_RESP_T *resp = (M2MB_SMS_WRITE_RESP_T*)resp_struct;
          printf("PDU index written is %ld", resp->index);
          break;
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      UINT8 pdu[] = {0x00, 0x11, 0x00, 0x0A, 0x91, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0xAA, 0x0C, 0xC8, 0xF7, 0x1D, 0x14, 0x96, 0x97, 0x41, 0xF9, 0x77, 0xFD, 0x07};
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_write(h, 25, pdu);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_write request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_write( M2MB_SMS_HANDLE h, UINT32 pdu_len, UINT8 *pdu );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_send_from_mem sends a SMS PDU from the current memory.

  @details
    m2mb_sms_send_from_mem sends a SMS PDU stored in the current memory (see m2mb_sms_set_storage and m2mb_sms_write function).
    The function is asynchronous. This means that the function returns immediately, but the confirmation of the SMS sending is received
    in the callback previously set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_SEND_FROM_MEM_RESP. The resp_struct must be casted to
    the M2MB_SMS_SEND_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] ind
    second parameter is the index where the SMS is stored in the current memory.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
    The API result will be shown in the "err" field of M2MB_SMS_SEND_RESP_T.

  @b
    m2mb_sms_send_from_mem(h, index);
  @code
    UINT32 globalIndex;
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...

        case M2MB_SMS_WRITE_RESP:
        {
          M2MB_SMS_WRITE_RESP_T *resp = (M2MB_SMS_WRITE_RESP_T*)resp_struct;
          printf("PDU index written is %ld", resp->index);
          globalIndex = resp->index;
          break;
        }

        case M2MB_SMS_SEND_FROM_MEM_RESP:
        {
          M2MB_SMS_SEND_RESP_T *resp = (M2MB_SMS_SEND_RESP_T*)resp_struct;
          printf("API result is %d; send resp msg ID %d", resp->err, resp->messageRef);
          break;
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      UINT8 pdu[] = {0x00, 0x11, 0x00, 0x0A, 0x91, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0xAA, 0x0C, 0xC8, 0xF7, 0x1D, 0x14, 0x96, 0x97, 0x41, 0xF9, 0x77, 0xFD, 0x07};
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_write(h, 25, pdu);
      //...
      retVal = m2mb_sms_send_from_mem(h, globalIndex);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_send_from_mem request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_send_from_mem( M2MB_SMS_HANDLE h, UINT32 index );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_ack acknowledge an incoming SMS.

  @details
    m2mb_sms_ack acknowledge an incoming SMS, if the option M2MB_SMS_FORWARD_ONLY is set (see m2mb_sms_set_route
    and m2mb_sms_get_route functions).
    The function is asynchronous. This means that the function returns immediately, but the result of the SMS ack is received
    in the callback previously set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_ACK_RESP. The resp_struct must be casted to
    the M2MB_SMS_ACK_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] res
    second parameter is the ack or nack to the incoming SMS.
  @param[in] transactionID
    third parameter is the transaction ID given in the M2MB_SMS_INCOMING_T structure.
  @param[in] failureCause1
    fourth parameter value has the following meaning (only if "res" is M2MB_SMS_ACK_FAIL):
    - rp_cause (See Table 8.4/3GPP TS 24.011) if the incoming SMS has 3GPP format,
    - error class (See par 3.4.3.6 of 3GPP2 C.S0015) if it has 3GPP2 format.
    If "res" is M2MB_SMS_ACK_SUCCESS, it is ignored.
  @param[in] failureCause2
    fifth parameter value has the following meaning (only if "res" is M2MB_SMS_ACK_FAIL):
    - tp_cause (See par. 9.2.3.22 of 3GPP TS 23.040) if the incoming SMS has 3GPP format,
    - SMS Cause Code (See par. 2.244 of 3GPP2 X.S0004-550-E) if it has 3GPP2 format.
    If "res" is M2MB_SMS_ACK_SUCCESS, it is ignored.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_ack(h, res, transactionID, failureCause1, failureCause2);
  @code
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...

        case M2MB_SMS_INCOMING_IND:
        {
          M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
          M2MB_SMS_INCOMING_T *resp = (M2MB_SMS_INCOMING_T*)resp_struct;
          retVal = m2mb_sms_ack(h, M2MB_SMS_ACK_SUCCESS, resp->transactionID, 0, 0);
          break;
        }

        case M2MB_SMS_ACK_RESP:
        {
          M2MB_SMS_ACK_RESP_T *resp = (M2MB_SMS_ACK_RESP_T*)resp_struct;
          printf("Ack resp is %d", resp->failCause);
          break;
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_enable_ind(h, M2MB_SMS_INCOMING_IND, 1);
      //...
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_ack( M2MB_SMS_HANDLE h, M2MB_SMS_ACK_RES_E res, UINT32 transactionID, UINT32 failureCause1, UINT32 failureCause2 );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_read reads a SMS PDU from the current memory.

  @details
    m2mb_sms_read reads a SMS PDU stored in the current memory (see m2mb_sms_set_storage, m2mb_sms_write, m2mb_sms_set_route
    and m2mb_sms_get_route functions).
    The function is asynchronous. This means that the function returns immediately, but the result of the SMS reading is received
    in the callback previously set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_READ_RESP. The resp_struct must be casted to
    the M2MB_SMS_READ_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] ind
    second parameter is the index where the SMS is stored in the current memory.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_read(h, index);
  @code
    UINT32 globalIndex;
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...

        case M2MB_SMS_WRITE_RESP:
        {
          M2MB_SMS_WRITE_RESP_T *resp = (M2MB_SMS_WRITE_RESP_T*)resp_struct;
          printf("PDU index written is %ld", resp->index);
          globalIndex = resp->index;
          break;
        }

        case M2MB_SMS_READ_RESP:
        {
          M2MB_SMS_READ_RESP_T *resp = (M2MB_SMS_READ_RESP_T*)resp_struct;
          // ... my code ...
          break;
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      UINT8 pdu[] = {0x00, 0x11, 0x00, 0x0A, 0x91, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0xAA, 0x0C, 0xC8, 0xF7, 0x1D, 0x14, 0x96, 0x97, 0x41, 0xF9, 0x77, 0xFD, 0x07};
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_write(h, 25, pdu);
      //...
      retVal = m2mb_sms_read(h, globalIndex);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_read request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_read( M2MB_SMS_HANDLE h, UINT32 index );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_delete deletes a SMS PDU from the current memory.

  @details
    m2mb_sms_delete deletes a SMS PDU stored in the current memory (see m2mb_sms_set_storage, m2mb_sms_write, m2mb_sms_set_route
    and m2mb_sms_get_route functions).
    The function is asynchronous. This means that the function returns immediately, but the result of the SMS deletion is received
    in the callback previously set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_DELETE_RESP. The resp_struct must be casted to
    the M2MB_SMS_GENERIC_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] ind
    second parameter is the index where the SMS is stored in the current memory.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_delete(h, index);
  @code
    UINT32 globalIndex;
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...

        case M2MB_SMS_WRITE_RESP:
        {
          M2MB_SMS_WRITE_RESP_T *resp = (M2MB_SMS_WRITE_RESP_T*)resp_struct;
          printf("PDU index written is %ld", resp->index);
          globalIndex = resp->index;
          break;
        }

        case M2MB_SMS_DELETE_RESP:
        {
          M2MB_SMS_GENERIC_RESP_T *resp = (M2MB_SMS_GENERIC_RESP_T*)resp_struct;
          printf("Delete resp is %d", resp->response);
          break;
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      UINT8 pdu[] = {0x00, 0x11, 0x00, 0x0A, 0x91, 0x11, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0xAA, 0x0C, 0xC8, 0xF7, 0x1D, 0x14, 0x96, 0x97, 0x41, 0xF9, 0x77, 0xFD, 0x07};
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_write(h, 25, pdu);
      //...
      retVal = m2mb_sms_delete(h, globalIndex);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_delete request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_delete( M2MB_SMS_HANDLE h, UINT32 index );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_set_tag changes the tag of a SMS PDU in the current memory.

  @details
    m2mb_sms_set_tag changes the tag of a SMS PDU in the current memory (see m2mb_sms_set_storage, m2mb_sms_write, m2mb_sms_set_route
    and m2mb_sms_get_route functions).
    The function is asynchronous. This means that the function returns immediately, but the result of the SMS deletion is received
    in the callback previously set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_SET_TAG_RESP. The resp_struct must be casted to
    the M2MB_SMS_GENERIC_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] ind
    second parameter is the index where the SMS is stored in the current memory.
  @param[in] tag
    third parameter is the tag to be set in the SMS at index ind.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_set_tag(h, index, tag);
  @code
    UINT32 globalIndex;
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...

        case M2MB_SMS_SET_TAG_RESP:
        {
          M2MB_SMS_GENERIC_RESP_T *resp = (M2MB_SMS_GENERIC_RESP_T*)resp_struct;
          printf("Set tag resp is %d", resp->response);
          break;
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_set_tag(h, 1, M2MB_SMS_TAG_MT_READ);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_set_tag request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_set_tag( M2MB_SMS_HANDLE h, UINT32 index, M2MB_SMS_TAG_E tag );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_set_sca sets the Service Center Address.

  @details
    m2mb_sms_set_sca sets the Service Center Address in the xSIM.
    The function is asynchronous. This means that the function returns immediately, but the Service Center Address setting
    is confirmed in the callback previously set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_SET_SCA_RESP. The resp_struct must be casted to
    the M2MB_SMS_GENERIC_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] sca
    second parameter is an array with the Service Center Address to be stored, written in ASCII chars.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_set_sca(h, sca);
  @code
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...

        case M2MB_SMS_SET_SCA_RESP:
        {
          M2MB_SMS_GENERIC_RESP_T *resp = (M2MB_SMS_GENERIC_RESP_T*)resp_struct;
          printf("SET SCA resp is %d", resp->response);
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      UINT8 sca[] = "1234567";
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_set_sca(h, sca);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_set_sca request succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_set_sca( M2MB_SMS_HANDLE h, UINT8 *sca );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_get_sca gets current Service Center Address.

  @details
    m2mb_sms_get_sca gets current Service Center Address read from the xSIM.
    The function is asynchronous. This means that the function returns immediately, but the Service Center Address value is received
    in the callback previously set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_GET_SCA_RESP. The resp_struct must be casted to
    the M2MB_SMS_GET_SCA_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_get_sca(h);
  @code
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...

        case M2MB_SMS_GET_SCA_RESP:
        {
          M2MB_SMS_GET_SCA_RESP_T *resp = (M2MB_SMS_GET_SCA_RESP_T*)resp_struct;
          printf("GET SCA resp is %s, %s", resp->addr_type, resp->sca);
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_get_sca(h);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_get_sca request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_sms_get_sca( M2MB_SMS_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_set_route manages the behaviour of incoming SMS.

  @details
    m2mb_sms_set_route sets the way each incoming SMS is managed, depending of its class.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] smClass
    second parameter is the SMS class for which the current setting is done.
  @param[in] stType
    third parameter indicates the storage type where incoming SMSs with "smClass" class will be stored.
  @param[in] stAct
    fourth parameter indicates the action that will be performed for all the SMS with class "smClass".

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_set_route(h, smClass, stType, stAct);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_SMS_HANDLE h;
    struct myStruct myUserdata;
    retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
    //...
    retVal = m2mb_sms_set_route(h, M2MB_SMS_CLASS_0, M2MB_SMS_STORAGE_SM, M2MB_SMS_STORE_AND_ACK);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "m2mb_sms_set_route setting succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_set_route( M2MB_SMS_HANDLE h, M2MB_SMS_CLASS_E smClass, M2MB_SMS_STORAGE_E stType, M2MB_SMS_INC_ACTION_E stAct );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_get_route gets the current behaviour of a specific SMS class.

  @details
    m2mb_sms_get_route gets the current behaviour of incoming SMS per specific SMS class.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] smClass
    second parameter is the SMS class for which the current setting is done.
  @param[in] *stType
    third parameter is the current storage type where incoming SMSs with class "smClass" are stored.
  @param[in] *stAct
    fourth parameter is the current action that is performed for all the SMS with class "smClass".

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_get_route(h, smClass, &stType, &stAct);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_SMS_HANDLE h;
    M2MB_SMS_STORAGE_E stType;
    M2MB_SMS_INC_ACTION_E stAct;
    struct myStruct myUserdata;
    retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
    //...
    retVal = m2mb_sms_get_route(h, M2MB_SMS_CLASS_0, &stType, &stAct);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "m2mb_sms_get_route succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_get_route( M2MB_SMS_HANDLE h, M2MB_SMS_CLASS_E smClass, M2MB_SMS_STORAGE_E *stType, M2MB_SMS_INC_ACTION_E *stAct );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_set_storage sets the current storage for SMSs.

  @details
    m2mb_sms_set_storage sets the current storage used by m2mb_sms_read,
    m2mb_sms_write, m2mb_sms_delete and m2mb_sms_send_from_mem

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] smClass
    second parameter indicates the storage to be set.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_set_storage(h, stType);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_SMS_HANDLE h;
    struct myStruct myUserdata;
    retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
    //...
    retVal = m2mb_sms_set_storage(h, M2MB_SMS_STORAGE_SM);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "m2mb_sms_set_storage setting succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_sms_set_storage( M2MB_SMS_HANDLE h, M2MB_SMS_STORAGE_E stType );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_get_storage_stat gets the status of current storage.

  @details
    m2mb_sms_get_storage_stat gets current storage and its status (total amount of space and number of SMS currently stored).
    The function is asynchronous. This means that the function returns immediately, but the storage status is received
    in the callback previously set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_GET_STORAGE_STAT_RESP. The resp_struct must be casted to
    the M2MB_SMS_GET_STORAGE_STAT_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_get_storage_stat(h);
  @code
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...

        case M2MB_SMS_GET_STORAGE_STAT_RESP:
        {
          M2MB_SMS_GET_STORAGE_STAT_RESP_T *resp = (M2MB_SMS_GET_STORAGE_STAT_RESP_T*)resp_struct;
          printf("Storage resp: current SMS %ld, max %ld", resp->smsCounter, resp->maxStorageSize);
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_get_storage_stat(h);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_get_storage_stat request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_sms_get_storage_stat( M2MB_SMS_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_sms_get_storage_indexes gets the index list of stored SMSs.

  @details
    m2mb_sms_get_storage_indexes gets current storage and the index list of SMS currently stored into it, having a certain tag
    (see M2MB_SMS_TAG_E enum).
    The function is asynchronous. This means that the function returns immediately, but the storage indexes are received
    in the callback previously set by the m2mb_sms_init function.
    The response event received in the callback will be the M2MB_SMS_GET_STORAGE_INDEXES_RESP. The resp_struct must be
    casted to the M2MB_SMS_GET_STORAGE_INDEXES_RESP_T type.

  @param[in] h
    first parameter is the handle to the SMS service, previously initialized by the m2mb_sms_init function.
  @param[in] tag
    second parameter is the tag of SMS that will be included in the returned index list.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_sms_get_storage_indexes(h, tag);
  @code
    void myCallback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(sms_event)
      {
        //...
        case M2MB_SMS_GET_STORAGE_INDEXES_RESP:
        {
          UINT32 i;
          M2MB_SMS_GET_STORAGE_INDEXES_RESP_T *resp = (M2MB_SMS_GET_STORAGE_INDEXES_RESP_T*)resp_struct;
          for(i=0; i<resp->num_indexes; i++)
          {
            printf("PDU index %ld is %d", i, resp->indexes[i]);
          }
        }
        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_SMS_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_sms_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_sms_get_storage_indexes(h, M2MB_SMS_TAG_MT_NOT_READ);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_sms_get_storage_indexes request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_sms_get_storage_indexes( M2MB_SMS_HANDLE h, M2MB_SMS_TAG_E tag );


#endif
