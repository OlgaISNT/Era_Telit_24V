/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_ble.h

  @brief
    BLE driver functionality

  @details
    This file contains the public APIs and structures of Bluetooth Low Energy
    (BLE) client and server module.

  @note

  @author
    
    
  @date
    12/08/2020
*/

#ifndef  M2MB_BLE_H
#define  M2MB_BLE_H

/* GLOBAL DECLARATIONS ==========================================================================*/

//#define M2MB_BLE_MAX_DEVICE_NAME_LENGTH         248
#define M2MB_BLE_MAX_DEVICE_NAME_LENGTH         (39+1) /**< GAP_DEVICE_NAME_LEN in gap.h */
#define M2MB_BLE_MAX_MANUFACTURER_NAME_LENGTH   M2MB_BLE_MAX_DEVICE_NAME_LENGTH
#define M2MB_BLE_FIXPIN_SIZE                    6
#define M2MB_BLE_ADV_DATA_SIZE                  32
#define M2MB_BLE_SCAN_RSP_DATA_SIZE             32
#define M2MB_BLE_ADDRESS_RESOLUTION_TIMEOUT     60
#define M2MB_BLE_MTU_SIZE_MAX                   158
#define M2MB_BLE_BOND_ENTRIES_MAX               20
#define M2MB_BLE_MAX_ATTRIBUTE_LENGTH						600

/* Max hex value string which can hold the Maximum MTU data */
#define M2MB_BLE_MAX_HEX_VALUE                  ((2 * M2MB_BLE_MTU_SIZE_MAX) + 1)

#define M2MB_BLE_MAX_PAIRED_DEVICES             4 /**< Maximum  paired devices allowed when the device is in Central Mode. This is platform specific */

#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_INVALID_HANDLE                      0x01
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_READ_NOT_PERMITTED                  0x02
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_WRITE_NOT_PERMITTED                 0x03
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_INVALID_PDU                         0x04
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_AUTHENTICATION         0x05
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_REQUEST_NOT_SUPPORTED               0x06
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_INVALID_OFFSET                      0x07
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_AUTHORIZATION          0x08
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_ATTRIBUTE_NOT_FOUND                 0x0A
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_ATTRIBUTE_NOT_LONG                  0x0B
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_ENCRYPTION_KEY_SIZE    0x0C
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_INVALID_ATTRIBUTE_VALUE_LENGTH      0x0D
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_ENCRYPTION             0x0F
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_UNSUPPORTED_GROUP_TYPE              0x10
#define M2MB_BLE_ATT_PROTOCOL_ERROR_CODE_INSUFFICIENT_RESOURCES              0x11

/**< The following define GATT Characteristic Properties Definitions */
#define M2MB_BLE_GATT_CHARACTERISTIC_PROPERTIES_BROADCAST                      0x01
#define M2MB_BLE_GATT_CHARACTERISTIC_PROPERTIES_READ                           0x02
#define M2MB_BLE_GATT_CHARACTERISTIC_PROPERTIES_WRITE_WITHOUT_RESPONSE         0x04
#define M2MB_BLE_GATT_CHARACTERISTIC_PROPERTIES_WRITE                          0x08
#define M2MB_BLE_GATT_CHARACTERISTIC_PROPERTIES_NOTIFY                         0x10
#define M2MB_BLE_GATT_CHARACTERISTIC_PROPERTIES_INDICATE                       0x20
#define M2MB_BLE_GATT_CHARACTERISTIC_PROPERTIES_AUTHENTICATED_SIGNED_WRITES    0x40
#define M2MB_BLE_GATT_CHARACTERISTIC_PROPERTIES_EXTENDED_PROPERTIES            0x80

/**< The following define GATT Client Configuration Characteristic Definitions */
#define M2MB_BLE_GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_NOTIFY_ENABLE        0x0001
#define M2MB_BLE_GATT_CLIENT_CONFIGURATION_CHARACTERISTIC_INDICATE_ENABLE      0x0002

/**< The following defines the GATT Client Characteristic Configuration
     UUID that is used when building the service tables.
*/
#define M2MB_BLE_GATT_CLIENT_CHARACTERISTIC_CONFIGURATION_BLUETOOTH_UUID_CONSTANT  {0x02, 0x29}


/**< The following values define the valid values that the Vendor ID
     Source field of the DIS PNP characteristic may contain.
*/
#define M2MB_BLE_DIS_PNP_ID_VENDOR_SOURCE_BLUETOOTH_SIG                 0x01
#define M2MB_BLE_DIS_PNP_ID_VENDOR_SOURCE_USB_IMPLEMENTORS_FORUM        0x02

/* Global typedefs ==============================================================================*/

/*!
	@brief BLE 16 bit UUID 

	@details used to store BLE 16 bit UUID bytes
*/

typedef struct M2MB_BLE_UUID_16_S
{
    UINT8 UUID_Byte0;
    UINT8 UUID_Byte1;
} M2MB_BLE_UUID_16_T;

/*!
	@brief BLE 32 bit UUID 

	@details used to store BLE 32 bit UUID bytes
*/

typedef struct M2MB_BLE_UUID_32_S
{
    UINT8 UUID_Byte0;
    UINT8 UUID_Byte1;
    UINT8 UUID_Byte2;
    UINT8 UUID_Byte3;
} M2MB_BLE_UUID_32_T;

/*!
	@brief BLE 128 bit UUID 

	@details used to store BLE 128 bit UUID bytes
*/

typedef struct M2MB_BLE_UUID_128_S
{
    UINT8 UUID_Byte0;
    UINT8 UUID_Byte1;
    UINT8 UUID_Byte2;
    UINT8 UUID_Byte3;
    UINT8 UUID_Byte4;
    UINT8 UUID_Byte5;
    UINT8 UUID_Byte6;
    UINT8 UUID_Byte7;
    UINT8 UUID_Byte8;
    UINT8 UUID_Byte9;
    UINT8 UUID_Byte10;
    UINT8 UUID_Byte11;
    UINT8 UUID_Byte12;
    UINT8 UUID_Byte13;
    UINT8 UUID_Byte14;
    UINT8 UUID_Byte15;
} M2MB_BLE_UUID_128_T;

/*!
	@brief BLE UUID 

	@details Enumeration that indicates BLE 16,32,128 bit UUID's
*/

typedef enum
{
    UUID_TYPE_16_BIT_E, /**< Indicates 16 bit UUID.  */
    UUID_TYPE_32_BIT_E, /**< Indicates 32 bit UUID.  */
    UUID_TYPE_128_BIT_E, /**< Indicates 128 bit UUID.  */
    UUID_INVALID_E,
} M2MB_BLE_UUID_TYPE_T;

/*!
	@brief BLE UUID 

	@details Enumeration that indicates BLE UUID type,16,32,128 bit
*/

typedef struct M2MB_BLE_UUID_S
{
    M2MB_BLE_UUID_TYPE_T uuid_type; /**< Indicates uuid type.  */
    union
    {
        M2MB_BLE_UUID_16_T uuid_16; /**< Indicates 16 bit UUID.  */
        M2MB_BLE_UUID_32_T uuid_32; /**< Indicates 32 bit UUID.  */
        M2MB_BLE_UUID_128_T uuid_128; /**< Indicates 128 bit UUID.  */
    };
} M2MB_BLE_UUID_T;


/*!
	@brief BLE Advertise event

	@details Enumeration that indicates BLE Advertise event types
*/

