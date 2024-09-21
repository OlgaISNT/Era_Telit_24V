/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m_mqtt.h

  @brief
    MQTT(Message Queuing Telemetry Transport) Client service.

  @details
    The MQTT (Message Queuing Telemetry Transport) Client service provides
    a collection of APIs that allow the application to implement client functionalities,
    such as CONNECT, SUBSCRIBE, UNSUBSCRIBE, PUBLISH, DISCONNECT, and so on.

  @note
    Dependencies:
    m2mb_types.h

  @author
    Oshin Bhatia

  @date
    25/10/2019
*/
#ifndef  M2M_M2MB_MQTT_API_H
#define  M2M_M2MB_MQTT_API_H

/** \defgroup m2mb_mqtt Documentation for m2mb_mqtt functionalities
 * @{
 * This group includes all the information about m2mb_mqtt configuration and usage *
 *
 * @}
 */

 /*!
@ingroup m2mb_mqtt
@{
*/
/*-----------------------------------------------------------------------------------------------*/

/* Global declarations ==========================================================================*/

/*-----------------------------------------------------------------------------------------------
  m2mb_mqtt_conf can be used without CMDS like below
  m2mb_mqtt_conf_(handle, M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo")
 -----------------------------------------------------------------------------------------------*/
#define m2mb_mqtt_conf_( h, args... )        m2mb_mqtt_conf( h, CMDS( args ) )

/*!
  @brief MQTT context handle.
    This is obtained from m2mb_mqtt_init API and then used in subsequent MQTT APIs
*/

/* Global typedef ==========================================================================*/

typedef HANDLE  M2MB_MQTT_HANDLE;

/*!
  @brief MQTT Indications enum
*/
typedef enum
{
  M2MB_MQTT_INDICATION_DISCONNECT
/**< This event is received when MQTT Broker triggers the disconnect asynchronously or when MQTT ping fails. */
} M2MB_MQTT_IND_E;

/*!
  @brief MQTT APIs response enum
*/
typedef enum
{
  M2MB_MQTT_SUCCESS = 0,                /**< Response is success */
  M2MB_MQTT_ERROR_BAD_ARG = -1,         /**< Bad argument was passed by the user */
  M2MB_MQTT_ERROR_OUT_OF_BUFFER = -2,   /**< Out of buffer */
  M2MB_MQTT_ERROR_MALFORMED_DATA = -3,  /**< Malformed Data */
  M2MB_MQTT_ERROR_PACKET_TYPE = -4,     /**< Wrong packet type */
  M2MB_MQTT_ERROR_PACKET_ID = -5,       /**< Wrong packet Id */
  M2MB_MQTT_ERROR_TLS_CONNECT = -6,     /**< Error in TCP connection */
  M2MB_MQTT_ERROR_TIMEOUT = -7,         /**< Timeout occurred */
  M2MB_MQTT_ERROR_NETWORK = -8,         /**< Error in Network */
  M2MB_MQTT_ERROR_MEMORY = -9,          /**< Out of Memory */
  M2MB_MQTT_ERROR_STAT = -10,           /**< Error in Stats */
  M2MB_MQTT_ERROR_PROPERTY = -11,       /**< Error in Properties */
  M2MB_MQTT_ERROR_SERVER_PROP = -12,    /**< Error in Server Properties */
  M2MB_MQTT_ERROR_CALLBACK = -13,       /**< Callback Error */

  M2MB_MQTT_ERROR_SEM = -14            /**< Error in connection semaphore */,
  M2MB_MQTT_ERROR_TOPIC_LIST = -15,    /**< Error related to list of subscribed topics */
  M2MB_MQTT_ERROR_ASYNC_THREAD = -16,   /**<  Error inAsync thread */
  M2MB_MQTT_ERROR_QUEUE = -17,          /**< Error in creating queue */

  M2MB_MQTT_CONTINUE = -101,            /**< MQTT Continue */
  M2MB_MQTT_STDIN_WAKE = -102,          /**< MQTT STDIN Wake */
} M2MB_MQTT_RESPONSE_E;

