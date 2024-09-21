/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_wlan.h

  @brief
    Header file for m2mb_wlan APIs

  @details
    m2mb_wlan APIs provide actions and events for WLAN management.

  @note

  @author
    Bhaskar Naik

  @date
    06/02/2020
*/

#ifndef  M2M_M2MB_WLAN_API_H
#define  M2M_M2MB_WLAN_API_H


/* Global declarations ==========================================================================*/

#define M2MB_WLAN_MAC_LEN					6			/**< Length of MAC address 			*/
#define M2MB_WLAN_MAC_STR_LEN               18
#define M2MB_WLAN_MAC_SEPERATOR  			':'

#define M2MB_WLAN_MAX_SSID_LEN				32			/**< Maximum length of SSID 		*/

#define M2MB_WLAN_MIN_PASSPHRASE_LEN		8			/**< Minimum length of passphrase	*/
#define M2MB_WLAN_MAX_PASSPHRASE_LEN        64			/**< Maximum length of passphrase 	*/

#define M2MB_WLAN_MIN_WEP_KEY_SIZE			10			/**< Minimum WEP key size			*/
#define M2MB_WLAN_MAX_WEP_KEY_SIZE			26			/**< Maximum WEP key size			*/

#define M2MB_WLAN_WPS_MAX_KEY_LEN 			64			/**< Maximum size of WPS key in 
															 bytes							*/
#define M2MB_WLAN_SCAN_TYPE_BLOCKING		1
#define M2MB_WLAN_SCAN_TYPE_NON_BLOCKING	0

#define M2MB_WLAN_BROADCAST_ENABLE			1			/**< Enable wlan Broadcast */
#define M2MB_WLAN_BROADCAST_DISABLE			0			/**< Disable wlan Broadcast */

#define M2MB_WLAN_MIN_EAP_IDENTITY_LEN		8			/**< Minimum length of EAP identity	*/
#define M2MB_WLAN_MAX_EAP_IDENTITY_LEN      64			/**< Maximum length of EAP identity 	*/

#define M2MB_WLAN_MIN_EAP_PASSWORD_LEN    	8			/**< Minimum length of EAP password */
#define M2MB_WLAN_MAX_EAP_PASSWORD_LEN      64			/**< Maximum length of EAP password */

#define M2MB_WLAN_MIN_EAP_PRIV_KEY_PASSWD_LEN   8			/**< Minimum length of EAP Key	password */
#define M2MB_WLAN_MAX_EAP_PRIV_KEY_PASSWD_LEN   64			/**< Maximum length of EAP Key password */

#define M2MB_MAX_FILE_READ_LEN				256

/* Global typedefs ==============================================================================*/

typedef HANDLE M2MB_WLAN_HANDLE;
typedef HANDLE M2MB_WLAN_IF_HANDLE;

typedef struct M2MB_WLAN_SSID_S
{
	INT8 ssid[M2MB_WLAN_MAX_SSID_LEN+1];
}M2MB_WLAN_SSID_T;

typedef struct M2MB_WLAN_MAC_S
{
	INT8 mac[M2MB_WLAN_MAC_LEN];
}M2MB_WLAN_MAC_T;

typedef enum 
{
	M2MB_WLAN_PROTOCOL_TYPE_B_ONLY,  	/* 0 – 'b-only' (2.4 GHz) */
	M2MB_WLAN_PROTOCOL_TYPE_B_G,		/* 1 – 'b/g' (2.4 GHz) */
	M2MB_WLAN_PROTOCOL_TYPE_B_G_N,		/* 2 – 'b/g/n' (2.4 GHz) */
	M2MB_WLAN_PROTOCOL_TYPE_A_N,		/* 3 – 'a/n' (5 GHz) */
	M2MB_WLAN_PROTOCOL_TYPE_A_N_AC,		/* 4 – 'a/n/ac' (5 GHz) */

}M2MB_WLAN_PROTOCOL_TYPE_E;


/*!
    @brief Different authentication modes supported by WLAN.

 

    @details Enumeration that identifies authentication modes supported by the WLAN 
    subsystem. The application sets the required authentication from one of 
    these modes using m2mb_wlan_set_param() with M2MB_WLAN_PARAM_AUTH_TYPE
    as the flag.

 

    @note This should be set before calling m2mb_wlan_connect or
    m2mb_wlan_create API
*/
typedef enum
{
    M2MB_WLAN_AUTH_TYPE_NONE                    = 0,    /**< Open mode authentication */
    M2MB_WLAN_AUTH_TYPE_WPA                     = 1,    /**< Wi-Fi protected Access v1*/
    M2MB_WLAN_AUTH_TYPE_WPA2                    = 2,    /**< Wi-Fi protected Access v2*/
    M2MB_WLAN_AUTH_TYPE_WPA_PSK                 = 3,    /**< Wi-Fi protected Access v1 PSK*/    
    M2MB_WLAN_AUTH_TYPE_WPA2_PSK                = 4,    /**< Wi-Fi protected Access v2 PSK*/
    M2MB_WLAN_AUTH_TYPE_WPA2_EAP                = 5,    /**< Wi-Fi protected Access v2 Enterprise*/
    M2MB_WLAN_AUTH_TYPE_WPA3                    = 6,    /**< Wi-Fi protected Access v3*/
    M2MB_WLAN_AUTH_TYPE_WEP                     = 8,    /**< WEP mode authentication  */
    M2MB_WLAN_AUTH_TYPE_INVALID
}M2MB_WLAN_AUTH_TYPE_E;

/*security types used in legacy AP configuration */
typedef enum
{
    M2MB_WLAN_AP_SEC_TYPE_DISABLE                = 0,    /**< Disable security     */        
    M2MB_WLAN_AP_SEC_TYPE_WPA1                   = 1,    /**< WPA1                 */
    M2MB_WLAN_AP_SEC_TYPE_WPA2                   = 2,    /**< WPA2                 */
    M2MB_WLAN_AP_SEC_TYPE_WPA1_WPA2              = 3,    /**< WPA1+WPA2            */
    M2MB_WLAN_AP_SEC_TYPE_WPA3                   = 4,    /**< WPA3                 */
    M2MB_WLAN_AP_SEC_TYPE_INVALID
}M2MB_WLAN_AP_SEC_TYPE_E;

typedef enum
{
	M2MB_WLAN_SECURITY_MODE_OPEN				= 0,	/**< Security mode for open */		
	M2MB_WLAN_SECURITY_MODE_WEP					= 1,	/**< Security mode for WEP 	*/		
	M2MB_WLAN_SECURITY_MODE_WPA_PSK				= 2,	/**< Security mode for WPA 	*/
	M2MB_WLAN_SECURITY_MODE_WPA_EAP				= 3,
	M2MB_WLAN_SECURITY_MODE_WPA_EAP_PSK			= 4,
	M2MB_WLAN_SECURITY_MODE_WPA3_SAE             = 5,    /**< Security mode for WPA  */
	M2MB_WLAN_SECURITY_MODE_INVALID
}M2MB_WLAN_SECURITY_MODE_E;
 

/*! 
    @brief Different encryption modes supported by WLAN.

 

    @details Enumeration that identifies encryption modes supported by the WLAN 
    subsystem. The application sets the required encryption from one of 
    these modes using m2mb_wlan_set_param() with M2MB_WLAN_PARAM_ENCRYPTION_TYPE
    as the flag.

 

    @note This should be set before calling m2mb_wlan_connect 
    or m2mb_wlan_create API
 */
typedef enum
{
    M2MB_WLAN_ENCRYPT_TYPE_NONE                   = 0,    /**< No encryption             */        
    M2MB_WLAN_ENCRYPT_TYPE_WEP                    = 1,    /**< WEP encryption            */
    M2MB_WLAN_ENCRYPT_TYPE_TKIP                   = 2,    /**< TKIP encryption           */
    M2MB_WLAN_ENCRYPT_TYPE_AES                    = 3,    /**< CCMP/AES encryption       */
    M2MB_WLAN_ENCRYPT_TYPE_TKIP_AES               = 4,    /**< TKIP-AES/CCMP encryption  */
    M2MB_WLAN_ENCRYPT_TYPE_INVALID
}M2MB_WLAN_ENCRYPT_TYPE_E;
/*!
  @brief WLAN APIs response enum
*/
typedef enum
{
  M2MB_WLAN_SUCCESS = 0,                /**< Response is success */
  M2MB_WLAN_ERROR_BAD_ARG = -1,         /**< Bad argument was passed by the user */
  M2MB_WLAN_ERROR_OUT_OF_BUFFER = -2,   /**< Out of buffer */
  M2MB_WLAN_ERROR_TIMEOUT = -3,         /**< Timeout occurred */
  M2MB_WLAN_ERROR_NETWORK = -4,         /**< Error in Network */
  M2MB_WLAN_ERROR_MEMORY = -5,          /**< Out of Memory */
  M2MB_WLAN_ERROR_CALLBACK = -6,        /**< Callback Error */
  M2MB_WLAN_ERROR_ASYNC_THREAD = -7,    /**< Error inAsync thread */
  M2MB_WLAN_ERROR_OP_NOT_ALLOWED = -8,  /**< Opertaion not allowed */
  M2MB_WLAN_ERROR_OP_FAILED = -9,       /**< Generic error */
  M2MB_WLAN_ERROR_NO_INTERNET = -10,     /**< No internet */

  M2MB_WLAN_ERROR_SUPPORN_AP_MODE = -11,         /**< Support only on AP mode */
  M2MB_WLAN_ERROR_SUPPORN_CLIENT_MODE = -12,     /**< Support only on STA mode */
  M2MB_WLAN_ERROR_DRIVER_FAIELD = -13,           /**< Could not load WLAN driver */
  M2MB_WLAN_ERROR_CONNECTION_FAILED = -14,       /**< connection failure */
  M2MB_WLAN_ERROR_OP_NOT_SUPPORTED = -99,        /**< feature not implemented */

}M2MB_WLAN_RESPONSE_E;