/**< Enumeration of the BLE Advertise event types*/
typedef enum
{
    M2MB_BLE_ADV_EVT_TYPE_UNDIRECTED = 0,    /**<  Connectable  undirected advertising. */
    M2MB_BLE_ADV_EVT_TYPE_DIRECTED   = 1,    /**<  Connectable directed advertising. */
    M2MB_BLE_ADV_EVT_TYPE_SCANNABLE  = 2,    /**<  Scannable undirected advertising. */
    M2MB_BLE_ADV_EVT_TYPE_NON_CONNECTABLE,   /**<  Nonconnectable undirected advertising. */
    M2MB_BLE_ADV_EVT_TYPE_SCAN_RSP           /**<  scan response. */
} M2MB_BLE_ADVERTISE_EVENT_TYPE_T;

/*!
	@brief BLE connectability modes

	@details Enumeration that indicates BLE connectability modes
*/

/**< Enumeration of the BLE connectability modes*/
typedef enum
{
    M2MB_BLE_NON_CONNECTABLE_E, /**< non-connectable mode.  */
    M2MB_BLE_CONNECTABLE_E, /**< connectable mode.  */
    M2MB_BLE_DIRECT_CONNECTABLE_E, /**< directed connectable mode.  */
} M2MB_BLE_CONNECTABILITY_MODE_T;

/*!
	@brief BLE discoverability modes

	@details Enumeration that indicates BLE discoverability modes
*/

/**< Enumeration of the BLE discoverability modes*/
typedef enum
{
    M2MB_BLE_NON_DISCOVERABLE_MODE_E, /**< Non-discoverable mode. */
    M2MB_BLE_LIMITED_DISCOVERABLE_MODE_E, /**< Limited-discoverable mode. */
    M2MB_BLE_GENERAL_DISCOVERABLE_MODE_E /**< General-discoverable mode. */
} M2MB_BLE_DISCOVERABILITY_MODE_T;

/*!
	@brief BLE pairability modes

	@details Enumeration that indicates BLE pairability modes
*/

/**< Enumeration of the BLE pairability modes*/
typedef enum
{
    M2MB_BLE_NON_PAIRABLEMODE_E, /**< Non-pairable mode.  */
    M2MB_BLE_PAIRABLE_MODE_E, /**< Pairable mode.  */
    M2MB_BLE_PAIRABLE_MODE_ENABLE_EXTENDED_EVENTS_E /**< Pairable mode with enabling of extended events.  */
} M2MB_BLE_PAIRABILITY_MODE_T;

/*!
	@brief BLE Role 

	@details Enumeration that indicates BLE roles
*/

typedef enum
{
    M2MB_BLE_PERIPHERAL_ROLE_E, /**< Peripheral role*/
    M2MB_BLE_CENTRAL_ROLE_E, /**< Central  role*/
    M2MB_BLE_DUAL_ROLE_E, /**< Both peripheral and central roles*/
} M2MB_BLE_ROLE_T;

/*!
	@brief BLE address types

	@details Enumeration that indicates BLE address types
*/

typedef enum
{
    /**< Indicates a public identity address.  */
    M2MB_BLE_PUBLIC_E,
    
    /**< Indicates a random identity address.  */
    M2MB_BLE_RANDOM_E,
    
    /**< Indicates the local use of a resolvable private address with the fall-back
        of using the devices public address.  */
    M2MB_BLE_RESOLVABLE_FALLBACK_PUBLIC_E,
    
    /**< Indicates the local use of a resolvable private address with the fall-back
        of using currently configured random address.  */
    M2MB_BLE_RESOLVABLE_FALLBACK_PRIVATE_E,
    
    /**< Indicates that address is that of anonymous device (address not present
        in advertising packet.      */
    M2MB_BLE_ANONYMOUS_E
} M2MB_BLE_ADDRESS_TYPE_T;


/*!
	@brief different IO
     Capabilities of an LE device

	@details enumerated type represents the different IO
     Capabilities of an LE device may have (used during pairing
     procedure).
*/

/**< The following enumerated type represents the different IO
     Capabilities of an LE device may have (used during pairing
     procedure).
*/
typedef enum
{
   /**< Indicates device can only display a value.  */
   M2MB_BLE_DISPLAY_ONLY_E,

   /**< Indicates device can only display a value and ask a yes/no confirmation.  */
   M2MB_BLE_DISPLAY_YES_NO_E,
   
   /**< Indicates device can only input a value.  */
   M2MB_BLE_KEYBOARD_ONLY_E,

   /**< Indicates device has no input and no output capability.  */
   M2MB_BLE_NO_INPUT_NO_OUTPUT_E,

   /**< Indicates device can both input and display a value.  */
   M2MB_BLE_KEYBOARD_DISPLAY_E
} M2MB_BLE_IO_CAPABILITY_T;

/*!
	@brief Bluetooth Device Address

	@details Ex: In MAC Addr = 0xAABBCCDDEEFF, then BD_ADDR0 = 0xAA and BD_ADDR5 = 0xFF 
*/
/** Bluetooth Device Address */
/** Ex: In MAC Addr = 0xAABBCCDDEEFF, then BD_ADDR0 = 0xAA and BD_ADDR5 = 0xFF */
typedef struct  M2MB_BLE_BD_ADDR_S
{
    UINT8 BD_ADDR0;
    UINT8 BD_ADDR1;
    UINT8 BD_ADDR2;
    UINT8 BD_ADDR3;
    UINT8 BD_ADDR4;
    UINT8 BD_ADDR5;
} M2MB_BLE_BD_ADDR_T;

/*!
	@brief BLE board address in string format

	@details Use to hold MAX address in Hex format. Ex: 0xAABBCCDDEEFF
*/

/* Use to hold MAX address in Hex format. Ex: 0xAABBCCDDEEFF*/
typedef INT8 M2MB_BLE_BOARDSTR_T[16];

/*!
	@brief Bluetooth Device Name

	@details Bluetooth Device Name within M2MB_BLE_MAX_DEVICE_NAME_LENGTH
*/

/** Bluetooth Device Name */
typedef struct M2MB_BLE_BD_NAME_S
{
    UINT8 bd_name[M2MB_BLE_MAX_DEVICE_NAME_LENGTH];/**< Bluetooth Device name */
} M2MB_BLE_BD_NAME_T;

/*!
	@brief BLE CCCD

	@details Enumeration that indicates BLE CCCD values
*/

typedef enum
{
    M2MB_BLE_DISABLE_INDICATION_NOTIFICATION_E, /**< Indicates disable indication and notification.  */
    M2MB_BLE_ENABLE_NOTIFICATION_E, /**< Indicates enable notification.  */
    M2MB_BLE_ENABLE_INDICATION_E, /**< Indicates enable indication.  */
} M2MB_BLE_CCCD_T;


/***************************************************************/
/*                  BLE Configuration / BLE Profile            */
/***************************************************************/

/*!
	@brief BLE configuration parameters

	@details Enumeration that indicates BLE configuration parameters like adv set, scan duration set, etc,.
*/