/*!
  @brief mqtt client status enum
*/
typedef enum
{

  M2MB_MQTT_NOT_CONNECTED,            /**< Client is initialized but not connected*/
  M2MB_MQTT_CLIENT_CONNECTED,         /**< Client performed MQTT authentication with broker*/
  M2MB_MQTT_CONN_RESET_BY_PEER,       /**< Connection closed or reset by the server*/
  M2MB_MQTT_PINGREQ_TIMEOUT_FAIL,     /**< The answer to the ping request packet was not received */

  M2MB_MQTT_CONNACK_TIMEOUT,          /**< The CONNACK packet was not received */
  M2MB_MQTT_CONNECT_TIMEOUT,          /**< The CONNECT packet was not delivered */
  M2MB_MQTT_FATAL_ERROR,              /**< Failure in the m2mb APIs */
  M2MB_MQTT_NETWORK_ERROR,            /**< Socket timeout or read error */
  M2MB_MQTT_STATUS_UNKNOWN = 0xFF,    /**< Unknown state or client not initialized */
} M2MB_MQTT_STATUS_E;

/*!
  @brief MQTT actions enum
  List of actions performed through m2mb_mqtt_conf API.
*/
typedef enum
{

  M2MB_MQTT_SET_CLIENT_ID,
/**< To set client id of the device.\n
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );
      or
      m2mb_mqtt_conf_( p_mqtt, M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ); */

  M2MB_MQTT_SET_TIMEOUT_MS,
/**< To set Timeout for MQTT operations in milliseconds.\n
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, 10*1000 ) ); */

  M2MB_MQTT_SECURE_OPT,
/**< To enable SSL and set SSL configurations. If this is not set, SSL is not enabled.\n
      M2MB_SSL_CONFIG_HANDLE    sslConfigHndl;
      M2MB_SSL_CTXT_HANDLE    sslCtxtHndl;
      //Get sslConfigHndl and sslCtxtHndl using m2mb_ssl_create_config, m2mb_ssl_create_ctxt API respectively
      //Perform required SSL configurations.
      m2mb_mqtt_conf(p_mqtt, CMDS(M2MB_MQTT_SECURE_OPT, sslConfigHndl, sslCtxtHndl)); */

  M2MB_MQTT_SET_KEEPALIVE_SEC,
/**< To set keep alive interval of the connection in seconds \n
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_KEEPALIVE_SEC, 60 ) ); */

  M2MB_MQTT_SET_CLEAN_SESSION,
/**< Set clean session flag that will be used by the MQTT core. User has to specify value to be set.\n
    If it is set to 1, session will be non-persistent. Else, it will be persistent.\n
    Default value is 1.
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLEAN_SESSION, 1 ) ); */

  M2MB_MQTT_SET_PROTOCOL_LEVEL,
/**< Set MQTT Protocol level: Protocol version: 4=v3.1.1 (default), No support 5=v5.0 \n
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PROTOCOL_LEVEL, 4 ) ); */

  M2MB_MQTT_SET_LAST_WILL,
/**< Set MQTT Protocol will flag that will be used by the MQTT core. \n
    If third parameter is 1, Optional Last will and testament will be enabled. In that case also give
    the following parameters: \n
      * Fourth parameter is the variable containing the will QoS value to be set. \n
      * Fifth parmeter is the variable containing the will retain value to be set. \n
      * Sixth parmeter is the variable containing the will topic string to be set. \n
      * Seventh parmeter is the variable containing the will message string to be set. \n
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_LAST_WILL, 1, M2MB_MQTT_QOS_1, 1, "things/topic1", "Will Message" ) ); */

  M2MB_MQTT_SET_USERNAME,
/**< Set the MQTT client username that will be used by the MQTT core. \n
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_USERNAME, "aabbcc" ) ); */

  M2MB_MQTT_SET_PASSWORD,
/**< The MQTT client username that will be used by the MQTT core. \n
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PASSWORD, "1234567890" ) ); */

  M2MB_MQTT_SET_PDP_CONTEXT,
/**< To set pdp context to be used for the MQTT client. \n
     m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PDP_CONTEXT, 2 ) ); */

  M2MB_MQTT_GET_CLIENT_ID,
/**< To retrieve client id of the device configured by the user. \n
      UINT8* cid = NULL;
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_GET_CLIENT_ID, &cid ) );
      printf("Cid = %s", cid); */

  M2MB_MQTT_GET_TIMEOUT_MS,
/**< To retrieve timeout of the MQTT operations in milliseconds. \n
      UINT32 timeout = 0;
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, &timeout ) );
      printf("timeout = %d ms", timeout); */

  M2MB_MQTT_GET_KEEPALIVE_SEC,
/**< To retrieve Keepalive interval in seconds. \n
      UINT32 keepalive = 0;
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, &keepalive ) );
      printf("keepalive = %d ms", keepalive); */

  M2MB_MQTT_GET_CLEAN_SESSION,
/**< To retrieve configured clean session flag that will be used by the MQTT core. \n
      UINT8 clean_session;
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLEAN_SESSION, &clean_session ) );
      printf("clean session = %d", clean_session); */

  M2MB_MQTT_GET_PROTOCOL_LEVEL,
/**< To retrieve MQTT protocol version as configured by the user. \n
      INT32 version;
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_GET_PROTOCOL_LEVEL, &version ) );
      printf("protocol version = %d", version); */

  M2MB_MQTT_GET_LAST_WILL,
/**< To retrieve Last will and testament settings used by MQTT core \n
      UINT8 will;
      M2MB_MQTT_QOS_E qos;
      UINT8 retain;
      UINT8* topic = NULL;
      UINT8* message = NULL;
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_GET_LAST_WILL, &will, &qos, &retain, &topic, &message ) );
      printf("last will = %d\r\n", will);
      printf("will qos = %d\r\n", qos);
      printf("will retain = %d\r\n", retain);
      printf("will topic = %s\r\n", topic);
      printf("will message = %s\r\n", message); */

  M2MB_MQTT_GET_PDP_CONTEXT,
 /**< To retrieve pdp context to be used for the MQTT client. \n
      UINT8 pdp;
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_GET_PDP_CONTEXT, &pdp ) );
      printf("pdp = %d", pdp); */

}M2MB_MQTT_ACTION_E;