/*!
	@brief Disconnection reason.
	 
	@details Identifies the disconnection reason.
*/
typedef enum {
    M2MB_WLAN_NO_NETWORK_AVAIL              = 0x01, 	/**< No network available. 				  */
    M2MB_WLAN_LOST_LINK                     = 0x02, 	/**< Missed beacons. 					  */
    M2MB_WLAN_DISCONNECT_CMD                = 0x03, 	/**< User disconnect command. 			  */
    M2MB_WLAN_BSS_DISCONNECTED              = 0x04, 	/**< BSS disconnected. 					  */
    M2MB_WLAN_AUTH_FAILED                   = 0x05, 	/**< Authentication failed. 			  */
    M2MB_WLAN_ASSOC_FAILED                  = 0x06, 	/**< Association failed. 				  */
    M2MB_WLAN_NO_RESOURCES_AVAIL            = 0x07, 	/**< No resources available. 			  */
    M2MB_WLAN_CSERV_DISCONNECT              = 0x08, 	/**< Disconnection due to connection
    													   services. 							  */
    M2MB_WLAN_INVALID_PROFILE               = 0x0a, 	/**< RSNA failure. 						  */
    M2MB_WLAN_DOT11H_CHANNEL_SWITCH         = 0x0b, 	/**< 802.11h channel switch. 			  */
    M2MB_WLAN_PROFILE_MISMATCH              = 0x0c, 	/**< Profile mismatched. 				  */
    M2MB_WLAN_CONNECTION_EVICTED            = 0x0d, 	/**< Connection evicted. 				  */
    M2MB_WLAN_IBSS_MERGE                    = 0x0e, 	/**< Disconnection due to merging of IBSS.*/
    M2MB_WLAN_EXCESS_TX_RETRY               = 0x0f, 	/**< TX frames failed after excessive
    													   retries. 							  */
    M2MB_WLAN_SEC_HS_TO_RECV_M1             = 0x10, 	/**< Security 4-way handshake timed out
    													   waiting for M1. 						  */
    M2MB_WLAN_SEC_HS_TO_RECV_M3             = 0x11, 	/**< Security 4-way handshake timed out 
    													   waiting for M3. 						  */
    M2MB_WLAN_TKIP_COUNTERMEASURES          = 0x12, 	/**< TKIP counter-measures. 			  */
    M2MB_WLAN_SCAN_FAIL						= 0x13,		/**< scan fail*/
    M2MB_L3_FAIL							= 0x14,		/**< L3 FAIL*/
} M2MB_DISASSOC_REASON_T;

/*!
	@brief WLAN connect event id

	@details Enumeration that indicates connect event type
*/
typedef enum
{
  M2MB_WLAN_CON_STA,          /**< STA mode connect event     */
  M2MB_WLAN_CON_AP,           /**< AP mode connect event      */
  M2MB_WLAN_CON_PEER          /**< peer station 
                                   with a MAC address in mac_Addr 
                                   has connected to SoftAP       */
}M2MB_WLAN_CON_EVENT_ID_E;

/*!
	@brief Events for an application registered CB functions.

	@details WLAN driver invokes an application-registered callback function to indicate
	various asynchronous events to the application. This data structure 
	enumerates the list of various event IDs for which the WLAN driver invokes
	the application-registered callback function.
*/

typedef enum
{
  M2MB_WLAN_SCAN_IND_CB,                  /**< ID to indicate scan result */
  M2MB_WLAN_SCAN_COMPLETE_CB,             /**< ID to indicate scan complete */
  M2MB_WLAN_CONNECT_CB,                   /**< ID to indicate assoc complete [only for client connect in AP mode] */
  M2MB_WLAN_DISCONNECT_CB,                /**< ID to indicate disconnect    */
  M2MB_WLAN_PROFILE_PNO_CB,               /**< ID for PNO event             */
  M2MB_WLAN_EVENT_WPS_CB,                 /**< ID for WPS event             */
  M2MB_WLAN_EVENT_PROMISCOUS_CB,          /**< ID for PROMISC event         */
  M2MB_WLAN_AP_CLIENT_IND_CB,             /**< ID to indicate client info */
  M2MB_WLAN_AP_CLIENT_INFO_DONE_CB,       /**< ID to indicate end of client info result */
  M2MB_WLAN_PING_IND_CB,                  /**< ID to indicate ping result */
  M2MB_WLAN_PING_COMPLETE_CB,             /**< ID to indicate end of ping */
  M2MB_WLAN_CLIENT_STATE_IND_CB,          /**< ID to indicate client connect state change indication */
  M2MB_WLAN_UNSOLICIT_IND_CB,
} M2MB_WLAN_IND_E;


/*!
	@brief Scan method.

	@details A client can use two scanning methods: active and passive. 
	During an active scan, the client radio transmits a probe request and 
	listens for a probe response from an AP. With a passive scan, the 
	client radio listens on each channel for beacons sent periodically 
	by an AP. A passive scan generally takes more time, since the client 
	must listen and wait for a beacon versus actively probing to find an AP
*/

typedef enum
{
  M2MB_WLAN_SCAN_ACTIVE,             /**< Active scan */
  M2MB_WLAN_SCAN_PASSIVE,            /**< Passive scan */
} M2MB_WLAN_SCAN_TYPE_E;

/*!
	@brief Scan result format.

	@details Scan result can have one of the below format.
	Normal result is returned in standard message structure.
	raw result will be returned in char array buffer and 
	raw format is used only for backward compatibilty.
*/
typedef enum
{
  M2MB_WLAN_SCAN_NORMAL,             /**< Normal scan result */
  M2MB_WLAN_SCAN_IW_RAW,             /**< Extended result in raw format */
} M2MB_WLAN_SCAN_FORMAT_E;

/*!
	@brief WLAN module operating modes.

	@details Enumeration that provides a list of supported operating modes
*/
typedef enum
{
  M2MB_WLAN_OP_MODE_STA,    /* single mode with station interface */
  M2MB_WLAN_OP_MODE_AP,     /* single mode with AP interface */
  M2MB_WLAN_OP_MODE_AP_STA, /* concurrent mode with AP+STA interface */
  M2MB_WLAN_OP_MODE_AP_AP,  /* concurrent mode with AP+AP interface */
  M2MB_WLAN_OP_MODE_STA_STA /* Not supported */
}M2MB_WLAN_OP_MODE_E;

/*!
	@brief WLAN interface operating modes.

	@details Enumeration that provides a list of supported operating modes for each 
	virtual device.
*/
typedef enum
{
	M2MB_WLAN_MODE_STATION,
	/**< Infrastructure non-AP Station mode, supported in both Single Device 
		 mode and Concurrent mode. When operating in Concurrent mode, virtual 
		 device 1 must be used for the Station mode of operation. 
	*/
	M2MB_WLAN_MODE_ACCESS_POINT,
	/**< Infrastructure AP Station mode, supported in both Single Device 
		 mode and Concurrent mode. When operating in Concurrent mode, virtual 
		 device 0 must be used for the AP mode of operation.
	*/
	M2MB_WLAN_MODE_ADHOC,	
	/**< Independent BSS mode of operation, supported in Single Device mode 
       	 only. Virtual device 0 must be used for the IBSS mode of operation. 
    */    
	M2MB_WLAN_MODE_INVALID						/**< Invalid device mode. */
}M2MB_WLAN_MODE_E;

/*!
	@brief WLAN device id or interface id

	@details Enumeration that provides a list of supported device id or interface id
*/
typedef enum
{
  M2MB_WLAN_DEV_0,				/**< Device ID 0 */
  M2MB_WLAN_DEV_1,				/**< Device ID 1 */
  M2MB_WLAN_DEV_2,				/**< Device ID 2 */
  M2MB_WLAN_DEV_3,				/**< Device ID 3 */
  M2MB_WLAN_DEV_MAX				/**< Device ID max */
}M2MB_WLAN_DEV_ID_E;

typedef enum
{
  M2MB_WLAN_BIN_BDWLAN,
  M2MB_WLAN_BIN_OTP
}M2MB_WLAN_BIN_ID_E;