/* BLE configuration parameters */
typedef enum
{
	M2MB_BLE_CFG_GET_E, /**< Indicates BLE configuration get.  */
	M2MB_BLE_CFG_SET_E, /**< Indicates BLE configuration set.  */

    M2MB_BLE_CFG_SCAN_DURATION_GET_E,		/**< Indicates BLE scan duration get.  */
    M2MB_BLE_CFG_SCAN_DURATION_SET_E,		/**< Indicates BLE scan duration set.  */
    M2MB_BLE_CFG_SCAN_INTERVAL_GET_E,		/**< Indicates BLE scan interval get.  */
    M2MB_BLE_CFG_SCAN_INTERVAL_SET_E,		/**< Indicates BLE scan interval set.  */
    M2MB_BLE_CFG_SCAN_WINDOW_GET_E,			/**< Indicates BLE scan window get.  */
    M2MB_BLE_CFG_SCAN_WINDOW_SET_E,			/**< Indicates BLE scan window set.  */
    M2MB_BLE_CFG_SCAN_FILTER_POLICY_GET_E,	/**< Indicates BLE scan filter policy get.  */
    M2MB_BLE_CFG_SCAN_FILTER_POLICY_SET_E,	/**< Indicates BLE scan filter policy set.  */
    M2MB_BLE_CFG_SCAN_FILTER_DUPLICATES_GET_E, /**< Indicates BLE scan filter duplicates get.  */
    M2MB_BLE_CFG_SCAN_FILTER_DUPLICATES_SET_E, /**< Indicates BLE scan filter duplicates set.  */
    
    M2MB_BLE_CFG_ADV_INT_MIN_GET_E,			/**< Indicates BLE advertise interval minimum get.  */
    M2MB_BLE_CFG_ADV_INT_MIN_SET_E,			/**< Indicates BLE advertise interval minimum set.  */
    M2MB_BLE_CFG_ADV_INT_MAX_GET_E,			/**< Indicates BLE advertise interval maximum get.  */
    M2MB_BLE_CFG_ADV_INT_MAX_SET_E,			/**< Indicates BLE advertise interval maximum set.  */
    M2MB_BLE_CFG_ADV_DATA_GET_E,			/**< Indicates BLE advertise data get.  */
    M2MB_BLE_CFG_ADV_DATA_SET_E,			/**< Indicates BLE advertise data set.  */
    M2MB_BLE_CFG_SCAN_RSP_DATA_GET_E,		/**< Indicates BLE scan response data get.  */
    M2MB_BLE_CFG_SCAN_RSP_DATA_SET_E,		/**< Indicates BLE scan response data set.  */
    M2MB_BLE_CFG_CONNECT_MODE_GET_E,		/**< Indicates BLE connect mode get.  */
    M2MB_BLE_CFG_CONNECT_MODE_SET_E,		/**< Indicates BLE connect mode set.  */
    M2MB_BLE_CFG_DISCOVER_MODE_GET_E,		/**< Indicates BLE discovery mode get.  */
    M2MB_BLE_CFG_DISCOVER_MODE_SET_E,		/**< Indicates BLE discovery mode set.  */
    M2MB_BLE_CFG_PAIR_MODE_GET_E,			/**< Indicates BLE pair mode get.  */
    M2MB_BLE_CFG_PAIR_MODE_SET_E,			/**< Indicates BLE paor mode set.  */

    M2MB_BLE_CFG_CONNECTION_INT_MIN_GET_E,	/**< Indicates BLE connection interval minimum get.  */
    M2MB_BLE_CFG_CONNECTION_INT_MIN_SET_E,	/**< Indicates BLE connection interval minimum set.  */
    M2MB_BLE_CFG_CONNECTION_INT_MAX_GET_E,	/**< Indicates BLE connection interval maximum get.  */
    M2MB_BLE_CFG_CONNECTION_INT_MAX_SET_E,	/**< Indicates BLE connection interval maximum set.  */
    M2MB_BLE_CFG_SLAVE_LATENCY_GET_E,		/**< Indicates BLE slave latency get.  */
    M2MB_BLE_CFG_SLAVE_LATENCY_SET_E,		/**< Indicates BLE slave latency set.  */
    M2MB_BLE_CFG_SUPERVISION_TIMEOUT_GET_E,	/**< Indicates BLE supervision timeout get.  */
    M2MB_BLE_CFG_SUPERVISION_TIMEOUT_SET_E,	/**< Indicates BLE supervision timeout set.  */

    M2MB_BLE_CFG_IOCAPABILITY_GET_E,		/**< Indicates BLE IO capability timeout get.  */
    M2MB_BLE_CFG_IOCAPABILITY_SET_E,		/**< Indicates BLE IO capability timeout set.  */
    M2MB_BLE_CFG_MITM_GET_E,				/**< Indicates BLE MITM get.  */
    M2MB_BLE_CFG_MITM_SET_E,				/**< Indicates BLE MITM set.  */
    M2MB_BLE_CFG_FIXPIN_GET_E,				/**< Indicates BLE FIXPIN get.  */
    M2MB_BLE_CFG_FIXPIN_SET_E,				/**< Indicates BLE FIXPIN set.  */
    M2MB_BLE_CFG_LEPRIVACY_GET_E,			/**< Indicates BLE LEPRIVACY get.  */
    M2MB_BLE_CFG_LEPRIVACY_SET_E,			/**< Indicates BLE LEPRIVACY set.  */
    M2MB_BLE_CFG_BOND_STORE_SUPPORT_GET_E,	/**< Indicates BLE bond store support get.  */
    M2MB_BLE_CFG_BOND_STORE_SUPPORT_SET_E,	/**< Indicates BLE bond store support set.  */
    M2MB_BLE_CFG_BOND_SIZE_GET_E,			/**< Indicates BLE bond size get.  */
    M2MB_BLE_CFG_BOND_SIZE_SET_E,			/**< Indicates BLE bond size set.  */

    M2MB_BLE_CFG_PNP_VERSION_ID_GET_E,		/**< Indicates BLE PNP version id get.  */
    M2MB_BLE_CFG_PNP_VERSION_ID_SET_E,		/**< Indicates BLE PNP version id set.  */
    M2MB_BLE_CFG_PNP_VERSION_ID_SOURCE_GET_E,/**< Indicates BLE PNP version id source get.  */
    M2MB_BLE_CFG_PNP_VERSION_ID_SOURCE_SET_E,/**< Indicates BLE PNP version id source set.  */
    M2MB_BLE_CFG_PNP_PRODUCT_ID_GET_E,		/**< Indicates BLE PNP product id get.  */
    M2MB_BLE_CFG_PNP_PRODUCT_ID_SET_E,		/**< Indicates BLE PNP product id set.  */
    M2MB_BLE_CFG_PNP_PRODUCT_VERSION_GET_E,	/**< Indicates BLE PNP product version get.  */
    M2MB_BLE_CFG_PNP_PRODUCT_VERSION_SET_E,	/**< Indicates BLE PNP product version set.  */

    M2MB_BLE_CFG_BLE_ROLE_GET_E,			/**< Indicates BLE role get.  */
    M2MB_BLE_CFG_BLE_ROLE_SET_E,			/**< Indicates BLE role set.  */
    M2MB_BLE_CFG_BLE_BD_ADDR_GET_E,			/**< Indicates BLE device address get.  */
    M2MB_BLE_CFG_BLE_DEVICE_NAME_GET_E,		/**< Indicates BLE device name get.  */
    M2MB_BLE_CFG_BLE_DEVICE_NAME_SET_E,		/**< Indicates BLE device name set.  */
    M2MB_BLE_CFG_BLE_MANUFACTURER_NAME_GET_E,/**< Indicates BLE device manufacturer name get.  */
    M2MB_BLE_CFG_BLE_MANUFACTURER_NAME_SET_E,/**< Indicates BLE device manufacturer name set.  */
    M2MB_BLE_CFG_BLE_TIO_MODE_GET_E,		/**< Indicates BLE TIO mode get.  */
    M2MB_BLE_CFG_BLE_TIO_MODE_SET_E,		/**< Indicates BLE TIO mode set.  */
} M2MB_BLE_CONFIG_PARAM_T;

/*!
	@brief BLE Central configuration parameters

	@details Enumeration that indicates BLE Central configuration parameters
*/

typedef struct M2MB_BLE_CENTRAL_CFG_S
{
    UINT16 scan_duration; /**< scan duration */
    UINT16 scan_interval; /**< scan interval */
    UINT16 scan_window; /**< scan window */
}M2MB_BLE_CENTRAL_CFG_T;

