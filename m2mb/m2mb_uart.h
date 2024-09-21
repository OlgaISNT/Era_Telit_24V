/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_uart.h

  @brief
    UART library implementation.

  @details
    The following functions are implemented:
    - m2mb_uart_open()
    - m2mb_uart_close()
    - m2mb_uart_ioctl()
    - m2mb_uart_read()
    - m2mb_uart_write()

  @note
    Dependencies: m2m/m2m_generic/common/m2mb_inc/m2mb_types.h

  @author
    Morgan Deidda

  @date
    04/01/2018
*/

#ifndef M2M_M2MB_UART_API_H
#define M2M_M2MB_UART_API_H

/* Global declarations ==========================================================================*/
#ifndef NULL
#define NULL  0
#endif

/* Global enums ==============================================================================*/

/*!
   @brief UART configuration requests.

   This enum defines the requests wich can be passed to the function m2mb_uart_ioctl()
*/
enum M2MB_UART_IOCTL_REQUEST
{
  M2MB_UART_IOCTL_SET_CFG = 0,                 /**< set whole uart device configuration */
  M2MB_UART_IOCTL_GET_CFG,                     /**< get whole uart device configuration */
  M2MB_UART_IOCTL_SET_BR,                      /**< set uart baudrate */
  M2MB_UART_IOCTL_GET_BR,                      /**< get uart baudrate */
  M2MB_UART_IOCTL_SET_FCTL,                    /**< set uart flow control type */
  M2MB_UART_IOCTL_GET_FCTL,                    /**< get uart flow control type */
  M2MB_UART_IOCTL_SET_PARITY_MODE,             /**< set uart parity mode */
  M2MB_UART_IOCTL_GET_PARITY_MODE,             /**< get uart parity mode */
  M2MB_UART_IOCTL_SET_STOP_BITS,               /**< set uart number of stop bits */
  M2MB_UART_IOCTL_GET_STOP_BITS,               /**< get uart number of stop bits */
  M2MB_UART_IOCTL_SET_BITS_PER_CHAR,           /**< set uart number of bits per character */
  M2MB_UART_IOCTL_GET_BITS_PER_CHAR,           /**< get uart number of bits per character */
  M2MB_UART_IOCTL_SET_TX_TIMEOUT = 14,         /**< set synchronous write timeout in ms (if 0, waits indefinitely until the write is completed) */
  M2MB_UART_IOCTL_GET_TX_TIMEOUT,              /**< get synchronous write timeout in ms (if 0, waits indefinitely until the write is completed) */
  M2MB_UART_IOCTL_SET_RX_TIMEOUT,              /**< set synchronous read timeout in ms (if 0, waits indefinitely until the read is completed) */
  M2MB_UART_IOCTL_GET_RX_TIMEOUT,              /**< get synchronous read timeout in ms (if 0, waits indefinitely until the read is completed) */
  M2MB_UART_IOCTL_SET_CB_FN,                   /**< set callback function pointer (if NULL, action is synchronous) */
  M2MB_UART_IOCTL_GET_CB_FN,                   /**< get callback function pointer (if NULL, action is synchronous) */
  M2MB_UART_IOCTL_SET_CB_USERDATA,             /**< set callback data pointer */
  M2MB_UART_IOCTL_GET_CB_USERDATA,             /**< get callback data pointer */
  M2MB_UART_IOCTL_SET_POWER_STATE,             /**< set current power state */
  M2MB_UART_IOCTL_GET_POWER_STATE,             /**< get current power state */
  M2MB_UART_IOCTL_GET_OP_MODE,                 /**< get current operational mode of device (see +CFUN) */
  M2MB_UART_IOCTL_GET_RTS_STATE,               /**< get current RTS state */
  M2MB_UART_IOCTL_GET_DTR_STATE,               /**< get current DTR state */
  M2MB_UART_IOCTL_SET_CTS,                     /**< set CTS pin */
  M2MB_UART_IOCTL_SET_DSR,                     /**< set DSR pin */
  M2MB_UART_IOCTL_SET_DCD,                     /**< set DCD pin */
  M2MB_UART_IOCTL_SET_RING,                    /**< set RING pin */
  M2MB_UART_IOCTL_SET_DTR_FCTL,                /**< No support. set DTR flow control */
  M2MB_UART_IOCTL_GET_DTR_FCTL,                /**< No support. get DTR flow control */
  M2MB_UART_IOCTL_NOF_REQ
};

