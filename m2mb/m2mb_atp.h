/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_atp.h

  @brief
    Header file for m2mb_atp APIs

  @details
    m2mb_atp APIs provide actions and events for AT parser.

  @notes

  @author
    F. Sansa

  @date
    02/02/2018
*/

#ifndef  M2M_M2MB_ATP_API_H
#define  M2M_M2MB_ATP_API_H


/* Global declarations ==========================================================================*/

#define ATP_CMD_BUF_SIZE                  6144
#define M2MB_ATP_MAX_NUM_OF_PARAMETERS    32
#if !defined (M2M_INTERNAL_AT_PARSER_PLUS_ATI)
#define M2MB_ATP_FIRST_INSTANCE_ID 0
#else
#define M2MB_ATP_FIRST_INSTANCE_ID 1
#endif

#define M2MB_ATP_MAX_PARAM_STRING_LEN    16
#define M2MB_ATP_MAX_PARAM_VALUE_LEN     128
#define M2MB_ATP_MAX_PARAM_FILE_LEN      (1+2*M2MB_ATP_MAX_PARAM_VALUE_LEN)
#define M2MB_ATP_MAX_PARAM_FILE_PATH_LEN 128
#define M2MB_ATP_MAX_OPT_ERR_STR_SIZE    60


#define ATP_THREAD_STACK_SIZE      (1024 * 32)
#define M2MB_ATP_THREAD_STACK_SIZE (1024 * 24)
#define ATP_THREAD_N_MSG           100
#define ATP_THREAD_PRIORITY        231

#define ATP_EXTATP_THREAD_STACK_SIZE (1024 * 4)
#define ATP_EXTATP_THREAD_PRIORITY ATP_THREAD_PRIORITY

// Used to build Flags parameter when registering a command
#define M2MB_ATP_NORML   0x0000  // Command accepted only if sim and pin inserted
#define M2MB_ATP_NOPIN   0x0001  // Command accepted even if pin enabled and not inserted yet
#define M2MB_ATP_NOSIM   0x0002  // Command accepted if sim not inserted
#define M2MB_ATP_SMLCK   0x0004  // Command accepted even if simlock enabled
#define M2MB_ATP_NOPAR   0x0200  // Marker for special AT+Cmd=<CR>  handling
#define M2MB_ATP_CLACH   0x0080  // Hidden command, not listed in +CLAC

#define BS    0x08    /* back space */
#define LF    0x0a    /* Line Feed */
#define CR    0x0d    /* Carriage Return */
#define ESC   0x1b    /* Escape */
#define CTRL_Z  0x1a  /* Control Z (end of SMS PDU) */

/* Global typedefs ==============================================================================*/

typedef HANDLE M2MB_ATP_HANDLE;

typedef INT16 SY_MESSAGE_ID;
typedef char  CHAR;

typedef enum
{
  M2MB_ATP_CMDTYP_SET,      // AT cmd of =  type
  M2MB_ATP_CMDTYP_READ,     // AT cmd of ?  type
  M2MB_ATP_CMDTYP_TEST,     // AT cmd of =? type
  M2MB_ATP_CMDTYP_EXEC,     // execution type AT command
  M2MB_ATP_CMDTYP_NOPARAMS, // Special AT+CMD= for command with default parameter
  M2MB_NUM_ATP_CMDTYP
} M2MB_ATP_CMDTYPE_E;

typedef enum
{
  ATP_RESOURCE_CALL,
  ATP_RESOURCE_EMAIL,
  ATP_RESOURCE_FTP,
  ATP_RESOURCE_IPEASY,
  ATP_RESOURCE_IPDIAL,
  ATP_RESOURCE_GPS,
  ATP_RESOURCE_TONES,
  ATP_RESOURCE_FLASH,
  ATP_RESOURCE_PING,
  ATP_RESOURCE_HTTP,
  ATP_RESOURCE_NUM
} ENUM_RESOURCE_ID;

/*  atp_event */
typedef enum
{
  M2MB_ATP_DUMMY_IND
} M2MB_ATP_IND_E;

typedef enum
{
  M2MB_ATP_CMD_CALLBACK_IND,
  M2MB_ATP_CMD_DELEGATION_IND
} M2MB_ATP_CMD_IND_E;

typedef enum
{
  M2MB_ATP_INPUT_MODE_NONE = 0,
  M2MB_ATP_SMS_INPUT_MODE,
  M2MB_ATP_EMAIL_INPUT_MODE,
  M2MB_ATP_M2M_WRITE_INPUT_MODE,
  M2MB_ATP_WLAN_INPUT_MODE,  
  M2MB_ATP_BLE_INPUT_MODE,
  M2MB_ATP_ONLINE_INPUT_MODE,
  M2MB_ATP_SILENT_INPUT_MODE,
  M2MB_ATP_RUNNING_INPUT_MODE
} M2MB_ATP_INPUT_MODE;

typedef enum
{
  M2MB_ATP_FRC_SILENT = -1,
  M2MB_ATP_FRC_OK = 0,
  M2MB_ATP_FRC_ERROR,
  M2MB_ATP_FRC_CME_ERROR,
  M2MB_ATP_FRC_CMS_ERROR
} M2MB_ATP_FRC_E;