/*!
	@brief BLE Peripheral configuration parameters

	@details Enumeration that indicates BLE Peripheral configuration parameters
*/

typedef struct M2MB_BLE_PERIPHERAL_CFG_S
{
    UINT16 advertise_interval_max; /**< advertise interval max */
    UINT16 advertise_interval_min; /**< advertise interval min */
    UINT8 advertise_data[M2MB_BLE_ADV_DATA_SIZE+1]; /**< advertise data */
    UINT8 scan_response_data[M2MB_BLE_SCAN_RSP_DATA_SIZE+1]; /**< scan response data */
    
    M2MB_BLE_CONNECTABILITY_MODE_T connect_mode; /**< Connectability mode*/
    M2MB_BLE_DISCOVERABILITY_MODE_T discover_mode; /**< Discoverability mode*/
    M2MB_BLE_PAIRABILITY_MODE_T pair_mode; /**< pairability mode*/
} M2MB_BLE_PERIPHERAL_CFG_T;

/*!
	@brief BLE connection configuration parameters

	@details Enumeration that indicates BLE connection configuration parameters
*/

typedef struct M2MB_BLE_CONN_PARAM_S
{
    UINT16 connection_interval_max; /**< connection interval maximum */
    UINT16 connection_interval_min; /**< connection interval minimum */
    UINT16 slave_latency; /**< slave latency */
    UINT16 supervision_timeout; /**< Supervision timeout */
} M2MB_BLE_CONN_PARAM_T;

/*!
	@brief BLE security configuration parameters

	@details Enumeration that indicates BLE security configuration parameters
*/

typedef struct M2MB_BLE_SECURITY_CFG_S
{
    M2MB_BLE_IO_CAPABILITY_T iocapability; /**< io capabilities */
    UINT8 mitm; /**< mitm protection */
    UINT8 fix_pin[M2MB_BLE_FIXPIN_SIZE+1]; /**< fix pin */
    UINT8 leprivacy; /**< le privacy support*/
    UINT8 ble_bond_store_support; /**< bond store support*/
    UINT8 ble_bond_size; /**< max number of bonds*/
}M2MB_BLE_SECURITY_CFG_T;

/*!
	@brief BLE DIS configuration parameters

	@details Enumeration that indicates BLE DIS configuration parameters
*/

typedef struct M2MB_BLE_DIS_INFO_S
{
    UINT16 pnpid_vendor_id; /**< PnP Vendor ID */
    UINT16 pnpid_vendor_id_source; /**< PnP Vendor ID Source */
    UINT16 pnpid_product_id; /**< PnP product ID */
    UINT16 pnpid_product_version; /**< PnP product version */
}M2MB_BLE_DIS_INFO_T;

/*!
	@brief BLE general configuration parameters

	@details Enumeration that indicates BLE general configuration parameters
*/

typedef struct M2MB_BLE_GENERAL_PARAM_S
{
    M2MB_BLE_ROLE_T ble_role; /**< ble role */
    M2MB_BLE_BD_ADDR_T ble_bd_address; /**< ble MAC address */
    UINT16 ble_device_appearance; /**< ble device appearance*/
    INT8 ble_device_name[M2MB_BLE_MAX_DEVICE_NAME_LENGTH+1]; /**< device name */
    INT8 ble_manufacturer_name[M2MB_BLE_MAX_MANUFACTURER_NAME_LENGTH+1]; /**< manufacturer name */
    UINT8 ble_tio_mode; /**< TIO mode */
} M2MB_BLE_GENERAL_PARAM_T;

/*!
	@brief BLE configuration parameters

	@details Enumeration that indicates BLE configuration parameters
*/

typedef struct M2MB_BLE_CGF_S
{
    M2MB_BLE_CENTRAL_CFG_T ble_central_cfg; /**< Indicates BLE central configuration.  */
    M2MB_BLE_PERIPHERAL_CFG_T ble_peripheral_cfg; /**< Indicates BLE peripheral configuration.  */
    M2MB_BLE_CONN_PARAM_T ble_conn_param; /**< Indicates BLE connection configuration.  */
    M2MB_BLE_SECURITY_CFG_T ble_security_cfg; /**< Indicates BLE security configuration.  */
    M2MB_BLE_DIS_INFO_T ble_dis_param; /**< Indicates BLE DIS configuration.  */
    M2MB_BLE_GENERAL_PARAM_T ble_gen_param; /**< Indicates BLE general configuration.  */
} M2MB_BLE_CGF_T;


/********************************************************************************************************/
/* BLE configuration related */
/********************************************************************************************************/

/*!
	@brief BLE status
	
	@details Enumeration that indicates status of BLE operations
*/

typedef enum
{
    M2MB_BLE_STATUS_SUCCESS_E,						/**< Indicates BLE status success.  */
    M2MB_BLE_STATUS_FAILURE_E,						/**< Indicates BLE status failure.  */
    M2MB_BLE_STATUS_OUT_OF_RANGE_E,					/**< Indicates BLE status out of range.  */
    M2MB_BLE_STATUS_NOT_READY_E,					/**< Indicates BLE status not ready.  */
    M2MB_BLE_STATUS_UNSUPPORTED_E,					/**< Indicates BLE status unsupported.  */
    M2MB_BLE_STATUS_PARAM_INVALID_E,				/**< Indicates BLE status invalid parameters.  */
    M2MB_BLE_STATUS_NO_MEM_E,						/**< Indicates BLE status no memory.  */
    M2MB_BLE_STATUS_AUTH_FAILURE_E,					/**< Indicates BLE status authentication failure.  */
    M2MB_BLE_STATUS_AUTH_REJECTED_E,				/**< Indicates BLE status authentication rejected.  */
    M2MB_BLE_STATUS_INVALID_E,						/**< Indicates BLE status Invalid.  */

    M2MB_BLE_STATUS_ERR_INVALID_HANDLE_E,			/**< Indicates BLE status invalid handle error.  */
    M2MB_BLE_STATUS_ERR_READ_NOT_PERMITTED_E,		/**< Indicates BLE status read not permitted error.  */
    M2MB_BLE_STATUS_ERR_WRITE_NOT_PERMITTED_E,		/**< Indicates BLE status write not permitted error.  */
    M2MB_BLE_STATUS_ERR_INVALID_PDU_E,				/**< Indicates BLE status invalid PDU error.  */
    M2MB_BLE_STATUS_ERR_REQUEST_NOT_SUPPORTED_E,	/**< Indicates BLE status request not supported error.  */
    M2MB_BLE_STATUS_ERR_INVALID_OFFSET_E,			/**< Indicates BLE status invalid OFFSET error.  */
    M2MB_BLE_STATUS_ERR_ATTRIBUTE_NOT_FOUND_E,		/**< Indicates BLE status attribute not found error.  */
    M2MB_BLE_STATUS_ERR_INVALID_ATTRIBUTE_VALUE_LENGTH_E,/**< Indicates BLE status invalid attribute value length error.  */
    M2MB_BLE_STATUS_ERR_OUT_OF_ORDER_E,				/**< Indicates BLE status out of order error.  */

    M2MB_BLE_STATUS_ERR_INSUFFICIENT_AUTHORIZATION_E,/**< Indicates BLE status insufficient authorization error.  */
    M2MB_BLE_STATUS_ERR_INSUFFICIENT_AUTHENTICATION_E,/**< Indicates BLE status insufficient authentication error.  */
    M2MB_BLE_STATUS_ERR_INSUFFICIENT_ENCRYPTION_E,	/**< Indicates BLE status insufficient encryption error.  */
} M2MB_BLE_RESPONSE_T;

/*!
	@brief BLE EVENTS to the user
	
	@details Enumeration that indicates BLE EVENTS to the user / application layer from M2MB to registered callback
*/