/*!
  @brief MQTT QoS Enum.
*/
typedef enum
{
  M2MB_MQTT_QOS_0 = 0,    /**< At most once delivery */
  M2MB_MQTT_QOS_1 = 1,    /**< At least once delivery */
  M2MB_MQTT_QOS_2 = 2,    /**< Exactly once delivery */
} M2MB_MQTT_QOS_E;

/*!
  @brief MQTT Receive Data Status Enum.
*/
typedef enum
{
  M2MB_MQTT_RX_FINISHED,   /**< Entire data is received */
  M2MB_MQTT_RX_MORE_DATA,  /**< Partial data is pending */
} M2MB_MQTT_RX_STATUS_E;

/*!
  @brief
    MQTT call back prototype

  @details
    This callback is triggered when MQTT client receives any event.(M2MB_MQTT_IND_E)

  @param[in] Handle
    handle to the MQTT client instance on which MQTT message is received
  @param[in] event
    event indication for which the callback has been triggered
  @param[in] resp_size
    size of the data sent
  @param[in] resp
    response
  @param[in] userdata
	user data returned by callback function

  @return
    None

  @note

  @b
    Example
  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //Set Client ID
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );

      //Set Timeout in milliseconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, 10*1000 ) );

      //Set Keepalive in seconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_KEEPALIVE_SEC, 60 ) );

      //Set Username
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_USERNAME, "aabbcc" ) );

      //Set Password
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PASSWORD, "1234567890" ) );

      result = m2mb_mqtt_connect( p_mqtt, "api-dev.devicewise.com", 1883 ) );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

      //... Use MQTT APIs ....

      result = m2mb_mqtt_deinit( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

    }

  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
typedef void ( *m2mb_mqtt_ind_callback ) ( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
/*!
  @brief
    MQTT message handler call back prototype

  @details
    This callback function is called when a message is received on a subscribed topic. This is registered
    while subscribing to a topic.

  @param[in] Handle
    The mqtt handle associated with the client for which data is received
  @param[in] arg
    User defined pointer, set while registering the callback
  @param[in] topic
    The topic string
  @param[in] topic_Length
    The topic string len
  @param[in] msg
    The received payload
  @param[in] msg_Length
    The payload size in bytes
  @param[in] status
    Status whethe more data is pending.

  @return
  None

  @note
    If the message payload is larger than receive data buffer (1024), this callback will be called multiple times
    for the same message recived. Parameter status is used to identify if there is pending data.

  @b
    Example
  @code

    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void mqtt_topic_cb( M2MB_MQTT_HANDLE handle, void* arg, const CHAR* topic, UINT16 topic_length, const CHAR* msg, UINT32 msg_length, M2MB_MQTT_RX_STATUS_E status )
    {
      UINT8 buf[1024];
      memcpy(buf, topic, topic_length);
      buf[topic_length] = '\0'; // Make sure it is null terminated

      printf("MQTT Message: Topic %s, Len %d\r\n", buf, msg_length);

      memcpy(buf, msg, msg_length);
      buf[msg_length] = '\0'; // Make sure it is null terminated
      printf("Message: %s",  buf);

      if(status == M2MB_MQTT_RX_MORE_DATA)
      {
        printf("More data to be received");
      }
    }

    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //Set Client ID
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );

      //Set Timeout in milliseconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, 10*1000 ) );

      //Set Keepalive in seconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_KEEPALIVE_SEC, 60 ) );

      //Set Username
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_USERNAME, "aabbcc" ) );

      //Set Password
      m2mb_mqtt_conf(p_mqtt, CMDS(M2MB_MQTT_SET_PASSWORD, "1234567890"));

      result = m2mb_mqtt_connect( p_mqtt, "api-dev.devicewise.com", 1883 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_connect succeeded");
      }

      result = m2mb_mqtt_publish( p_mqtt, 0, 0, 10, "things/aabbcc/property/temp", "123", 3 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_publish succeeded");
      }

      M2MB_MQTT_TOPIC_T topics[2] = {0};

      CHAR* topic1 = "things/topic1";
      CHAR* topic2 = "things/topic2";

      topics[0].topic_filter = topic1;
      topics[0].qos = 0;
      topics[0].cb = mqtt_topic_cb;

      topics[1].topic_filter = topic2;
      topics[1].qos = 1;
      topics[1].cb = mqtt_topic_cb;

      result = m2mb_mqtt_subscribe( p_mqtt, 11, 2, topics );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_subscribe succeeded");
      }

      //... Use MQTT APIs ....

      result = m2mb_mqtt_disconnect( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_disconnect succeeded");
      }

      result = m2mb_mqtt_deinit( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

    }
    @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
typedef void ( *M2MB_MQTT_MSG_HNDLR_CB_T ) ( M2MB_MQTT_HANDLE Handle, void* arg, const CHAR* topic,
              UINT16 topic_length, const CHAR* msg, UINT32 msg_length, M2MB_MQTT_RX_STATUS_E status );


/*!
  @brief MQTT Topic Structure
  This is used in m2mb_mqtt_subscribe and m2mb_mqtt_unsubscribe API
*/
typedef struct M2MB_MQTT_TOPIC
{
  const CHAR* topic_filter;     /**< Topic string */
  M2MB_MQTT_QOS_E qos;          /**< QOS */
  M2MB_MQTT_MSG_HNDLR_CB_T cb;  /**< Message handler callback for the topic */
  void* arg;/**< User argument to be passed to callback function */
} M2MB_MQTT_TOPIC_T;


