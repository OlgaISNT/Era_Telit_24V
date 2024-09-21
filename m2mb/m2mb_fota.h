/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_fota.h

  @brief
    Header file for m2mb_fota APIs

  @notes
    Dependencies: stdarg.h, m2mb_types.h

  @author
    mb_

  @date
    26/05/2017
*/


#ifndef M2M_M2MB_FOTA_H
#define M2M_M2MB_FOTA_H


/* Global declarations ==========================================================================*/
#define M2MB_LOAD_UPDATED_APPS         0xAABB
#define M2MB_SKIP_BACKUP_RESTORE_PARAM 0xCCDD
#define M2MB_DEFAULT_LOAD_UPDATED_APPS 0xFFFF
#define M2MB_PARAM_LEN                 (255+1)

/* Global typedefs ==============================================================================*/
typedef enum
{
  M2MB_FOTA_STATE_UPDATE_START = 0,
  M2MB_FOTA_STATE_UPDATE_PHASE_LAST = 0xF000,
  M2MB_FOTA_STATE_UPDATE_CHECK_IN_AGENT = 0xF800,
  M2MB_FOTA_STATE_CHECK_VERSION = 0xFF00,
  M2MB_FOTA_STATE_UPDATED = 0xFF10,
  M2MB_FOTA_STATE_UPDATED_NO_BACKUP_PARAMETERS = 0xFF15,
  M2MB_FOTA_STATE_BACKUP_PARAMETERS = 0xFF20,
  M2MB_FOTA_STATE_AFTER_BACKUP_PARAMETERS = 0xFF23,
  M2MB_FOTA_STATE_RESTORE_PARAMETERS = 0xFF30,
  M2MB_FOTA_STATE_AFTER_RESTORE_PARAMETERS = 0xFF33,
  M2MB_FOTA_STATE_URC_PURGE = 0xFF40,
  M2MB_FOTA_STATE_AFTER_PURGE = 0xFF50,
  M2MB_FOTA_STATE_SPECIAL_CASE = 0xFF60,
  M2MB_FOTA_STATE_IDLE = 0xFFFF
} M2MB_FOTA_STATE_E;

typedef enum
{
  M2MB_XQCN_STATE_BACKUPED        = 0,
  M2MB_XQCN_STATE_WRITTEN         = 0x1000,
  M2MB_XQCN_STATE_ERASED          = 0x1500,
  M2MB_XQCN_STATE_END             = 0x2000,
  M2MB_XQCN_STATE_READY           = 0xFFFF
} M2MB_XQCN_STATE_E;

typedef enum
{
  M2MB_Q6ZIP_STATE_READY          = 0xFFFF,
  M2MB_Q6ZIP_STATE_ERASING        = 0xFFFE,
  M2MB_Q6ZIP_STATE_END            = 0xFFFD
} M2MB_Q6ZIP_STATE_E;

typedef enum
{
  M2MB_CLADE_STATE_READY          = 0xFFFF,
  M2MB_CLADE_STATE_BACKUP_START   = 0xFFFE,
  M2MB_CLADE_STATE_BACKUP_END     = 0xFFFD
} M2MB_CLADE_STATE_E;

typedef enum
{
  M2MB_FOTA_INTEGRITY_CHECK       = 0xFFFF,
  M2MB_FOTA_INTEGRITY_PASS        = 0xFFFE,
  M2MB_FOTA_INTEGRITY_FAIL        = 0xFFFD
} M2MB_FOTA_INTEGRITY_E;

typedef enum
{
  M2MB_FOTA_TRACE_VERBOSE_LEVEL_0,
  M2MB_FOTA_TRACE_VERBOSE_LEVEL_1,
  M2MB_FOTA_TRACE_VERBOSE_LEVEL_2,
  M2MB_FOTA_TRACE_VERBOSE_LEVEL_ALL,
  M2MB_FOTA_TRACE_VERBOSE_LEVEL_NO_CHANGE,
  M2MB_FOTA_TRACE_VERBOSE_LEVEL_PRINT = 0xFFF0,
  M2MB_FOTA_TRACE_VERBOSE_LEVEL_NUM = 0xFFFF
} M2MB_FOTA_TRACE_VERBOSE_LEVEL_E;