typedef enum {
  M2MB_WLAN_START_STATUS_OFF = 0,
  M2MB_WLAN_START_STATUS_ON,
  M2MB_WLAN_START_STATUS_AUTO,
} M2MB_WLAN_START_STATUS_E;
/*!
	@brief WLAN interface set or get flags.

	@details Enumeration of flags which are used to set or get the parameters to the
	driver and from the driver. Some flags may support both set and get
	functionalities.
*/
typedef enum
{
	/**< To GET or SET SSID, If the device is in AP mode, this SSID will acts
	   like SSID for the network. If the device is in STA mode, SSID
	   will be the SSID of the network to which STA wants to connect . 
	   M2MB_WLAN_MAX_SSID_LEN is the max length of SSID */
	M2MB_WLAN_SET_SSID,
	M2MB_WLAN_GET_SSID,
	/**< To GET or SET the channel number. If the device is in AP mode, this
	   channel number is used as network channel network and the network will
	   start in this channel. If the device is in STA mode, then this
	   channel number is used to scanning purpose in a particular channel.
	   channel values are set in numbers[1-14][36-165] not in frequencies */
	M2MB_WLAN_SET_CHANNEL,
	M2MB_WLAN_GET_CHANNEL,
	/**< To GET the MAC address. This works for both AP mode and STA
	   mode also. MAC address can be set before association in STA mode and 
	   before starting the network in AP mode. MAC address can also be got at any time,
	   until device is up. M2MB_WLAN_MAC_LEN is MAC address length, and array
	   must be declared with above mentioned size */
	M2MB_WLAN_SET_MAC_ADDRESS,
	M2MB_WLAN_GET_MAC_ADDRESS,
	/**< To GET or SET the operating mode. STA mode and soft AP mode are two modes.
	   device identifier 0 will supports both STA and AP modes. The device
	   identifier 1 will supports only STA mode, so when the user starts WLAN in STA
	   mode device ID 1 is assigned and when the user starts WLAN in AP mode device id
	   0 is assigned */
	M2MB_WLAN_SET_OPERATION_MODE,
	M2MB_WLAN_GET_OPERATION_MODE,
	/**< To SET the listen interval. Setting of listen interval only supports
	   in STA mode i.e operating mode should be STA. This value is used in
	   association request frame. Listen interval is multiple of beacon intervals.
	   For example value of 1 corresponds to 1 beacon interval */
	M2MB_WLAN_SET_LISTEN_INTERVAL,
	M2MB_WLAN_GET_LISTEN_INTERVAL,
	/**< To SET the DTIM interval. Setting of DTIM interval only supports in
	   softAP mode i.e operating mode should be set to Soft AP and DTIM interval
	   should set before the starting device in AP mode. DTIM interval is multiple of
	   beacon intervals. For example, if it is set as 2 every beacon after 2 beacons
	   is DTIM beacon, at which every station must wake and receive this DTIM beacon
	   to know buffered packets is there or not for its association id*/
	M2MB_WLAN_SET_DTIM_INTERVAL,
	M2MB_WLAN_GET_DTIM_INTERVAL,
	/**< To enable or disable the hidden SSID feature. Setting this flag 
	   is only supports in softAP operating mode. Firstly, set the operating mode
	   to AP mode and then enable or disable hidden SSID feature. This flag supports
	   only to SET */
	M2MB_WLAN_SET_AP_ENABLE_HIDDEN_MODE,
	M2MB_WLAN_GET_AP_ENABLE_HIDDEN_MODE,
	/**< To SET keep alive frame interval in sec. Setting this flag only
	   supports in STA mode. If this value set to '0', the keep alive timer
	   will be disabled */
	M2MB_WLAN_SET_STA_KEEP_ALIVE_IN_SEC,
	M2MB_WLAN_GET_STA_KEEP_ALIVE_IN_SEC,
	/**< To SET the beacon interval in TU(time units). This flag is set and
	   applicable only if device is in softAP operating mode. 
	   One TU = 1024 microseconds. Number of TUs between every beacon */
	M2MB_WLAN_SET_BEACON_INTERVAL_TU,
	M2MB_WLAN_GET_BEACON_INTERVAL_TU,
	/**< To SET the beacon missing parameters. M2MB_STA_BMISS_CONFIG_T is used
	   as the parameter to set to the driver. On receiving this request from user
	   in STA mode, WLAN firmware reports a link loss after a specified interval.
	   Either of the values will be set by user, bmiss_time_in_ms or num_beacons.
	   The other value should be set to 0. Based on the values the WLAN firmware
	   detects link loss and disconnected from the network */
	M2MB_WLAN_SET_BMISS_CONFIG,
	M2MB_WLAN_GET_BMISS_CONFIG,
	/**< To GET or SET the RSSI value. For setting, this RSSI value is used for 
	   setting the threshold. If the user wants to GET the RSSI value, it returns
	   RSSI value in dB. *TO DO* Need some structure to set the threshold values */
	M2MB_WLAN_SET_RSSI,
	M2MB_WLAN_GET_RSSI,
	/**< To SET or GET the transmit power level in dBm. To set the transmit
	   power level, the range is between 0-63. By default, the value is 63 (max) */
	M2MB_WLAN_SET_TX_POWER,
	M2MB_WLAN_GET_TX_POWER,
	/**< To GET the regulatory domain which driver is currently using. This 
	   flag is used to only get the regulatory domain */
	M2MB_WLAN_SET_REG_DOMAIN,
	M2MB_WLAN_GET_REG_DOMAIN,
	/**< To GET or SET the WLAN rate of packets transmitted over radio. This 
	   flag is used to GET the transmission rate of received packets over radio,
	   or to SET the transmission rate of the packets which is going to 
	   transmit over the radio. TO DO */
	M2MB_WLAN_SET_TX_RATE,
	M2MB_WLAN_GET_TX_RATE,
	/**< Need some clarification about setting values and setting power save*/
	M2MB_WLAN_SET_POWER_SAVE_MODE,
	M2MB_WLAN_GET_POWER_SAVE_MODE,
	M2MB_WLAN_SET_POWER_SAVE_POLICY,
	M2MB_WLAN_GET_POWER_SAVE_POLICY,
	/**< To GET or SET the wireless PHY mode(a|b|g|an|ag|gn|agn). The SET operation
	   should be done before establishing the connection . The GET 
	   operation can be done before or after the connection */
	M2MB_WLAN_SET_PHY_MODE,
	M2MB_WLAN_GET_PHY_MODE,
	/**< To GET or SET the authentication type. Authentication should be set before
	   connecting to the peer. 0 for none , 3 for wpa_psk, 4 for wpa2_psk */
	M2MB_WLAN_SET_AUTH_TYPE,
	M2MB_WLAN_GET_AUTH_TYPE,
	M2MB_WLAN_SET_AUTH_TYPE_NUM,
	M2MB_WLAN_GET_AUTH_TYPE_NUM,
	M2MB_WLAN_SET_ENCRYPTION_TYPE,
	M2MB_WLAN_GET_ENCRYPTION_TYPE,
	M2MB_WLAN_SET_ENCRYPTION_TYPE_NUM,
	M2MB_WLAN_GET_ENCRYPTION_TYPE_NUM,
	M2MB_WLAN_SET_SECURITY_MODE,
	M2MB_WLAN_GET_SECURITY_MODE,
	M2MB_WLAN_SET_BSSID,
	M2MB_WLAN_GET_BSSID,
	/**< To SET the passphrase for upcoming WPA/WPA2 association procedure. This
	   should be set before initiating the association. The length should be 
	   between 8 and 64 */
	M2MB_WLAN_SET_WPA_PSK,
	M2MB_WLAN_GET_WPA_PSK,
	M2MB_WLAN_SET_SECURITY_PASSPHRASE,
	M2MB_WLAN_GET_SECURITY_PASSPHRASE,
	/**< To SET or GET the WEP key index. SET should be done before initiating an 
	   association. The value should be set between 1 to 4. If user has given key
	   index as 1, driver uses key at index 1 for upcoming association. If key is
	   not present , then it will return error */
	M2MB_WLAN_SET_WEP_KEY_INDEX,
	M2MB_WLAN_GET_WEP_KEY_INDEX,
	/**< To SET or GET the {keyindex and key} pair for upcoming association. SET
	   should be done before initiating the association. This is like setting 
	   a key for the given key index. This flag is used for getting the key at
	   particular key index */
	M2MB_WLAN_SET_WEP_KEY_PAIR,
	M2MB_WLAN_GET_WEP_KEY_PAIR,
	/**< To SET the WPS credentials */
	M2MB_WLAN_SET_WPS_CREDENTIALS,
	M2MB_WLAN_GET_WPS_CREDENTIALS,
	M2MB_WLAN_SET_STATS,
    M2MB_WLAN_GET_STATS,

	M2MB_WLAN_SET_PNO_INFO,
    M2MB_WLAN_GET_PNO_INFO,

	M2MB_WLAN_SET_PROFILE_INFO,
    M2MB_WLAN_GET_PROFILE_INFO,

	M2MB_WLAN_SET_COUNTRY_CODE, 
    M2MB_WLAN_GET_COUNTRY_CODE, 

	M2MB_WLAN_SET_11N_HT,
    M2MB_WLAN_GET_11N_HT,

	M2MB_WLAN_SET_AP_WPS_SUPPORT,
    M2MB_WLAN_GET_AP_WPS_SUPPORT,

	M2MB_WLAN_SET_PROMISC_FILTER_INFO,
    M2MB_WLAN_GET_PROMISC_FILTER_INFO,

	M2MB_WLAN_SET_PROMISC_CMD,
	M2MB_WLAN_GET_PROMISC_CMD,
	
	M2MB_WLAN_SET_EAP_8021X,
	M2MB_WLAN_GET_EAP_8021X,
	
	M2MB_WLAN_SET_INACTIVITY_TIME_IN_MIN,
    M2MB_WLAN_GET_INACTIVITY_TIME_IN_MIN,

	M2MB_WLAN_SET_SCAN_TIME,
	M2MB_WLAN_GET_SCAN_TIME,

    /**< Enable or disable specific async event from driver 
    */    
    M2MB_WLAN_SET_IND, 
    M2MB_WLAN_GET_IND,
    
    /**< Enable/disable MAC address filtering. Only for AP mode. 
    */    
    M2MB_WLAN_SET_MAC_FILTER, 
    M2MB_WLAN_GET_MAC_FILTER,
    
    /**< add or remove MAC address to or from accept list 
    */    
    M2MB_WLAN_SET_MAC_ACCEPT,
    M2MB_WLAN_GET_MAC_ACCEPT,
    
    /**< add or remove MAC address to or from deny list 
    */    
    M2MB_WLAN_SET_MAC_DENY,
    M2MB_WLAN_GET_MAC_DENY,

     /**< WLAN PDP context ID 
    */    
    M2MB_WLAN_SET_PDP_CID,
    M2MB_WLAN_GET_PDP_CID,

    M2MB_WLAN_SET_EAP_METHOD,
    M2MB_WLAN_GET_EAP_METHOD,
    M2MB_WLAN_SET_EAP_IDENTITY,
    M2MB_WLAN_GET_EAP_IDENTITY,
    M2MB_WLAN_SET_EAP_PASSWORD,
    M2MB_WLAN_GET_EAP_PASSWORD,
    M2MB_WLAN_SET_EAP_PRIV_KEY_PASSWD,
    M2MB_WLAN_GET_EAP_PRIV_KEY_PASSWD,
    
	M2MB_WLAN_ACTION_MAX
}M2MB_WLAN_ACTION_E;

typedef enum
{
  M2MB_WLAN_ICMP_PING_DISABLE,
  M2MB_WLAN_ICMP_PING_ENABLE
}M2MB_WLAN_ICMP_PING_CFG_E;

typedef enum
{ 
    M2MB_WLAN_EAP_METHOD_PEAPV0_MSCHAPV2 = 0,
    M2MB_WLAN_EAP_METHOD_PEAPV1_GTC,
    M2MB_WLAN_EAP_METHOD_TTLS_MSCHAPV2,
    M2MB_WLAN_EAP_METHOD_TTLS_GTC,
    M2MB_WLAN_EAP_METHOD_TLS,
}M2MB_WLAN_EAP_METHOD_E;

typedef enum 
{
    M2MB_WLAN_EAP_CERT_TYPE_CA_CERT = 0,
    M2MB_WLAN_EAP_CERT_TYPE_CLI_CERT,
    M2MB_WLAN_EAP_CERT_TYPE_KEY,
}M2MB_WLAN_EAP_CERT_TYPE_E;

typedef struct M2MB_WLAN_CONFIG_LEGACY_S
{
  int wlan_op_mode;
  int wlan_auto_mode;
  int wlan_cid;
  int wlan_ip_family;
}M2MB_WLAN_CONFIG_LEGACY_T;

/*! 
@brief Scan request 

@details Data structure that is used to start scan operation from application to driver. 
The driver uses this data structure to start scan operation.
*/
typedef struct M2MB_WLAN_SCAN_MSG_S
{
    M2MB_WLAN_SCAN_TYPE_E scan_type; /* Scanning type ; [active - 0] or [passive - 1] */
    UINT8 *ssid; /* SSID of the particular Access Point to scan [optional]*/
    UINT8 no_of_channels; /* Number of channels to scan [optional]. 0 for all channel scan */
    UINT8 *channel_list;  /* List of channels that user wants to scan [optional] */
    UINT16 time_out; /* Max duration for which scan operation shall be performed. 
                       Time out in milli seconds. 0 for no timeout.
                     */
    UINT8 blocking; /* 0 - non blocking 1 - Blocking */
    M2MB_WLAN_SCAN_FORMAT_E result_format;
    UINT16 max_size;    /* For normal scan result : No of buffers provided by user application to store the scan result  */
                        /* For raw scan result : size of char array provided by user application to store the scan result  */
    
    UINT16 *result_size; /* For normal scan result : Buffer which is provided by the user to store scan result count */
                         /* For raw scan result : Buffer which is provided by the user to store scan result size */
                        
    void *result_buff; /* Buffer which is provided by the user to store scan results.
                          For normal scan result - Array of M2MB_WLAN_BSS_SCAN_INFO_T structures
                          For iw raw scan result - array of char buffer 
                       */
}M2MB_WLAN_SCAN_MSG_T;

/*!
	@brief Structure of members for scan results.

	@details Structure for storing scan results. User application should send the array of
	this structure as a parameter to get filled when scan completed by the driver. This structure can
	be passed as one of the parameters for the m2mb_wlan_scan function.
*/
typedef struct M2MB_WLAN_BSS_SCAN_INFO_S
{
    UINT8  channel;                        				/**< Wireless channel 				*/
    UINT8  ssid_Length;                    				/**< SSID length 					*/
    UINT8  rssi;                           				/**< Received signal strength 
    													     indicator 						*/
    UINT8  security_Enabled;               				/**< 1:Security enabled.
                                              			     0:Security disabled.			*/
    UINT16 beacon_Period;                  				/**< Beacon period 					*/
    UINT8  preamble;                       				/**< Preamble 						*/
    UINT8  bss_type;                       				/**< BSS type [ADHOC, INFRA]		*/
    UINT8  bssid[M2MB_WLAN_MAC_LEN];        			/**< BSSID of network				*/
    UINT8  ssid[M2MB_WLAN_MAX_SSID_LEN];    			/**< SSID of Access point			*/
    UINT8  rsn_Cipher;                     				/**< RSN Cipher 					*/
    UINT8  rsn_Auth;                       				/**< RSN authentication 			*/
    UINT8  wpa_Cipher;                     				/**< WPA cipher 					*/
    UINT8  wpa_Auth;                       				/**< WPS authentication 			*/
    UINT16 caps;                           				/**< Capability IE 					*/
    UINT8  wep_Support;                    				/**< Support for WEP 				*/
    UINT8  reserved;                       				/**< Reserved 						*/
	UINT16 frequency;									/**< Frequency						*/
} M2MB_WLAN_BSS_SCAN_INFO_T;

