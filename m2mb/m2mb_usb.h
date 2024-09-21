/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_usb.h

  @brief
    USB library implementation.

  @details
    The following functions are implemented:
    m2mb_usb_ioctl
    m2mb_usb_open
    m2mb_usb_read
    m2mb_usb_write
    m2mb_usb_close

  @notes
    Dependencies: m2m/m2m_generic/common/m2mb_inc/m2mb_types.h

  @author
    Morgan Deidda

  @date
    30/11/2017
*/

#ifndef M2M_M2MB_USB_API_H
#define M2M_M2MB_USB_API_H

/* Global declarations ==========================================================================*/
#ifndef NULL
#define NULL  0
#endif

/* defines used to check if USB cable is connected or not */
#define M2MB_USB_CABLE_ATTACHED  TRUE
#define M2MB_USB_CABLE_DETACHED  FALSE


/* Global enums ==============================================================================*/

/*!
   @brief USB configuration requests.

   This enum defines the requests wich can be passed to the function m2mb_usb_ioctl()
*/
typedef enum
{
  M2MB_USB_IOCTL_SET_CFG = 0,     /**< set usb device configuration (user callback + data) */
  M2MB_USB_IOCTL_GET_CFG,         /**< get usb device configuration (user callback + data) */
  M2MB_USB_IOCTL_SET_CB,          /**< set usb user callback function */
  M2MB_USB_IOCTL_GET_CB,          /**< get usb user callback function */
  M2MB_USB_IOCTL_SET_USERDATA,    /**< set usb userdata */
  M2MB_USB_IOCTL_GET_USERDATA,    /**< get usb userdata */
  M2MB_USB_IOCTL_SET_DCD,         /**< set usb DCD level */
  M2MB_USB_IOCTL_GET_RTS_STATE,   /**< get usb RTS state */
  M2MB_USB_IOCTL_GET_DTR_STATE,   /**< get usb DTR state */
  M2MB_USB_IOCTL_GET_CABLE_STATE, /**< get usb cable state attached or detached */
  M2MB_USB_IOCTL_SET_DTR_FCTL,    /**< No support. set usb DTR flow control */
  M2MB_USB_IOCTL_GET_DTR_FCTL,    /**< No support. get usb DTR flow control */
  M2MB_USB_IOCTL_SET_DSR,         /**< set usb DSR level */
  M2MB_USB_IOCTL_SET_RING,        /**< set usb RING level */
  M2MB_USB_IOCTL_SET_TX_TIMEOUT,  /**< set synchronous write timeout in ms
                                       (if 0, waits indefinitely until the write is completed) */
  M2MB_USB_IOCTL_GET_TX_TIMEOUT,  /**< get synchronous write timeout in ms
                                       (if 0, waits indefinitely until the write is completed) */
  M2MB_USB_IOCTL_SET_RX_TIMEOUT,  /**< set synchronous read timeout in ms
                                       (if 0, waits indefinitely until the read is completed) */
  M2MB_USB_IOCTL_GET_RX_TIMEOUT,  /**< get synchronous read timeout in ms
                                       (if 0, waits indefinitely until the read is completed) */
  M2MB_USB_IOCTL_NOF_REQ,
  ENUM_TO_INT( M2MB_USB_IOCTL_REQUEST )
} M2MB_USB_IOCTL_REQUEST;

/* Global typedefs ==============================================================================*/

/*!
   @brief USB event type.

   This type defines the indications which can be passed to the callback.
*/
typedef enum
{
  M2MB_USB_RX_EVENT,               /**< Received data Event */
  M2MB_USB_TX_EVENT,               /**< Transmitted data Event */
  M2MB_USB_CABLE_CONNECTED_EVENT,  /**< Cable attach/detach Event */
  M2MB_USB_DTR_EVENT,              /**< DTR state change Event */
  ENUM_TO_INT( M2MB_USB_IND_E )
} M2MB_USB_IND_E;

/*!
   @brief USB control signal value.

   This type defines the USB control signal output level that the client can set
   by m2mb_usb_ioctl API.
*/
typedef enum
{
  M2MB_USB_LOW_VALUE,                   /**< Set control signal output level LOW. */
  M2MB_USB_HIGH_VALUE,                  /**< Set control signal output level HIGH*/
  ENUM_TO_INT( M2MB_USB_SIGNAL_VALUE_E )
} M2MB_USB_SIGNAL_VALUE_E;

/*!
   @brief Transmit and receive operation callback type.

   This type defines the callback functions that the client can register by m2mb_usb_ioctl API.
   The user callback function is called when an event is received.
*/
typedef void ( *m2mb_usb_ind_callback )( INT32 fd, M2MB_USB_IND_E usb_event, UINT16 resp_size, void *resp_struct, void *userdata );