typedef enum
{
  M2MB_FOTA_TRACE_SPEED_NO_CHANGE,
  M2MB_FOTA_TRACE_SPEED_4800,
  M2MB_FOTA_TRACE_SPEED_9600,
  M2MB_FOTA_TRACE_SPEED_115200,
  M2MB_FOTA_TRACE_SPEED_230400,
  M2MB_FOTA_TRACE_SPEED_460800,
  M2MB_FOTA_TRACE_SPEED_921600,
  M2MB_FOTA_TRACE_SPEED_BAUD_NUMBER,
  M2MB_FOTA_TRACE_SPEED_NUM = 0xFFFF
} M2MB_FOTA_TRACE_SPEED_E;

typedef enum
{
  M2MB_FOTA_RESULT_CODE_UNDEF,
  M2MB_FOTA_RESULT_CODE_OK,
  M2MB_FOTA_RESULT_CODE_ERROR_GET_STATE,
  M2MB_FOTA_RESULT_CODE_READ_ERROR,
  M2MB_FOTA_RESULT_CODE_WRITE_ERROR,
  M2MB_FOTA_RESULT_CODE_GET_INFO_ERROR,
  M2MB_FOTA_RESULT_CODE_INVALID_TRACE_VERBOSE_LEVEL,
  M2MB_FOTA_RESULT_CODE_INVALID_TRACE_SPEED,
  M2MB_FOTA_RESULT_CODE_GET_PARTITION_FAIL,
  M2MB_FOTA_RESULT_CODE_BAD_MALLOC,
  M2MB_FOTA_RESULT_CODE_ERROR_NO_TAG,
  M2MB_FOTA_RESULT_CODE_ERROR_MD5_CALCULATE,
  M2MB_FOTA_RESULT_CODE_ERROR_MD5_CHECK,
  M2MB_FOTA_RESULT_CODE_ERROR_BAD_ACTION,
  M2MB_FOTA_RESULT_CODE_ERROR_PARTITION_COPY,
  M2MB_FOTA_RESULT_CODE_ERROR_GET_FW_CNV_ELEMENTS,
  M2MB_FOTA_RESULT_CODE_ERROR_CHECK_FW_DELTA_CNV_ELEMENTS,
  M2MB_FOTA_RESULT_CODE_ERROR_ERASE_FW_CNV_ELEMENTS,
  M2MB_FOTA_RESULT_CODE_INVALID_HANDLE,
  M2MB_FOTA_RESULT_CODE_TASK_ERROR,
  M2MB_FOTA_RESULT_CODE_ERROR_SHA256_CALCULATE,
  M2MB_FOTA_RESULT_CODE_ERROR_SHA256_CHECK,
  M2MB_FOTA_RESULT_CODE_ERROR_FOTA_IN_PROGRESS,
  M2MB_FOTA_RESULT_CODE_NUM
} M2MB_FOTA_RESULT_CODE_E;

typedef enum
{
  M2MB_FOTA_CHECK_SETUP_SOURCE,
  M2MB_FOTA_CHECK_SETUP_DESTINATION,
  M2MB_FOTA_CHECK_SETUP_NUM
} M2MB_FOTA_CHECK_SETUP_E;

typedef enum
{
  M2MB_FOTA_DOWNLOAD_TYPE_HTTP,
  M2MB_FOTA_DOWNLOAD_TYPE_HTTPS,
  M2MB_FOTA_DOWNLOAD_TYPE_FTP,
  M2MB_FOTA_DOWNLOAD_TYPE_FTPS,
  M2MB_FOTA_DOWNLOAD_TYPE_COAP,
  M2MB_FOTA_DOWNLOAD_TYPE_COAPS,
  M2MB_FOTA_DOWNLOAD_TYPE_MAX_NUM
} M2MB_FOTA_DOWNLOAD_TYPE_E;

typedef enum
{
  M2MB_FOTA_DOWNLOAD_SUCCESS,
  M2MB_FOTA_DOWNLOAD_TASK_INTERNAL_ERROR,
  M2MB_FOTA_DOWNLOAD_NOT_RESOLVED,
  M2MB_FOTA_DOWNLOAD_CONN_SOCKET_ERROR,
  M2MB_FOTA_DOWNLOAD_CONN_CONTEXT_ERROR,
  M2MB_FOTA_DOWNLOAD_ACCESS_DENIED,
  M2MB_FOTA_DOWNLOAD_FILE_NOT_FOUND,
  M2MB_FOTA_DOWNLOAD_SESSION_ERROR,
  M2MB_FOTA_DOWNLOAD_MEMORY_ERROR
} M2MB_FOTA_DOWNLOAD_RESULT_E;

typedef enum
{
  M2MB_FOTA_EXTENDED_FTP_PROTOCOL,
  M2MB_FOTA_BASIC_FTP_PROTOCOL
} M2MB_FOTA_FTP_PROTOCOL_E;

