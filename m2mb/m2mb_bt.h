/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_bt.h

  @brief
    Header file for m2mb_bt APIs

  @details
    m2mb_bt APIs provide actions and events for BT classic core functionalities

  @note

  @author
    bhaskar.naik@telit.com

  @date
    27/08/2020
  @ Modified
    21/01/2021  BT SPP Feature support
*/
#ifndef  M2MB_BT_H
#define  M2MB_BT_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "m2mb_types.h"
#include "m2mb_os_types.h"
#include "m2mb_os_api.h"
#include "m2mb_os_sem.h"

#define M2MB_BT_MAX_DEVICE_NAME_LENGTH         (39+1) /**< GAP_DEVICE_NAME_LEN in gap.h */
#define M2MB_BT_MAX_MANUFACTURER_NAME_LENGTH   M2MB_BT_MAX_DEVICE_NAME_LENGTH
#define M2MB_BT_FIXPIN_SIZE                    6
#define M2MB_BT_ADV_DATA_SIZE                  32
#define M2MB_BT_SCAN_RSP_DATA_SIZE             32
#define M2MB_BT_ADDRESS_RESOLUTION_TIMEOUT     60

#define M2MB_BT_MAX_PHONE_NUMBER  82

typedef enum
{
    M2MB_BT_STATUS_SUCCESS_E,
    M2MB_BT_STATUS_FAILURE_E,
    M2MB_BT_STATUS_OUT_OF_RANGE_E,
    M2MB_BT_STATUS_NOT_READY_E,
    M2MB_BT_STATUS_UNSUPPORTED_E,
    M2MB_BT_STATUS_PARAM_INVALID_E,
    M2MB_BT_STATUS_NO_MEM_E,
    M2MB_BT_STATUS_AUTH_FAILURE_E,
    M2MB_BT_STATUS_AUTH_REJECTED_E,
    M2MB_BT_STATUS_INVALID_E,

    M2MB_BT_STATUS_ERR_INVALID_HANDLE_E,
    M2MB_BT_STATUS_ERR_REQUEST_NOT_SUPPORTED_E,
    M2MB_BT_STATUS_ERR_TIMEOUT_E,
    
} M2MB_BT_STATUS_T;

typedef enum
{
    M2MB_BT_STATE_UNINITIALIZED,
    M2MB_BT_STATE_INITIALIZED,
} M2MB_BT_INIT_STATE_T;

typedef enum
{
    M2MB_BT_ADVERTISE_ON_WITH_CUSTOMIZED_ADV_DISABLE,
    M2MB_BT_ADVERTISE_ON_WITH_CUSTOMIZED_ADV_ENABLE,
    M2MB_BT_ADVERTISE_OFF,
} M2MB_BT_ADV_TYPE_T;

typedef enum
{
    M2MB_BT_GAP_EVENT_SSP_REQUEST,
    M2MB_BT_GAP_API_SSP_REPLY,
} M2MB_BT_MSG_T;

typedef enum
{
    M2MB_BT_AG_HFP_CONNECT_EVENT_E, /* HF device connect */
    M2MB_BT_AG_HFP_DISCONNECT_EVENT_E,  /* HF device disconnect */
    M2MB_BT_AG_HFP_CALL_INCOMING_EVENT_E, /* Incoming call indication */
    M2MB_BT_AG_HFP_CALL_ANSWER_EVENT_E, /* Call accepted by HF */
    M2MB_BT_AG_HFP_CALL_REJECT_EVENT_E, /* call rejected by HF or remote number */
    M2MB_BT_AG_HFP_CALL_HOLD_EVENT_E, /* Call hold event from HF */  
    M2MB_BT_AG_HFP_CALL_OUTGOING_EVENT_E, /* Outgoing call initiated by HF */
    M2MB_BT_AG_HFP_CALL_DISCONNECT_EVENT_E, /* current call dis-connected either by HF or remote caller */
    M2MB_BT_AG_HFP_AUDIO_CONNECT_EVENT_E, /* Audio path established */
    M2MB_BT_AG_HFP_AUDIO_DISCONNECT_EVENT_E, /* Audio path disconnected */
    M2MB_BT_AG_HFP_VOLUME_CTRL_EVENT_E, /* Volume control */
    M2MB_BT_AG_HFP_CODEC_INFO_EVENT_E, /* Codec information */
    M2MB_BT_SCAN_EVENT_E, /* scan result event */
    M2MB_BT_DISCOVERY_START_EVENT_E, /* discovery start event */
    M2MB_BT_DISCOVERY_STOP_EVENT_E, /* discovery stop event */
    M2MB_BT_SSP_PIN_NOTIFICATION_EVENT_E,
    M2MB_BT_SSP_PIN_EVENT_E,
    M2MB_BT_SSP_CONF_EVENT_E,   /**< Indicates BT SSP conformation event.  */						
    M2MB_BT_SSP_REQUEST_EVENT_E,

    M2MB_BT_SPP_SERVER_STATE_CHANGE_EVENT_E = 256, /**< SPP state change indication */
    M2MB_BT_SPP_PEER_CONNECT_IND,     /**< Connect indication */
    M2MB_BT_SPP_PEER_DISCONNECT_IND,   /**< Disconnect indication */
    M2MB_BT_SPP_PEER_RX_DATA_EVENT_E, /**< Data reception indication */
    M2MB_BT_SPP_PEER_RX_END_EVENT_E,  /**< End of data reception */
    
}M2MB_BT_USER_EVENTS_T;