typedef enum
{
  M2MB_ATP_CME_PHONE_FAILURE = 0,
  M2MB_ATP_CME_NO_CONNECTION_TO_PHONE = 1,
  M2MB_ATP_CME_PHONE_LINK_RESERVED = 2,
  M2MB_ATP_CME_OPERATION_NOT_ALLOWED = 3,
  M2MB_ATP_CME_OPERATION_NOT_SUPPORTED = 4,
  M2MB_ATP_CME_PHSIM_PIN_REQUIRED = 5,
  M2MB_ATP_CME_PHFSIM_PIN_REQUIRED = 6,
  M2MB_ATP_CME_SIM_NOT_INSERTED = 10,
  M2MB_ATP_CME_SIM_PIN_REQUIRED = 11,
  M2MB_ATP_CME_SIM_PUK_REQUIRED = 12,
  M2MB_ATP_CME_SIM_FAILURE = 13,
  M2MB_ATP_CME_SIM_BUSY = 14,
  M2MB_ATP_CME_SIM_WRONG = 15,
  M2MB_ATP_CME_INCORRECT_PASSWORD = 16,
  M2MB_ATP_CME_SIM_PIN2_REQUIRED = 17,
  M2MB_ATP_CME_SIM_PUK2_REQUIRED = 18,
  M2MB_ATP_CME_MEMORY_FULL = 20,
  M2MB_ATP_CME_INVALID_INDEX = 21,
  M2MB_ATP_CME_NOT_FOUND = 22,
  M2MB_ATP_CME_MEMORY_FAILURE = 23,
  M2MB_ATP_CME_TEXT_TOO_LONG = 24,
  M2MB_ATP_CME_INVALID_CHARS_IN_TEXT = 25,
  M2MB_ATP_CME_DIAL_TOO_LONG = 26,
  M2MB_ATP_CME_INVALID_CHARS_IN_DIAL = 27,
  M2MB_ATP_CME_NO_NETWORK_SERVICE = 30,
  M2MB_ATP_CME_NETWORK_TIMEOUT = 31,
  M2MB_ATP_CME_NETWORK_NOT_ALLOWED = 32,
  M2MB_ATP_CME_NCK_REQUIRED = 40,
  M2MB_ATP_CME_NET_PERS_PUK_REQUIRED = 41,
  M2MB_ATP_CME_NSCK_REQUIRED = 42,
  M2MB_ATP_CME_NET_SUBSET_PERS_PUK_REQUIRED = 43,
  M2MB_ATP_CME_SPCK_REQUIRED = 44,
  M2MB_ATP_CME_SERV_PROV_PERS_PUK_EQUIRED = 45,
  M2MB_ATP_CME_CPCK_REQUIRED = 46,
  M2MB_ATP_CME_CORP_PERS_PUK_REQUIRED = 47,
  M2MB_ATP_CME_HIDDEN_KEY_REQUIRED = 48,
  M2MB_ATP_CME_UNKNOWN = 100,
  M2MB_ATP_CME_ILLEGAL_MS = 103,
  M2MB_ATP_CME_ILLEGAL_ME = 106,
  M2MB_ATP_CME_GPRS_SERVICE_NOT_ALLW = 107,
  M2MB_ATP_CME_PLMN_NOT_ALLW = 111,
  M2MB_ATP_CME_LOC_AREA_NOT_ALLW = 112,
  M2MB_ATP_CME_ROAM_NOT_ALW_IN_THIS_AREA = 113,
  M2MB_ATP_CME_SERV_OPT_NOT_SUPPORTED = 132,
  M2MB_ATP_CME_REQ_SERV_OPT_NOT_SUBSCRIBED = 133,
  M2MB_ATP_CME_SERV_OPT_TEMP_OOF = 134,
  M2MB_ATP_CME_UNSPECIFIED_GPRS_ERROR = 148,
  M2MB_ATP_CME_PDP_AUTHENT_FAILURE = 149,
  M2MB_ATP_CME_INVALID_MOBILE_CLASS = 150,

  // Manufacturer specific +CME ERROR errors(512..., see ETSI TS 100 585 V7.0.1 (1999-07))

  M2MB_ATP_CME_TEXT_PARAM_INSTEAD_OF_NUM_PARAM  = 35,
  M2MB_ATP_CME_NUM_PARAM_OUT_OF_BOUNDS          = 36,
  M2MB_ATP_CME_INVALID_PARAM_NUMBER             = 277,

  M2MB_ATP_CME_CONTEXT_NOT_OPENED           = 556,
  M2MB_ATP_CME_CANNOT_SETUP_SOCKET          = 557,
  M2MB_ATP_CME_CANNOT_RESOLVE_DN            = 558,
  M2MB_ATP_CME_TIMEOUT_IN_OPENING_SOCKET    = 559,
  M2MB_ATP_CME_CANNOT_OPEN_SOCKET           = 560,
  M2MB_ATP_CME_REMOTE_DISCONNECTED          = 561,
  M2MB_ATP_CME_CONNECTION_FAILED            = 562,
  M2MB_ATP_CME_TX_ERROR                     = 563,
  M2MB_ATP_CME_ALREADY_LISTENING            = 564,
  M2MB_ATP_CME_EASY_SKT_ABORT               = 565,
  M2MB_ATP_CME_EASY_NO_SOCKET               = 566,
  M2MB_ATP_CME_IP_VERSION_TYPE_INCOMPATIBLE = 567,
  M2MB_ATP_CME_IPV6_NOT_ENABLED             = 568,
  M2MB_ATP_CME_EASY_CONNECTION_ABORT        = 569,

  M2MB_ATP_CME_FTP_ALREADY_CONNECTED        = 608,
  M2MB_ATP_CME_CMDMODE                      = 613,
  M2MB_ATP_CME_CMDMODE_NODATAPORT           = 614,
  M2MB_ATP_CME_FTP_NOT_CONNECTED            = 615,
  M2MB_ATP_CME_FTP_DISCONNECTED             = 616,
  M2MB_ATP_CME_FTP_READ_CMD_CLOSED          = 617,
  M2MB_ATP_CME_FTP_READ_CMD_ERROR           = 618,
  M2MB_ATP_CME_FTP_WRITE_CMD_CLOSED         = 619,
  M2MB_ATP_CME_FTP_WRITE_CMD_ERROR          = 620,
  M2MB_ATP_CME_FTP_READ_DATA_CLOSED         = 621,
  M2MB_ATP_CME_FTP_READ_DATA_ERROR          = 622,
  M2MB_ATP_CME_FTP_WRITE_DATA_CLOSED        = 623,
  M2MB_ATP_CME_FTP_WRITE_DATA_ERROR         = 624,
  M2MB_ATP_CME_FTP_HOST_NOT_FOUND           = 625,
  M2MB_ATP_CME_FTP_ACCEPT_FAIL              = 626,
  M2MB_ATP_CME_FTP_LISTEN_FAIL              = 627,
  M2MB_ATP_CME_FTP_BIND_FAIL                = 628,
  M2MB_ATP_CME_FTP_FILE_CREATE_FAIL         = 629,
  M2MB_ATP_CME_FTP_FILE_GET_COMPLETE        = 630,
  M2MB_ATP_CME_FTP_FILE_PUT_COMPLETE        = 631,
  M2MB_ATP_CME_FTP_FILE_NOT_FOUND           = 632,
  M2MB_ATP_CME_FTP_TIME_OUT                 = 633,
  M2MB_ATP_CME_FTP_LOGIN_INCORRECT          = 634,
  M2MB_ATP_CME_FTP_CLOSE_ERROR              = 635,
  M2MB_ATP_CME_FTP_SERVER_NOT_READY         = 636,
  M2MB_ATP_CME_FTP_SERVER_SHUTDOWN          = 637,
  M2MB_ATP_CME_FTP_UNEXPECTED_REPLY         = 638,
  M2MB_ATP_CME_FTP_USERID_INVALID           = 639,
  M2MB_ATP_CME_FTP_PASSWD_INVALID           = 640,
  M2MB_ATP_CME_FTP_USER_ALREADY_LOGIN       = 641,
  M2MB_ATP_CME_FTP_OPEN_CHANNEL_TIMEOUT     = 642,
  M2MB_ATP_CME_FTP_COMM_TIMEOUT             = 643,
  M2MB_ATP_CME_FTP_UNKNOWN_ERROR            = 644,

  M2MB_ATP_CME_SSL_SSL_NOT_ACTIVATED        = 1000,
  M2MB_ATP_CME_SSL_SSL_CERT_KEYS_WRONG      = 1001,
  M2MB_ATP_CME_SSL_SSL_GENERIC_ERROR        = 1002,
  M2MB_ATP_CME_SSL_SSL_ALREADY_ACTIVATED    = 1003,
  M2MB_ATP_CME_SSL_SSL_ERR_DURING_HANDSHAKE = 1004,
  M2MB_ATP_CME_SSL_SSL_SOCKET_ERROR         = 1005,
  M2MB_ATP_CME_SSL_SSL_INVALID_STATE        = 1006,
  M2MB_ATP_CME_SSL_SSL_CAN_NOT_ACTIVATE     = 1007,
  M2MB_ATP_CME_SSL_SSL_NOT_CONNECTED        = 1008,
  M2MB_ATP_CME_SSL_SSL_ALREADY_CONNECTED    = 1009,
  M2MB_ATP_CME_SSL_SSL_ENC_DEC_DATA_ERROR   = 1010,
  M2MB_ATP_CME_SSL_SSL_DISCONNECTED         = 1011,

  M2MB_ATP_CME_MQTT_INIT_FAIL               = 1100,
  M2MB_ATP_CME_MQTT_DEINIT_FAIL             = 1101,
  M2MB_ATP_CME_MQTT_CONF_FAIL               = 1102,
  M2MB_ATP_CME_MQTT_CONNECT_FAIL            = 1103,
  M2MB_ATP_CME_MQTT_DISCONNECT_FAIL         = 1104,
  M2MB_ATP_CME_MQTT_PUBLISH_FAIL            = 1105,
  M2MB_ATP_CME_MQTT_SUBSCRIBE_FAIL          = 1106,
  M2MB_ATP_CME_MQTT_UNSUBSCRIBE_FAIL        = 1107,
  M2MB_ATP_CME_MQTT_SSL_CONFIG_FAIL         = 1108,

  M2MB_ATP_CME_GTP_TIMEOUT_ERROR            = 1150,
  M2MB_ATP_CME_GTP_PDP_ERROR                = 1151,
  M2MB_ATP_CME_GTP_DISABLED_ERROR           = 1152,
  M2MB_ATP_CME_GTP_ENA_CMD_NOT_SUPPORTED    = 1153,
  M2MB_ATP_CME_GTP_REBOOT_FAILED            = 1154,
} M2MB_ATP_CME_E;