/* BLE EVENTS to the user / application layer from M2MB to registered callback */
typedef enum
{
    M2MB_BLE_PERIPHERAL_CONNECT_EVENT_E,				/**< Indicates BLE peripheral connect event.  */
    M2MB_BLE_PERIPHERAL_DISCONNECT_EVENT_E,				/**< Indicates BLE peripheral disconnect event.  */
    M2MB_BLE_PERIPHERAL_MTU_UPDATE_EVENT_E,				/**< Indicates BLE peripheral MTU update event.  */

    M2MB_BLE_GATT_SERVER_READ_REQUEST_EVENT_E,			/**< Indicates BLE GATT server read request event.  */
    M2MB_BLE_GATT_SERVER_WRITE_REQUEST_EVENT_E,			/**< Indicates BLE GATT server write request event.  */
    M2MB_BLE_GATT_SERVER_WRITE_WITHOUT_RESPONSE_EVENT_E,/**< Indicates BLE GATT server write response request event.  */
    M2MB_BLE_GATT_SERVER_NOTIFICATION_ENABLE_EVENT_E,	/**< Indicates BLE GATT server notification enable event.  */
    M2MB_BLE_GATT_SERVER_INDICATION_ENABLE_EVENT_E,		/**< Indicates BLE GATT server indication enable event.  */
    M2MB_BLE_GATT_SERVER_NOTIFY_INDICATE_ENABLE_EVENT_E,/**< Indicates BLE GATT server notification indication enable event.  */
    M2MB_BLE_GATT_SERVER_NOTIFY_INDICATE_DISABLE_EVENT_E,/**< Indicates BLE GATT server notification indication disable event.  */
	M2MB_BLE_GATT_SERVER_NOTIFY_INDICATE_READ_EVENT_E,	/**< Indicates BLE GATT server notification indication read event.  */
    M2MB_BLE_GATT_SERVER_CHAR_VALUE_UPDATED_EVENT_E, 	/**< Indicates BLE GATT server characteristic value update event.  *//* This is covered by write events */
    
    M2MB_BLE_SCAN_EVENT_E,								/**< Indicates BLE scan event.  */						
    M2MB_BLE_CENTRAL_CONNECT_EVENT_E,					/**< Indicates BLE central connect event.  */						
    M2MB_BLE_CENTRAL_DISCONNECT_EVENT_E,				/**< Indicates BLE central disconnect event.  */						
    M2MB_BLE_CENTRAL_MTU_UPDATE_EVENT_E,				/**< Indicates BLE central MTU update event.  */						
    M2MB_BLE_CENTRAL_NOTIFY_INDICATE_EVENT_E,			/**< Indicates BLE central notify indicate event.  */						
    M2MB_BLE_CENTRAL_WRITE_EVENT_E,						/**< Indicates BLE central write event.  */						
    M2MB_BLE_CENTRAL_READ_EVENT_E,						/**< Indicates BLE central read event.  */						
    
    M2MB_BLE_CONNECT_PARAM_UPDATE_EVENT_E,				/**< Indicates BLE connection parameter update event.  */						

    M2MB_BLE_SSP_PIN_NOTIFICATION_EVENT_E,				/**< Indicates BLE SSP pin notification event.  */						
    M2MB_BLE_SSP_PIN_EVENT_E,							/**< Indicates BLE SSP pin event.  */						
    M2MB_BLE_SSP_CONF_EVENT_E,							/**< Indicates BLE SSP conformation event.  */						
    M2MB_BLE_PROVISIONING_SCAN_EVENT_E,					/**< Indicates BLE provisioning scan event.  */						
    M2MB_BLE_PROVISIONING_CONNECT_EVENT_E,				/**< Indicates BLE provisioning connect event.  */						
} M2MB_BLE_USER_EVENTS_T;

/*!
	@brief BLE scan report data
	
	@details When scan for BLE devices, send the list of scanned devices information in this format to the application
*/

/* When scan for BLE devices, send the list of scanned devices information in this format to the application */
/* 
** [TODO]: How to handle device name and advertise data at m2mb and app layer. Who allocates this memory and who frees this memory.
** Better solution is, in the register callback at app layer, memory is allocated. The scan event is posted over the message queue.
** Worker thread at application layer on the scan event over the queue handles / prints scan data and free the memory.
*/
typedef struct M2MB_BLE_SCAN_REPORT_DATA_S
{
    M2MB_BLE_BD_ADDR_T              BD_ADDR; /**< Indicates BLE address.  */
    INT8                            RSSI; /**< Indicates BLE RSSI value.  */
    INT8                            device_name[M2MB_BLE_MAX_DEVICE_NAME_LENGTH+1]; /**< Indicates BLE device name.  */
    M2MB_BLE_ADDRESS_TYPE_T         bd_addr_type; /**< Indicates BLE address type.  */
    M2MB_BLE_ADVERTISE_EVENT_TYPE_T bd_adv_type;  /**< Indicates BLE advertise type.  */                   
    UINT8                           datalen; /**< Indicates data length.  */                   
    UINT8                           data[M2MB_BLE_ADV_DATA_SIZE]; /**< Indicates data.  */
} M2MB_BLE_SCAN_REPORT_DATA_T;

#if 0
/* Connection param updates are specific to particular connection */
typedef struct M2MB_BLE_CONN_PARAM_UPADTE_S
{
    UINT8 connection_id;
    M2MB_BLE_CONN_PARAM_T connection_params;
} M2MB_BLE_CONN_PARAM_UPADTE_T;


/* Connection param updates are specific to particular connection */
typedef struct M2MB_BLE_MTU_UPADTE_S
{
    UINT8 connection_id;
    UINT32 mtu;
} M2MB_BLE_MTU_UPADTE_T;
#endif
/*!
	@brief BLE notification and indication data
	
	@details structure to hold notification and indication data from remote GATT server
*/

/* structure to hold notification and indication data from remote GATT server */
typedef struct M2MB_BLE_CENTRAL_NOTIFY_INDICATE_DATA_S
{
    UINT8 connection_id; /**< Indicates BLE connection id.  */
    UINT8 notify_ind_flag; /**< For Notification, notify_ind_flag = 1. For Indication, notify_ind_flag = 0 */
    UINT16 handle; /**< characteristic handle */
    UINT16 value_size; /**<Indicates value size */
    UINT8 p_value[M2MB_BLE_MAX_ATTRIBUTE_LENGTH]; /**<Indicates p value */
} M2MB_BLE_CENTRAL_NOTIFY_INDICATE_DATA_T;

/*!
	@brief Scan results duplicate filtering
	
	@details Enumeration that indicates Scan results duplicate filtering
*/

typedef enum
{
    M2MB_BLE_SCAN_FILTER_DUPLICATE_DISABLE, /**<  Duplicate filtering disabled. */
    M2MB_BLE_SCAN_FILTER_DUPLICATE_ENABLE,  /**<  Duplicate filtering enabled. */
} M2MB_BLE_CENTRAL_SCAN_FILTER_DUPLICATES_T;

/*!
	@brief BLE GATT server event details
	
	@details maintain BLE GATT server event details like service id, value, etc,.
*/

typedef struct M2MB_BLE_GATT_SERVER_EVENT_DETAILS_S
{
	M2MB_BLE_RESPONSE_T *result_flag; /**< Tells whether the read request or write request is success or failure from app layer to m2mb layer */
	UINT32 service_id; /**<Indicates service id */
	UINT32 char_id; /**< Equivalent to attribute index */
	UINT32 char_cccd_id; /**< can be used only when notification / indication is enabled or disabled from Central */
	UINT8 **value; /**< Data value. Double pointer because in case of read request data is filled in the pointer sent from m2mb layer */
	UINT16 *value_length; /* value length. Pointer because, in case of read request value length should be filled and sent to m2mb layer */
	UINT8 cccd_val; /**< 0x10 = Notification Enable, 0x20 = Indication Enable, 0x30 = Notification and Indication Enable , 0x00 = Notification and Indication disable */
	UINT8	 more_data; /**<Indicates more data */
	UINT32 offset; /**<Indicates offset */
} M2MB_BLE_GATT_SERVER_EVENT_DETAILS_T;