/* codec type - WBS or NBS */
typedef enum
{
   M2MB_BT_HFP_AG_CODEC_NONE,
   M2MB_BT_HFP_AG_CODEC_WBS,
   M2MB_BT_HFP_AG_CODEC_NBS
}M2MB_BT_HFP_AG_CODEC_TYPE_T;

typedef enum
{
  M2MB_BT_AG_HFP_DIAL_MSG_ID_DIAL_OUT_NUMBER,
  M2MB_BT_AG_HFP_DIAL_MSG_ID_REDIAL,
  M2MB_BT_AG_HFP_DIAL_MSG_ID_MEM_DIAL,
  
}M2MB_BT_AG_HFP_DIAL_MSG_ID_T;

typedef enum {
    M2MB_BT_SPP_STATE_SERVER_INACTIVE = 0,
    M2MB_BT_SPP_STATE_SERVER_ACTIVE=1,
}M2MB_BT_SPP_SERVER_STATE_T; 

typedef enum {
    M2MB_BT_SPP_STATE_DEVICE_CONNECTED=2,
    M2MB_BT_SPP_STATE_DEVICE_RECEIVE=3,
    M2MB_BT_SPP_STATE_DEVICE_DISCONNECTED=4
}M2MB_BT_SPP_DEVICE_STATE_T;

typedef struct  M2MB_BT_BD_ADDR_S
{
    UINT8 BD_ADDR0;
    UINT8 BD_ADDR1;
    UINT8 BD_ADDR2;
    UINT8 BD_ADDR3;
    UINT8 BD_ADDR4;
    UINT8 BD_ADDR5;
} M2MB_BT_BD_ADDR_T;

typedef struct M2MB_BT_SCAN_REPORT_DATA_S
{
    M2MB_BT_BD_ADDR_T               BD_ADDR; /**< Indicates BT address.  */
    INT8                            RSSI; /**< Indicates BT RSSI value.  */
    INT8                            device_name[M2MB_BT_MAX_DEVICE_NAME_LENGTH+1]; /**< Indicates BT device name.  */
    INT32                           bluetooth_class; /* device class */
} M2MB_BT_SCAN_REPORT_DATA_T;


/**
 * Event for notifying pin request
 */
typedef struct 
{
    M2MB_BT_BD_ADDR_T     bd_addr;
    INT8        bd_name[M2MB_BT_MAX_DEVICE_NAME_LENGTH+1]; /**< Indicates BT device name.  */
    uint32_t    pass_key;
    uint8_t     accept;
} M2MB_BT_SSP_REQUEST_T;

/*!
	@brief BT PIN request
	
	@details maintain PIN request details
*/
typedef struct {
    M2MB_BT_BD_ADDR_T     bd_addr; /**<remote bd address */
    INT8    bd_name[M2MB_BT_MAX_DEVICE_NAME_LENGTH+1]; /**< Indicates BT device name.  */
	INT8    pincode[7]; /**< to hold 6 digits SSPPIN */
	UINT8   pin_size; /**<Indicates pin size */
	UINT8   accept_flag; /**<Indicates accept flag */

} M2MB_BT_PIN_REQUEST_T;