/*!
@brief Scan indication 

@details Data structure that is used to send scan result from driver to application
*/
typedef struct M2MB_WLAN_SCAN_IND_S
{
    UINT16 no_of_entries; /* scan result count */
    M2MB_WLAN_BSS_SCAN_INFO_T *result_buff; /* Buffer which contains scan results. 
                                               Array of M2MB_WLAN_BSS_SCAN_INFO_T structures 
                                            */
}M2MB_WLAN_SCAN_IND_T;

/*! 
@brief Connection event info for both AP and STA modes.

@details Data structure that presents connect event information 
from the driver to the application. The application uses this data structure to
interpret the event payload received with an M2MB_WLAN_CONNECT_CB or 
M2MB_WLAN_CONNECT_FAIL_CB events.
*/

typedef struct M2MB_CON_IND_S
{
    M2MB_WLAN_DEV_ID_E dev_id; 
    /**< WLAN device id or interface id 
    */
    M2MB_WLAN_CON_EVENT_ID_E eventId;
    /**< Connect event type  Refer M2MB_WLAN_CON_EVENT_ID_E
    */
	UINT8 mac_addr[M2MB_WLAN_MAC_LEN];
	/**< MAC address related to the connect event. Based on the eventId, 
         the driver fills in a different MAC addresses as follows. 
         - M2MB_WLAN_CON_STA   -- MAC address of the connected access point
         - M2MB_WLAN_CON_AP    -- MAC address of the device itself
         - M2MB_WLAN_CON_PEER  -- MAC address of peer station
                                  that was connected to the AP
    */
    M2MB_DISASSOC_REASON_T reason; 
    /**< Identifies the connect fail reason. 
        Valid only in case of M2MB_WLAN_CONNECT_FAIL_CB event
    */

}M2MB_CON_IND_T;


/*! 
@brief Disconnection event info for both AP and STA modes.

@details Data structure that presents dis-connect event information 
from the driver to the application. The application uses this data structure to
interpret the event payload received with an M2MB_WLAN_DISCONNECT_CB event.
*/

typedef struct M2MB_DISCON_IND_S
{
    M2MB_WLAN_DEV_ID_E dev_id; 
    /**< WLAN device id or interface id 
    */
    M2MB_WLAN_CON_EVENT_ID_E eventId;
    /**< Disconnect event type  Refer M2MB_WLAN_CON_EVENT_ID_E
    */
	UINT8 mac_addr[M2MB_WLAN_MAC_LEN];
	/**< MAC address related to the connect event. Based on the operating mode, 
         the driver fills in a different MAC addresses as follows. 
         - M2MB_WLAN_CON_STA event   -- MAC address contains all zeros
         - M2MB_WLAN_CON_AP event    -- MAC address contains zeros
         - M2MB_WLAN_CON_PEER event  -- MAC address of a peer station
                                        that was disconnected from the AP
    */
    M2MB_DISASSOC_REASON_T reason; 
    /**< Identifies the disconnection reason
    */
    
}M2MB_DISCON_IND_T;

/*! 
@brief AP client info.

@details Data structure that presents AP client information.
IT contains client information such as MAC addresses, IP addresses, 
Host names, Tx/Rx statistics.
*/

typedef struct M2MB_WLAN_AP_CLIENT_INFO_S
{
   M2MB_WLAN_DEV_ID_E dev_id; 
    /**< WLAN device id or interface id 
    */
   UINT8 mac_addr[M2MB_WLAN_MAC_LEN];
	/**< MAC address of the client 
    */
   UINT8 ip_addr[48];
    /**< IP address of client. String type
    */
   INT8 *host_name;
    /**< Buffer to store the host name. String type
    */
   UINT32 tx_stats;
   /**< Tx statistics
   */
   UINT32 rx_stats;
   /**< Rx statistics
   */
}M2MB_WLAN_AP_CLIENT_INFO_T;

/*! 
@brief AP client info request 

@details Data structure that is used to get AP client information
from application to driver.
*/
typedef struct M2MB_WLAN_AP_CLIENT_MSG_S
{
    UINT8 blocking;     /* 0 - non blocking 1 - Blocking */
    UINT16 max_cnt;     /* No of buffers provided by user application to store the client info */
    
    UINT16 *result_cnt; /* Buffer which is provided by the user to store result count */
                        
    void *result_buff; /* Buffer which is provided by the user to store the client info.
                          Array of M2MB_WLAN_AP_CLIENT_INFO_T structures 
                       */
   
   UINT16 host_name_size;
   /**< Max size allocated by application (inlcuding NULL char) 
        to store the host name
    */
}M2MB_WLAN_AP_CLIENT_MSG_T;

/*!
@brief Client info indication 

@details Data structure that is used to send AP client information from driver to application
*/
typedef struct M2MB_WLAN_AP_CLIENT_IND_S
{
    UINT16 result_cnt; /* result count */
    M2MB_WLAN_AP_CLIENT_INFO_T *result_buff; /* Buffer which contains client info. 
                                                Array of M2MB_WLAN_AP_CLIENT_INFO_T structures 
                                            */
}M2MB_WLAN_AP_CLIENT_IND_T;

#define M2MB_WLAN_MAC_STR_MAX_SIZE  20

typedef struct M2MB_WLAN_MAC_ADDR_STRING_S
{
  char mac[M2MB_WLAN_MAC_STR_MAX_SIZE]; /* MAC address stored as string (xx:xx:xx:xx:xx:xx */
  
}M2MB_WLAN_MAC_ADDR_STRING_T;

typedef struct M2MB_WLAN_MAC_ADDR_LIST_S
{
  void *mac_str;    /* points to first element */
  int max_element;  /* Max no of address that can be stored */
  int result_cnt;   /* No of mac address available in the result  */
}M2MB_WLAN_MAC_ADDR_LIST_T;

/*! 
@brief WLAN error info request 

@details Data structure that is used to get last error information
from driver to application.
*/
typedef struct M2MB_WLAN_ERROR_MSG_S
{
    UINT16 max_size;    /* Buffer size provided by user application to store the error info */
                        /* If error info exceeds this size then result will be truncated */
    INT8  *error_info;  /* Buffer which is provided by the user to store the client info
                           character array of max_size.
                        */
}M2MB_WLAN_ERROR_MSG_T;

typedef struct{
 int rsp_id;
 int status;
 int reply_time;
 int ttl_value;
 char ip_addr[32];
 char rsp[128]; /* <replyId>,<Ip Address>,<replyTime>,<ttl>*/
}M2MB_WLAN_PING_RESULT_T;

 typedef struct{
  M2MB_WLAN_RESPONSE_E status; 
 }M2MB_WLAN_PING_COMPLETE_T;

typedef struct M2MB_WLAN_CON_INFO_S
{
    UINT8 device_id;                        /**< device Id*/
    UINT8 mac_addr[6];                      /**< MAC address of device interface */
    INT32 wstate;                           /**< WLAN state */
    INT32 mode;                             /**< AP, STA modes */
    UINT8 bssid_mac_addr[6];                /**< BSSID MAC addr of connected network 
                                                 or created network */
    UINT8 ssid[32];                         /**< SSID of connected network or 
                                                 created network */
    INT32 channel;                          /**<channel number*/
    INT32 security;                         /**< security type OPEN, WPA PSK, WPA2 PSK*/
    INT32 rssi;                             /**< RSSI value in STA mode */
    UINT32 ip_addr;                         /**< IP address got from connected network 
                                                 or static IP address of AP mode */
    UINT32 subnet_mask;                     /**< subnet mask got from connected network 
                                                 or subnet mask of AP mode */
    UINT32 gateway;                         /**< gateway got from connected network 
                                                 or gateway of AP mode */
    UINT32 dns1;                            /**<DNS1 address */
    UINT32 dns2;                            /**<DNS2 address */
    UINT32 dns3;                            /**<DNS3 address */
}M2MB_WLAN_CON_INFO_T;



