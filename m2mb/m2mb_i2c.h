/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_i2c.h

  @brief
    I2C library implementation.

  @details
    The following functions are implemented:
    m2mb_i2c_ioctl
    m2mb_i2c_open
    m2mb_i2c_read
    m2mb_i2c_write
    m2mb_i2c_close

  @notes
    Dependencies: m2m/m2m_generic/common/m2mb_inc/m2mb_types.h

  @author
    Morgan Deidda

  @changes
    Enrico Perini

  @date
    31/08/2017
*/

#ifndef M2M_M2MB_I2C_API_H
#define M2M_M2MB_I2C_API_H



/* Global declarations ==========================================================================*/
#ifndef NULL
#define NULL  0
#endif

#define I2C_M_WR              0x0000  /* write data, from master to slave */
#define I2C_M_RD              0x0001  /* read data, from slave to master */

/* Global enums ==============================================================================*/

enum M2MB_I2C_IOCTL_REQUEST
{
  M2MB_I2C_IOCTL_SET_CFG, /* set i2c device configuration */
  M2MB_I2C_IOCTL_GET_CFG, /* get i2c device configuration */
  M2MB_I2C_IOCTL_RDWR     /* i2c combined format */
};

/* Global typedefs ==============================================================================*/

typedef struct
{
  UINT16 flags;     /* I2C_M_WR or I2C_M_RD*/
  UINT16 len;       /* msg length */
  UINT8 *buf;       /* msg data */
} M2MB_I2C_MSG;

/* This is the structure as used in the I2C_RDWR ioctl call */
typedef struct
{
  M2MB_I2C_MSG *msgs;    /* pointers to i2c_msgs */
  UINT32 nmsgs;          /* number of i2c_msgs: 1 if only a write or a read operation is performed; 2 if both operations are performed */
} M2MB_I2C_RDWR_IOCTL_DATA;

/* I2C device configuration struct */
typedef struct
{
  UINT8 sdaPin;
  UINT8 sclPin;
  UINT8 registerId;
  M2MB_I2C_RDWR_IOCTL_DATA *rw_param;
} M2MB_I2C_CFG_T;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    open an I2C device

  @details
    This function opens a I2C device (from 1 to 10)

  @param[in] path
    the path has a string of this type: /dev/I2C-# where # is in decimal format
    I2C address is in 8 bit format, where last bit is the R/W flag
  @param[in] flags
    currently unused

  @return
    file descriptor on SUCCESS
    -1 on FAILURE

  @b
    Example
  @code
    fd = m2mb_i2c_open( "/dev/I2C-212", 0 ); // device address 0xD4 (8 bit format)
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