typedef enum
{
  M2MB_FOTA_EVENT_HTTP_DELTA_TRANSFER_END,
  M2MB_FOTA_EVENT_FTP_DELTA_TRANSFER_END
} M2MB_FOTA_IND_E;

typedef void *M2MB_FOTA_HANDLE;

typedef struct
{
  UINT8 cid;
  CHAR server_addr[256];      // es.: "lwm2m.devicewise.com" or "34.200.145.8"
  UINT16 port;                // HTTP port
  CHAR resource[256];         // HTTP resource path, es.: "/pubfile/lwm2m/MOTDATA_N.txt"
  CHAR user[M2MB_PARAM_LEN];  // username (empty if not present)
  CHAR psw[M2MB_PARAM_LEN];   // password (empty if not present)
  UINT32 timeout;             // milliseconds
  BOOLEAN HTTPSEn;            // secure HTTP ( on = TRUE /off = FALSE )
  void *sslConfig;            // the M2MB_SSL_CONFIG_T type must be used to create the sslConfig object. If sslConfig is NULL, the default SSL configuration will be used.
} M2MB_FOTA_HTTP_SESSION_CFG_T;

typedef struct
{
  CHAR URI[256];  /* hostName/[pathName/]fileName */
  CHAR user[M2MB_PARAM_LEN];
  CHAR psw[M2MB_PARAM_LEN];
  BOOLEAN passiveMode;
  UINT32 timeout;
  BOOLEAN IPPignoring;
  BOOLEAN FTPSEn;
  M2MB_FOTA_FTP_PROTOCOL_E protocol;
} M2MB_FOTA_FTP_SESSION_CFG_T;

typedef struct
{
  UINT8 connID;
  UINT8 cid;
  BOOLEAN cid_active;
  UINT16 pktSize;
  UINT16 inactivityTimeout;
  UINT16 connectionTimeout;
  UINT16 dataSendingTimeout;
} M2MB_FOTA_SOCKET_CFG_T;

typedef struct
{
  M2MB_FOTA_FTP_SESSION_CFG_T sessionCfg;
  M2MB_FOTA_SOCKET_CFG_T socketCfg;
} M2MB_FOTA_FTP_CFG_T;

// Callback resp_struct for M2MB_FOTA_EVENT_HTTP_DELTA_TRANSFER_END and M2MB_FOTA_EVENT_FTP_DELTA_TRANSFER_END events
typedef struct
{
  M2MB_FOTA_DOWNLOAD_RESULT_E result;
  UINT32 packageSize;
} M2MB_FOTA_CB_RESP_DELTA_TRANSFER_END_T;

// Callback for FOTA events
// Params: h -> handle
//         fota_event -> event type
//         resp_size -> size of resp_struct
//         resp_struct -> pointer to M2MB_FOTA_CB_RESP_DELTA_TRANSFER_END_T, in case of
//                        M2MB_FOTA_EVENT_HTTP_DELTA_TRANSFER_END and M2MB_FOTA_EVENT_FTP_DELTA_TRANSFER_END events
//         userdata -> generic user data returned as it is in the callback.
typedef void ( *m2mb_fota_ind_callback )( M2MB_FOTA_HANDLE h, M2MB_FOTA_IND_E fota_event, UINT16 resp_size, void *resp_struct, void *userdata );