typedef struct {
    M2MB_BT_BD_ADDR_T     bd_addr; /**<remote bd address */
    INT32 connection_id;
} M2MB_BT_HFP_AG_CON_EVENT_T;

typedef struct {
    M2MB_BT_BD_ADDR_T     bd_addr; /**<remote bd address */
    INT32 connection_id;
} M2MB_BT_HFP_AG_DISCON_EVENT_T;

typedef struct {
    M2MB_BT_BD_ADDR_T     bd_addr; /**<remote bd address */
    INT32 connection_id;
    M2MB_BT_HFP_AG_CODEC_TYPE_T codec; /**< NBS/WBS codec */
} M2MB_BT_HFP_AG_AUDIO_CON_EVENT_T;

typedef struct {
    M2MB_BT_BD_ADDR_T     bd_addr; /**<remote bd address */
    INT32 connection_id;
} M2MB_BT_HFP_AG_AUDIO_DISCON_EVENT_T;

typedef struct {
    M2MB_BT_BD_ADDR_T     bd_addr; /**<remote bd address */
    int con_id;
    M2MB_BT_AG_HFP_DIAL_MSG_ID_T msg_id;
    char dial_str[M2MB_BT_MAX_PHONE_NUMBER];
} M2MB_BT_HFP_AG_DIAL_EVENT_T;

typedef struct {
    char number[M2MB_BT_MAX_PHONE_NUMBER];
} M2MB_BT_HFP_AG_CALL_IND_EVENT_T;

typedef struct {
    M2MB_BT_BD_ADDR_T     bd_addr; /**<remote bd address */
    M2MB_BT_HFP_AG_CODEC_TYPE_T codec_type; 
}M2MB_BT_HFP_AG_CODEC_INFO_EVENT_T;

typedef struct M2MB_BT_SPP_STATE_CHANGE_EVENT_S
{
    M2MB_BT_SPP_SERVER_STATE_T state; /**< current state */
} M2MB_BT_SPP_STATE_CHANGE_EVENT_T;


typedef struct M2MB_BT_SPP_RX_DATA_EVENT_S
{
    M2MB_BT_BD_ADDR_T     bd_addr; /**<remote bd address */
    INT32 connection_id; /**< connection ID */
    int rx_len;     /**<RX data length */
    char *rx_buf;   /**<RX buffer. Note: Application must copy the data in cb itself */

} M2MB_BT_SPP_RX_DATA_EVENT_T;

typedef struct M2MB_BT_SPP_RX_END_EVENT_S
{
    M2MB_BT_BD_ADDR_T bd_addr; /**<remote bd address */
    INT32 connection_id; /**< connection ID */
    M2MB_BT_STATUS_T status;    
} M2MB_BT_SPP_RX_END_EVENT_T;

typedef struct M2MB_BT_SPP_CONNECT_EVENT_S
{
    M2MB_BT_BD_ADDR_T bd_addr; /**<remote bd address */
    INT32 connection_id; /**< connection ID */
    M2MB_BT_STATUS_T status; /**< connection status */
} M2MB_BT_SPP_CONNECT_EVENT_T;

typedef struct M2MB_BT_SPP_DISCONNECT_EVENT_S
{
    M2MB_BT_BD_ADDR_T bd_addr; /**<remote bd address */
    INT32 connection_id; /**< connection ID */
    M2MB_BT_STATUS_T status;    
} M2MB_BT_SPP_DISCONNECT_EVENT_T;