/* Global functions =============================================================================*/

/*---------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_init is called to initialize MQTT context.

  @details
    m2mb_mqtt_init initialize MQTT client context returning the handle that must be passed as first parameter
    for all MQTT actions. Calling the m2mb_mqtt_init is mandatory before using the MQTT client service.
    It should be called once for each MQTT client.

  @param[out] pHandle
    pointer to MQTT context handle that is is initialized by the function. This is to be used in subsequent MQTT APIs.
  @param[out] callback
    callback that will be called if client receives an event. (ex: server disconnect the client and disconnect event is triggered)
  @param[out] userdata
    a pointer to generic user data that will be returned as it is in the callback.

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note

  @b
    Example
  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }

    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //... Use MQTT APIs ....
    }
  @endcode
*/

/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_init( M2MB_MQTT_HANDLE *h, m2mb_mqtt_ind_callback callback, void *userdata );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_deinit deinitializes MQTT service for specified MQTT client.

  @details
    m2mb_mqtt_deinit deinitialize MQTT service for specified MQTT client releasing the resources associated with the handle
    as first parameter. Calling the m2mb_mqtt_deinit is needed after having completed the usage of MQTT service to release reserved memory

  @param[in] Handle
    handle to the MQTT client service, that will be deinitialized by the function.

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note

  @b
    Example

  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //... Use MQTT APIs ....

      result = m2mb_mqtt_deinit( p_mqtt );
      if ( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_deinit( M2MB_MQTT_HANDLE Handle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_conf sets/gets some MQTT parameters and configurations.

  @details
    m2mb_mqtt_conf sets/gets MQTT parameters listed in the M2MB_MQTT_ACTION_E enum. These are to be performed before
    m2mb_mqtt_connect.

  @param[in] Handle
    handle to the MQTT client service, previously initialized by the m2mb_mqtt_init function.
  @param[in] argc
    determines the number of parameters that follows
  @param[in]
    action (among the ones listed in M2MB_MQTT_ACTION_E), while the parameters
    that follow depend on the action (see following note for further details)

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note
    Action: M2MB_MQTT_SET_CLIENT_ID
    refer m2mb_mqtt_conf_ to avoid specifiying number of arguments or without using CMDS

    Description: set the Unique clinet ID in order to connect to the MQTT broker. This setting could be
    performed only before m2mb_mqtt_connect.

    Params(in): h(M2MB_MQTT_HANDLE), action(M2MB_MQTT_ACTION_E), format(UINT8*)

    - example: set the CId 2

    m2mb_mqtt_conf_( h, M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" );
    or
    m2mb_mqtt_conf( h, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );

  @b
  Example

  @code
    void m2mb_mqtt_ind_callback(M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //Set Client ID
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );

      //Set Timeout in milliseconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, 10*1000 ) );

      //Set Keepalive in seconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_KEEPALIVE_SEC, 60 ) );

      //Set Username
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_USERNAME, "aabbcc" ) );

      //Set Password
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PASSWORD, "1234567890" ) );

      //... Use MQTT APIs ....

      result = m2mb_mqtt_deinit( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_conf( M2MB_MQTT_HANDLE Handle, UINT8 nCmds, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_connect is called to connect to the specified MQTT broker.

  @details
    m2mb_mqtt_connect performs MQTT connection to the URL specified in host.

  @param[in] Handle
    handle to the MQTT client service, previously initialized by the m2mb_mqtt_init function.
  @param[in] host
    URL of the remote MQTT Broker that MQTT client needs to connect.(Ex: "api-dev.devicewise.com")
  @param[in] port
    Port number of the remote MQTT Broker that MQTT client needs to connect.(Ex: 1883)

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note

  @b
    Example
  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //Set Client ID
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );

      //Set Timeout in milliseconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, 10*1000 ) );

      //Set Keepalive in seconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_KEEPALIVE_SEC, 60 ) );

      //Set Username
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_USERNAME, "aabbcc" ) );

      //Set Password
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PASSWORD, "1234567890" ) );

      result = m2mb_mqtt_connect( p_mqtt, "api-dev.devicewise.com", 1883 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

      //... Use MQTT APIs ....

      result = m2mb_mqtt_deinit( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

    }

  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_connect( M2MB_MQTT_HANDLE Handle, const CHAR *host, UINT32 port );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_disconnect is called to disconnect from the MQTT broker.

  @details
    m2mb_mqtt_disconnect disconnects the specified MQTT client from the Broker.

  @param[in] Handle
    handle to the MQTT client service, previously initialized by the m2mb_mqtt_init function.

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note

  @b
    Example
  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //Set Client ID
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );

      //Set Timeout in milliseconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, 10*1000 ) );

      //Set Keepalive in seconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_KEEPALIVE_SEC, 60 ) );

      //Set Username
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_USERNAME, "aabbcc" ) );

      //Set Password
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PASSWORD, "1234567890" ) );

      result = m2mb_mqtt_connect( p_mqtt, "api-dev.devicewise.com", 1883 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_connect succeeded");
      }

      //... Use MQTT APIs ....

      result = m2mb_mqtt_disconnect( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_disconnect succeeded");
      }

      result = m2mb_mqtt_deinit( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

    }
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_disconnect( M2MB_MQTT_HANDLE Handle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_publish is called to Publish a message to the MQTT broker.

  @details
    m2mb_mqtt_publish Publishes a message on a given topic.

  @param[in] Handle
    handle to the MQTT client service, previously initialized by the m2mb_mqtt_init function
  @param[in] qos
    Integer value 0, 1, or 2 indicating the QOS to be used for msg.
  @param[in] retain
    Set to TRUE to make the message retained
  @param[in] message_id
    Message ID of the MQTT message
  @param[in] topic
    Topic name
  @param[in] data
    MQTT Message payload data
  @param[in] data_len
    Data length

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note
  @b
    Example
  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //Set Client ID
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );

      //Set Timeout in milliseconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, 10*1000 ) );

      //Set Keepalive in seconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_KEEPALIVE_SEC, 60 ) );

      //Set Username
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_USERNAME, "aabbcc" ) );

      //Set Password
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PASSWORD, "1234567890" ) );

      result = m2mb_mqtt_connect( p_mqtt, "api-dev.devicewise.com", 1883 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_connect succeeded");
      }

      result = m2mb_mqtt_publish( p_mqtt, 0, 0, 10, "things/aabbcc/property/temp", "123", 3 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_publish succeeded");
      }

      //... Use MQTT APIs ....

      result = m2mb_mqtt_disconnect( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_disconnect succeeded");
      }

      result = m2mb_mqtt_deinit( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_publish( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_QOS_E qos, UINT8 retain, UINT16 message_id, const CHAR *topic, const CHAR *data, UINT32 data_len );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_subscribe is called to Subscribe.

  @details
    m2mb_mqtt_subscribe Subscribes to one or multiple topics.

  @param[in] Handle
    handle to the MQTT client service, previously initialized by the m2mb_mqtt_init function.
  @param[in] message_id
    Message ID of the MQTT message
  @param[in] topic_count
    Number of topics to be subscribed
  @param[in] topics
    Contiguous list of topics to subscribe to

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note

  @b
    Example

  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }

    void mqtt_topic_cb( M2MB_MQTT_HANDLE handle, void* arg, const CHAR* topic, UINT16 topic_length, const CHAR* msg, UINT32 msg_length, M2MB_MQTT_RX_STATUS_E status )
    {
      UINT8 buf[1024];
      memcpy(buf, topic, topic_length);
      buf[topic_length] = '\0'; // Make sure it is null terminated

      printf("MQTT Message: Topic %s, Len %d\r\n", buf, msg_length);

      memcpy(buf, msg, msg_length);
      buf[msg_length] = '\0'; // Make sure it is null terminated
      printf("Message: %s",  buf);

      if(status == M2MB_MQTT_RX_MORE_DATA)
      {
        printf("More data to be received");
      }
    }


    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //Set Client ID
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );

      //Set Timeout in milliseconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, 10*1000 ) );

      //Set Keepalive in seconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_KEEPALIVE_SEC, 60 ) );

      //Set Username
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_USERNAME, "aabbcc" ) );

      //Set Password
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PASSWORD, "1234567890" ) );

      result = m2mb_mqtt_connect( p_mqtt, "api-dev.devicewise.com", 1883 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_connect succeeded");
      }

      result = m2mb_mqtt_publish( p_mqtt, 0, 0, 10, "things/aabbcc/property/temp", "123", 3 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_publish succeeded");
      }

      M2MB_MQTT_TOPIC_T topics[2] = {0};

      CHAR* topic1 = "things/topic1";
      CHAR* topic2 = "things/topic2";

      topics[0].topic_filter = topic1;
      topics[0].qos = 0;
      topics[0].cb = mqtt_topic_cb;

      topics[1].topic_filter = topic2;
      topics[1].qos = 1;
      topics[1].cb = mqtt_topic_cb;

      result = m2mb_mqtt_subscribe( p_mqtt, 11, 2, topics );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_subscribe succeeded");
      }

      //... Use MQTT APIs ....

      result = m2mb_mqtt_disconnect( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_disconnect succeeded");
      }

      result = m2mb_mqtt_deinit( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_subscribe( M2MB_MQTT_HANDLE Handle, UINT16 message_id, UINT32 topic_count, M2MB_MQTT_TOPIC_T *pTopics );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_unsubscribe is called to unsubscribe.

  @details
    m2mb_mqtt_unsubscribe unsubscribes to one or multiple topics.

  @param[in] Handle
    handle to the MQTT client service, previously initialized by the m2mb_mqtt_init function.
  @param[in] message_id
    Message ID of the MQTT message
  @param[in] topic_count
    Number of topics to be subscribed
  @param[in] topics
    Contiguous list of topics to unsubscribe to

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note

  @b
    Example
  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //Set Client ID
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );

      //Set Timeout in milliseconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, 10*1000 ) );

      //Set Keepalive in seconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_KEEPALIVE_SEC, 60 ) );

      //Set Username
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_USERNAME, "aabbcc" ) );

      //Set Password
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PASSWORD, "1234567890" ) );

      result = m2mb_mqtt_connect( p_mqtt, "api-dev.devicewise.com", 1883 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_connect succeeded");
      }

      result = m2mb_mqtt_publish( p_mqtt, 0, 0, 10, "things/aabbcc/property/temp", "123", 3 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_publish succeeded");
      }

      M2MB_MQTT_TOPIC_T topics[2] = {0};

      CHAR* topic1 = "things/topic1";
      CHAR* topic2 = "things/topic2";

      topics[0].topic_filter = topic1;
      topics[0].qos = 0;

      topics[1].topic_filter = topic2;
      topics[1].qos = 1;

      result = m2mb_mqtt_subscribe( p_mqtt, 11, 2, topics );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_subscribe succeeded");
      }

      //... Use MQTT APIs ....

      result = m2mb_mqtt_unsubscribe( p_mqtt, 11, 2, topics );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_unsubscribe succeeded");
      }

      result = m2mb_mqtt_disconnect( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_disconnect succeeded");
      }

      result = m2mb_mqtt_deinit( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_unsubscribe( M2MB_MQTT_HANDLE Handle, UINT16 message_id, UINT32 topic_count, M2MB_MQTT_TOPIC_T *topics );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_ping is called to ping

  @details
    m2mb_mqtt_ping is called to ping the remote MQTT broker

  @param[in] Handle
    handle to the MQTT client service, previously initialized by the m2mb_mqtt_init function.

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note

  @b
    Example
  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      //Set Client ID
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_CLIENT_ID, "MQTT_demo" ) );

      //Set Timeout in milliseconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_TIMEOUT_MS, 10*1000 ) );

      //Set Keepalive in seconds
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_KEEPALIVE_SEC, 60 ) );

      //Set Username
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_USERNAME, "aabbcc" ) );

      //Set Password
      m2mb_mqtt_conf( p_mqtt, CMDS( M2MB_MQTT_SET_PASSWORD, "1234567890" ) );

      result = m2mb_mqtt_connect( p_mqtt, "api-dev.devicewise.com", 1883 );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_connect succeeded");
      }

      result = m2mb_mqtt_ping( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_ping succeeded");
      }

      //... Use MQTT APIs ....

      result = m2mb_mqtt_disconnect( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_disconnect succeeded");
      }

      result = m2mb_mqtt_deinit( p_mqtt );
      if( result == M2MB_MQTT_SUCCESS )
      {
        printf( "m2mb_mqtt_deinit succeeded");
      }

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_ping( M2MB_MQTT_HANDLE Handle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_getClientStatus gets MQTT client current status

  @details
    This function allows to retrieve current status of the specified client.

  @param[in] handle
    handle of the mqtt client
  @param[out] status
    pointer to the variable that will be filled with current client status (refer to M2MB_MQTT_STATUS_E enum)

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note

  @b
    Example

  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;
      M2MB_MQTT_STATUS_E status;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      m2mb_mqtt_getClientStatus( p_mqtt, &status );
      if(status == M2MB_MQTT_NOT_CONNECTED)
      {
        printf("Client is initialized but not connected");
      }

      //... Use MQTT APIs ....
    }
    @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_getClientStatus( M2MB_MQTT_HANDLE handle, M2MB_MQTT_STATUS_E *status );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_mqtt_setClientStatus sets MQTT client current status

  @details
    This function allows to set current status of the specified client.

  @param[in] Handle
    handle of the mqtt client
  @param[in] status
    set as current client status (refer to M2MB_MQTT_STATUS_E enum)

  @return
    M2MB_MQTT_RESPONSE_E values.

  @note

  @b
    Example

  @code
    void m2mb_mqtt_ind_callback( M2MB_MQTT_HANDLE Handle, M2MB_MQTT_IND_E event, UINT16 resp_size, void *resp, void *userdata );
    {
      (void)handle;
      switch(event)
      {
        case M2MB_MQTT_INDICATION_DISCONNECT:
          printf("Disconnect Callback: event =  %d", event);
        break;
        default :
          printf("default case");
        break;
      }
    }
    void main(void)
    {
      M2MB_MQTT_HANDLE p_mqtt;
      M2MB_MQTT_RESPONSE_E result = 0;

      result = m2mb_mqtt_init( &p_mqtt, m2mb_mqtt_ind_callback, NULL );
      if(result != M2MB_MQTT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_mqtt_init succeeded");
      }

      result = m2mb_mqtt_setClientStatus( p_mqtt, M2MB_MQTT_NOT_CONNECTED );
      if(result == M2MB_MQTT_SUCCESS)
      {
        printf("Client status set successfully");
      }

       //... Use MQTT APIs ....
    }
    @endcode
  */
/*-----------------------------------------------------------------------------------------------*/
M2MB_MQTT_RESPONSE_E m2mb_mqtt_setClientStatus( M2MB_MQTT_HANDLE handle, M2MB_MQTT_STATUS_E status );

/*!
@}
*/
/*-----------------------------------------------------------------------------------------------*/
#endif /*M2MB_MQTT_API_H*/