typedef void ( *m2mb_wlan_ind_callback )( M2MB_WLAN_IF_HANDLE ifh, M2MB_WLAN_IND_E wlan_event, 
    UINT16 resp_size, void *resp_struct, void *userdata );

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_wlan_init initializes WLAN service for current client

  @details
    m2mb_wlan_init initialize WLAN module returning the handle that must be passed as first
    parameter for m2mb_wlan_interface_init function. Calling the m2mb_wlan_init is mandatory
    before using the WLAN service.

  @param[in] *h
    first parameter is the handle to the WLAN service, that will be initialized by the function.    
  
  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_wlan_init(&h);
  @code
    
    int main()
    {
      M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
      M2MB_WLAN_HANDLE h;
        
      device_id = M2MB_WLAN_DEV_0;
    
      retVal = m2mb_wlan_init(&h);
      if ( retVal == M2MB_WLAN_SUCCESS )
        printf( "m2mb_wlan_init succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_init( M2MB_WLAN_HANDLE *h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_wlan_deinit deinitializes WLAN service. It will shut down the wlan module.

  @details
    m2mb_wlan_deinit deinitialize WLAN service for freeing the handle passed
    as first parameter. Calling the m2mb_wlan_deinit is useful after using the 
    WLAN service in order to free memory space.

  @param[in] h
    first parameter is the handle to the WLAN service, that will be deinitialized 
    by the function.

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_wlan_deinit(&h);
  @code
    M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
    M2MB_WLAN_HANDLE h;
        
    device_id = M2MB_WLAN_DEV_0;
        
    retVal = m2mb_wlan_init(&h);
    
    //... use WLAN APIs ...

    retVal = m2mb_wlan_deinit(h);
    if ( retVal == M2MB_WLAN_SUCCESS )
      printf( "m2mb_wlan_deinit succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_deinit( M2MB_WLAN_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    WLAN Interface Initialization

  @details
    This API initializes the interface for a given device id.
    Since, m2mb_wlan_init is the dependency for this API, so m2mb_wlan_init should be called
    before calling the m2m_wlan_interface_init.

  @param[in] *ifh
    First parameter is the handle to the WLAN interface handle, that will be initialized 
    by the function.
  @param[in] h
    Second parameter is the handle to the WLAN service  
  @param[in] device_id
  	User should pass the mode using M2MB_WLAN_DEV_ID_E.
  @param[in] callback
      callback that will be called if an event or a response happens.
  @param[in] userdata
      pointer to generic user data that will be returned as it is in the callback.
  	
  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @note
  	m2mb_wlan_init function should be called properly before calling m2mb_wlan_interface_init
  	
  @b
    m2mb_wlan_interface_init(M2MB_WLAN_IF_HANDLE &ifh, M2MB_WLAN_HANDLE h,
        M2MB_WLAN_DEV_ID_E device_id);
    
  @code
 	void myCallback(M2MB_WLAN_IF_HANDLE ifh, M2MB_WLAN_IND_E wlan_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      //my code
    }
    
  	int main()
  	{
		M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        M2MB_WLAN_IF_HANDLE ifh;
        struct myStruct myUserdata;
        M2MB_WLAN_DEV_ID_E device_id;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_STA;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        device_id = M2MB_WLAN_DEV_0;
	  	retVal = m2mb_wlan_interface_init(&ifh,h,device_id, myCallback, (void*)myUserdata);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            return -1;
        }

	  	return 0;
	}
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_interface_init(M2MB_WLAN_IF_HANDLE *ifh, M2MB_WLAN_HANDLE h, 
    M2MB_WLAN_DEV_ID_E device_id, m2mb_wlan_ind_callback callback, void *userdata );  
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    WLAN Interface bring up or start

  @details
    This API brings up the interface depend on the mode given by the user.
    Since, m2mb_wlan_init and m2mb_wlan_interface_init is the dependency for this API, 
    so m2mb_wlan_init and m2mb_wlan_interface_init should be called before calling the 
    m2m_wlan_interface_start.

  @param[in] ifh
    first parameter is the handle to the WLAN interface handle
  @param[in] mode
  	User should pass the mode using M2MB_WLAN_MODE_E.
  	
  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @note
  	m2mb_wlan_init and m2mb_wlan_interface_init function should be called properly before calling 
  	m2mb_wlan_interface_start function
  	
  @b
    m2mb_wlan_interface_start(M2MB_WLAN_IF_HANDLE ifh, M2MB_WLAN_MODE_E mode);
    
  @code

    void myCallback(M2MB_WLAN_IF_HANDLE ifh, M2MB_WLAN_IND_E wlan_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      //my code
    }
    
  	int main()
  	{
		M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        M2MB_WLAN_IF_HANDLE ifh;
        struct myStruct myUserdata;
        M2MB_WLAN_DEV_ID_E device_id;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_STA;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        device_id = M2MB_WLAN_DEV_0;
	  	retVal = m2mb_wlan_interface_init(&ifh,h,device_id, myCallback, (void*)myUserdata);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            return -1;
        }

        retVal = m2mb_wlan_interface_start(ifh,M2MB_WLAN_MODE_STATION);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }
        
	  	return 0;
	}
	
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_interface_start( M2MB_WLAN_IF_HANDLE ifh, 
    M2MB_WLAN_MODE_E mode);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     WLAN Interface bring down or stop
 
   @details
     This API brings down the interface.
  
   @param[in] ifh
     first parameter is the handle to the WLAN interface
    
   @return
     returns M2MB_WLAN_SUCCESS on success, a different value on error.
 
   @note
     m2mb_wlan_interface_start function should be called properly before calling 
     m2mb_wlan_interface_stop function
     
   @b
     m2mb_wlan_interface_stop(ifh );
     
   @code
 
     void myCallback(M2MB_WLAN_IF_HANDLE ifh, M2MB_WLAN_IND_E wlan_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
     {
       //my code
     }
     
     int main()
     {
         M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
         M2MB_WLAN_HANDLE h;
         M2MB_WLAN_IF_HANDLE ifh;
         struct myStruct myUserdata;
         M2MB_WLAN_DEV_ID_E device_id;
         M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_STA;
         
         retVal = m2mb_wlan_init(&h);
         if ( retVal != M2MB_WLAN_SUCCESS )
         {
             printf( "m2mb_wlan_init failed");
             return -1;
         }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
         device_id = M2MB_WLAN_DEV_0;
         retVal = m2mb_wlan_interface_init(&ifh,h,device_id, myCallback, (void*)myUserdata);
         if ( retVal != M2MB_WLAN_SUCCESS )
         {
             printf( "m2mb_wlan_interface_init failed");
             return -1;
         }
 
         retVal = m2mb_wlan_interface_start(ifh,M2MB_WLAN_MODE_STATION);
         if ( retVal != M2MB_WLAN_SUCCESS )
         {
             printf( "m2mb_wlan_interface_start failed");
             return -1;
         }
         
         retVal = m2mb_wlan_interface_stop(ifh);
         if ( retVal != M2MB_WLAN_SUCCESS )
         {
             printf( "m2mb_wlan_interface_stop failed");
             return -1;
         }
         
         return 0;
     }
     
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
 M2MB_WLAN_RESPONSE_E m2mb_wlan_interface_stop( M2MB_WLAN_IF_HANDLE ifh);  

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Removes the interface from the WLAN driver
    
  @details
    This API used to remove the device id or interface from the WLAN driver. 
    But, this API does not disable the WLAN module

  @param[in] ifh
    first parameter is the handle to the WLAN interface handle

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @b
    m2mb_wlan_interface_deinit(ifh);
    
  @code
    void myCallback(M2MB_WLAN_IF_HANDLE ifh, M2MB_WLAN_IND_E wlan_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      //my code
    }
    
  	int main()
  	{
		M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        M2MB_WLAN_IF_HANDLE ifh;
        struct myStruct myUserdata;
        M2MB_WLAN_DEV_ID_E device_id;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_STA;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        device_id = M2MB_WLAN_DEV_0;
	  	retVal = m2mb_wlan_interface_init(&ifh,h,device_id, myCallback, (void*)myUserdata);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            return -1;
        }

        retVal = m2mb_wlan_interface_start(ifh,M2MB_WLAN_MODE_STATION);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }

        retVal = m2mb_wlan_interface_stop(ifh);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_stop failed");
            return -1;
        }
         
        retVal = m2mb_wlan_interface_deinit(ifh);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            return -1;
        }
        
	  	return 0;
	}
	
  @endcode
*/

/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_interface_deinit( M2MB_WLAN_IF_HANDLE ifh);

/*-----------------------------------------------------------------------------------------------*/

/*!
  @brief
    m2mb_wlan_conf sets/gets some WLAN parameters.

  @details
    m2mb_wlan_conf sets/gets WLAN parameters listed in the M2MB_WLAN_ACTION_E enum.

  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function.
  @param[in] argc
    It determines the number of parameters that follows, plus one (e.g.: if two parameters follow, argc is 3)
  @param[in] ...
    It determines the action (among the ones listed in M2MB_WLAN_ACTION_E), while the parameters
    that follow depend on the action (see following note for further details)

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @note
    Action: M2MB_WLAN_SET_SSID

    Description: set the SSID. If the device is in AP mode, this SSID will acts
	             like SSID for the network. If the device is in STA mode, SSID
	             will be the SSID of the network to which STA wants to connect
	   
    Params(in): ifh(M2MB_WLAN_IF_HANDLE), argc(UINT8), action(M2MB_WLAN_ACTION_E), 
                dev_id(M2MB_WLAN_DEV_ID_E), ssid(M2MB_WLAN_SSID_T *)

    - example: set the SSID as "demoAP"

    M2MB_WLAN_SSID_T ssid;
    m2mb_wlan_conf(ifh, 3, M2MB_WLAN_SET_SSID, &ssid);


    Action: M2MB_WLAN_GET_SSID

    Description: get the SSID.

    Params(in): ifh(M2MB_WLAN_IF_HANDLE), argc(UINT8), action(M2MB_WLAN_ACTION_E), 
                ssid(M2MB_WLAN_SSID_T *)

    - example: get SSID

    M2MB_WLAN_SSID_T ssid;
    m2mb_wlan_conf(ifh, 3, M2MB_WLAN_GET_SSID, &ssid);

    Action: M2MB_WLAN_SET_IND
    Description : To enable or disable specific async event indication
    
    Params(in): h(M2MB_WLAN_IF_HANDLE), argc(UINT8), action(M2MB_WLAN_ACTION_E), 
                        indication, enable)
    
  - example: enable disconnect indication

    UINT8 enable_flag=1;
    M2MB_WLAN_IND_E ind =  M2MB_WLAN_DISCONNECT_CB;
    m2mb_wlan_conf(ifh, 4, M2MB_WLAN_SET_IND, ind, enable_flag);
    
  - example: get current configuration of disconnect indication

    UINT8 enable_flag;
    M2MB_WLAN_IND_E ind =  M2MB_WLAN_DISCONNECT_CB;
    m2mb_wlan_conf(ifh, 4, M2MB_WLAN_SET_IND, ind, &enable_flag);
    if(enable_flag == 1)
        //enabled
    else
        //disabled
  @b
    m2mb_wlan_conf(ifh, 4, M2MB_WLAN_SET_IND, ind, &enable_flag);
  @code
	
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_conf( M2MB_WLAN_IF_HANDLE ifh, UINT8 argc, ... );
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This API is used to initiate a wireless scan to find nearby access points
    m2mb_wlan_init, m2mb_wlan_interface_init and wlan_interface_start 
    should call before calling m2mb_wlan_scan

  @details
    This function does the wireless scan depending on the parameters passed by
    the user.
    Blocking call: WLAN driver fill the user buffer with the most recent scan results and
    result count.
    Non-blocking call: User callback function gets the asynchronous event for scan indication and
    scan complete. Scan indication event is used by driver to send scan results to application. 
    Each scan indication may contain one or more scan results which is informed using result count.
    Once scan is completed driver indicates this to application via separate asynchronous event.
    
  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function.
  @param[in] scan_req 
    Refer M2MB_WLAN_SCAN_MSG_T structure
  	  
  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @note
	1. Before calling m2mb_wlan_scan, m2mb_wlan_init and m2mb_wlan_interface_init
	   should be called and should return successful.

    2. In case of blocking call m2mb_wlan_scan will return, if
          a) time-out occurs or
          b) no of scan result exceeded the max_scan_entries or
          c) all channels are scanned
          
    3. In case of non-blocking call m2mb_wlan_scan will return immediately and
        M2MB_WLAN_SCAN_COMPLETE_CB will be indicated if,
          a) time-out occurs or
          b) all channels are scanned
          
        M2MB_WLAN_SCAN_IND_CB will be generated for scan entry.
  	
  @b
    m2m_wlan_scan( ifh, scan_req);
		
  @code
    void wlan_callback(M2MB_WLAN_IF_HANDLE ifh, M2MB_WLAN_IND_E wlan_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(wlan_event)
  		{
  			case M2MB_WLAN_SCAN_IND_CB:
  			{
  				M2MB_WLAN_SCAN_IND_T  *scan_ind_cb = 
  										(M2MB_WLAN_SCAN_IND_T *)resp_struct;
				int entries = 0, i = 0;
				entries = scan_ind_cb->no_of_entries;
				for (i = 0; i < entries; i++)
				{
				    //scan_ind_cb->result_buff[i]
					printf("scan results\n");
				}
  			}
  			break;

  			case M2MB_WLAN_SCAN_COMPLETE_CB:
  			{
  				//end of scan operation
  			}
  			break;
  		}
    }

    #define DEMO_APP_MAX_SCAN_BUF 16
    M2MB_WLAN_BSS_SCAN_INFO_T resultBuffer[DEMO_APP_MAX_SCAN_BUF];
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        struct myStruct myUserdata;
        UINT8 channel_list[3] = {1, 6, 11};
        M2MB_WLAN_SCAN_TYPE_E scan_type = M2MB_WLAN_SCAN_ACTIVE;
        UINT8 *ssid = NULL;
        M2MB_WLAN_DEV_ID_E dev_id = M2MB_WLAN_DEV_0;
        UINT16 scan_entry_count=0;
        M2MB_WLAN_SCAN_MSG_T scan_req;
        M2MB_WLAN_IF_HANDLE ifh_sta;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_STA;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        retVal = m2mb_wlan_interface_init(&ifh_sta,h,device_id,wlan_callback, (void*)myUserdata);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            return -1;
        }
        
        retVal = m2mb_wlan_interface_start(ifh_sta,M2MB_WLAN_MODE_STATION);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }
        
        //Blocking call example        
        memset(&scan_req,0,sizeof(scan_req));
        scan_req.scan_type = M2MB_WLAN_SCAN_ACTIVE;
        scan_req.no_of_channels = 3;
        scan_req.channel_list = &channel_list[0];
        scan_req.time_out = 5000; // 5 seconds 
        scan_req.blocking = 1 ;   // blocking call 
        scan_req.max_scan_entries = DEMO_APP_MAX_SCAN_BUF;
        scan_req.no_of_scan_entries = &scan_entry_count;
        scan_req.result_buff = &resultBuffer[0];
        
        retVal = m2mb_wlan_scan (ifh_sta, &scan_req);
		
		if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_scan failed");
            return -1;
        }

        //Non-blocking call        
        memset(&scan_req,0,sizeof(scan_req));
        scan_req.scan_type = M2MB_WLAN_SCAN_ACTIVE;
        scan_req.time_out = 10000; // 10 seconds 
        scan_req.blocking = 0 ;   // non-blocking call 
        scan_req.max_scan_entries = DEMO_APP_MAX_SCAN_BUF;
        scan_req.result_buff = &resultBuffer[0];
        retVal = m2mb_wlan_scan (ifh_sta, &scan_req); //This call returns immediately 
		
		if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_scan failed");
            return -1;
        }

        // wait for scan indication events
        // Note: M2MB_WLAN_SCAN_IND_CB will be generated by driver to send scant result to application
        // Note: scan events must be processed in user callback
        // wait for M2MB_WLAN_SCAN_COMPLETE_CB which indicates end of scan operation
        
      return 0;
    }
	
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_scan(M2MB_WLAN_IF_HANDLE ifh, M2MB_WLAN_SCAN_MSG_T *scan_req);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Abort the current scan operation.
    
  @details
    This API allows the user to abort or stop the non-blcking scan operation which is started 
    using wlan_scan
  
  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function.
 
  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  
  @b
    m2mb_wlan_scan_abort(ifh);
    
  @code  
  
   #define DEMO_APP_MAX_SCAN_BUF 16
    M2MB_WLAN_BSS_SCAN_INFO_T resultBuffer[DEMO_APP_MAX_SCAN_BUF];
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        struct myStruct myUserdata;
        UINT8 channel_list[3] = {1, 6, 11};
        M2MB_WLAN_SCAN_TYPE_E scan_type = M2MB_WLAN_SCAN_ACTIVE;
        M2MB_WLAN_SCAN_FORMAT_E scan_format=M2MB_WLAN_SCAN_NORMAL;
        UINT8 *ssid = NULL;
        M2MB_WLAN_DEV_ID_E dev_id = M2MB_WLAN_DEV_0;
        UINT16 scan_entry_count=0;
        M2MB_WLAN_SCAN_MSG_T scan_req;
        M2MB_WLAN_IF_HANDLE ifh_sta;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_STA;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        retVal = m2mb_wlan_interface_init(&ifh_sta,h,device_id,wlan_callback, (void*)myUserdata);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            return -1;
        }
        
        retVal = m2mb_wlan_interface_start(ifh_sta,M2MB_WLAN_MODE_STATION);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }
       
        //Non-blocking call        
        memset(&scan_req,0,sizeof(scan_req));
        scan_req.scan_type = M2MB_WLAN_SCAN_ACTIVE;
        scan_req.result_format = scan_format;
        scan_req.time_out = 10000; // 10 seconds 
        scan_req.blocking = 0 ;   // non-blocking call 
        scan_req.max_scan_entries = DEMO_APP_MAX_SCAN_BUF;
        scan_req.result_buff = &resultBuffer[0];
        retVal = m2mb_wlan_scan (ifh_sta, &scan_req); //This call returns immediately 
		
		if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_scan failed");
            return -1;
        }

        // wait for scan indication events
        // Note: M2MB_WLAN_SCAN_IND_CB will be generated by driver to send scant 
        //       result to application
        // Note: abort can be called before recieving M2MB_WLAN_SCAN_COMPLETE_CB event

        m2mb_wlan_scan_abort(ifh_sta);
        
      return 0;
    }
	
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_scan_abort( M2MB_WLAN_IF_HANDLE ifh);

/*-----------------------------------------------------------------------------------------------*/