typedef enum
{
  M2MB_ATP_CMS_UNASSIGNED_NUMBER = 1,
  M2MB_ATP_CMS_OPERATOR_BARRING = 8,
  M2MB_ATP_CMS_CALL_BARRED = 10,
  M2MB_ATP_CMS_SMS_TRANSFER_REJECTED = 21,
  M2MB_ATP_CMS_OUT_OF_SERVICE_DEST = 27,
  M2MB_ATP_CMS_UNIDENTIFIED_SUBSCRIBER = 28,
  M2MB_ATP_CMS_FACILITY_REJECTED = 29,
  M2MB_ATP_CMS_UNKNOWN_SUBSCRIBER = 30,
  M2MB_ATP_CMS_OUT_OF_ORDER_NETWORK = 38,
  M2MB_ATP_CMS_TEMP_FAILURE = 41,
  M2MB_ATP_CMS_CONGESTION = 42,
  M2MB_ATP_CMS_UNAVAILABLE_RESOURCE = 47,
  M2MB_ATP_CMS_FACILTY_NOT_SUBSCRIBED = 50,
  M2MB_ATP_CMS_FACILTY_NOT_IMPLEMENTED = 69,
  M2MB_ATP_CMS_INVALID_SMS_MSG_REF = 81,
  M2MB_ATP_CMS_INVALID_SMS = 95,
  M2MB_ATP_CMS_INVALID_MANDAT_INFO = 96,
  M2MB_ATP_CMS_MSG_TYPE_NOT_IMPLEMENTED = 97,
  M2MB_ATP_CMS_MSG_TYPE_NOT_COMPATIBLE = 98,
  M2MB_ATP_CMS_IE_NOT_IMPLEMENTED = 99,
  M2MB_ATP_CMS_PROTOCOL_ERROR = 111,
  M2MB_ATP_CMS_INTERWORKING = 127,
  M2MB_ATP_CMS_TEL_INTERWORK_NOT_SUPPORTED = 128,
  M2MB_ATP_CMS_SMTYPE0_NOT_SUPPORTED = 129,
  M2MB_ATP_CMS_CANNOT_REPLACE_SMS = 130,
  M2MB_ATP_CMS_UNSPECIFIED_PID_ERROR = 143,
  M2MB_ATP_CMS_DCS_NOT_SUPPORTED = 144,
  M2MB_ATP_CMS_UNSPECIFIED_DCS_ERROR = 159,
  M2MB_ATP_CMS_TPDU_NOT_SUPPORTED = 176,
  M2MB_ATP_CMS_SC_BUSY = 192,
  M2MB_ATP_CMS_NO_SC_SUBSCRIPTION = 193,
  M2MB_ATP_CMS_SC_SYSTEM_FAILURE = 194,
  M2MB_ATP_CMS_INVALID_SME_ADDRESS = 195,
  M2MB_ATP_CMS_DESTINATION_SME_BARRED = 196,
  M2MB_ATP_CMS_SM_REJ_DUPL_SM = 197,
  M2MB_ATP_CMS_VPF_NOT_SUPPORTED = 198,
  M2MB_ATP_CMS_VP_NOT_SUPPORTED = 199,
  M2MB_ATP_CMS_UNSPECIFIED_TPFCS_ERROR = 255,
  M2MB_ATP_CMS_ME_FAILURE = 300,
  M2MB_ATP_CMS_SMS_SERV_RESERVED = 301,
  M2MB_ATP_CMS_OPERATION_NOT_ALLOWED = 302,
  M2MB_ATP_CMS_OPERATION_NOT_SUPPORTED = 303,
  M2MB_ATP_CMS_INVALID_PDU_MODE_PARAMETER = 304,
  M2MB_ATP_CMS_INVALID_TEXT_MODE_PARAMETER = 305,
  M2MB_ATP_CMS_SIM_NOT_INSERTED = 310,
  M2MB_ATP_CMS_SIM_PIN_REQUIRED = 311,
  M2MB_ATP_CMS_PHSIM_PIN_REQUIRED = 312,
  M2MB_ATP_CMS_SIM_FAILURE = 313,
  M2MB_ATP_CMS_SIM_BUSY = 314,
  M2MB_ATP_CMS_SIM_WRONG = 315,
  M2MB_ATP_CMS_SIM_PUK_REQUIRED = 316,
  M2MB_ATP_CMS_SIM_PIN2_REQUIRED = 317,
  M2MB_ATP_CMS_SIM_PUK2_REQUIRED = 318,
  M2MB_ATP_CMS_MEMORY_FAILURE = 320,
  M2MB_ATP_CMS_INVALID_INDEX = 321,
  M2MB_ATP_CMS_MEMORY_FULL = 322,
  M2MB_ATP_CMS_SMSC_ADDRESS_UNKNOWN = 330,
  M2MB_ATP_CMS_NO_NETWORK_SERVICE = 331,
  M2MB_ATP_CMS_NETWORK_TIMEOUT = 332,
  M2MB_ATP_CMS_NO_CNMA_ACK_EXPECTED = 340,
  M2MB_ATP_CMS_UNKNOWN = 500,
  M2MB_ATP_CMS_FDN_NOT_ALLOWED_NUMBER = 512
} M2MB_ATP_CMS_E;