/* Global typedefs ==============================================================================*/

/*!
   @brief UART flow control type.

   This type defines the flow control type that the client can set in the
   UART open configuration or set/get by m2mb_uart_ioctl API.
*/
typedef enum
{
  M2MB_UART_FCTL_OFF = 0,       /**< Disable flow control */
  M2MB_UART_CTSRFR_AUTO_FCTL,   /**< Use CTS/RFR flow control with auto RX RFR signal generation */
  M2MB_UART_FCTL_MAX
} M2MB_UART_FCTL_E;

/*!
   @brief UART parity mode.

   This type defines the parity mode that the client can set in the
   UART open configuration or get by m2mb_uart_ioctl API.
*/
typedef enum
{
  M2MB_UART_NO_PARITY = 0,            /**< no parity */
  M2MB_UART_ODD_PARITY,               /**< No support. odd parity */
  M2MB_UART_EVEN_PARITY,              /**< No support. even parity */
  M2MB_UART_SPACE_PARITY,             /**< No support. space parity */
  M2MB_UART_PARITY_MAX
} M2MB_UART_PARITY_E;

/*!
   @brief UART number of stop bits.

   This type defines the number of stop bits that the client can set in the
   UART open configuration or get by m2mb_uart_ioctl API.
*/
typedef enum
{
  M2MB_UART_0_5_STOP_BITS = 0,       /**< No support. 0.5 stop bits */
  M2MB_UART_1_0_STOP_BITS,           /**< 1.0 stop bits */
  M2MB_UART_1_5_STOP_BITS,           /**< No support. 1.5 stop bits */
  M2MB_UART_2_0_STOP_BITS,           /**< No support. 2.0 stop bits */
  M2MB_UART_STOP_BITS_MAX
} M2MB_UART_STOP_BITS_E;

/*!
   @brief UART number of bits per character.

   This type defines the number of bits per character that the client can set in the
   UART open configuration or get by m2mb_uart_ioctl API.
*/
typedef enum
{
  M2MB_UART_5_BITS_PER_CHAR = 0,     /**< No support. 5 bits per character */
  M2MB_UART_6_BITS_PER_CHAR,         /**< No support. 6 bits per character */
  M2MB_UART_7_BITS_PER_CHAR,         /**< No support. 7 bits per character */
  M2MB_UART_8_BITS_PER_CHAR,         /**< 8 bits per character */
  M2MB_UART_BITS_PER_CHAR_MAX
} M2MB_UART_BITS_PER_CHAR_E;

/*!
   @brief UART event type.

   This type defines the indications which can be passed to the callback.
*/
typedef enum
{
  M2MB_UART_TX_EV,                   /**< Transmitted data Event */
  M2MB_UART_RX_EV,                   /**< Received data Event */
  M2MB_UART_DTR_EV                   /**< Received DTR Event */  
} M2MB_UART_IND_E;

/*!
   @brief UART pin voltage value.

   This type defines the pin output voltage that the client can set
   by m2mb_uart_ioctl API.
*/
typedef enum
{
  M2MB_UART_LOW_VALUE,                   /**< Set pin output LOW. */
  M2MB_UART_HIGH_VALUE,                  /**< Set pin output HIGH*/
  ENUM_TO_INT( M2MB_UART_PIN_VALUE_E )
} M2MB_UART_PIN_VALUE_E;



/*!
   @brief Transmit and receive operation callback type.

   This type defines the callback functions that the client can set in the
   UART open configuration or get by m2mb_uart_ioctl API.
*/
typedef void ( *m2mb_uart_ind_callback )( INT32 fd, M2MB_UART_IND_E uart_event, UINT16 resp_size, void *resp_struct, void *userdata );