typedef struct M2MB_BT_USER_EVENT_CTX_S
{
    /*!
    @brief BT user event data

    @details contains BT user event data
    */
    union
    {
        M2MB_BT_SCAN_REPORT_DATA_T scan_data; /**<Indicates scan data */
        M2MB_BT_SSP_REQUEST_T ssp_req; /**< used with SSPCONF Request event */
        M2MB_BT_PIN_REQUEST_T ssp_pin; /**< used with SSPPIN event */
        
        /* HFP AG related events */
        M2MB_BT_HFP_AG_CON_EVENT_T hfp_ag_con_event;
        M2MB_BT_HFP_AG_DISCON_EVENT_T hfp_ag_discon_event;
        M2MB_BT_HFP_AG_AUDIO_CON_EVENT_T hfp_ag_audio_con_event;
        M2MB_BT_HFP_AG_AUDIO_DISCON_EVENT_T hfp_ag_audio_discon_event;
        M2MB_BT_HFP_AG_DIAL_EVENT_T hfp_ag_dial_event;
        M2MB_BT_HFP_AG_CALL_IND_EVENT_T hfp_ag_call_ind;
        M2MB_BT_HFP_AG_CODEC_INFO_EVENT_T hfp_ag_codec_info_event;

        /*SPP related events */
        M2MB_BT_SPP_RX_DATA_EVENT_T spp_rx_data_ind; /**< SPP receive data indication */
        M2MB_BT_SPP_STATE_CHANGE_EVENT_T spp_state_ind; /**< SPP server state change indication */
        M2MB_BT_SPP_RX_END_EVENT_T spp_rx_end_ind;  /**< SPP receive data end or termination event */
        M2MB_BT_SPP_CONNECT_EVENT_T spp_connect_ind; /**< SPP connect indication */
        M2MB_BT_SPP_DISCONNECT_EVENT_T spp_disconnect_ind; /**< SPP disconnect indication */
     }  event_data;
 } M2MB_BT_USER_EVENT_CTX_T;


typedef struct M2MB_BT_BOND_LIST_INFO_S
{
    M2MB_BT_BD_ADDR_T      remote_bd_address;
} M2MB_BT_BOND_LIST_INFO_T;


/* BT configuration parameters */
typedef enum
{   
    M2MB_BT_CFG_BD_ADDR_GET_E,			/**< Indicates BT device address get.  */
    M2MB_BT_CFG_DEVICE_NAME_GET_E,		/**< Indicates BT device name get.  */
    M2MB_BT_CFG_DEVICE_NAME_SET_E,		/**< Indicates BT device name set.  */
    M2MB_BT_CFG_MANUFACTURER_NAME_GET_E,/**< Indicates BT device manufacturer name get.  */
    M2MB_BT_CFG_MANUFACTURER_NAME_SET_E,/**< Indicates BT device manufacturer name set.  */

} M2MB_BT_CONFIG_PARAM_T;

typedef enum
{
    M2MB_BT_HFP_VOL_CTRL_TYPE_SPK,
    M2MB_BT_HFP_VOL_CTRL_TYPE_MIC
}M2MB_BT_HFP_VOL_CTRL_TYPE_T;

typedef HANDLE M2MB_BT_HANDLE;

/*!
	@brief BT event callback

	@details register this callback during BT init
*/
typedef void (*m2mb_bt_event_callback) (M2MB_BT_HANDLE bt_handle, M2MB_BT_USER_EVENTS_T bt_event, UINT16 resp_size, void *resp, void *userdata);
/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
      Initialization of BT

   @details
      Initializes BT Module and gives handle for application to be used in all contexts and starts event callback

   @param[in] bt_callback
     BT event callback function pointer. This function is called for all BT related events
     
   @param[in] user_data
     A pointer to generic BT user data that will be returned as it is in the callback

   @param[out] bt_handle
    Pointer to bt_handle. This is filled by m2mb_bt_initialize API. This should be used in all M2MB BT API's

   @return
    M2MB_BT_STATUS_T: used for success or failure see m2mb_bt.h for details
    
   @note
    M2MB_BT_HANDLE is valid until m2mb_bt_shutdown(M2MB_BT_HANDLE bt_handle) is called

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BT_STATUS_T m2mb_bt_initialize(M2MB_BT_HANDLE *bt_handle, m2mb_bt_event_callback bt_callback, void *user_data);
/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
      Getting BT initialization state

   @details
      Gets BT init state by passing handle

   @param[in] bt_handle
     specifies the BT handle previously created through m2mb_bt_initialize

   @param[out] state
     filled with BT state

   @return
    M2MB_BT_STATUS_T: used for success or failure see m2mb_bt.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BT_STATUS_T m2mb_bt_get_init_state(M2MB_BT_HANDLE bt_handle, M2MB_BT_INIT_STATE_T *state);