/*!
	@brief BLE SSP request
	
	@details maintain BLE SSP request details
*/

typedef struct
{
	M2MB_BLE_BD_ADDR_T remote_bd_addr; /**<Indicates remote bd address */
	UINT32 pass_key; /**<Indicates pass key */
	UINT8 accept_flag; /**<Indicates accept flag */
} M2MB_BLE_SSP_REQUEST_CTX_T;

/*!
	@brief BLE PIN request
	
	@details maintain BLE PIN request details
*/

typedef struct
{
	M2MB_BLE_BD_ADDR_T remote_bd_addr; /**<Indicates remote bd address */
	INT8 pincode[7]; /**< to hold 6 digits SSPPIN */
	UINT8 pin_size; /**<Indicates pin size */
	UINT8 accept_flag; /**<Indicates accept flag */
} M2MB_BLE_PIN_REQUEST_CTX_T;

/*!
	@brief BLE client write response
	
	@details maintain client write response details
*/

typedef struct
{
	INT32 status; /**<Indicates write status */
	INT32 attribute_handle; /**<Indicates attribute handle */
} M2MB_BLE_CLIENT_WRITE_RESP_CTX_T;

/*!
	@brief BLE client read response
	
	@details maintain client read response details
*/

typedef struct
{
  INT32 status; /**<Indicates read status */
  INT32 value_length; /**<Indicates read value length */
  INT8 *value; /**<Indicates read value */
	INT32 attribute_handle;
} M2MB_BLE_CLIENT_READ_RESP_CTX_T;

/*!
	@brief BLE user event
	
	@details This BLE user event context is sent to registered callback with user event type
*/

/* This user context is sent to registered callback with user event type */
typedef struct M2MB_BLE_USER_EVENT_CTX_S
{
    UINT32 peripheral_connect_id; /**< Used for peripheral connect and disconnect event */
    UINT32 central_connect_id; /**< Used for central connect and disconnect event */
    UINT32 mtu; /**< Hold MTU value of both central and peripheral, MTU based on central / peripheral MTU event */
	/*!
		@brief BLE user event data
	
		@details contains BLE user event data
	*/
    union M2MB_BLE_EVENT_DATA_U
    {
        M2MB_BLE_GATT_SERVER_EVENT_DETAILS_T gatt_attrib; /**<Indicates GATT server event attribute details */
        M2MB_BLE_SCAN_REPORT_DATA_T scan_data; /**<Indicates scan data */
        M2MB_BLE_CONN_PARAM_T connection_params; /**< This is for the connection identified by peripheral / central connection ID */
				M2MB_BLE_CENTRAL_NOTIFY_INDICATE_DATA_T notify_data; /**<Indicates notification data */
        M2MB_BLE_SSP_REQUEST_CTX_T ssp_conf; /**< used with SSPCONF event */
        M2MB_BLE_PIN_REQUEST_CTX_T ssp_pin; /**< used with SSPPIN event */
				M2MB_BLE_CLIENT_WRITE_RESP_CTX_T central_write; /**< Issued when write with security completes*/
				M2MB_BLE_CLIENT_READ_RESP_CTX_T central_read; /**< Issued when read with security completes*/
    }  event_data;
} M2MB_BLE_USER_EVENT_CTX_T;




/*!
	@brief BLE device information
	
	@details 
    	Device Info structure is maintained for handling the device information of connected peer devices (both in Central and Peripheral mode). So, this device info handling API's should be protected with a Mutex
    	so that the device info will not be corrupted in case of simultaneous events that handles the device info part. 
    	Ex: In central mode simultaneous operations are possible like, One device got connected and other device got disconnected at the same time.
    
    	BLE Device info will hold all the information related to connected peer device.
    	BLE Device info can be implemented as either array or linked list.
    
    	Better to go with array with array size as (1 peripheral connection + max number of simultaneous  connection supported by central).
    	On successful connection the device info should be filled in the array, and on disconnect the device info should be deleted.
    
    	In peripheral mode, when Central connects to the BLE device, few stacks do not provide connect ID. In this case try to have some special connection ID defined for peripheral connection.
*/

/***************************************************************/
/*                      BLE Device Info                        */
/***************************************************************/
/*
  Device Info structure is maintained for handling the device information of connected peer devices (both in Central and Peripheral mode). So, this device info handling API's should be protected with a Mutex
  so that the device info will not be corrupted in case of simultaneous events that handles the device info part. 
  Ex: In central mode simultaneos operations are possible like, One device got connected and other device got disconnected at the same time.

  BLE Device info will hold all the information related to connected peer device.
  BLE Device info can be implemented as either array or linked list.

  Better to go with array with array size as (1 peripheral connection + max number of simultaneous connection supported by central).
  On successful connection the device info should be filled in the array, and on disconnect the device info should be deleted.
  
  In peripheral mode, when Central connects to the BLE device, few stacks do not provide connect ID. In this case try to have some special connection ID defined for peripheral connection.
  
*/

typedef struct M2MB_BLE_DEVICE_INFO_S
{
    BOOLEAN                     used_flag; /**< Represents that this device info entry is used or not */
    UINT32                      connection_id; /**< Connection ID on connect */
    BOOLEAN                     remote_device_is_master; /**< Indicates remote device is master or not */
    M2MB_BLE_BD_ADDR_T          remote_address; /**< Indicates remote device address */
    M2MB_BLE_ADDRESS_TYPE_T     remote_address_type; /**< Indicates remote device type */
    M2MB_BLE_BD_ADDR_T          identity_address_bdaddr; /**< Used for LE privacy */
    M2MB_BLE_ADDRESS_TYPE_T     identity_address_type; /**< Used for LE privacy */
    UINT32                      mtu_size; /**< MTU size should be handled per connection */
    M2MB_BLE_CONN_PARAM_T       conn_params; /**< Indicates connection parameters */
    UINT8                       congested_flag; /**< Used to notify congestion state of BLE stack */ 
} M2MB_BLE_DEVICE_INFO_T;


/***************************************************************/
/*                      M2MB BLE API's                         */
/***************************************************************/

/*!
	@brief BLE handle
	
	@details BLE handle for all contexts
*/

//typedef void   *HANDLE;
typedef HANDLE M2MB_BLE_HANDLE;

/*!
	@brief BLE bond list information
	
	@details maintains BLE bond list information
*/

typedef struct M2MB_BLE_BOND_LIST_INFO_S
{
    M2MB_BLE_BD_ADDR_T      remote_bd_address; /**< Indicates remote Bluetooth device address. */
    M2MB_BLE_ADDRESS_TYPE_T remote_bd_address_type; /**< Indicates remote Bluetooth device address type. */
    M2MB_BLE_ROLE_T         remote_bd_role;/**< Indicates remote Bluetooth device role. */
} M2MB_BLE_BOND_LIST_INFO_T;

/*!
	@brief BLE ADV type

	@details Enumeration that indicates BLE advertisement type
*/

typedef enum
{
    M2MB_BLE_ADVERTISE_ON_WITH_CUSTOMIZED_ADV_DISABLE, /**< Indicates BLE advertise on with customized advertisement disable. */
    M2MB_BLE_ADVERTISE_ON_WITH_CUSTOMIZED_ADV_ENABLE, /**< Indicates BLE advertise on with customized advertisement enable. */
    M2MB_BLE_ADVERTISE_OFF, /**< Indicates BLE advertise OFF. */
} M2MB_BLE_ADV_TYPE_T;