/*!
  @brief
    This API is part of connect process in non-AP station mode

  @details
    This function scans for particular SSID given by user and try to connects 
    to it.If SSID is set to a non-empty string before calling this API then 
    it is considered as a connect request. Then, user callback gets called after 
    connect event with status.
  
  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the 
    m2mb_wlan_interface_init function.
  @param[in] ssid
  	  SSID of the particular Access Point to connect [mandatory]

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @note
	Before calling m2m_wlan_connect, m2mb_wlan_init, m2mb_wlan_interface_init and
	m2mb_wlan_interface_start should be called and should return successful. 
	If user wants to connect to an secured network, then use m2mb_wlan_conf to configure parameters. 
	Refer M2MB_WLAN_ACTION_E for setting secured parameters.
  	
  @b
    m2m_wlan_connect ( handle, ssid);
    
  @code
    void wlan_callback(M2MB_WLAN_IF_HANDLE ifh, M2MB_WLAN_IND_E wlan_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(wlan_event)
  		{
  			case M2MB_WLAN_CONNECT_CB:
  			{//client connect event in AP mode
  				M2MB_CON_IND_T *con_ind = 
  										(M2MB_CON_IND_T *)resp_struct;
				//Process the connect event
  			}
  			break;
  			case M2MB_WLAN_DISCONNECT_CB:
  			{
  				M2MB_DISCON_IND_T *con_ind = 
  										(M2MB_DISCON_IND_T *)resp_struct;
				//Process the disconnect event
  			}
  			break;
  		}
    }

    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;        
        M2MB_WLAN_IF_HANDLE ifh_sta;
        struct myStruct myUserdata;
        UINT8 *ssid = "scan_ap";
        M2MB_WLAN_DEV_ID_E dev_id = M2MB_WLAN_DEV_0;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_STA;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        retVal = m2mb_wlan_interface_init(&ifh_sta,h,device_id, wlan_callback, (void*)myUserdata);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            return -1;
        }

       retVal = m2mb_wlan_interface_start(ifh_sta,M2MB_WLAN_MODE_STATION);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }
        
      retVal = m2mb_wlan_connect ( ifh_sta, ssid); //its blocking call. No call back to application
      if ( retVal != M2MB_WLAN_SUCCESS )
      {
            printf( "m2mb_wlan_connect failed");
            return -1;
      }

      return 0;
    }
    
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_connect (M2MB_WLAN_IF_HANDLE ifh, UINT8 *ssid );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Disconnects a device from a network
    
  @details
    This API allows the user to disconnect a device, or abort the current 
    connection process. 
  
  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function.
 
  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  
  @b
    m2mb_wlan_disconnect(ifh);
    
  @code  
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        M2MB_WLAN_IF_HANDLE ifh_sta;
        struct myStruct myUserdata;
        UINT8 *ssid = "scan_ap";
        M2MB_WLAN_DEV_ID_E dev_id = M2MB_WLAN_DEV_0;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_STA;

        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        device_id = M2MB_WLAN_DEV_0;
	  	retVal = m2mb_wlan_interface_init(&ifh_sta,h,device_id, myCallback, (void*)myUserdata);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            return -1;
        }

        retVal = m2mb_wlan_interface_start(ifh_sta,M2MB_WLAN_MODE_STATION);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }
        
        retVal = m2mb_wlan_connect ( ifh_sta, ssid); //its blocking call
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_connect failed");
            return -1;
        }

        retVal = m2mb_wlan_disconnect( ifh_sta ); //its blocking call

        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_disconnect failed");
            return -1;
        }
      
      return 0;
    }
    
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_disconnect( M2MB_WLAN_IF_HANDLE ifh);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Configure WLAN mode

  @details
    This API allows the user to configure operational mode as STA, AP, AP_AP or AP_STA
  
  @param[in] h
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.  
  @param[in] mode
  	  Operating mode of WLAN module [mandatory]

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  
  @b
    m2mb_wlan_mode(h,op_mode);
    
  @code  
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        M2MB_WLAN_IF_HANDLE ifh_ap;        
        M2MB_WLAN_IF_HANDLE ifh_sta;
        struct myStruct myUserdata;
        UINT8 *ssid = "scan_ap";
        M2MB_WLAN_DEV_ID_E dev_id_ap = M2MB_WLAN_DEV_0;
        M2MB_WLAN_DEV_ID_E dev_id_sta = M2MB_WLAN_DEV_1;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_AP_STA;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        device_id = M2MB_WLAN_DEV_0;
	  	retVal = m2mb_wlan_interface_init(&ifh_ap,h,device_id, myCallback, (void*)myUserdata);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            m2mb_wlan_deinit(h);
            return -1;
        }

        device_id = M2MB_WLAN_DEV_1;
	  	retVal = m2mb_wlan_interface_init(&ifh_sta,h,device_id, myCallback, (void*)myUserdata);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        retVal = m2mb_wlan_interface_start(ifh_ap,M2MB_WLAN_MODE_ACCESS_POINT);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }

        retVal = m2mb_wlan_interface_start(ifh_sta,M2MB_WLAN_MODE_STATION);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }
        
      return 0;
    }
    
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_mode_set( M2MB_WLAN_HANDLE h,M2MB_WLAN_OP_MODE_E mode);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get WLAN operating mode

  @details
    This API allows the user to get operational mode
    
  @param[in] h
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function. 
  @param[in] mode
  	  Operating mode of WLAN module

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  
  @b
    m2mb_wlan_mode_get(h,op_mode);
    
  @code  
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        M2MB_WLAN_OP_MODE_E op_mode;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_get(h,&op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode_get failed");
            m2mb_wlan_deinit(h);
            return -1;
        }

        if(op_mode == M2MB_WLAN_OP_MODE_STA)
        {
            //Single mode with station interface
        }else 
        if(op_mode == M2MB_WLAN_OP_MODE_AP)
        {
            //Single mode with AP interface
        }else
        if(op_mode == M2MB_WLAN_OP_MODE_AP_STA)
        {
            //Concurrent mode with AP plus station interface
        }else
        if(op_mode == M2MB_WLAN_OP_MODE_AP_AP)
        {
            //Concurrent mode with AP plus AP interface
        }

      return 0;
    }
    
  @endcode
*/