/*!
   @brief UART ioctl configuration.

   The UART ioctl configuration is the collection of settings specified for each UART
   transfer call to select the various possible UART transfer parameters.
*/
typedef struct
{
  UINT32 baud_rate;                             /**< Baud rate (bps) */
  M2MB_UART_FCTL_E flow_control;                /**< Transmit flow control type */
  M2MB_UART_PARITY_E parity_mode;               /**< Parity Mode */
  M2MB_UART_STOP_BITS_E stop_bits;              /**< Number of stop bits */
  M2MB_UART_BITS_PER_CHAR_E bits_per_char;      /**< Number of bits per character */
  BOOLEAN loopback_mode;                        /**< Enable loopback mode: not supported */
  UINT16 tx_timeout_ms;                         /**< Synchronous write timeout (ignored if tx_cb_fn is set) */
  UINT16 rx_timeout_ms;                         /**< Synchronous read timeout (ignored if rx_cb_fn is set) */
  m2mb_uart_ind_callback cb_fn;                 /**< Callback, called from ISR context. Do not call m2mb_uart_read or m2mb_uart_write APIs from this callback */
  void *cb_data;                                /**< Callback data pointer. If present, memory must be allocated and freed by the user */
} M2MB_UART_CFG_T;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_uart_open opens a UART device.

  @details
    m2mb_uart_open opens a UART device returning the file descriptor that must be passed as first
    parameter for all the other UART actions.
    Calling the m2mb_uart_open is mandatory before using the UART device.

  @param[in] path
    first parameter is the path to the UART device, that will be initialized by the function.
    It is a string like "/dev/ttyX" where X is the UART instance index.
    Path "/dev/tty" is equivalent to default "/dev/tty0".
  @param[in] flags
    second parameter is the access mode flags. Currently unused

  @return
    returns the File Descriptor on SUCCESS, -1 on FAILURE.

  @note
    A default configuration is applied, with following values:
    -  Baud rate:                 115200 bps
    -  Flow Control:              off
    -  Parity Mode:               none
    -  Stop Bits:                 1
    -  Bits Per Char:             8
    -  Loopback Mode:             disabled
    -  Synchronous Write Timeout: 0 ms
    -  Synchronous Read Timeout:  0 ms
    -  Callback Function:         none
    -  Callback Data:             none

  @b
    m2mb_uart_open(path, flags);
  @code
    int main()
    {
      INT32 fd;
      fd = m2mb_uart_open("/dev/tty1", 0);
      if ( fd != -1 )
        printf( "m2mb_uart_open succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_uart_open( const CHAR *path, INT32 flags, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_uart_close closes a UART device.

  @details
    m2mb_uart_close closes a UART device freeing the data associated to the file descriptor passed
    as parameter.

  @param[in] fd
    File Descriptor returned by m2mb_uart_open()

  @return
    returns 0 on SUCCESS, -1 on FAILURE.

  @note

  @b
    m2mb_uart_close(fd);
  @code
    int main()
    {
      INT32 fd;
      INT32 retVal;
      fd = m2mb_uart_open("/dev/tty1", 0);

      //... use UART device ...

      retVal = m2mb_uart_close(fd);
      if ( retVal != -1 )
        printf( "m2mb_uart_close succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_uart_close( INT32 fd );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_uart_ioctl configures an open UART device.

  @details
    m2mb_uart_ioctl sets/gets the full UART configuration or single settings by means of requests
    listed in enum M2MB_UART_IOCTL_REQUEST.

  @param[in] fd
    first parameter is the File Descriptor returned by m2mb_uart_open()
  @param[in] request
    second parameter indicates the action to be performed.
    The allowed values ??are those listed in enum M2MB_UART_IOCTL_REQUEST.
  @param[in,out] cfg
    third parameter can be a pointer to a previously defined variable or a value, depending on the
    action requested by the second parameter (see following note for further details).

  @return
    returns 0 on SUCCESS, -1 on FAILURE.

  @note
    Use cases:

    - Action: M2MB_UART_IOCTL_SET_CFG

      Description: set whole uart device configuration

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pCfgStruct(M2MB_UART_CFG_T *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_CFG, &cfgStruct)

    - Action: M2MB_UART_IOCTL_GET_CFG

      Description: get whole uart device configuration

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pCfgStruct(M2MB_UART_CFG_T *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_CFG, &cfgStruct)

    - Action: M2MB_UART_IOCTL_SET_BR
      
      Description: set uart baudrate
      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), baudRate(UINT32)
      The supported baudRates are 300,600,1200,2400,4800,9600,19200,38400,57600,115200,230400,
                                  460800,921600,2000000,2500000,3000000,3500000,3750000

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_BR, 115200)

    - Action: M2MB_UART_IOCTL_GET_BR

      Description: get uart baudrate

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pBaudRate(UINT32 *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_BR, &baudRate)

    - Action: M2MB_UART_IOCTL_SET_FCTL

      Description: set uart flow control type

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), flowControl(M2MB_UART_FCTL_E)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_FCTL, M2MB_UART_CTSRFR_AUTO_FCTL)

    - Action: M2MB_UART_IOCTL_GET_FCTL

      Description: get uart flow control type

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pFlowControl(M2MB_UART_FCTL_E *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_FCTL, &flowControl)

    - Action: M2MB_UART_IOCTL_SET_PARITY_MODE

      Description: set uart parity mode

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), parityMode(M2MB_UART_PARITY_E)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_PARITY_MODE, M2MB_UART_NO_PARITY)

    - Action: M2MB_UART_IOCTL_GET_PARITY_MODE

      Description: get uart parity mode

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pParityMode(M2MB_UART_PARITY_E *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_PARITY_MODE, &parityMode)

    - Action: M2MB_UART_IOCTL_SET_STOP_BITS

      Description: set uart number of stop bits

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), stopBits(M2MB_UART_STOP_BITS_E)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_STOP_BITS, M2MB_UART_1_0_STOP_BITS)

    - Action: M2MB_UART_IOCTL_GET_STOP_BITS

      Description: get uart number of stop bits

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pStopBits(M2MB_UART_STOP_BITS_E *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_STOP_BITS, &stopBits)

    - Action: M2MB_UART_IOCTL_SET_BITS_PER_CHAR

      Description: set uart number of bits per character

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), bitsPerChar(M2MB_UART_BITS_PER_CHAR_E)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_BITS_PER_CHAR, M2MB_UART_8_BITS_PER_CHAR)

    - Action: M2MB_UART_IOCTL_GET_BITS_PER_CHAR

      Description: get uart number of bits per character

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pBitsPerChar(M2MB_UART_BITS_PER_CHAR_E *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_BITS_PER_CHAR, &bitsPerChar)

    - Action: M2MB_UART_IOCTL_SET_TX_TIMEOUT

      Description: set synchronous write timeout in ms (if 0, waits indefinitely until the write is completed)

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), timeout(UINT16)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_TX_TIMEOUT, 100)

    - Action: M2MB_UART_IOCTL_GET_TX_TIMEOUT

      Description: get synchronous write timeout in ms (if 0, waits indefinitely until the write is completed)

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pTimeout(UINT16 *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_TX_TIMEOUT, &timeout)

    - Action: M2MB_UART_IOCTL_SET_RX_TIMEOUT

      Description: set synchronous read timeout in ms (if 0, waits indefinitely until the read is completed)

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), timeout(UINT16)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_RX_TIMEOUT, 100)

    - Action: M2MB_UART_IOCTL_GET_RX_TIMEOUT

      Description: get synchronous read timeout in ms (if 0, waits indefinitely until the read is completed)

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pTimeout(UINT16 *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_RX_TIMEOUT, &timeout)

    - Action: M2MB_UART_IOCTL_SET_CB_FN

      Description: set callback function pointer (if NULL, action is synchronous)

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), cb_fn(m2mb_uart_ind_callback)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_CB_FN, cb_fn)

    - Action: M2MB_UART_IOCTL_GET_CB_FN

      Description: get callback function pointer (if NULL, action is synchronous)

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pCb_fn(m2mb_uart_ind_callback *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_CB_FN, &cb_fn)

    - Action: M2MB_UART_IOCTL_SET_CB_USERDATA

      Description: set callback data pointer

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), cbData(void *)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_CB_USERDATA, cbData)

    - Action: M2MB_UART_IOCTL_GET_CB_USERDATA

      Description: get callback data pointer

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pCbData(void **)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_CB_USERDATA, &cbData)

    - Action: M2MB_UART_IOCTL_SET_POWER_STATE

      Description: set uart power state (TRUE = power on; FALSE = low power)

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), powerState(BOOLEAN)

      Example:
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_POWER_STATE, TRUE)

    - Action: M2MB_UART_IOCTL_GET_POWER_STATE

      Description: get uart power state (TRUE = power on; FALSE = low power)

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pPowerState(BOOLEAN *)

      Example:
      BOOLEAN powerState;
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_POWER_STATE, &powerState)

    - Action: M2MB_UART_IOCTL_GET_OP_MODE

      Description: get uart operational mode (see +CFUN)

      Params(in): fd(INT32), request(M2MB_UART_IOCTL_REQUEST), pOpMode(UINT8 *)

      Example:
      UINT8 opMode;
      m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_OP_MODE, &opMode)

  @b
    m2mb_uart_ioctl(fd, request, cfg);
  @code
    int main()
    {
      INT32 fd;
      INT32 retVal;
      M2MB_UART_CFG_T cfg;
      fd = m2mb_uart_open("/dev/tty1", 0);

      //...

      retVal = m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_GET_CFG, &cfg);
      if ( retVal != -1 )
        printf( "m2mb_uart_ioctl: get cfg struct succeeded");

      // modify some configuration structure fields...
      cfg.baud_rate = 9600;
      //...

      retVal = m2mb_uart_ioctl(fd, M2MB_UART_IOCTL_SET_CFG, &cfg);
      if ( retVal != -1 )
        printf( "m2mb_uart_ioctl: set cfg struct succeeded");

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_uart_ioctl( INT32 fd, INT32 request, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_uart_read reads data from an open UART device.

  @details
    m2mb_uart_read reads up to nbyte Bytes from a UART device into a destination buffer.

  @param[in] fd
    first parameter is the File Descriptor returned by m2mb_uart_open()
  @param[in] buf
    second parameter is the destination buffer, previously allocated.
  @param[in] nbyte
    third parameter is the length of the destination buffer in Bytes.

  @return
    returns the number of read Bytes on SUCCESS, -1 on FAILURE.

  @note
    Reading can be synchronous or asynchronous.
    If the callback has been registered (by means of m2mb_uart_ioctl), the function
    m2mb_uart_read() has an asynchronous behavior: when new data is available from the UART device,
    the registered callback is triggered in order to signal that a not blocking reading can be
    performed.
    If no callback has been registered, the function m2mb_uart_read() has a synchronous behavior
    and the reading remains blocked until new data is available from the UART device or for a
    timeout set by means of m2mb_uart_ioctl.

  @b
    m2mb_uart_read(fd, buf, nbyte);
  @code
    #define BUF_LEN 32
    int main()
    {
      INT32 fd;
      INT32 rdBytes;
      UINT8 dstBuf[BUF_LEN];
      fd = m2mb_uart_open("/dev/tty1", 0);

      //...

      rdBytes = m2mb_uart_read(fd, dstBuf, BUF_LEN);
      if ( rdBytes >= 0 )
        printf( "m2mb_uart_read succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
SSIZE_T m2mb_uart_read( INT32 fd, void *buf, SIZE_T nbyte );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_uart_write writes data to an open UART device.

  @details
    m2mb_uart_write writes nbyte Bytes from a source buffer to a UART device.

  @param[in] fd
    first parameter is the File Descriptor returned by m2mb_uart_open()
  @param[in] buf
    second parameter is the source buffer, previously allocated.
  @param[in] nbyte
    third parameter is the length of the source buffer in Bytes.

  @return
    returns the number of written Bytes on SUCCESS, -1 on FAILURE.

  @note
    Writing can be synchronous or asynchronous.
    If the callback has been registered (by means of m2mb_uart_ioctl), the function
    m2mb_uart_write() has an asynchronous behavior: after a not blocking writing has been performed,
    the registered callback is triggered in order to signal that the whole data has been
    successfully transmitted to the UART device.
    If no callback has been registered, the function m2mb_uart_write() has a synchronous behavior
    and the wtiting remains blocked until the data transmission to the UART device is completed or
    for a timeout set by means of m2mb_uart_ioctl.

  @b
    m2mb_uart_read(fd, buf, nbyte);
  @code
    #define BUF_LEN 32
    #define WR_BYTE_VAL 0xAB
    int main()
    {
      INT32 fd;
      INT32 wrBytes;
      UINT8 srcBuf[BUF_LEN];
      fd = m2mb_uart_open("/dev/tty1", 0);

      //...

      memset(srcBuf, WR_BYTE_VAL, BUF_LEN);
      wrBytes = m2mb_uart_write(fd, srcBuf, BUF_LEN);
      if ( wrBytes >= 0 )
        printf( "m2mb_uart_write succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
SSIZE_T m2mb_uart_write( INT32 fd, const void *buf, SIZE_T nbyte );

#endif  /* M2M_M2MB_UART_API_H */