/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
      Deinitialization of BT

   @details
      Deinitialization of BT by passing bt handle

   @param[in] bt_handle
     specifies the BT handle previously created through m2mb_bt_initialize

   @return
    M2MB_BT_STATUS_T: used for success or failure see m2mb_bt.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T m2mb_bt_shutdown(M2MB_BT_HANDLE bt_handle);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     BT configuration API

   @details
     This API will accept variable length arguments as part of its last parameter.
	 bt_handle:bt_handle from BT init API
	 num_cmds: number of params follows after this param
	 __VA_ARGS__ (...):  This final parameter is variable argument list.
	 Make sure the first parameter in the variable arugument list is the Action on param (enum type to set or get bt param), followed by the "bt config param values".
	 "bt config param values" can be a pointer to structure which is defined based on m2mb_bt_conf enum type 
	 (or)
 	 bt config param values can be passed individually to the API.

   @param[in] bt_handle
     specifies the BT handle previously created through m2mb_bt_initialize

   @param[in] num_cmds
	 specifies number of params follows after this param
	 
   @return
     M2MB_BT_STATUS_T: used for success or failure see m2mb_bt.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BT_STATUS_T m2mb_bt_conf(M2MB_BT_HANDLE bt_handle, UINT8 num_cmds, ...);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Starts BT scanning

   @details
     Starts scanning for particular device (BT address) or scans available devices (Advertising devices) 

   @param[in] bt_handle
     specifies the BT handle previously created through m2mb_bt_initialize
     
   @param[in] bd_addr
     remote device address
    
   @return
    M2MB_BT_STATUS_T: used for success or failure see m2mb_bt.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T  m2mb_bt_scan_start(M2MB_BT_HANDLE bt_handle, M2MB_BT_BD_ADDR_T bd_addr);


/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Stops BT scanning

   @details
     Stop BT scanning

   @param[in] bt_handle
     specifies the BT handle previously created through m2mb_bt_initialize
    
   @return
     M2MB_BT_STATUS_T: used for success or failure see m2mb_bt.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T  m2mb_bt_scan_stop(M2MB_BT_HANDLE bt_handle);


/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Gets the bond information

   @details
     This function gives the bond list information
     
   @param[in] bt_handle
     specifies the BT handle previously created through m2mb_bt_initialize
     
   @param[out] bond_list_info
     Pointer to bond list info structure

   @param[out] num_of_entries
     number of bonded bt devices in the bond list
   
   @return
    M2MB_BT_STATUS_T: used for success or failure see m2mb_bt.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T m2mb_bt_bond_list_get(M2MB_BT_HANDLE bt_handle, M2MB_BT_BOND_LIST_INFO_T **bond_list_info, UINT32 *num_of_entries);


/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Deletes bond information

   @details
     This function deletes bond information for a praticular bd address or entire bond information

   @param[in] bt_handle
     specifies the BT handle previously created through m2mb_bt_initialize
     
   @param[in] bd_addr
     remote device address   
   
   @return
     M2MB_BT_STATUS_T: used for success or failure see m2mb_bt.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T m2mb_bt_bond_delete(M2MB_BT_HANDLE bt_handle, M2MB_BT_BD_ADDR_T bd_addr);


/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     passkey conformation

   @details
     This function conforms the passkey from user end

   @param[in] bt_handle
     specifies the BT handle previously created through m2mb_bt_initialize
     
   @param[in] bd_addr
     remote device address
          
   @param[in] confirmation_flag
     flag states whether to pair or not (true or false). 
     
   @return
     M2MB_BT_STATUS_T: used for success or failure see m2mb_bt.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T m2mb_bt_sspconf_reply(M2MB_BT_HANDLE bt_handle, M2MB_BT_BD_ADDR_T bd_addr, UINT32 accept_flag);


/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     passkey/pin response

   @details
     This function is called whenever a callback has been registered
     for the specified passkey response data event.

   @param[in] bt_handle:
     specifies the BT handle previously created through m2mb_bt_initialize
   
   @param[in] bd_addr
     remote device address
      
   @param[in] ssppin
     SSP passkey - 6 digits SSPPIN
   
   @param[in] accept_flag
     flag states whether to pair or not (true or false). 
     
   @return
     M2MB_BT_STATUS_T: used for success or failure see m2mb_bt.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T m2mb_bt_ssppin_reply( M2MB_BT_HANDLE bt_handle, M2MB_BT_BD_ADDR_T bd_addr, INT32 ssppin, UINT8 accept_flag);

#endif  /* M2MB_BT_H */