/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_mode_get( M2MB_WLAN_HANDLE h,M2MB_WLAN_OP_MODE_E *mode);

/*-----------------------------------------------------------------------------------------------*/

#if 0 //Note: M2MB_WLAN_GET_RSSI can be used 
/*!
  @brief
    Get signal strength of the network

  @details
    This API allows the user to get the current signal strength of the network
    
  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function. 
  @param[in] sig_val
    Parameter to store signal strength value
    
  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  
  @b
    m2mb_wlan_sig_get(ifh,sig_val);
    
  @code  
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        M2MB_WLAN_IF_HANDLE ifh_sta;
        INT8 sig_val;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_STA;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        retVal = m2mb_wlan_interface_init(&ifh_sta,h,device_id, wlan_callback, (void*)myUserdata);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            return -1;
        }

       retVal = m2mb_wlan_interface_start(ifh_sta,M2MB_WLAN_MODE_STATION);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }
        
      retVal = m2mb_wlan_connect ( ifh_sta, ssid); //its blocking call. No call back to application
      if ( retVal != M2MB_WLAN_SUCCESS )
      {
            printf( "m2mb_wlan_connect failed");
            return -1;
      }

      retVal = m2mb_wlan_sig_get(ifh_sta,&sig_val);
      if ( retVal != M2MB_WLAN_SUCCESS )
      {
          printf( "m2mb_wlan_sig_get failed");
      }

      return 0;
    }
    
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_sig_get( M2MB_WLAN_IF_HANDLE ifh,INT8 *sig_val);

/*-----------------------------------------------------------------------------------------------*/
#endif