/*!
	@brief BLE INIT state

	@details Enumeration that indicates BLE INIT state
*/

typedef enum
{
    M2MB_BLE_STATE_INITIALIZED, /**< Indicates BLE state initialized. */
    M2MB_BLE_STATE_UNINITIALIZED, /**< Indicates BLE state uninitialized. */
} M2MB_BLE_INIT_STATE_T;

/*!
	@brief BLE GATT attribute type

	@details Enumeration that indicates BLE GATT attribute type
*/

typedef enum
{
    M2MB_BLE_GATT_PRIMARY_SERVICE_E, /**< Indicates GATT primary service. */
    M2MB_BLE_GATT_SECONDARY_SERVICE_E, /**< Indicates GATT secondary service. */
    M2MB_BLE_GATT_INCLUDED_SERVICE_E, /**< Indicates GATT included service. */
     M2MB_BLE_GATT_CHARACTERISTIC_E, /**< Indicates GATT characteristic. */
    M2MB_BLE_GATT_DESCRIPTOR_E, /**< Indicates GATT descriptor. */
} M2MB_BLE_GATT_ATTRIB_TYPE_T;

/*!
	@brief BLE GATT elements

	@details maintains BLE GATT elements
*/

/* used for service discovery feature */
typedef struct M2MB_BLE_GATT_ELEMENT_S
{
    M2MB_BLE_GATT_ATTRIB_TYPE_T type; /**< Indicates GATT attribute type. */
    M2MB_BLE_UUID_T uuid; /**< Indicates UUID. */
    UINT16 attrib_handle; /**< Indicates attribute handle. */
    UINT8 properties; /**< properties is used only when type = M2MB_BLE_GATT_CHARACTERISTIC_E */
    
    /* start and end hanldes are used only when type = M2MB_BLE_GATT_PRIMARY_SERVICE_E */
    UINT16 start_handle; /**< Indicates start handle. */
    UINT16 end_handle; /**< Indicates end handle. */
} M2MB_BLE_GATT_ELEMENT_T;

/*!
	@brief BLE service discovery feature

	@details used for service discovery feature
*/

/* used for service discovery feature */
typedef struct M2MB_BLE_GATT_SRVD_S
{
    UINT16 elements_count; /**< Indicates service discovery elements count. */
    M2MB_BLE_GATT_ELEMENT_T *gatt_elements; /**< Indicates service discovery GATT elements. */
} M2MB_BLE_GATT_SRVD_T;

/*!
	@brief BLE event callback

	@details register this callback during BLE init
*/

typedef void (*m2mb_ble_event_callback) (M2MB_BLE_HANDLE ble_handle, M2MB_BLE_USER_EVENTS_T ble_event, UINT16 resp_size, void *resp, void *userdata);

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
      Initialization of BLE

   @details
      Initializes BLE and gives handle for application to be used in all contexts and starts event callback

   @param[in] ble_callback
     BLE event callback function pointer. This function is called for all BLE related events
     
   @param[in] user_data
     A pointer to generic BLE user data that will be returned as it is in the callback

   @param[out] ble_handle
    Pointer to ble_handle. This is filled by ble_init API. This should be used in all M2MB BLE API's

   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note
    M2MB_BLE_HANDLE is valid until m2mb_ble_shutdown(M2MB_BLE_HANDLE ble_handle) is called

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_initialize(M2MB_BLE_HANDLE *ble_handle, m2mb_ble_event_callback ble_callback, void *user_data);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
      Getting BLE initialization state

   @details
      Gets BLE init state by passing handle

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @param[out] state
     filled with BLE state

   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_get_init_state(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_INIT_STATE_T *state);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
      Deinitialization of BLE

   @details
      Deinitialization of BLE by passing ble handle

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_shutdown(M2MB_BLE_HANDLE ble_handle);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     BLE configuration API

   @details
     This API will accept variable length arguments as part of its last parameter.
	 ble_handle:ble_handle from BLE init API
	 num_cmds: number of params follows after this param
	 __VA_ARGS__ (...):  This final parameter is variable argument list.
	 Make sure the first parameter in the variable arugument list is the Action on param (enum type to set or get ble param), followed by the "ble config param values".
	 "ble config param values" can be a pointer to structure which is defined based on m2mb_ble_conf enum type 
	 (or)
 	 ble config param values can be passed individually to the API.

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @param[in] num_cmds
	 specifies number of params follows after this param
	 
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_conf(M2MB_BLE_HANDLE ble_handle, UINT8 num_cmds, ...);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     passkey response

   @details
     This function is called whenever a callback has been registered
     for the specified passkey response data event.

   @param[in] ble_handle:
     specifies the BLE handle previously created through m2mb_ble_initialize
   
   @param[in] bd_addr
     remote device address
   
   @param[in] bd_addr_type
     remote device address type
   
   @param[in] ssppin
     SSP passkey
   
   @param[in] accept_flag
     flag states whether to pair or not (true or false). If SSPPIN is not received from the user within timeout, the AT command can call this m2mb API with accept_flag as false

   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_ssppin_reply(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_BD_ADDR_T bd_addr, M2MB_BLE_ADDRESS_TYPE_T bd_addr_type, INT32 ssppin, UINT8 accept_flag);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     passkey conformation

   @details
     This function conforms the passkey from user end

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] bd_addr
     remote device address
     
   @param[in] bd_addr_type
     remote device address type
     
   @param[in] confirmation_flag
     flag states whether to pair or not (true or false). If SSPPIN is not received from the user within timeout, the AT command can call this m2mb API with accept_flag as false

   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

    