/*!
   @brief USB ioctl configuration.

   This type defines the USB ioctl configuration structure with the user callback and user data
   specified for each USB port.
*/
typedef struct
{
  m2mb_usb_ind_callback m2mb_usb_app_cb_func; /**< This is a callback that user can register;
                                                   it will be called when new rx data is available
                                                   when tx data has been transmitted and when a USB
                                                   attach or detach happens */
  void *userData;                             /**< This is a callback data pointer. If present, memory must be
                                                   allocated and freed by the user */
} M2MB_USB_CFG_T;

typedef struct M2MB_USB_HANDLE_TAG *M2MB_USB_HANDLE;


/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_usb_open opens a USB device.

  @details
    m2mb_usb_open opens a USB device returning the file descriptor that must be passed as first
    parameter for all the other USB actions.
    Calling the m2mb_usb_open is mandatory before using the USB device.

  @param[in] path
    first parameter is the path to the USB device, that will be initialized by the function.
    It is a string like "/dev/usbX" where X is the USB instance index in decimal format.
    Path "/dev/usb" is equivalent to default "/dev/usb0".
  @param[in] flags
    second parameter is the access mode flags. Currently unused

  @return
     returns the file descriptor on SUCCESS, -1 on FAILURE

  @note

  @b
    m2mb_usb_open(path, flags);

  @code
    int main()
    {
      INT32 fd;
      fd = m2mb_usb_open("/dev/usb0", 0);
      if ( fd != -1 )
        printf( "m2mb_usb_open succeeded");
    }

  @endcode
*/

