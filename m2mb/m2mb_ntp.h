/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_ntp.h

  @brief
    m2m base api for ntp functionalities

  @details
    SNTP (RFC2030) implementation, providing time synchronization functionality.
    RFC5905 (symmetric key authentication) is implemented.

  @notes
    Dependencies: m2mb_types.h

  @author
    Fabio Pintus
    Davide Cubeddu (porting)

  @date
    09/04/2019
*/


#ifndef  M2M_M2MB_NTP_API_H
#define  M2M_M2MB_NTP_API_H


/** \defgroup m2mb_ntp Documentation for m2mb_ntp functionalities
 * @{
 * This group includes all the information about m2mb_ntp configuration and usage *
 *
 * @}
 */

 /*!
@ingroup m2mb_mqtt
@{
*/
/*-----------------------------------------------------------------------------------------------*/


/* Global declarations ==========================================================================*/
/*!
  @brief
    NTP defaults.

  @details
    These defines provide default values for NTP related parameters.
*/
#define M2MB_NTP_DEF_PORT 123       /**< Default NTP server port. */
#define M2MB_NTP_DEF_TIMEOUT 10     /**< Default operation timeout in secs. */


/* Global typedefs ==============================================================================*/
typedef HANDLE M2MB_NTP_HANDLE;

/*!
  @brief
    NTP Events.

  @details
    Events that will be passed to ntp callback when operation completes (either with error or success).
*/
typedef enum
{
  M2MB_NTP_VALID_TIME               = 0,          /**< The provided parameter contains a valid time in unix timestamp format*/
  M2MB_NTP_ERR_NOT_ENOUGH_RESOURCES = -1,         /**< The client was not able to obtain needed resources*/
  M2MB_NTP_ERR_REG_TIMEOUT          = -2,         /**< The operation timed out*/
  M2MB_NTP_ERR_CONN_ERR             = -3,         /**< Error enabling network */
  M2MB_NTP_ERR_CONN_TIMEOUT         = -4,         /**< Connection timed out */
  M2MB_NTP_ERR_HOST_RESOLVE         = -5,         /**< Error resolving host name*/
  M2MB_NTP_ERR_INVALID_PARAMETER    = -6,         /**< An invalid parameter was passed*/
  M2MB_NTP_ERR_SOCK_OPEN            = -7,         /**< Error opening socket*/
  M2MB_NTP_ERR_TX_TIMEOUT           = -8,         /**< Transmission timeout*/
  M2MB_NTP_ERR_TX_ERROR             = -9,         /**< Transmission error*/
  M2MB_NTP_ERR_RX_TIMEOUT           = -11,        /**< Receive timeout*/
  M2MB_NTP_ERROR                    = -20,        /**< Generic error*/
  M2MB_NTP_ERR_SERVER_AUTH_FAIL     = -30,        /**< Server authentication failure */
  M2MB_NTP_ERR_KEYS_FILE_MISSING    = -31         /**< Server keys file missing */
} M2MB_NTP_EVENTS_E;

/*!
  @brief
    Authentication options.
*/
typedef enum
{
  m2mb_ntp_noAuth        = 0,         /**< Do not authenticate server*/
  m2mb_ntp_symmetricKey  = 1,         /**< Authenticate server with symmetric key*/
  m2mb_ntp_autokey       = 2          /**< Authenticate server with autokey  - Not available yet*/
} M2MB_NTP_AUTH_TYPE_E;

typedef enum
{
  M2MB_NTP_CFG_TIMEOUT        = 0,        /**<Operation timeout in seconds (UINT32)*/
  M2MB_NTP_CFG_PORT           = 1,        /**<Server Port. Set to 0 to use default (UINT16)*/
  M2MB_NTP_CFG_SERVER         = 2,        /**<Server name. can be a resolvable address, or IPv4 /v6 ip address string */
  M2MB_NTP_CFG_AUTH_TYPE      = 3,        /**<Authentication type, see enum M2MB_NTP_AUTH_TYPE_E */
  M2MB_NTP_CFG_AUTH_KEYID     = 4,        /**<Authentication key ID (UINT32)*/
  M2MB_NTP_CFG_AUTH_KEYSFILE  = 5         /**<Authentication keys filename, already allocated*/
} M2MB_NTP_CFG_E;