M2MB_BLE_RESPONSE_T m2mb_ble_sspconf_reply(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_BD_ADDR_T bd_addr, M2MB_BLE_ADDRESS_TYPE_T bd_addr_type, UINT32 confirmation_flag);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Gets the bond information

   @details
     This function gives the bond list information
     
   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[out] bond_list_info
     Pointer to bond list info structure

   @param[out] num_of_entries
     number of bonded ble devices in the bond list
   
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_bond_list_get(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_BOND_LIST_INFO_T **bond_list_info, UINT32 *num_of_entries);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Deletes bond information

   @details
     This function deletes bond information for a praticular bd address or entire bond information

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] bd_addr
     remote device address   
   
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_bond_delete(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_BD_ADDR_T bd_addr);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Starts advertising

   @details
     This function starts advertisement based on advertisement type

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] adv_type
     Type of advertisement
      
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_advertise( M2MB_BLE_HANDLE ble_handle, M2MB_BLE_ADV_TYPE_T adv_type );

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Adds ble service
   @details
     create service with UUID provided by the user

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] service_uuid
     Service UUID   
   
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note
	 The GATT server configuration like,  the number of services supported and number of characteristics supported in each service
	 should be defined by the application based on the memory available in the respective platform.
   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_gatts_service_add(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_UUID_T service_uuid);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
      Adds characteristic information

   @details
     This function adds characteristic information like UUID, properties

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] properties
     Characteristic properties
   
   @param[in] characteristic_uuid
     Characteristic UUID
   
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note
     The GATT server configuration like,	the number of services supported and number of characteristics supported in each service
     should be defined by the application based on the memory available in the respective platform.
	 This function must call after m2mb_ble_gatts_service_add

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_gatts_characteristic_add(M2MB_BLE_HANDLE ble_handle, UINT8 properties, M2MB_BLE_UUID_T characteristic_uuid);
/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
      Adds characteristic value information

   @details
      This function adds characteristic value information such as permissions, value length, value

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] permissions
     Characteristic permissions
   
   @param[in] value_len
     Characteristic value length

   @param[in] value
     value in string format
   
   @param[out] characteristic_id
     Characteristic ID or Handle. This value is filled from the m2mb API and this value should be notified to the user
   
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note
     The GATT server configuration like,	the number of services supported and number of characteristics supported in each service
     should be defined by the application based on the memory available in the respective platform.
     This function must call after m2mb_ble_gatts_characteristic_add

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_gatts_characteristic_value_add(M2MB_BLE_HANDLE ble_handle, UINT16 permissions, UINT16 value_len, UINT8 *value, UINT32 *characteristic_id);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
      Register service to the BLE stack

   @details
      This function register services to the BLE stack

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
       
   @param[out] service_id
     This value is filled from the m2mb API and this value should be notified to the user
   
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note
     The GATT server configuration like,	the number of services supported and number of characteristics supported in each service
     should be defined by the application based on the memory available in the respective platform.
	 This function must call after m2mb_ble_gatts_service_add
   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_gatts_complete(M2MB_BLE_HANDLE ble_handle, UINT32 *service_id);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Send data to the server

   @details
     Send data to the characteristic ID in a particular service

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @param[in] serv_id
     Service ID
    
   @param[in] char_id
     Characteristic ID
     
   @param[in] value
     Characteristic value
     
   @param[in] value_len
     Characteristic value length
   
   @param[in] cccd
     CCCD value
   
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_gatts_serv_send_data(M2MB_BLE_HANDLE ble_handle, UINT32 serv_id, UINT32 char_id, UINT8 *value, UINT16 value_len, M2MB_BLE_CCCD_T cccd);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Starts BLE scanning

   @details
     Starts scanning for particular device (BLE address) or scans available devices (Advertising devices) 

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] bd_addr
     remote device address
    
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_scan_start(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_BD_ADDR_T bd_addr);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Stops BLE scanning

   @details
     Stop BLE scanning

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
    
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_scan_stop(M2MB_BLE_HANDLE ble_handle);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Connects to the BLE remote device

   @details
     Connects to the particular BLE device (BD address provided by the user)

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @param[in] bd_addr
     remote device address
    
   @param[in] bd_addr_type
     remote device address type
        
   @param[out] connection_id   
     This parameter is set by the m2mb API on successful connect. This connection ID is used for read, write, disconnect etc.,
      
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_connect(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_BD_ADDR_T bd_addr, M2MB_BLE_ADDRESS_TYPE_T bd_addr_type, INT32 *connection_id);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Disconnects BLE devices

   @details
     Based on the connection ID, this function disconnects remote device
      
   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @param[in] connection_id
     connection id previously created by m2mb_ble_connect API
    
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_disconnect(M2MB_BLE_HANDLE ble_handle, INT32 connection_id);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Starts service discovery

   @details
     Starts service discovery and gives available services information from the server

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @param[in] connection_id
     connection id previously created by m2mb_ble_connect API
    
   @param[in] uuid
     specifies UUID value
        
   @param[out] results
     service discovery results
   
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_start_discovery(M2MB_BLE_HANDLE ble_handle, INT32 connection_id, M2MB_BLE_UUID_T uuid, M2MB_BLE_GATT_SRVD_T **results);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Reads value request

   @details
     Reads value request from particular service's characteristic ID

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @param[in] connection_id
     specifies connection id previously created by m2mb_ble_connect api
    
   @param[in] char_handle
     specifies the characteristic handle

   @param[out] value_len
     this param is filled with the characteristic handle value length from m2mb API
        
   @param[out] value
     this param is filled with the characteristic handle value read from m2mb API
   
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_read_value_request(M2MB_BLE_HANDLE ble_handle, INT32 connection_id, INT32 char_handle, UINT8 **value, UINT16 *value_len);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Write value request

   @details
     Write value request to the particular service's characteristic ID

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @param[in] connection_id
     specifies connection id previously created by m2mb_ble_connect api
    
   @param[in] char_handle
     specifies the characteristic handle
        
   @param[in] value
     specifies characteristic value

   @param[in] value_len
     specifies characteristic value length
   
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_write_value_request(M2MB_BLE_HANDLE ble_handle, INT32 connection_id, INT32 char_handle, UINT8 *value, UINT16 value_len);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Write value to the particular service's characteristic ID

   @details
     Write value to the particular service's characteristic ID

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @param[in] connection_id
     specifies connection id previously created by m2mb_ble_connect API
    
   @param[in] char_handle
     specifies the characteristic handle
        
   @param[in] value
     specifies characteristic value

   @param[in] value_len
     specifies characteristic value length
   
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_write_value_command(M2MB_BLE_HANDLE ble_handle, INT32 connection_id, INT32 char_handle, UINT8 *value, UINT16 value_len);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     CCCD value request

   @details
     CCCD value request to the particular service's characteristic ID

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize

   @param[in] connection_id
     specifies connection id previously created by m2mb_ble_connect api
    
   @param[in] char_handle
     specifies the characteristic handle
     
   @param[in] cccd_val
     specifies the characteristic cccd value
   
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_cccd_request(M2MB_BLE_HANDLE ble_handle, INT32 connection_id, INT32 char_handle, M2MB_BLE_CCCD_T cccd_val);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Gets the BLE device bd address

   @details
     Gets the BLE device bd address

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[out] bd_addr
     specifies remote device address
   
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_device_bd_addr_get(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_BD_ADDR_T *bd_addr);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Gets the provisioning status

   @details
     Gets the provisioning status

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[out] status
     This argument filled with provisioning status
   
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_provisioning_get( M2MB_BLE_HANDLE ble_handle, INT32 *status );

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Sets the provisioning status

   @details
     provisioning status update when provisioning starts or stops

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] status
     specifies provisioning status
   
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_provisioning_set( M2MB_BLE_HANDLE ble_handle, INT32 status );


/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Sending indication to the service 

   @details
     Sending indication to the particular service characteristic id 

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] attribute_handle
     specifies attribute handle
        
   @param[in] conn_id
     specifies connection id
        
   @param[in] len
     specifies length
        
   @param[in] confirm
     specifies confirm

   @param[in] value
     specifies value
   
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_send_indication( M2MB_BLE_HANDLE ble_handle,  INT32 attribute_handle, INT32 conn_id, INT32 len, INT32 confirm, UINT8 *value);

/*****************************************************/
/******             Generic functions       **********/
/*****************************************************/

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Gets the Device information

   @details
     Gets device information based on connection id

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] conn_id
     specifies connection id
        
   @param[out] dev_info
     this argument filled with device info
   
   @return
    M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BLE_RESPONSE_T m2mb_ble_connid_device_info_get(M2MB_BLE_HANDLE ble_handle, UINT8 conn_id, M2MB_BLE_DEVICE_INFO_T *dev_info);

/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Gets the Device information

   @details
     Gets the Device information based on bd address

   @param[in] ble_handle
     specifies the BLE handle previously created through m2mb_ble_initialize
     
   @param[in] bd_addr
     specifies remote device address
        
   @param[in] dev_info
     specifies device information
   
   @return
     M2MB_BLE_RESPONSE_T: used for success or failure see m2mb_ble.h for details
    
   @note

   @b
     Example
   @code
     <C code example>
   @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


M2MB_BLE_RESPONSE_T m2mb_ble_bdaddr_device_info_get(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_BD_ADDR_T bd_addr, M2MB_BLE_DEVICE_INFO_T *dev_info);

M2MB_BLE_RESPONSE_T m2mb_ble_get_initialization_state(M2MB_BLE_HANDLE ble_handle, M2MB_BLE_INIT_STATE_T *state);

#endif