/*!
  @brief
    Update hostapd.conf file

  @details
    This API allows the user to updated hostapd.conf file.
    Hostapd configuration file controls many options with regards to AP mode only.
    
  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function.  
  @param[in] dev_id
  	User should pass the mode using M2MB_WLAN_DEV_ID_E
  @param[in] conf_buf
      Buffer containing the hostapd configuration
  @param[in] conf_size
      Size of hostapd configuration buffer

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  
  @b
    m2mb_wlan_hostapd_set(ifh,conf_buf, conf_size);
    
  @code  
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        M2MB_WLAN_IF_HANDLE ifh_ap;        
        struct myStruct myUserdata;
        M2MB_WLAN_DEV_ID_E dev_id_ap = M2MB_WLAN_DEV_0;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_AP;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        device_id = M2MB_WLAN_DEV_0;
        retVal = m2mb_wlan_interface_init(&ifh_ap,h,device_id, myCallback, (void*)myUserdata);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            m2mb_wlan_deinit(h);
            return -1;
        }

        retVal = m2mb_wlan_interface_start(ifh_ap,M2MB_WLAN_MODE_ACCESS_POINT);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }

       m2mb_wlan_hostapd_set(ifh_ap,con_buf,conf_size);
        
      return 0;
    }
    
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_hostapd_set( M2MB_WLAN_IF_HANDLE ifh,
        M2MB_WLAN_DEV_ID_E dev_id, INT8 *conf_buf, UINT32 conf_size);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Add or update wlan custom bin file

  @details
    This API allows the user to allows to add or change the wlan board data firmware file 
    or otp file.
    
  @param[in] h
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] bin_id
    Binary ID or binary index
  @param[in] bin_buf
      Buffer containing the file content
  @param[in] bin_size
      Size to write
  @param[in] offset
      Binary offset
  @param[in] eof_flag
      End of file flag which indicates its last chunk to write.

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  
  @b
    m2mb_wlan_bd_set(h, bin_id, bin_buf, bin_size, offset, eof_flag);
    
  @code  
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        INT8 *bin_buf;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

       offset = 0;       
       //If file size is small then we can write in single chunk
       eof_flag = 1; 
       //Get file size
       bin_size =  get_bin_size();
       
       bin_buf = m2mb_os_malloc(bin_size);

       // Get binary file data from user and store it in bin_buf[]
       get_bin_file(bin_buf);
       
       m2mb_wlan_bd_set(h,bin_id,bin_buf,bin_size,offset,eof_flag);
        
      return 0;
    }
    
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_bd_set( M2MB_WLAN_HANDLE h,
        M2MB_WLAN_BIN_ID_E bin_id, INT8 *bin_buf, UINT32 bin_size,
        UINT32 offset, UINT8 eof_flag);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get status of the currently added wlan board data firmware file or otp file.

  @details
    This API allows the user to read the sttaus of currently added firmware file.
    If it returns size as 0, it means there are no added wlan board data firmware file.
    If the size is lager than 0, it means there are added firmware file.

  @param[in] h
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] bin_id
    Binary ID or binary index
  @param[in] bin_size
      Size to write

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  
  @b
    m2mb_wlan_bd_get(h, bin_id, bin_size):
    
  @code  
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        INT8 *bin_buf;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

       offset = 0;       
       //If file size is small then we can write in single chunk
       eof_flag = 1; 
       //Get file size
       bin_size =  get_bin_size();
       
       bin_buf = m2mb_os_malloc(bin_size);

       // Get binary file data from user and store it in bin_buf[]
       get_bin_file(bin_buf);
       
       m2mb_wlan_bd_set(h,bin_id,bin_buf,bin_size,offset,eof_flag);

       m2mb_wlan_bd_get(h,bin_id,&bin_size);

       if(bin_size >0)
         //added firmware file exist in the system
       else
         //there are no added wlan firmware file

      return 0;
    }
    
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_bd_get( M2MB_WLAN_HANDLE h,
        M2MB_WLAN_BIN_ID_E bin_id, UINT32 *bin_size);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This API is used to get client inofrmation in AP mode.
    m2mb_wlan_init, m2mb_wlan_interface_init and wlan_interface_start 
    should call before calling m2mb_wlan_ap_client_info_get

  @details
    This function get the client information.

    Blocking call: WLAN driver fill the user buffer with connected client information
    and result count. If number of connected clients is greater than application provided buffer
    then driver will fill only max available buffers ('max_cnt' entries).

    Non-blocking call: User callback function gets the asynchronous event for client information.
    AP client info indication event is used by driver to send client info to application. 
    Each indication may contain one or more client information which is informed using result count.
    At of the result is indicated via separate asynchronous event. Non-blocking method is
    useful if client list is large and application does not have enough memory.
    
  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function.
  @param[in] *client_req 
    Refer M2MB_WLAN_AP_CLIENT_MSG_T structure
  	  
  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @note
	1. Before calling m2mb_wlan_ap_client_info_get, m2mb_wlan_init and 
	   m2mb_wlan_interface_init should be called and should return successful.
          
    2. In case of non-blocking call m2mb_wlan_ap_client_info_get will return 
        immediately and M2MB_WLAN_AP_CLIENT_INFO_DONE_CB will be indicated once all
        the client information is sent to application.
          
        M2MB_WLAN_AP_CLIENT_IND_CB will be generated for client result.
        
  @b
    m2mb_wlan_ap_client_info_get( ifh, client_req);
		
  @code
    void wlan_callback(M2MB_WLAN_IF_HANDLE ifh, M2MB_WLAN_IND_E wlan_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(wlan_event)
  		{
  			case M2MB_WLAN_AP_CLIENT_IND_CB:
  			{
  				M2MB_WLAN_AP_CLIENT_IND_T  *client_ind_cb = 
  										(M2MB_WLAN_AP_CLIENT_IND_T *)resp_struct;
				int entries = 0, i = 0;
				entries = client_ind_cb->no_of_entries;
				for (i = 0; i < entries; i++)
				{
				    //client_ind_cb->result_buff[i]
					printf("client information \n");
				}
  			}
  			break;

  			case M2MB_WLAN_AP_CLIENT_INFO_DONE_CB:
  			{
  				//end of ap client result
  			}
  			break;
  		}
    }

    #define DEMO_APP_MAX_CLIENT_BUF 16
    M2MB_WLAN_AP_CLIENT_INFO_T resultBuffer[DEMO_APP_MAX_CLIENT_BUF];
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        struct myStruct myUserdata;
        UINT8 *host_name = NULL;
        M2MB_WLAN_DEV_ID_E dev_id = M2MB_WLAN_DEV_0;
        UINT16 client_info_count=0;
        M2MB_WLAN_AP_CLIENT_MSG_T client_req;
        M2MB_WLAN_IF_HANDLE ifh_ap;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_AP;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }
        
        retVal = m2mb_wlan_interface_init(&ifh_ap,h,device_id,wlan_callback, (void*)myUserdata);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            return -1;
        }
        
        retVal = m2mb_wlan_interface_start(ifh_ap,M2MB_WLAN_MODE_ACCESS_POINT);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }
        
        //Blocking call example        
        memset(&client_req,0,sizeof(client_req));
        client_req.blocking = 1 ;   // blocking call 
        client_req.max_cnt = DEMO_APP_MAX_CLIENT_BUF;
        client_req.result_cnt = &client_info_count;
        client_req.result_buff = &resultBuffer[0];
        
        retVal = m2mb_wlan_ap_client_info_get (ifh_ap, &client_req);
		
		if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_ap_client_info_get failed");
            return -1;
        }

        //Non-blocking call        
        memset(&client_req,0,sizeof(client_req));
        client_req.blocking = 0 ;   // non-blocking call 
        client_req.max_cnt = DEMO_APP_MAX_CLIENT_BUF;
        client_req.result_cnt = &client_info_count;
        client_req.result_buff = &resultBuffer[0];
        
        retVal = m2mb_wlan_ap_client_info_get (ifh_sta, &client_req); //This call returns immediately 
		
		if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_ap_client_info_get failed");
            return -1;
        }

        // wait for client info indication events
        // Note: M2MB_WLAN_AP_CLIENT_IND_CB will be generated by driver to send client info
        // Note: client info events must be processed in user callback
        // wait for M2MB_WLAN_AP_CLIENT_INFO_DONE_CB which indicates end of result
        
      return 0;
    }
	
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_ap_client_info_get(M2MB_WLAN_IF_HANDLE ifh, 
    M2MB_WLAN_AP_CLIENT_MSG_T *client_req);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get WLAN last error 

  @details
    This API allows the user to get last wlan error in detailed form. (string format)
    
  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function. 
  @param[in] *err_msg
  	refer M2MB_WLAN_ERROR_MSG_T structure
  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  
  @b
    m2mb_wlan_error_info_get(ifh,err_info);
    
  @code  
    #define DEMO_APP_ERR_INFO_MAX_SIZE 256
    INT8 demo_app_err_buf[DEMO_APP_ERR_INFO_MAX_SIZE];
    int main()
    {
        M2MB_WLAN_RESPONSE_E retVal = M2MB_WLAN_SUCCESS;
        M2MB_WLAN_HANDLE h;
        M2MB_WLAN_ERROR_MSG_T err_msg;
        
        M2MB_WLAN_IF_HANDLE ifh_ap;
        M2MB_WLAN_OP_MODE_E op_mode = M2MB_WLAN_OP_MODE_AP;
        
        retVal = m2mb_wlan_init(&h);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_init failed");
            return -1;
        }

        retVal = m2mb_wlan_mode_set(h,op_mode);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_mode failed");
            m2mb_wlan_deinit(h);
            return -1;
        }        

        retVal = m2mb_wlan_interface_init(&ifh_ap,h,device_id,wlan_callback, (void*)myUserdata);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_init failed");
            return -1;
        }
        
        retVal = m2mb_wlan_interface_start(ifh_ap,M2MB_WLAN_MODE_ACCESS_POINT);
	  	if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_interface_start failed");
            return -1;
        }
        
        err_msg.max_size = DEMO_APP_ERR_INFO_MAX_SIZE;
        err_msg.error_info = demo_app_err_buf;
        
        retVal = m2mb_wlan_error_info_get(ifh,&err_msg);
        if ( retVal != M2MB_WLAN_SUCCESS )
        {
            printf( "m2mb_wlan_error_info_get failed");
            return -1;
        }

        printf("WLAN Error Info : %s",err_msg.error_info);
        
      return 0;
    }
    
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_error_info_get( M2MB_WLAN_IF_HANDLE ifh, 
			M2MB_WLAN_ERROR_MSG_T *err_msg);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
  	Set Security Configuration in hostapd file

  @details
  	API allows get security parameters configured for Access Point

  @param[in] h
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] device_id
  	User should pass the mode using M2MB_WLAN_DEV_ID_E.
  @param[in] *sec_type
  	Refer M2MB_WLAN_AP_SEC_TYPE_E Structure
  @param[in] *key_mgmt
  	Refer M2MB_WLAN_SECURITY_MODE_E Structure
  @param[in] *pairwise
  	Refer M2MB_WLAN_ENCRYPT_TYPE_E Structure
  @param[in] *key
  	Last parameter points to the pass-key that has to be updated.

  @return
  	returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @b
    m2mb_wlan_hostapd_get_sec_config(h,device_id,sec_type,sec_mode,enc_type,key);
    
  @code  

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_WLAN_RESPONSE_E m2mb_wlan_hostapd_get_sec_config(
            M2MB_WLAN_HANDLE h,
            M2MB_WLAN_DEV_ID_E device_id,            
            M2MB_WLAN_AP_SEC_TYPE_E *sec_type,
            M2MB_WLAN_SECURITY_MODE_E *key_mgmt,
            M2MB_WLAN_ENCRYPT_TYPE_E *pairwise,
            INT8* key);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
	Set Security Configuration in hostapd file
  @details
  	API allows modification of security parameters for Access Point
  @param[in] h
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] device_id
  	User should pass the mode using M2MB_WLAN_DEV_ID_E.
  @param[in] sec_type
  	Refer M2MB_WLAN_AP_SEC_TYPE_E Structure
  @param[in] sec_mode
  	Refer M2MB_WLAN_SECURITY_MODE_E Structure
  @param[in] enc_type
  	Refer M2MB_WLAN_ENCRYPT_TYPE_E Structure
  @param[in] *key
  	Last parameter points to the pass-key that has to be updated.

  @return
  	returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @b
    m2mb_wlan_hostapd_set_sec_config(h,device_id,sec_type,sec_mode,enc_type,key);
    
  @code  

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_hostapd_set_sec_config(
            M2MB_WLAN_HANDLE h,
            M2MB_WLAN_DEV_ID_E device_id,            
            M2MB_WLAN_AP_SEC_TYPE_E sec_type,
            M2MB_WLAN_SECURITY_MODE_E sec_mode,
            M2MB_WLAN_ENCRYPT_TYPE_E enc_type,
            INT8* key);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief

  @details
  
  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function. 
  @param[in] *addrstr
  @param[in] retry_count
  	Parameter defines the number of Ping Echo Request to send.
  @param[in] pkt_size
  	Parameter defines the lenght of Ping Echo Request message
  @param[in] timeout
  	Parameter defines the timeout, in 100 ms units, waiting a single Echo
	Reply 
  @param[in] ttl
  	Parameter defines time to live
  	
  @return
  	returns M2MB_WLAN_SUCCESS on success, a different value on error.
    
  @b
    m2mb_wlan_ping_req(ifh,addrstr,retry_count,pkt_size,timeout,ttl);
    
  @code  

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_ping_req(
    M2MB_WLAN_IF_HANDLE ifh,
    char *addrstr,
    INT32  retry_count,
    INT32  pkt_size,
    INT32  timeout,
    INT32  ttl);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
	Get the status of WLAN Interface
  @details
  	API allows user to get the status of WLAN interface.
  @param[in] h
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] if_id
  	User should pass the mode using M2MB_WLAN_DEV_ID_E.
  @param[in] *if_status
	Pointer to read the interface status.
  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  @b
    m2mb_wlan_interface_status_get(h,if_id,if_status);
    
  @code  

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_interface_status_get(M2MB_WLAN_HANDLE h, 
    M2MB_WLAN_DEV_ID_E if_id, 
    int *if_status);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief

  @details
  
  @param[in] h
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] *cfg_legacy
  	Refer M2MB_WLAN_CONFIG_LEGACY_T Structure
  @return
	returns M2MB_WLAN_SUCCESS on success, a different value on error.
  @b
    m2mb_wlan_le910cx_cfg_get(h,cfg_legacy);
    
  @code  

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_le910cx_cfg_get(M2MB_WLAN_HANDLE h, 
        M2MB_WLAN_CONFIG_LEGACY_T *cfg_legacy);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief

  @details
  
  @param[in] h
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] *cfg_legacy
  	Refer M2MB_WLAN_CONFIG_LEGACY_T Structure

  @return
  	returns M2MB_WLAN_SUCCESS on success, a different value on error.
  @b
    m2mb_wlan_le910cx_cfg_set(h,cfg_legacy);
    
  @code  

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_le910cx_cfg_set(M2MB_WLAN_HANDLE h, 
        M2MB_WLAN_CONFIG_LEGACY_T *cfg_legacy);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief

  @details
  
  @param[in] wlan_hdl
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] clock_val
	Parameter sets WLAN SDIO Clock setting
	  1 - 400khz
	  2 - 20Mhz
	  3 - 25Mhz
	  4 - 50Mhz
	  5 - 100Mhz
	  6 - 200Mhz (default)

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  @b
    m2mb_wlan_le910cx_clock_set(wlan_hdl,clock_val);
    
  @code  

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_le910cx_clock_set(
	M2MB_WLAN_HANDLE wlan_hdl,
        int clock_val);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief

  @details
  
  @param[in] wlan_hdl
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] clock_val
	Parameter to get WLAN SDIO Clock setting

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  @b
    m2mb_wlan_le910cx_clock_get(wlan_hdl,clock_val);
    
  @code  
  
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_le910cx_clock_get(
        M2MB_WLAN_HANDLE wlan_hdl,
        int *clock_val);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
	Select WE866Cx module 

  @details
  	API allows user to select the module in LE910Cx. i.e, either WE866C3 or WE866C6
  	
  @param[in] wlan_hdl
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] module_id
  	Parameter allows user to set the module id. 0- WE866C3, 1- WE866C6

  @return
  	returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @b
    m2mb_wlan_le910cx_module_select(wlan_hdl,module_id);
    
  @code  
  
	static M2MB_WLAN_HANDLE g_wlan_hdl;
  	int main()
  	{
  		module_id = 0;	//0- WE866C3, 1- We866C6
  		m2mb_wlan_le910cx_module_select(g_wlan_hdl,module_id);
  	}
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_le910cx_module_select(
            M2MB_WLAN_HANDLE wlan_hdl,
            int module_id);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
	Check WLAN start state
  @details
    This API allows user to check the status of WLAN start command
    
  @param[in] wlan_hdl
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] *wlan_start_state
    Parameter indicates the status of WLAN start

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @b
    m2mb_wlan_le910cx_start_state_get(wlan_hdl,wlan_start_state);
    
  @code
	int main()
	{
		
		M2MB_WLAN_PVT_CONTEXT_T *wlan_hdl = (M2MB_WLAN_PVT_CONTEXT_T *)h;
		int wlan_start_state;
		int *if_status

	    if( M2MB_WLAN_SUCCESS == m2mb_wlan_le910cx_start_state_get(wlan_hdl,&wlan_start_state ))
	    {
	       if((M2MB_WLAN_START_STATUS_E)wlan_start_state == M2MB_WLAN_START_STATUS_OFF)
	       {
			  *if_status = 1;
	       }
    	}
	}
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_le910cx_start_state_get(
            M2MB_WLAN_HANDLE wlan_hdl,
            int *wlan_start_state);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
	Restore driver config

  @details
  	This API allows user to restore the driver configuration from the backup configuration file 

  @param[in] wlan_hdl
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] dfs_master_flag
  	Flag indicates dfs master enable or disable . 1- Enable, 0- Disable

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.

  @b
    m2mb_wlan_le910cx_driver_cfg_restore(wlan_hdl, dfs_master_en);
    
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_le910cx_driver_cfg_restore(
        M2MB_WLAN_HANDLE wlan_hdl,
        int dfs_master_flag);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
	Dynamic Frequency Selection Master enable or disable

  @details
  	This API allows the user to enable or disable the DFS Master.

  @param[in] wlan_hdl
    Handle to the WLAN module, previously initialized by the m2mb_wlan_init function.
  @param[in] dfs_enable
  	Flag indicates dfs master enable or disable . 1- Enable, 0- Disable

  @return
    returns M2MB_WLAN_SUCCESS on success, a different value on error.
  @b
	m2mb_wlan_le910cx_dfs_set(g_wlan_hdl,dfs_enable);
    
  @code  

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_WLAN_RESPONSE_E m2mb_wlan_le910cx_dfs_set(
            M2MB_WLAN_HANDLE wlan_hdl, 
            int dfs_enable);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
	Device Information

  @details
  	This API is used to get the device information in the scan.

  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function. 
  @param[in] *pConInfo
  	Refer M2MB_WLAN_CON_INFO_T Structure

  @return
    returns 0 on success, a different value on error.

  @b
	m2mb_wlan_le910cx_con_info_get(ifh, pConInfo);
    
  @code  
  
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
int m2mb_wlan_le910cx_con_info_get(
        M2MB_WLAN_IF_HANDLE ifh,
        M2MB_WLAN_CON_INFO_T *pConInfo);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
	Check if WLAN is already enabled
  
  @details
	This API used to check if WLAN is already enabled.

  @param[in] ifh
    Handle to the WLAN interface, previously initialized by the m2mb_wlan_interface_init function. 


  @return
    returns TRUE on success, FALSE on error.

  @b
	m2mb_wlan_le910cx_is_connected(ifh);
    
  @code
  	int main()
  	{
  		
		M2MB_WLAN_RESPONSE_E ret_val = M2MB_WLAN_SUCCESS;
		M2MB_WLAN_IF_HANDLE ifh;

		ifh = m2m_wlan_sta_interface_hld_get();
		if(ifh == NULL)
			return false;

		return	m2mb_wlan_le910cx_is_connected(ifh);
	}
	
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
bool m2mb_wlan_le910cx_is_connected(M2MB_WLAN_IF_HANDLE ifh);


#endif