INT32 m2mb_i2c_open( const CHAR *path, INT32 flags, ... );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    close an I2C device

  @details
    This function closes a I2C device (from 1 to 10)

  @param[in] fd
    file descriptor returned by m2mb_i2c_open

  @return
    0 on SUCCESS
    negative number on FAILURE
  @b
    Example
  @code
    ret = m2mb_i2c_close( fd );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_i2c_close( INT32 fd );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    configure, read from and/or write to an I2C device
  @details
    Depending on request parameter, this function can configure an I2C device, or can send and/or receive data through I2C combined format

  @param[in] fd
    file descriptor returned by m2mb_i2c_open
  @param[in] request
    required operation (see M2MB_I2C_IOCTL_REQUEST)
  @param[in] cfg
    pointer to the configuration struct M2MB_I2C_CFG_T casted to void*
    The parameters depend on request code:
    if request == M2MB_I2C_IOCTL_SET_CFG: SDA and SCL pins are mapped on GPIOs,
    and the register of the device from which reading or to which writing is set;
    in struct M2MB_I2C_CFG_T, sdaPin, sclPin and registerId are mandatory
    if request == M2MB_I2C_IOCTL_RDWR: reading and writing through I2C combined format;
    in struct M2MB_I2C_CFG_T, rw_param is mandatory
    if request == M2MB_I2C_IOCTL_GET_CFG: a pointer to the structure M2MB_I2C_CFG_T previously set is got

  @return
    0 on SUCCESS if request == M2MB_I2C_IOCTL_SET_CFG or request == M2MB_I2C_IOCTL_GET_CFG
    number of written Bytes on SUCCESS if request == M2MB_I2C_IOCTL_RDWR and the function is used only for writing
    number of read Bytes on SUCCESS if request == M2MB_I2C_IOCTL_RDWR and the function is used only for reading or both for writing and reading
    -1 on FAILURE

  @b
    Example
  @code
    M2MB_I2C_CFG_T i2c_data;
    i2c_data.sdaPin = 2; //SDA pin on GPIO_02
    i2c_data.sclPin = 3; //SCL pin on GPIO_03
    i2c_data.registerId = 4; //register to write to or to read from: 0x04
    i2c_data.rw_param = (M2MB_I2C_RDWR_IOCTL_DATA*) m2mb_os_malloc( sizeof(M2MB_I2C_RDWR_IOCTL_DATA) );

    Ret = m2mb_i2c_ioctl( fd, M2MB_I2C_IOCTL_SET_CFG, (void *)&i2c_data );
    if ( Ret )
    {
      m2mb_trace_printf(M2MB_TC_M2M_USER, M2MB_TL_ERROR, " m2mb_i2c_ioctl() - I2C FAIL" );
      testErrs = 1;
    }
  @endcode
  @code
    //write 2 bytes starting from register 0x04
    M2MB_I2C_CFG_T i2c_data;
    i2c_data.registerId = 0x4;
    memset( iicbuf_wr, 0x00, 256 );
    memset( iicbuf_rd, 0x00, 256 );
    iicbuf_wr[0] = i2c_data.registerId;  //first send register id
    iicbuf_wr[1] = 0xaa;  //data to write in register 0x04
    iicbuf_wr[2] = 0xbb;  //data to write to register 0x05
    msgs[0].buf = iicbuf_wr;
    msgs[0].flags = I2C_M_WR;
    msgs[0].len = 3; //send 1 data to external device
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 0; //read 10 bytes starting fom 0x4 register
    msgs[1].buf = iicbuf_rd;
    i2c_data.rw_param->msgs = &msgs[0];
    i2c_data.rw_param->nmsgs = 1; //both writing and reading
    IICres =	m2mb_i2c_ioctl(fd, M2MB_I2C_IOCTL_RDWR, (void *)&i2c_data);
  @endcode
  @code
    //read 2 bytes starting fom 0x4 register
    M2MB_I2C_CFG_T i2c_data;
    i2c_data.registerId = 0x4;
    memset( iicbuf_wr, 0x00, 256 );
    memset( iicbuf_rd, 0x00, 256 );
    iicbuf_wr[0] = i2c_data.registerId;  //first send register id
    msgs[0].buf = iicbuf_wr;
    msgs[0].flags = I2C_M_WR;
    msgs[0].len = 1; //send 1 data to external device
    msgs[1].flags = I2C_M_RD;
    msgs[1].len = 2;
    msgs[1].buf = iicbuf_rd;
    i2c_data.rw_param->msgs = &msgs[0];
    i2c_data.rw_param->nmsgs = 2; //both writing and reading
    IICres =	m2mb_i2c_ioctl(fd, M2MB_I2C_IOCTL_RDWR, (void *)&i2c_data);
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_i2c_ioctl( INT32 fd, INT32 request, ... );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    read from an I2C device

  @details
    This function reads nbyte Bytes from a I2C device into the array pointed to by buf

  @param[in] fd
    file descriptor returned by m2mb_i2c_open
  @param[in] buf
    destination buffer, previously allocated
  @param[in] nbyte
    length of destination buffer in Bytes

  @return
    number of read Bytes on SUCCESS
    -1 on FAILURE

  @b
    Example
  @code
    IICres = m2mb_i2c_read(fd, iicbuf, 32);
    if ( IICres == -1 ) m2mb_trace_printf(M2MB_TC_M2M_USER, M2MB_TL_ERROR, "I2C read FAIL" );
    else
    {
      if ( IICres != 32 )
        m2mb_trace_printf(M2MB_TC_M2M_USER, M2MB_TL_ERROR, "I2C read FAIL" );
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
SSIZE_T m2mb_i2c_read( INT32 fd, void *buf, SIZE_T nbyte );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    write to an I2C device

  @details
    This function writes nbyte Bytes from the array pointed to by buf to a I2C device

  @param[in] fd
    file descriptor returned by m2mb_i2c_open
  @param[in] buf
    source buffer, previously allocated
  @param[in] nbyte
    length of source buffer in Bytes

  @return
    number of written Bytes on SUCCESS
    -1 on FAILURE

  @b
    Example
  @code
    //writes 2 bites from register set with m2mb_i2c_ioctl
    memset( iicbuf_wr, 0x00, 256 );
    iicbuf_wr[0] = 0;
    iicbuf_wr[1] = 16;
    IICres = m2mb_i2c_write(fd, iicbuf_wr, 2);
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
SSIZE_T m2mb_i2c_write( INT32 fd, const void *buf, SIZE_T nbyte );


#endif  /* M2M_M2MB_I2C_API_H */