/*!
  @brief
    This is the ntp user callback function prototype

  @details
    This callback function is executed by ntp core when a query operation completes (either with success or failure).

  @param[in] handle
    the npt handle associated with the operation
  @param[in] ntp_event
    The resulting event of the operation
  @param[in] resp_size
    the size in bytes of the response structure (depends on the event)
  @param[in] resp_struct
    the output data pointer. it must be cast to the correct type, depending on the event.
  @param[in] cb_args
    User specific data to be passed to the callback.

  @return
    None

  @code
    #include <time.h>
    #include "m2mb_types.h"
    #include "m2mb_socket.h"

    void m2mb_ntp_ind_callback ( M2MB_NTP_HANDLE h,
                                 M2MB_NTP_EVENTS_E ntp_event,
                                 UINT16 resp_size,
                                 void *resp_struct,
                                 void *cb_args )
    {
      struct M2MB_SOCKET_BSD_TIMEVAL *tv;
      time_t current_time;

      switch(ntp_event)
      {
      case M2MB_NTP_VALID_TIME:
      {
        tv = (struct M2MB_SOCKET_BSD_TIMEVAL *) resp_struct;
        current_time = tv->m_tv_sec;
        DEBUG("Received UNIX timestamp: %u\r\n", current_time);
      }
      break;
      default:
        ERROR("ntp error %d\r\n", ntp_event);
        break;
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
typedef void ( *m2mb_ntp_callback )( M2MB_NTP_HANDLE handle,
                                         M2MB_NTP_EVENTS_E ntp_event,
                                         UINT16 resp_size,
                                         void *resp_struct,
                                         void *cb_args );


/* Global functions =============================================================================*/
/*!
  @brief
    This is the ntp initialization function

  @details
    This function will initialize the ntp core allowing to perform further operations.

  @param[in] pHandle
    pointer to the handle that will be initialized, and will be used by all other APIs.
    Handle pointed by pHandle must be initialized to NULL before calling this API.
  @param[in] callback
    User callback that will be called by ntp core
  @param[in] cb_args
    User specific data, that will be passed to user callback when it is called.

  @return
    M2MB_RESULT_E value

  @code
    void m2mb_ntp_ind_callback ( M2MB_NTP_HANDLE h,
                                 M2MB_NTP_EVENTS_E ntp_event,
                                 UINT16 resp_size,
                                 void *resp_struct,
                                 void *cb_args )
    {
      ...
    }

    void main(void)
    {
      M2MB_NTP_HANDLE ntpH;
      M2MB_RESULT_E retVal;
      void *userContext = NULL; //user provided context data

      retVal = m2mb_ntp_init(&ntpH, m2mb_ntp_ind_callback, (void*) userContext);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("init failed.\r\n");
        return;
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_ntp_init( M2MB_NTP_HANDLE *pHandle, m2mb_ntp_callback callback, void *cb_args);

/*!
  @brief
    Set the CID to be used with NTP

  @details
    This function will set the CID associated to the NTP core to perform further operations.
    NOTE: The PDP context associated with the CID must be activated with m2mb_pdp_* APIs
          (refer to m2mb_pdp_activate).

  @param[in] handle
    the handle to be used for the operation
  @param[in] cid
    CID value to be associated with the NTP client

  @return
    M2MB_RESULT_E value

  @code
    void m2mb_ntp_ind_callback ( M2MB_NTP_HANDLE h,
                                 M2MB_NTP_EVENTS_E ntp_event,
                                 UINT16 resp_size,
                                 void *resp_struct,
                                 void *cb_args )
    {
      ...
    }

    void main(void)
    {
      M2MB_NTP_HANDLE ntpH;
      M2MB_RESULT_E retVal;
      INT32 cid = 1;
      void *userContext = NULL; //user provided context data

      retVal = m2mb_ntp_init(&ntpH, m2mb_ntp_ind_callback, (void*) userContext);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("init failed.\r\n");
        return;
      }

      retVal = m2mb_ntp_setCid(ntpH, cid);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("Set cid failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);

        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      ...

      m2mb_ntp_deinit(ntpH);
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_ntp_setCid( M2MB_NTP_HANDLE handle, INT16 cid);

/*!
  @brief
    Set the one of the NTP parameters

  @details
    will set the requested parameter associated to the NTP core to perform further operations.
    refer to M2MB_NTP_CFG_E values.

  @param[in] handle
    the handle to be used for the operation
  @param[in] param
    enum selecting which param should be set
@param[in] value
    opaque pointer to the parameter value to be set

  @return
    M2MB_RESULT_E value

  @code
    void m2mb_ntp_ind_callback ( M2MB_NTP_HANDLE h,
                                 M2MB_NTP_EVENTS_E ntp_event,
                                 UINT16 resp_size,
                                 void *resp_struct,
                                 void *cb_args )
    {
      ...
    }

    void main(void)
    {
      M2MB_NTP_HANDLE ntpH;
      M2MB_RESULT_E retVal;
      UINT16 ntpPort = 123;
      UINT32 timeout = 5; //5 seconds

      UINT16 authType = m2mb_ntp_symmetricKey;
      UINT32 keyId = 0;
      CHAR authKeysFilePath[] = "/path/to/server/keys/file";

      CHAR ntpServer[] = "0.pool.ntp.org";
      void *userContext = NULL; //user provided context data

      retVal = m2mb_ntp_init(&ntpH, m2mb_ntp_ind_callback, (void*) userContext);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("init failed.\r\n");
        return;
      }

      retVal = m2mb_ntp_setCfg(ntpH, M2MB_NTP_CFG_SERVER, (void*)ntpServer);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("set server failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);
        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      retVal = m2mb_ntp_setCfg(ntpH, M2MB_NTP_CFG_PORT, (void*) &(ntpPort));
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("set server port failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);
        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      retVal = m2mb_ntp_setCfg(ntpH, M2MB_NTP_CFG_TIMEOUT, (void*) &(timeout));
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("set timeout failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);
        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      // OPTIONAL: authentication configuration

      retVal = m2mb_ntp_setCfg(ntpH, M2MB_NTP_CFG_AUTH_TYPE, (void*) &(authType));
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("set authType failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);
        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      retVal = m2mb_ntp_setCfg(ntpH, M2MB_NTP_CFG_AUTH_KEYID, (void*) &(keyId));
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("set key id failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);
        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      retVal = m2mb_ntp_setCfg(ntpH, M2MB_NTP_CFG_AUTH_KEYSFILE, (void*) authKeysFilePath);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("set server keys file failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);
        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      ...

      m2mb_ntp_deinit(ntpH);
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_ntp_setCfg( M2MB_NTP_HANDLE handle, M2MB_NTP_CFG_E param, void *value);

/*!
  @brief
    This is the ntp query function

  @details
    This function will ask for a time value to the server specified in the passed parameters.
    IMPORTANT: This function returns immediately, operation result will be reported in user callback

  @param[in] handle
    the handle to be used for the operation

  @return
    M2MB_RESULT_E value

  @code
    #include <time.h>
    #include "m2mb_types.h"
    #include "m2mb_socket.h"

    void m2mb_ntp_ind_callback ( M2MB_NTP_HANDLE h,
                                 M2MB_NTP_EVENTS_E ntp_event,
                                 UINT16 resp_size,
                                 void *resp_struct,
                                 void *cb_args )
    {
      struct M2MB_SOCKET_BSD_TIMEVAL *tv;
      time_t current_time;

      switch(ntp_event)
      {
      case M2MB_NTP_VALID_TIME:
      {
        tv = (struct M2MB_SOCKET_BSD_TIMEVAL *) resp_struct;
        current_time = tv->m_tv_sec;

        DEBUG("Received UNIX timestamp: %u\r\n", current_time);

      }
      break;
      default:
        ERROR("ntp error %d\r\n", ntp_event);
        break;
      }
    }

    void main(void)
    {
      M2MB_NTP_HANDLE ntpH;
      M2MB_RESULT_E retVal;
      INT32 cid = 1;
      UINT16 ntpPort = 123;
      UINT32 timeout = 5; //5 seconds
      CHAR ntpServer[] = "0.pool.ntp.org";
      void *userContext = NULL; //user provided context data

      retVal = m2mb_ntp_init(&ntpH, m2mb_ntp_ind_callback, (void*) userContext);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("init failed.\r\n");
        return;
      }

      retVal = m2mb_ntp_setCid(ntpH, cid);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("Set cid failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);

        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      retVal = m2mb_ntp_setCfg(ntpH, M2MB_NTP_CFG_SERVER, (void*)ntpServer);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("set server failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);

        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      retVal = m2mb_ntp_setCfg(ntpH, M2MB_NTP_CFG_PORT, (void*) &(ntpPort));
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("set server port failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);

        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      retVal = m2mb_ntp_setCfg(ntpH, M2MB_NTP_CFG_TIMEOUT, (void*) &(timeout));
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("set timeout failed.\r\n");

        retVal = m2mb_ntp_deinit(ntpH);

        if(retVal == M2MB_RESULT_SUCCESS)
        {
          DEBUG("Deinit OK\r\n");
        }
        return;
      }

      ///=======
      CID activation

      m2mb_pdp_init(...)
      m2mb_pdp_activate(...,cid,...)

      =======///

      retVal = m2mb_ntp_queryServerTime( ntpH );

      if(M2MB_RESULT_SUCCESS != retVal)
      {
        FATAL("query failed.\r\n");
        return;
      }
      // Wait for ntp callback to complete and return the outcome,
      // either by sleep or by semaphore/event-flag

      retVal = m2mb_ntp_deinit(ntpH);
      if(retVal == M2MB_RESULT_SUCCESS)
      {
        DEBUG("Deinit OK\r\n");
      }
      else
      {
        ERROR("Cannot deinit! retVal: %d\r\n", retVal);
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_ntp_queryServerTime( M2MB_NTP_HANDLE handle );

/*!
  @brief
    This is the ntp version get function

  @details
    This function will return the ntp lib version as a string.

  @param[in] handle
    the handle to be used for the operation
  @param[inout] version
    pointer to an allocated buffer (at least 20 bytes in size) to which copy the version string
  @param[in] maxlen
    length of the allocated buffer (at least 20 bytes in size)

  @return

  @code
    void m2mb_ntp_ind_callback ( M2MB_NTP_HANDLE h,
                                 M2MB_NTP_EVENTS_E ntp_event,
                                 UINT16 resp_size,
                                 void *resp_struct,
                                 void *cb_args )
    {
      ...
    }

    void main(void)
    {
      M2MB_NTP_HANDLE ntpH;
      M2MB_RESULT_E retVal;
      CHAR ntp_ver[32];

      void *userContext = NULL; //user provided context data

      retVal = m2mb_ntp_init(&ntpH, m2mb_ntp_ind_callback, (void*) userContext);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("init failed.\r\n");
        return;
      }

      m2mb_ntp_getVersion(ntpH, ntp_ver, 32);
      INFO("ntp_version: %s\r\n",ntp_ver);

      m2mb_ntp_deinit(ntpH);
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_ntp_getVersion( M2MB_NTP_HANDLE handle, CHAR *version, UINT16 maxlen );

/*!
  @brief
    This is the ntp deinitialization function

  @details
    This function will deinitialize the ntp core, releasing all resources (including PDP context

  @param[in] handle
    the handle that will be deinitialized

  @return
    M2MB_RESULT_E value

  @code
    void m2mb_ntp_ind_callback ( M2MB_NTP_HANDLE h,
                                 M2MB_NTP_EVENTS_E ntp_event,
                                 UINT16 resp_size,
                                 void *resp_struct,
                                 void *cb_args )
    {
      ...
    }

    void main(void)
    {
      M2MB_NTP_HANDLE ntpH;
      M2MB_RESULT_E retVal;

      void *userContext = NULL; //user provided context data

      retVal = m2mb_ntp_init(&ntpH, m2mb_ntp_ind_callback, (void*) userContext);
      if(M2MB_RESULT_SUCCESS != retVal)
      {
        ERROR("init failed.\r\n");
        return;
      }

      ...

      m2mb_ntp_deinit(ntpH);
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_ntp_deinit( M2MB_NTP_HANDLE handle );


/*!
@}
*/
/*-----------------------------------------------------------------------------------------------*/
#endif  /* M2M_M2MB_NTP_API_H */