/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    FOTA system open

  @details
    This function opens base FOTA system

  @param[in] *pHandle
    Pointer to the handle of the FOTA system, that will be initialized by the function.
  @param[in] callback
    Callback that will be called if an event or a response happens.
  @param[in] userdata
    Pointer to generic user data that will be returned as it is in the callback.

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @notes
    Normal FOTA process should be:
     m2mb_fota_init
     m2mb_fota_reset
     m2mb_fota_state_get
     if M2MB_FOTA_STATE_IDLE
      m2mb_fota_get_delta
      m2mb_fota_update_package_check_setup
      m2mb_fota_start
      reboot (m2mb_power_reboot)
      ...
      m2mb_fota_deinit

  @b
    Example
  @code
    void myCallback( M2MB_FOTA_HANDLE h, M2MB_FOTA_IND_E fota_event, UINT16 resp_size, void *resp_struct, void *userdata )
    {
      if( ( fota_event == M2MB_FOTA_EVENT_HTTP_DELTA_TRANSFER_END ) || ( fota_event == M2MB_FOTA_EVENT_FTP_DELTA_TRANSFER_END ) )
      {
        M2MB_FOTA_CB_RESP_DELTA_TRANSFER_END_T *resp = ( M2MB_FOTA_CB_RESP_DELTA_TRANSFER_END_T *)resp_struct;

        if(resp->result == M2MB_FOTA_DOWNLOAD_SUCCESS)
        {
          PRINT("FOTA download PASS - performing packet validation...");
          // Call dedicated task to check the delta
        }
        else
        {
          PRINT("FOTA download FAIL");
          // Application exit - error handling
        }
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_FOTA_HANDLE h = NULL;
      struct myStruct myUserdata;

      retVal = m2mb_fota_init( &h, myCallback, ( void * )myUserdata );
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_init
(
  M2MB_FOTA_HANDLE *pHandle,
  m2mb_fota_ind_callback callback,
  void *userdata
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    FOTA system close

  @details
    This function closes base FOTA system

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @b
    Example
  @code
    M2MB_RESULT_E Ret = m2mb_fota_deinit( h );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_deinit
(
  M2MB_FOTA_HANDLE handle
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get FOTA delta

  @details
    This function gets FOTA delta

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.
  @param[in] type
    Type of protocol used for download the FOTA delta. Defined in M2MB_FOTA_DOWNLOAD_TYPE_E.
  @param[in] settings
    Pointer to a configuration structure, relative to the protocol "type" used for the download:
    M2MB_FOTA_FTP_CFG_T in case of FTP protocol.
    M2MB_FOTA_HTTP_SESSION_CFG_T in case of HTTP protocol.

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @b
    Example
  @code
    M2MB_FOTA_FTP_CFG_T fotaDwFtp;
    fotaDwFtp.socketCfg.cid = ...

    M2MB_RESULT_E Ret = m2mb_fota_get_delta( h, M2MB_FOTA_DOWNLOAD_TYPE_FTP, ( void * )( &fotaDwFtp ) );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_get_delta
(
  M2MB_FOTA_HANDLE handle,
  M2MB_FOTA_DOWNLOAD_TYPE_E type,
  void *settings
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Base FOTA system initialization

  @details
    This function initializes base FOTA system

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes
  @note
    Not supported in LE910Cx Linux series
  @b
    Example
  @code
    M2MB_RESULT_E Ret = m2mb_fota_reset( h );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_reset
(
  M2MB_FOTA_HANDLE handle
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get last FOTA result code

  @details
    This function returns result code of the last FOTA function called

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.
  @param[out] pResultCode
    Specific FOTA result code is defined in M2MB_FOTA_RESULT_CODE_E

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @b
    Example
  @code
    M2MB_FOTA_RESULT_CODE_E ResultCode;

    M2MB_RESULT_E Ret = m2mb_fota_result_code_get( h, &ResultCode );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_result_code_get
(
  M2MB_FOTA_HANDLE handle,
  M2MB_FOTA_RESULT_CODE_E *pResultCode
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set FOTA configuration

  @details
    This function sets trace verbose level and, if UART is available for FOTA tracing, trace speed.
    With M2MB_FOTA_VERBOSE_LEVEL_NO_CHANGE and/or M2MB_FOTA_TRACE_SPEED_NO_CHANGE the specific parameter
    remains unchanged

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.
  @param[in] TraceVerboseLevel
    Trace verbose level defined in M2MB_FOTA_TRACE_VERBOSE_LEVEL_E

  @param[in] TraceSpeed
    Trace speed defined in M2MB_FOTA_TRACE_SPEED_E

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes
  @note
    Not supported in LE910Cx Linux series

  @b
    Example
  @code
    M2MB_RESULT_E Ret = m2mb_fota_config_set( h, M2MB_FOTA_VERBOSE_LEVEL_0, M2MB_FOTA_TRACE_SPEED_115200 );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_config_set
(
  M2MB_FOTA_HANDLE handle,
  M2MB_FOTA_TRACE_VERBOSE_LEVEL_E TraceVerboseLevel,
  M2MB_FOTA_TRACE_SPEED_E TraceSpeed
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get FOTA configuration

  @details
    This function gets trace verbose level and speed. Default values are M2MB_FOTA_VERBOSE_LEVEL_0
    for verbose level and M2MB_FOTA_TRACE_SPEED_115200 for speed

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.
  @param[out] pTraceVerboseLevel
    Returned trace verbose level defined in M2MB_FOTA_TRACE_VERBOSE_LEVEL_E
  @param[out] pTraceSpeed
    Returned trace speed defined in M2MB_FOTA_TRACE_SPEED_E

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes
  @note
    Not supported in LE910Cx Linux series

  @b
    Example
  @code
    M2MB_FOTA_TRACE_VERBOSE_LEVEL_E VerboseLevel;
    M2MB_FOTA_TRACE_SPEED_E TraceSpeed;

    M2MB_RESULT_E Ret = m2mb_fota_config_get( h, &VerboseLevel, &TraceSpeed );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_config_get
(
  M2MB_FOTA_HANDLE handle,
  M2MB_FOTA_TRACE_VERBOSE_LEVEL_E *pTraceVerboseLevel,
  M2MB_FOTA_TRACE_SPEED_E *pTraceSpeed
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get FOTA update package partition info

  @details
    This function gets the erasable block size and the overall partition size of the
    partition used to save update package

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.
  @param[out] pBlockSize
    Returned block size in bytes
  @param[out] pPartitionSize
    Returned overall partition size in bytes

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes
  @note
    Not supported in LE910Cx Linux series

  @b
    Example
  @code
    UINT32 BlockSize;
    UINT32 PartitionSize;

    M2MB_RESULT_E Ret = m2mb_fota_update_package_info_get( h, &BlockSize, &PartitionSize );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_update_package_info_get
(
  M2MB_FOTA_HANDLE handle,
  UINT32 *pBlockSize,
  UINT32 *pPartitionSize
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Write update package data to flash

  @details
    This function writes blocks of update package to flash. Writes are intended block by block.
    The block is the minimum erasable region of the flash.

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.
  @param[in] BlockAddress
    Address of the block to be written.
  @param[in] pBuffer
    Pointer to a buffer that contains data to be written. Size of the buffer comes from m2mb_fota_update_package_info_get
    pBlockSize parameter.

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes
  @note
    Not supported in LE910Cx Linux series

  @b
    Example
  @code
    UINT32 BlockSize;
    UINT32 PartitionSize;
    UINT8 *pBuffer;

    m2mb_fota_update_package_info_get( h, &BlockSize, &PartitionSize );
    pBuffer = ( UINT8 * )malloc( BlockSize );

    // put something in pBuffer

    M2MB_RESULT_E Ret = m2mb_fota_update_package_write( h, 0, pBuffer );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_update_package_write
(
  M2MB_FOTA_HANDLE handle,
  UINT32 BlockAddress,
  UINT8 *pBuffer
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Check update package integrity, applicability and prepare for FOTA update

  @details
    This function verifies update package integrity and applicability. If everything OK the
    functions prepares all the necessary stuff for a FW update.

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.
  @param[in] Action
    Refer to M2MB_FOTA_CHECK_SETUP_E.
    Use always M2MB_FOTA_CHECK_SETUP_SOURCE

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @b
    Example
  @code
    M2MB_RESULT_E Ret = m2mb_fota_update_package_check_setup( h, M2MB_FOTA_CHECK_SETUP_SOURCE );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_update_package_check_setup
(
  M2MB_FOTA_HANDLE handle,
  M2MB_FOTA_CHECK_SETUP_E Action
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Start FOTA process

  @details
    Prepares everything to start FOTA process. A reboot is necessary to start the process

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes

  @b
    Example
  @code
    M2MB_RESULT_E Ret = m2mb_fota_start( h );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_start
(
  M2MB_FOTA_HANDLE handle
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    End FOTA process

  @details
    After FOTA process in completed this API has to be called to change state from M2MB_FOTA_STATE_UDPATED
    to M2MB_FOTA_STATE_IDLE. This step is necessary to logically finish the FOTA process
    In recent SW versions there is no more need to use this API, after the reboot:
    the state will be changed automatically.

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes
  @note
    Not supported in LE910Cx Linux series

  @b
    Example
  @code
    M2MB_RESULT_E Ret = m2mb_fota_end( h );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_end
(
  M2MB_FOTA_HANDLE handle
);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get FOTA process state

  @details
    This function gets the FOTA process state

  @param[in] handle
    Handle of the FOTA system, previously initialized by the m2mb_fota_init function.
  @param[out] pState
    Refer to M2MB_FOTA_STATE_E

  @return
    M2MB_RESULT_SUCCESS - Ok
  @return
    See M2MB_RESULT_E for error codes
  @note
    Not supported in LE910Cx Linux series

  @b
    Example
  @code
    M2MB_FOTA_STATE_E State;

    M2MB_RESULT_E Ret = m2mb_fota_state_get( h, &State );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fota_state_get
(
  M2MB_FOTA_HANDLE handle,
  M2MB_FOTA_STATE_E *pState
);


#endif /* M2M_M2MB_FOTA_H */