typedef enum
{
  M2MB_ATP_SLNT_NONE = -1,
  M2MB_ATP_SLNT_COMPLETED = 0,
  M2MB_ATP_SLNT_NO_CARRIER = 1,
  M2MB_ATP_SLNT_NO_CARRIER_MODE = 2 /* BUG 51036 */
} M2MB_ATP_SLNT_E;

typedef enum
{
  M2MB_ATP_UNS_BUFFER_IF_BUSY=0,
  M2MB_ATP_UNS_DISCARD_IF_BUSY,
} M2MB_ATP_UNS_MODE_E;

typedef struct
{
  M2MB_ATP_HANDLE atpHandle;
  M2MB_ATP_CMD_IND_E atpEvent;
  UINT16 resp_size;
  void *resp_struct;
  void *atptaskUserdata;
} M2MB_ATP_REQ_T;

/*  atpParam */
typedef struct
{
  CHAR *atpCmdString;         // Command string
  M2MB_ATP_CMDTYPE_E type;    // Command type (Read, Set, Test, TestExt)
  UINT16 rawDataLen;          // Length of raw command parameters
  CHAR *rawData;              // Pointer to raw command parameters
  UINT16 itemNum;             // Number of parameters
  CHAR **item;                // Pointers to parameters
} M2MB_ATP_PARAM_T;

/*  resp_struct in case of  M2MB_ATP_CALLBACK_IND event */
typedef struct
{
  UINT16 instanceNumber;
} M2MB_ATP_CALLBACK_IND_T;

/*  delegationEventMsg in case of  M2MB_ATP_DATA_IND delegationEvent */
typedef struct
{
  INT32 x;
} M2MB_ATP_DATA_IND_T;

/*  delegationEventMsg in case of  M2MB_ATP_ESCAPE_IND delegationEvent */
typedef struct
{
  INT32 y;
} M2MB_ATP_ESCAPE_IND_T;

/*  delegationEvent */
typedef enum
{
  M2MB_ATP_DATA_IND,
  M2MB_ATP_ESCAPE_IND,
  M2MB_ATP_BUFFER_EMPTY_IND,
  M2MB_ATP_CLOSE_CON_IND
} M2MB_ATP_DELEGATION_IND_E;

/*  resp_struct in case of  M2MB_ATP_DELEGATION_IND event */
typedef struct
{
  UINT16 instanceNumber;
  M2MB_ATP_DELEGATION_IND_E event;
  UINT16 msg_size;
  void *msg;
} M2MB_ATP_DELEGATION_IND_T;

typedef void ( *m2mb_atp_ind_callback )( M2MB_ATP_HANDLE h, M2MB_ATP_IND_E atp_event, UINT16 resp_size, void *resp_struct, void *userdata );
typedef void ( *m2mb_atp_cmd_ind_callback )( M2MB_ATP_HANDLE h, M2MB_ATP_CMD_IND_E atp_event, UINT16 resp_size, void *resp_struct, void *userdata );
typedef struct M2MB_ATP_CMD_HANDLE_T
{
  M2MB_ATP_HANDLE atpHandle;
  CHAR *atpCmdString;
  UINT16 atpFlags;
  m2mb_atp_cmd_ind_callback atpCallback;
  void *atpUserData;
  struct M2MB_ATP_CMD_HANDLE_T *pNext;
} M2MB_ATP_CMD_HANDLE_T;

typedef union
{
  INT32 get_integer;
  CHAR *get_string;
} M2MB_ATP_AT_COMMAND_VAL_U;