/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_usb_open( const CHAR *path, INT32 flags, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_usb_close closes a USB device.

  @details
    m2mb_usb_close closes a USB device freeing the data associated to the file descriptor passed
    as parameter.

  @param [in] fd
    file descriptor returned by m2mb_usb_open()

  @return
    returns 0 on SUCCESS, -1 on FAILURE.

  @note

  @b
    m2mb_usb_close(fd);

  @code
    int main()
    {
      INT32 fd;
      INT32 retVal;
      fd = m2mb_usb_open("/dev/usb0", 0);

      //... use USB device ...

      retVal = m2mb_usb_close(fd);
      if ( retVal != -1 )
        printf( "m2mb_usb_close succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_usb_close( INT32 fd );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_usb_ioctl configures an open USB device.

  @details
    m2mb_usb_ioctl sets/gets the USB device settings by means of requests
    listed in enum M2MB_USB_IOCTL_REQUEST.

  @param[in] fd
    File Descriptor returned by m2mb_usb_open()
  @param[in] request
    the action to be performed.
    The allowed values are those listed in enum M2MB_USB_IOCTL_REQUEST.
  @param[in,out] cfg
    parameter of the request: can be a pointer to a previously defined variable or a value, depending on the
    action requested by the second parameter (see following note for further details).

  @return
    returns 0 on SUCCESS, -1 on FAILURE.

  @note
    The following options are not supported.
    M2MB_USB_IOCTL_SET_DTR_FCTL
    M2MB_USB_IOCTL_GET_DTR_FCTL
    M2MB_USB_IOCTL_SET_TX_TIMEOUT
    M2MB_USB_IOCTL_GET_TX_TIMEOUT
    M2MB_USB_IOCTL_SET_RX_TIMEOUT
    M2MB_USB_IOCTL_GET_RX_TIMEOUT

    Use cases:

    - Action: M2MB_USB_IOCTL_SET_CFG

      Description: set usb device configuration for setting user callback and data

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), pCfgStruct(M2MB_USB_CFG_T *)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_SET_CFG, &cfgStruct)

    - Action: M2MB_USB_IOCTL_GET_CFG

      Description: get usb device configuration with current user callback and data

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), pCfgStruct(M2MB_USB_CFG_T *)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_GET_CFG, &cfgStruct)

    - Action: M2MB_USB_IOCTL_SET_CB

      Description: set callback function pointer (if NULL, action is synchronous)

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), cb_fn(m2mb_usb_ind_callback)

      Note:
      When a USB callback is set, the m2mb_usb_read can be used inside the callback
      with the M2MB_USB_RX_EVENT event or outside the callback in another thread,
      where it will get the data if it has not been already read

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_SET_CB, cb_fn)

    - Action: M2MB_USB_IOCTL_GET_CB

      Description: get callback function pointer (if NULL, action is synchronous)

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), pCb_fn(m2mb_usb_ind_callback *)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_GET_CB, &cb_fn)

    - Action: M2MB_USB_IOCTL_SET_USERDATA

      Description: set callback data pointer

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), cbData(void *)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_SET_USERDATA, cbData)

    - Action: M2MB_USB_IOCTL_GET_USERDATA

      Description: get callback data pointer

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), pCbData(void **)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_GET_USERDATA, &cbData)

    - Action: M2MB_USB_IOCTL_SET_DCD

      Description: set usb DCD level

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), dcdLevel(M2MB_USB_SIGNAL_VALUE_E)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_SET_DCD, dcdLevel)

    - Action: M2MB_USB_IOCTL_SET_DSR

      Description: set usb DSR level

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), dsrLevel(M2MB_USB_SIGNAL_VALUE_E)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_SET_DSR, dsrLevel)

    - Action: M2MB_USB_IOCTL_SET_RING

      Description: set usb RING level

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), ringLevel(M2MB_USB_SIGNAL_VALUE_E)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_SET_RING, ringLevel)

    - Action: M2MB_USB_IOCTL_GET_RTS_STATE

      Description: get usb RTS state

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), pRtsLevel(M2MB_USB_SIGNAL_VALUE_E *)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_GET_RTS_STATE, &rtsLevel)

    - Action: M2MB_USB_IOCTL_GET_DTR_STATE

      Description: get usb DTR state

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), pDtrLevel(M2MB_USB_SIGNAL_VALUE_E *)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_GET_DTR_STATE, &dtrLevel)

    - Action: M2MB_USB_IOCTL_GET_CABLE_STATE

      Description: get usb cable state attached or detached

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), pCableState(BOOLEAN *)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_GET_CABLE_STATE, &cableState)

    - Action: M2MB_USB_IOCTL_SET_DTR_FCTL

      Description: set usb DTR flow control

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), dtrFctl(INT32)

      Note:
      This command configures the behavior of the outbound flow according to the DTR control line level
      transitions.
      Admitted parameter values are:
      1 : DTR level transitions control the outbound data flow.
          If DTR is asserted, the output data is sent; if DTR is not asserted, the output data is blocked.
      2 : Like setting 1, but receiving input data has the same effect of a low to high level transition.
          Therefore, if the DTR is not asserted, or if it is asserted but its level has not been signaled
          by the DTE to the DCE, after receiving input data the DTR is considered asserted.
          A new DTR level transition from asserted to not asserted will block again the outbound flow.

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_SET_DTR_FCTL, dtrFctl)

    - Action: M2MB_USB_IOCTL_GET_DTR_FCTL

      Description: get usb DTR flow control

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), pDtrFctl(INT32 *)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_GET_DTR_FCTL, &dtrFctl)

    - Action: M2MB_USB_IOCTL_SET_TX_TIMEOUT

      Description: set synchronous write timeout in ms (if 0, waits indefinitely until the write is completed)

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), timeout(UINT16)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_SET_TX_TIMEOUT, 100)

    - Action: M2MB_USB_IOCTL_GET_TX_TIMEOUT

      Description: get synchronous write timeout in ms (if 0, waits indefinitely until the write is completed)

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), pTimeout(UINT16 *)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_GET_TX_TIMEOUT, &timeout)

    - Action: M2MB_USB_IOCTL_SET_RX_TIMEOUT

      Description: set synchronous read timeout in ms (if 0, waits indefinitely until the read is completed)

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), timeout(UINT16)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_SET_RX_TIMEOUT, 100)

    - Action: M2MB_USB_IOCTL_GET_RX_TIMEOUT

      Description: get synchronous read timeout in ms (if 0, waits indefinitely until the read is completed)

      Params(in): fd(INT32), request(M2MB_USB_IOCTL_REQUEST), pTimeout(UINT16 *)

      Example:
      m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_GET_RX_TIMEOUT, &timeout)

  @b
    m2mb_uSB_ioctl(fd, request, cfg);

  @code

    static void USB_Cb( INT32 fd, M2MB_USB_IND_E usb_event, UINT16 resp_size, void *resp_struct, void *userdata )
    {
      //...
    }

    int main()
    {
      INT32 fd;
      INT32 retVal;
      M2MB_USB_CFG_T cfg;
      fd = m2mb_usb_open("/dev/USB0", 0);

      //...

      retVal = m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_GET_CFG, &cfg);
      if ( retVal != -1 )
        printf( "m2mb_usb_ioctl: get cfg struct succeeded");

      // modify some configuration structure fields...
      cfg.m2mb_usb_app_cb_func = USB_Cb;
      //...

      retVal = m2mb_usb_ioctl(fd, M2MB_USB_IOCTL_SET_CFG, &cfg);
      if ( retVal != -1 )
        printf( "m2mb_usb_ioctl: set cfg struct succeeded");
    }

  @endcode

  @code
  //if the callback is used to get USB cable state event information (syncronous mode)
  //check the M2MB_USB_CABLE_CONNECTED_EVENT event

  static void USB_Cb( INT32 fd, M2MB_USB_IND_E usb_event, UINT16 resp_size, void *resp_struct, void *userdata )
  {
    if( usb_event == M2MB_USB_CABLE_CONNECTED_EVENT )
    {
      BOOLEAN is_USB_connected = *((BOOLEAN*)(resp_struct));

      if( is_USB_connected == M2MB_USB_CABLE_ATTACHED )
      {
        // The cable is connnected again
      }
    }

  }

  @endcode

  @code

  BOOLEAN state = FALSE;

  retVal = m2mb_usb_ioctl(USBHandle, M2MB_USB_IOCTL_GET_CABLE_STATE, &state);

  @endcode