/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_init initializes AT Parser for current client.

  @details
    m2mb_atp_init initialize AT Parser for current client returning the handle that must be passed as first parameter
    for all AT Parser actions. Calling the m2mb_atp_init is mandatory before using the AT Parser service.

  @param[in] *h
    Handle to the AT Parser, that will be initialized by the function.
  @param[in] callback
    Callback that will be called if an event or a response happens.
  @param[in] userdata
    Pointer to generic user data that will be returned as it is in the callback.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_init(&h, myCallback, myUserdata);
  @code
    void myCallback( M2MB_ATP_HANDLE h, M2MB_ATP_IND_E sim_event, UINT16 resp_size, void *resp_struct, void *userdata )
    {
      //my code
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_ATP_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_atp_init(&h, myCallback, (void*)myUserdata);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_atp_init succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_init( M2MB_ATP_HANDLE *h, m2mb_atp_ind_callback callback, void *userdata );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_deinit deinitializes AT Parser for current client.

  @details
    m2mb_atp_deinit deinitialize AT Parser for current client freeing the handle passed
    as first parameter. Calling the m2mb_atp_deinit is useful after using the AT Parser in order to free memory space.

  @param[in] h
    Handle to the AT Parser, that will be deinitialized by the function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_deinit(&h);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_ATP_HANDLE h;
    struct myStruct myUserdata;
    retVal = m2mb_atp_init(&h, myCallback, (void*)myUserdata);

    //... use AT parser APIs ...

    retVal = m2mb_atp_deinit(h);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "m2mb_atp_deinit succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_deinit( M2MB_ATP_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_register registers an AT command in the AT parser.

  @details
    m2mb_atp_register registers an AT command in the AT parser for subsequent execution.

  @param[in] atpHandle
    Handle to the AT Parser, previously initialized by the m2mb_atp_init function.
  @param[in] atpCmdString
    String with the name of the command to be registered
  @param[in] atpFlags
    Set of or'ed flags to set conditions for the command to be executed (SIM present, PIN verified).
  @param[in] atpCallback
    Callback to be called when an AT command has been recognized.
  @param[in] atpUserdata
    Contains optional user data to be passed to AT Parser. It will be returned in the callback event.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_register(h, atpCmdString, atpFlags, atpCallback, atpUserdata);
  @code

    void CMGWCallback( M2MB_ATP_HANDLE h, UINT16 atpI )
    {
      ... my code ...
    }

    void CMGWDelegation( M2MB_ATP_HANDLE h, UINT16 atpI, M2MB_ATP_DELEGATION_IND_E Prim, void *pMsg )
    {
      ... my code ...
    }

    void my_atp_callback( M2MB_ATP_HANDLE atpHandle, M2MB_ATP_CMD_IND_E atpEvent, UINT16 resp_size, void *resp_struct, void *atptaskUserdata )
    {
      switch( atpEvent )
      {
        case M2MB_ATP_CMD_CALLBACK_IND:
        {
          M2MB_ATP_CALLBACK_IND_T *tmp_resp = ( M2MB_ATP_CALLBACK_IND_T * )resp_struct;
          CMGWCallback( atpHandle, tmp_resp->instanceNumber );
          break;
        }
        case M2MB_ATP_CMD_DELEGATION_IND:
        {
          M2MB_ATP_DELEGATION_IND_T *tmp_resp = ( M2MB_ATP_DELEGATION_IND_T * )resp_struct;
          CMGWDelegation( atpHandle, tmp_resp->instanceNumber, tmp_resp->event, tmp_resp->msg );
          break;
        }
        default:
          break;
      }
    }

    int mymain()
    {
      UINT16 atpFlags = 0;
      ...
      m2mb_atp_register( atpHandle, "+CMGW", atpFlags, my_atp_callback, NULL );
      ...
    }

  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_register( M2MB_ATP_HANDLE atpHandle, CHAR *atpCmdString, UINT16 atpFlags, m2mb_atp_cmd_ind_callback atpCallback, void *atpUserdata );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_get_cmd_userdata gets userdata linked with input AT command.

  @details
    m2mb_atp_get_cmd_userdata gets the userdata linked with the input AT command. It was previously stored by m2mb_atp_register.

  @param[in] atpHandle
    Handle to the AT Parser, previously initialized by the m2mb_atp_init function
  @param[in] atpCmdString
    String with the name of the AT command related to userdata
  @param[out] atpUserdata
    userdata previously stored by m2mb_atp_register

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_get_cmd_userdata(h, atpCmdString, atpUserdata);
  @code

    void CMGWCallback( M2MB_ATP_HANDLE h, UINT16 atpI )
    {
      ... my code ...
    }

    void CMGWDelegation( M2MB_ATP_HANDLE h, UINT16 atpI, M2MB_ATP_DELEGATION_IND_E Prim, void *pMsg )
    {
      ... my code ...
    }

    void my_atp_callback( M2MB_ATP_HANDLE atpHandle, M2MB_ATP_CMD_IND_E atpEvent, UINT16 resp_size, void *resp_struct, void *atptaskUserdata )
    {
      switch( atpEvent )
      {
        case M2MB_ATP_CMD_CALLBACK_IND:
        {
          M2MB_ATP_CALLBACK_IND_T *tmp_resp = ( M2MB_ATP_CALLBACK_IND_T * )resp_struct;
          CMGWCallback( atpHandle, tmp_resp->instanceNumber );
          break;
        }
        case M2MB_ATP_CMD_DELEGATION_IND:
        {
          M2MB_ATP_DELEGATION_IND_T *tmp_resp = ( M2MB_ATP_DELEGATION_IND_T * )resp_struct;
          CMGWDelegation( atpHandle, tmp_resp->instanceNumber, tmp_resp->event, tmp_resp->msg );
          break;
        }
        default:
          break;
      }
    }

    int mymain()
    {
      UINT16 atpFlags = 0;
      mystruct *my_userdata = m2mb_os_malloc( sizeof( mystruct ) );
      mystruct *retrived_userdata = NULL;
      ...
      m2mb_atp_register( atpHandle, "+CMGW", atpFlags, my_atp_callback, ( void *)my_userdata );
      ...
      m2mb_atp_get_cmd_userdata( atpHandle, "+CMGW", ( void ** )( &retrived_userdata ) );
    }

  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_get_cmd_userdata( M2MB_ATP_HANDLE atpHandle, CHAR *atpCmdString, void **atpUserdata );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_deregister deregisters an AT command in the AT parser.

  @details
    m2mb_atp_deregister deregisters an AT command previously registered in the AT parser with m2mb_atp_register.

  @param[in] atpHandle
    Handle to the AT Parser, previously initialized by the m2mb_atp_init function.
  @param[in] atpCmdString
    String with the name of the command to be deregistered

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_deregister(h, atpCmdString);
  @code

    int mymain()
    {
      UINT16 atpFlags = 0;
      ...
      m2mb_atp_deregister( atpHandle, "+CMGW" );
      ...
    }

  @endcode

*/

M2MB_RESULT_E m2mb_atp_deregister( M2MB_ATP_HANDLE atpHandle, CHAR *atpCmdString );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_msgout sends a string on terminal connected to AT parser.

  @details
    m2mb_atp_msgout sends a string on terminal connected to AT parser.

  @param[in] atpHandle
    Handle to the AT Parser callback.
  @param[in] instanceNum
    AT parser instance where run the action
  @param[in] msg
    String to be output

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_msgout(atpHandle, instanceNum, msg);
  @code
    void CMGFCallback( M2MB_ATP_HANDLE h, UINT16 atpI )
    {
      UINT8 temp = 0;
      CHAR tmp_str[50 + 1];
      M2MB_ATP_PARAM_T *atpParam;
      m2mb_atp_get_input_data( h, atpI, &atpParam );

      switch (atpParam->type)
      {
        case M2MB_ATP_CMDTYP_SET:
          temp = atoi( atpParam->item[0]);

          if ( ( temp == 0 ) || ( temp == 1 ) )
          {
            m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
          }
          else
            m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_CME_ERROR, (INT16)M2MB_ATP_CME_OPERATION_NOT_SUPPORTED, NULL);
          break;

        case M2MB_ATP_CMDTYP_READ:
          sprintf( tmp_str, "+CMGF: %d", temp);
          m2mb_atp_msgout(atpHandle, atpI, tmp_str );
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
          break;

        case M2MB_ATP_CMDTYP_TEST:
          m2mb_atp_msgout(atpHandle, atpI, "+CMGF: (0,1)" );
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
          break;

        case M2MB_ATP_CMDTYP_NOPARAMS:
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
          break;

        default:
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_CME_ERROR, (INT16)M2MB_ATP_CME_OPERATION_NOT_SUPPORTED, NULL);
          break;

      } // end switch
    }
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_msgout( M2MB_ATP_HANDLE atpHandle, UINT16 instanceNum, CHAR *msg );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_release ends the execution of an AT command in AT parser.

  @details
    m2mb_atp_release ends the execution of an AT command in AT parser and shows an OK/ERROR message.

  @param[in] atpHandle
    Handle to the AT Parser callback.
  @param[in] instanceNum
    AT parser instance where run the action
  @param[in] res
    Command result type (OK/ERROR/SILENT). See M2MB_ATP_FRC_E enum.
  @param[in] errorCode
    Error code number to be shown if +CMEE=1. See M2MB_ATP_CME_E and M2MB_ATP_CMS_E for
    predefined error codes. In alternative, a custom error code in INT16 format can be used.
    If optionalErrString parameter is NULL, a predefined error string (see ETSI GSM 07.05 and GSM 07.07) will be shown if +CMEE=2.
  @param[in] optionalErrString
    Optional string to be shown instead of the predefined one for the specified errorCode, if +CMEE=2.
    Max length is M2MB_ATP_MAX_OPT_ERR_STR_SIZE.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_release(atpHandle, instanceNum, res, errorCode, optionalErrString);
  @code
    void CMGFCallback( M2MB_ATP_HANDLE h, UINT16 atpI )
    {
      UINT8 temp = 0;
      CHAR tmp_str[50 + 1];
      M2MB_ATP_PARAM_T *atpParam;
      m2mb_atp_get_input_data( h, atpI, &atpParam );

      switch (atpParam->type)
      {
        case M2MB_ATP_CMDTYP_SET:
          temp = atoi( atpParam->item[0]);

          if ( ( temp == 0 ) || ( temp == 1 ) )
          {
            m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
          }
          else
            m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_CME_ERROR, (INT16)M2MB_ATP_CME_OPERATION_NOT_SUPPORTED, NULL);
          break;

        case M2MB_ATP_CMDTYP_READ:
          sprintf( tmp_str, "+CMGF: %d", temp);
          m2mb_atp_msgout(atpHandle, atpI, tmp_str );
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
          break;

        case M2MB_ATP_CMDTYP_TEST:
          m2mb_atp_msgout(atpHandle, atpI, "+CMGF: (0,1)" );
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
          break;

        case M2MB_ATP_CMDTYP_NOPARAMS:
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
          break;

        default:
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_CME_ERROR, (INT16)M2MB_ATP_CME_OPERATION_NOT_SUPPORTED, NULL);
          break;

      } // end switch
    }
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_release( M2MB_ATP_HANDLE h, UINT16 instanceNum, M2MB_ATP_FRC_E res, INT16 errorCode, CHAR *optionalErrString );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_change_input_mode changes AT parser input mode (command or text entering, e.g. SMS text).

  @details
    m2mb_atp_change_input_mode changes AT parser input mode (command or text entering, e.g. SMS text).

  @param[in] atpHandle
    Handle to the AT Parser callback.
  @param[in] instanceNum
    AT parser instance where run the action
  @param[in] mode
    Input mode

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_change_input_mode(atpHandle, instanceNum, mode);
  @code
    void CMGWCallback( M2MB_ATP_HANDLE h, UINT16 atpI )
    {
      UINT8 temp = 0;
      CHAR tmp_str[50 + 1];
      M2MB_ATP_PARAM_T *atpParam;
      m2mb_atp_get_input_data( h, atpI, &atpParam );

      m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "CMGWCallback %d %d", atpParam->type, atpParam->itemNum);

      switch (atpParam->type)
      {
        case M2MB_ATP_CMDTYP_SET:
          m2mb_atp_change_input_mode( atpHandle, atpI, M2MB_ATP_SMS_INPUT_MODE );
          break;

        case M2MB_ATP_CMDTYP_TEST:
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
          break;

        default:
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_ERROR, -1, NULL);
          break;

      } // end switch
    }

    void CMGWDelegation( M2MB_ATP_HANDLE h, UINT16 atpI, M2MB_ATP_DELEGATION_IND_E delegationEvent, void *delegationEventMsg )
    {
      M2MB_ATP_PARAM_T *atpParam;
      m2mb_atp_get_input_data( h, atpI, &atpParam );
      UINT8 Buffer[USB_BUF_LEN];
      INT32 ret = -1;
      BOOLEAN inputEnd = FALSE;

      m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "CMGWDelegation %d", delegationEvent );
      m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR,"command [%s]", atpParam->atpCmdString );

      switch (delegationEvent)
      {
        case M2MB_ATP_DATA_IND:
          ret = m2mb_atp_rx( atpHandle, atpI, Buffer, USB_BUF_LEN );
          if ( ret > 0 )
          {
            UINT16 i;

            for ( i = 0; i < ret; i++ )
            {
              m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "received %02x", Buffer[i]);
              if ( Buffer[i] == CTRL_Z )
              {
                inputEnd = TRUE;
                ret = ret - 1;
              }
            }
            m2mb_atp_tx( atpHandle, atpI,Buffer, ret ); //echo input

            if ( inputEnd )
            {
              m2mb_atp_change_input_mode(atpHandle, atpI, M2MB_ATP_INPUT_MODE_NONE );
              m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
            }
          }
          break;

        default:
          break;

      } // end switch
    }
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_change_input_mode( M2MB_ATP_HANDLE atpHandle,
                                          UINT16 instanceNum,
                                          M2MB_ATP_INPUT_MODE mode );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_get_input_mode gets current AT parser input mode.

  @details
    m2mb_atp_get_input_mode gets current AT parser input mode (command or text entering, e.g. SMS text).

  @param[in] atpHandle
    Handle to the AT Parser callback.
  @param[in] instanceNum
    AT parser instance where run the action
  @param[in] *mode
    Current input mode

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_get_input_mode(atpHandle, instanceNum, &mode);
  @code
    void CMGWCallback( M2MB_ATP_HANDLE h, UINT16 atpI )
    {
      UINT8 temp = 0;
      CHAR tmp_str[50 + 1];
      M2MB_ATP_PARAM_T *atpParam;
      m2mb_atp_get_input_data( h, atpI, &atpParam );

      m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "CMGWCallback %d %d", atpParam->type, atpParam->itemNum);

      switch (atpParam->type)
      {
        case M2MB_ATP_CMDTYP_SET:
          m2mb_atp_change_input_mode( atpHandle, atpI, M2MB_ATP_SMS_INPUT_MODE );
          break;

        case M2MB_ATP_CMDTYP_TEST:
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
          break;

        default:
          m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_ERROR, -1, NULL);
          break;

      } // end switch
    }

    void CMGWDelegation( M2MB_ATP_HANDLE h, UINT16 atpI, M2MB_ATP_DELEGATION_IND_E delegationEvent, void *delegationEventMsg )
    {
      M2MB_ATP_PARAM_T *atpParam;
      m2mb_atp_get_input_data( h, atpI, &atpParam );
      UINT8 Buffer[USB_BUF_LEN];
      INT32 ret = -1;
      BOOLEAN inputEnd = FALSE;
      M2MB_ATP_INPUT_MODE mode = M2MB_ATP_INPUT_MODE_NONE;

      m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "CMGWDelegation %d", delegationEvent );
      m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR,"command [%s]", atpParam->atpCmdString );

      m2mb_atp_get_input_mode( atpHandle, instanceNum, &mode );
      if( mode != M2MB_ATP_SMS_INPUT_MODE )
      {
        return;
      }

      switch (delegationEvent)
      {
        case M2MB_ATP_DATA_IND:
          ret = m2mb_atp_rx( atpHandle, atpI, Buffer, USB_BUF_LEN );
          if ( ret > 0 )
          {
            UINT16 i;

            for ( i = 0; i < ret; i++ )
            {
              m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "received %02x", Buffer[i]);
              if ( Buffer[i] == CTRL_Z )
              {
                inputEnd = TRUE;
                ret = ret - 1;
              }
            }
            m2mb_atp_tx( atpHandle, atpI,Buffer, ret ); //echo input

            if ( inputEnd )
            {
              m2mb_atp_change_input_mode(atpHandle, atpI, M2MB_ATP_INPUT_MODE_NONE );
              m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
            }
          }
          break;

        default:
          break;

      } // end switch
    }
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_get_input_mode( M2MB_ATP_HANDLE atpHandle,
                                       UINT16 instanceNum,
                                       M2MB_ATP_INPUT_MODE *mode );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_rx reads in a buffer characters just received on current AT parser instance port.

  @details
    m2mb_atp_rx reads in a buffer characters just received on current AT parser instance port.

  @param[in] atpHandle
    Handle to the AT Parser callback.
  @param[in] instanceNum
    AT parser instance where run the action
  @param[in] rxBuffer
    Buffer where read data have to be stored
  @param[in] rxBufferSize
    Size of the buffer where read data have to be stored

  @return
    when rxBuffer is not NULL and rxBufferSize is > 0, returns number of read bytes;
    when rxBuffer is NULL and rxBufferSize is -1, returns number of bytes available in the rx buffer;
    returns -1 on error.

  @note

  @b
    m2mb_atp_rx(atpHandle, instanceNum, rxBuffer, rxBufferSize);
  @code
    void CMGWDelegation( M2MB_ATP_HANDLE h, UINT16 atpI, M2MB_ATP_DELEGATION_IND_E delegationEvent, void *delegationEventMsg )
    {
      M2MB_ATP_PARAM_T *atpParam;
      m2mb_atp_get_input_data( h, atpI, &atpParam );
      UINT8 *Buffer;
      INT32 ret = -1;
      BOOLEAN inputEnd = FALSE;

      m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "CMGWDelegation %d", delegationEvent );
      m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR,"command [%s]", atpParam->atpCmdString );

      switch (delegationEvent)
      {
        case M2MB_ATP_DATA_IND:
          ret = m2mb_atp_rx( atpHandle, atpI, NULL, -1 );
          Buffer = m2mb_os_malloc( ret );
          ret = m2mb_atp_rx( atpHandle, atpI, Buffer, ret );
          if ( ret > 0 )
          {
            UINT16 i;

            for ( i = 0; i < ret; i++ )
            {
              m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "received %02x", Buffer[i]);
              if ( Buffer[i] == CTRL_Z )
              {
                inputEnd = TRUE;
                ret = ret - 1;
              }
            }
            m2mb_atp_tx( atpHandle, atpI,Buffer, ret ); //echo input
            m2mb_os_free( Buffer );
            if ( inputEnd )
            {
              m2mb_atp_change_input_mode(atpHandle, atpI, M2MB_ATP_INPUT_MODE_NONE );
              m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
            }
          }
          break;

        default:
          break;

      } // end switch
    }
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_atp_rx( M2MB_ATP_HANDLE atpHandle, UINT16 instanceNum, void *rxBuffer, INT32 rxBufferSize );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_tx transmits on AT parser instance port the characters in the input buffer.

  @details
    m2mb_atp_tx transmits on AT parser instance port the characters in the input buffer.

  @param[in] atpHandle
    Handle to the AT Parser callback.
  @param[in] instanceNum
    AT parser instance where run the action
  @param[in] txBuffer
    Buffer where characters to be transmitted have been stored
  @param[in] txBufferSize
    Size of the buffer where characters to be transmitted have been stored

  @return
    when txBuffer is not NULL and txBufferSize is > 0, returns number of transmitted bytes;
    when txBuffer is NULL and txBufferSize is -1, returns number of bytes available in the tx buffer;
    returns -1 on error.

  @note

  @b
    m2mb_atp_tx(atpHandle, instanceNum, txBuffer, txBufferSize);
  @code
    void CMGWDelegation( M2MB_ATP_HANDLE h, UINT16 atpI, M2MB_ATP_DELEGATION_IND_E delegationEvent, void *delegationEventMsg )
    {
      M2MB_ATP_PARAM_T *atpParam;
      m2mb_atp_get_input_data( h, atpI, &atpParam );
      UINT8 Buffer[USB_BUF_LEN];
      INT32 ret = -1, ret_tx = -1;
      BOOLEAN inputEnd = FALSE;

      m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "CMGWDelegation %d", delegationEvent );
      m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR,"command [%s]", atpParam->atpCmdString );

      switch (delegationEvent)
      {
        case M2MB_ATP_DATA_IND:
          ret = m2mb_atp_rx( atpHandle, atpI, Buffer, USB_BUF_LEN );
          if ( ret > 0 )
          {
            UINT16 i;

            for ( i = 0; i < ret; i++ )
            {
              m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "received %02x", Buffer[i]);
              if ( Buffer[i] == CTRL_Z )
              {
                inputEnd = TRUE;
                ret = ret - 1;
              }
            }
            ret_tx = m2mb_atp_tx( atpHandle, atpI, NULL, -1 );
            m2mb_atp_tx( atpHandle, atpI, Buffer, ( ret_tx < ret ) ? ret_tx : ret ); //echo input

            if ( inputEnd )
            {
              m2mb_atp_change_input_mode(atpHandle, atpI, M2MB_ATP_INPUT_MODE_NONE );
              m2mb_atp_release(atpHandle, atpI, M2MB_ATP_FRC_OK, -1, NULL);
            }
          }
          break;

        default:
          break;

      } // end switch
    }
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_atp_tx( M2MB_ATP_HANDLE atpHandle, UINT16 instanceNum, void *txBuffer, INT32 txBufferSize );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_get_input_data gets a structure allocated by AT parser containing input data.

  @details
    m2mb_atp_get_input_data gets the M2MB_ATP_PARAM_T structure allocated by AT parser and containing input data.

  @param[in] atpHandle
    Handle to the AT Parser callback.
  @param[in] instanceNum
    AT parser instance where run the action

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_get_input_data(h, instanceNum);
  @code
  void AT_command_callback( M2MB_ATP_HANDLE h, UINT16 atpI )
  {
    M2MB_ATP_PARAM_T *Param;
    m2mb_atp_get_input_data( h, atpI, &Param );
  }
  void AT_command_delegation( M2MB_ATP_HANDLE h, UINT16 atpI, M2MB_ATP_DELEGATION_IND_E Prim, void *pMsg )
  {
    M2MB_ATP_PARAM_T *Param;
    m2mb_atp_get_input_data( h, atpI, &Param );
  }
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_get_input_data( M2MB_ATP_HANDLE h, UINT16 instanceNum, M2MB_ATP_PARAM_T **input_data );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_unsolicited_instance sends an unsolicited indication to an AT parser instance.

  @details
    m2mb_atp_unsolicited_instance sends an unsolicited indication to an AT parser instance.

  @param[in] h
    Handle to the AT Parser callback.
  @param[in] instanceNum
    Instance number
  @param[in] unsMsg
    Unsolicited text to be sent
  @param[in] mode
    Unsolicited mode

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_unsolicited_instance(h, instanceNum, unsMsg, mode);
  @code
    m2mb_atp_unsolicited_instance(atpHndl, 0, "UNSOLICITED 0", M2MB_ATP_UNS_DISCARD_IF_BUSY );
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_unsolicited_instance( M2MB_ATP_HANDLE h, UINT16 instanceNum, CHAR *unsMsg, M2MB_ATP_UNS_MODE_E mode );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_unsolicited_broadcast broadcasts an unsolicited indication to all AT parser instances.

  @details
    m2mb_atp_unsolicited_broadcast broadcasts an unsolicited indication to all AT parser instances.

  @param[in] h
    Handle to the AT Parser callback.
  @param[in] unsMsg
    Unsolicited text to be sent
  @param[in] mode
    Unsolicited mode

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_unsolicited_broadcast(h, unsMsg, mode);
  @code
    m2mb_atp_unsolicited_broadcast(atpHndl, "BROADCAST", M2MB_ATP_UNS_DISCARD_IF_BUSY );
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_unsolicited_broadcast( M2MB_ATP_HANDLE h, CHAR *unsMsg, M2MB_ATP_UNS_MODE_E mode );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_at_command_conf sets/gets some AT commands configuration values.

  @details
    m2mb_atp_at_command_conf sets/gets AT commands values among the ones available (see following note).

  @param[in] h
    Handle to the ATP service, previously initialized by the m2mb_atp_init function.
  @param[in] instanceNum
    Instance number.
  @param[is]
    AT command string (for example "E0", "+CMEE?", etc... ).
  @param[in]
    Determines the number of parameters that follows
  @param[in]
    Parameters that follow depend on the action (see following note for further details)

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
    AT command string: "E0" or "E1"

    Description: set the echo format (see ATE command)

    Params(in): h(M2MB_ATP_HANDLE), instanceNum(UINT16), "E0" or "E1", 0

    - example: set echo_enabled on ATP instance 0

    m2mb_atp_at_command_conf(h, 0, (CHAR *)"E1", 0);


    AT command string: "E?"

    Description: get the current echo format

    Params(in): h(M2MB_ATP_HANDLE), instanceNum(UINT16), "E?", 1, value(M2MB_ATP_AT_COMMAND_VAL_U*: 0=echo_disabled or 1=echo_enabled)

    - example: get current echo format on ATP instance 1

    M2MB_ATP_AT_COMMAND_VAL_U echoCfg;
    m2mb_atp_at_command_conf(h, 1, (CHAR *)"E?", 1, &echoCfg);
    m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_LOG, "ECHO current value on instance 1 is %d", echoCfg.get_integer );


    AT command string: "+CMEE?"

    Description: get the current +CMEE setting

    Params(in): h(M2MB_ATP_HANDLE), instanceNum(UINT16), "+CMEE?", 1, value(M2MB_ATP_AT_COMMAND_VAL_U*: see +CMEE spec)

    - example: get current +CMEE setting on ATP instance 1

    M2MB_ATP_AT_COMMAND_VAL_U cmeeCfg;
    m2mb_atp_at_command_conf(h, 1, (CHAR *)"+CMEE?", 1, &cmeeCfg);
    m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_LOG, "+CMEE current value on instance 1 is %d", cmeeCfg.get_integer );

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_at_command_conf( M2MB_ATP_HANDLE h, UINT16 instanceNum, CHAR *atCmdString, UINT8 argc, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_forward_parser forwards an AT commands to internal AT parser.

  @details
    m2mb_atp_forward_parser forwards to the internal AT parser the AT command currently issued on instanceNum or,
    in alternative, the AT command in the third parameter.

  @param[in] h
    Handle to the AT Parser callback.
  @param[in] instanceNum
    Instance number.
  @param[is]
    Optional AT command string (for example "+CMEE=1", "+CMGF?", etc... ).

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_forward_parser(h,instanceNum, optionalCmdString);
  @code

    void CMGFCallback( M2MB_ATP_HANDLE h, UINT16 atpI )
    {
      INT8 temp;
      M2MB_ATP_PARAM_T *atpParam;
      m2mb_atp_get_input_data( h, atpI, &atpParam );

      printf( "CMGFCallback %d %d", atpParam->type, atpParam->itemNum );

      switch( atpParam->type )
      {
        case M2MB_ATP_CMDTYP_SET:
          temp = atoi( atpParam->item[0] );

          if( ( temp == 0 ) || ( temp == 1 ) )
          {
#ifdef FORWARD_AT_PARSER
            m2mb_atp_forward_parser( h, atpI, NULL );
            m2mb_atp_release( h, atpI, M2MB_ATP_FRC_SILENT, M2MB_ATP_SLNT_NONE, NULL );
#else
            store_cmgf_current_val = temp;
            m2mb_atp_release( h, atpI, M2MB_ATP_FRC_OK, -1, NULL );
#endif
          }
          else
          {
            m2mb_atp_release( h, atpI, M2MB_ATP_FRC_ERROR, -1, NULL );
          }
          break;

        case M2MB_ATP_CMDTYP_READ:
          sprintf( tmp_sms_str, "+CMGF: %d", store_cmgf_current_val );
          m2mb_atp_msgout( h, atpI,  tmp_sms_str );
          m2mb_atp_release( h, atpI, M2MB_ATP_FRC_OK, -1, NULL );
          break;

        case M2MB_ATP_CMDTYP_TEST:
          m2mb_atp_msgout( h, atpI, "+CMGF: (0,1)" );
          m2mb_atp_release( h, atpI, M2MB_ATP_FRC_OK, -1, NULL );
          break;

        case M2MB_ATP_CMDTYP_NOPARAMS:
          m2mb_atp_release( h, atpI, M2MB_ATP_FRC_OK, -1, NULL );
          break;

        default:
          m2mb_atp_release( h, atpI, M2MB_ATP_FRC_ERROR, -1, NULL );
          break;
      }
    }

  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_forward_parser( M2MB_ATP_HANDLE h, UINT16 instanceNum, CHAR *optionalCmdString );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_data_out_process passes data delegation function of the current AT command.

  @details
    m2mb_atp_data_out_process transmits to the AT Parser new data to be processed by the
    delegation function (see following note).

  @param[in] h
    Handle to the ATP service, previously initialized by the m2mb_atp_init function.
  @param[in] instanceNum
    Instance number.
  @param[in] data
    Pointer to the custom data to be processed by the delegation function
  @param[in] dataLen
    Custom data size

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_data_out_process(M2MB_ATP_HANDLE h, UINT16 instanceNum,
                                         void *data, UINT32 dataLen );
#endif  /* M2M_M2MB_ATP_API_H */