*/
  /*---------------------------------------------------------------------------------------------*/
INT32 m2mb_usb_ioctl( INT32 fd, INT32 request, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!

  @brief
    m2mb_usb_read

  @details
    read nbyte Bytes from a USB device into the array pointed to by buf

  @param [in] fd: file descriptor returned by m2mb_usb_open
  @param [in] buf: destination buffer, previously allocated
  @param [in] nbyte: length of destination buffer in Bytes

  @return
     number of read Bytes on SUCCESS
     -1 on FAILURE

  @note
    When a USB callback is set, the m2mb_usb_read can be used inside the callback
    with the M2MB_USB_RX_EVENT event or outside the callback in another thread,
    where it will get the data if it has not been already read

  @b

  @code

    #define BUF_LEN 32
    int main()
    {
      INT32 fd;
      INT32 rdBytes;
      UINT8 dstBuf[BUF_LEN];
      fd = m2mb_usb_open("/dev/USB0", 0);

      //...

      rdBytes = m2mb_usb_read(fd, dstBuf, BUF_LEN);
      if ( rdBytes >= 0 )
        printf( "m2mb_usb_read succeeded");
    }

  @endcode

*/
  /*---------------------------------------------------------------------------------------------*/
SSIZE_T m2mb_usb_read( INT32 fd, void *buf, SIZE_T nbyte );

/*-----------------------------------------------------------------------------------------------*/
/*!

  @brief
    m2mb_usb_write

  @details
    write nbyte Bytes from the array pointed to by buf to a USB device

  @param [in] fd: file descriptor returned by m2mb_usb_open
  @param [in] buf: source buffer, previously allocated
  @param [in] nbyte: length of source buffer in Bytes

  @return
     number of written Bytes on SUCCESS
     on FAILURE:
       -1

  @note

  @b

  @code

    #define BUF_LEN 32
    #define WR_BYTE_VAL 0xAB
    int main()
    {
      INT32 fd;
      INT32 wrBytes;
      UINT8 srcBuf[BUF_LEN];
      fd = m2mb_usb_open("/dev/USB0", 0);

      //...

      memset(srcBuf, WR_BYTE_VAL, BUF_LEN);
      wrBytes = m2mb_usb_write(fd, srcBuf, BUF_LEN);
      if ( wrBytes >= 0 )
        printf( "m2mb_usb_write succeeded");
    }

  @endcode

*/
  /*-----------------------------------------------------------------------------------------------*/
SSIZE_T m2mb_usb_write( INT32 fd, const void *buf, SIZE_T nbyte );


/*-----------------------------------------------------------------------------------------------*/
/*!

  @brief
    m2mb_usb_isCableAttached

  @details
    Check if the USB cable is connected

  @param [in] path: /dev/USB# where # is in decimal format.
              path can be NULL and checks the connection of the primary or the unique USB cable,
              otherwise only USB channel related cable is checked

  @return
    -1 on FAILURE
  @return
    0 if the cable is disconnected
  @return
    1 if the cable is connected

  @note
    When used with NULL as a path, the unique or the primary USB cable is checked.
    For future products with more than one USB connector:
    If path is provided, only the related USB channel cable is checked.
    E.g In case of multi USB connector, if USB0 is on USB connector 0, and USB3 is on USB connector 1,
    invoking m2mb_usb_isCableAttached( "/dev/USB3" ), only cable on connector 1 is checked.
    Connector 0 is the primary or in case the unique one.
    Currenlty, the primary USB connector 0 has 3 logical USB channels from USB0 to USB2.

  @b
  @code

    int main()
    {
      INT32 is_cable_conn;
      is_cable_conn = m2mb_usb_isCableAttached("/dev/USB0");

      if( is_cable_conn == 1 )
        printf( "USB cable connected");
      else if( is_cable_conn == 0 )
        printf( "USB cable disconnected");
      else
        printf( "Error reading usb connection");
    }

  @endcode

*/
  /*-----------------------------------------------------------------------------------------------*/

INT32 m2mb_usb_isCableAttached( const CHAR *path );


#endif  /* M2M_M2MB_USB_API_H */


