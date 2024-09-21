/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_spi.h

  @brief
    SPI library implementation.

  @details
    The following functions are implemented:
    m2mb_spi_ioctl
    m2mb_spi_open
    m2mb_spi_read
    m2mb_spi_write
    m2mb_spi_close

  @notes
    Dependencies: m2m/m2m_generic/common/m2mb_inc/m2mb_types.h

  @author
    Morgan Deidda

  @date
    16/10/2017
*/

#ifndef M2M_M2MB_SPI_API_H
#define M2M_M2MB_SPI_API_H

/* Global declarations ==========================================================================*/
#ifndef NULL
#define NULL  0
#endif

/* Global enums ==============================================================================*/

enum M2MB_SPI_IOCTL_REQUEST
{
  M2MB_SPI_IOCTL_SET_CFG = 0,                 /* set whole spi device configuration */
  M2MB_SPI_IOCTL_GET_CFG,                     /* get whole spi device configuration */
  M2MB_SPI_IOCTL_SET_SHIFT_MODE,              /* set spi device shift mode */
  M2MB_SPI_IOCTL_GET_SHIFT_MODE,              /* get spi device shift mode */
  M2MB_SPI_IOCTL_SET_CS_POLARITY,             /* set spi device CS polarity */
  M2MB_SPI_IOCTL_GET_CS_POLARITY,             /* get spi device CS polarity */
  M2MB_SPI_IOCTL_SET_CS_MODE,                 /* set spi device CS mode */
  M2MB_SPI_IOCTL_GET_CS_MODE,                 /* get spi device CS mode */
  M2MB_SPI_IOCTL_SET_BYTE_ORDER,              /* set spi device endianness */
  M2MB_SPI_IOCTL_GET_BYTE_ORDER,              /* get spi device endianness */
  M2MB_SPI_IOCTL_SET_CALLBACK_FN,             /* set spi device callback function */
  M2MB_SPI_IOCTL_GET_CALLBACK_FN,             /* get spi device callback function */
  M2MB_SPI_IOCTL_SET_CALLBACK_CTXT,           /* set spi device callback context */
  M2MB_SPI_IOCTL_GET_CALLBACK_CTXT,           /* get spi device callback context */
  M2MB_SPI_IOCTL_SET_CLK_FREQ_HZ,             /* set spi device clock frequency in Hz */
  M2MB_SPI_IOCTL_GET_CLK_FREQ_HZ,             /* get spi device clock frequency in Hz */
  M2MB_SPI_IOCTL_SET_BITS_PER_WORD,           /* set spi device bits per word */
  M2MB_SPI_IOCTL_GET_BITS_PER_WORD,           /* get spi device bits per word */
  M2MB_SPI_IOCTL_SET_CS_CLK_DELAY_CYCLES,     /* set spi device cs_clk_delay_cycles */
  M2MB_SPI_IOCTL_GET_CS_CLK_DELAY_CYCLES,     /* get spi device cs_clk_delay_cycles */
  M2MB_SPI_IOCTL_SET_INTER_WORD_DELAY_CYCLES, /* set spi device inter_word_delay_cycles */
  M2MB_SPI_IOCTL_GET_INTER_WORD_DELAY_CYCLES, /* get spi device inter_word_delay_cycles */
  M2MB_SPI_IOCTL_SET_LOOPBACK_MODE,           /* set spi device loopback mode */
  M2MB_SPI_IOCTL_GET_LOOPBACK_MODE,           /* get spi device loopback mode  */
  M2MB_SPI_IOCTL_NOF_REQ                      /* number of m2mb ioctl requests */
};

/* Global typedefs ==============================================================================*/

/* SPI phase type.
   This type defines the clock phase that the client can set in the
   SPI configuration.
*/
typedef enum
{
  M2MB_SPI_MODE_0,  /* CPOL = 0, CPHA = 0 */
  M2MB_SPI_MODE_1,  /* CPOL = 0, CPHA = 1 */
  M2MB_SPI_MODE_2,  /* CPOL = 1, CPHA = 0 */
  M2MB_SPI_MODE_3,  /* CPOL = 1, CPHA = 1 */
  M2MB_SPI_MODE_INVALID = 0x7FFFFFFF
} M2MB_SPI_SHIFT_MODE_T;

/* SPI chip select polarity type.
*/
typedef enum
{
  M2MB_SPI_CS_ACTIVE_LOW,   /* During Idle state, the CS line is held low */
  M2MB_SPI_CS_ACTIVE_HIGH,  /* During Idle state, the CS line is held high */
  M2MB_SPI_CS_ACTIVE_INVALID = 0x7FFFFFFF
} M2MB_SPI_CS_POLARITY_T;

/* SPI chip select assertion type.
   This type defines how the chip select line is configured between N word cycles.
*/
typedef enum
{
  M2MB_SPI_CS_DEASSERT,        /* CS is deasserted after transferring data for N clock cycles */
  M2MB_SPI_CS_KEEP_ASSERTED,   /* CS is asserted as long as the core is in the Run state */
  M2MB_SPI_CS_MODE_INVALID = 0x7FFFFFFF
} M2MB_SPI_CS_MODE_T;

/* Order in which bytes from Tx/Rx buffer words are put on the bus.
*/
typedef enum
{
  M2MB_SPI_NATIVE = 0,         /* Native */
  M2MB_SPI_LITTLE_ENDIAN = 0,  /* Little Endian */
  M2MB_SPI_BIG_ENDIAN          /* Big Endian (network) */
} M2MB_SPI_BYTE_ORDER_T;

typedef enum
{
  M2MB_SPI_EV
} M2MB_SPI_IND_E;

typedef void ( *m2mb_spi_ind_callback )( INT32 fd, M2MB_SPI_IND_E spi_event, UINT16 resp_size, void *resp_struct, void *userdata );

/* SPI configuration.
   The SPI configuration is the collection of settings specified for each SPI
   transfer call to select the various possible SPI transfer parameters.
*/
typedef struct
{
  M2MB_SPI_SHIFT_MODE_T spi_mode;     /* Shift mode */
  M2MB_SPI_CS_POLARITY_T cs_polarity; /* CS polarity */
  M2MB_SPI_CS_MODE_T cs_mode;         /* CS mode */
  M2MB_SPI_BYTE_ORDER_T endianness;   /* Endianness */
  m2mb_spi_ind_callback callback_fn;  /* Callback function; if NULL, transfer operates in synchronous mode */
  HANDLE callback_ctxt;               /* Pointer to a client object that will be returned as an argument to callback_fn */
  UINT32 clk_freq_Hz;                 /* Host sets the SPI clock frequency closest (>=) to the requested frequency. Must be at least 62500 */
  UINT8 bits_per_word;                /* bits per word; any value from 3 to 31 */
  UINT8 cs_clk_delay_cycles;          /* Number of clock cycles to wait after asserting CS before starting transfer */
  UINT8 inter_word_delay_cycles;      /* Number of clock cycles to wait between SPI words */
  BOOLEAN loopback_mode;              /* Normally 0. If set, the SPI controller will enable Loopback mode;
                                         used primarily for testing */
} M2MB_SPI_CFG_T;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/* m2mb_spi_open:
   description:
     open a SPI device
   arguments:
     path: /dev/spidevX.Y where
           X is the HW SPI master instance index (from 1 to 6). Default is 5.
           Y is the SPI slave device index. Default is 0.
           Path "/dev/spidevX" is equivalent to default "/dev/spidevX.0"
           Path "/dev/spidev" is equivalent to default "/dev/spidev5.0"
     flags: currently unused
   return:
     file descriptor on SUCCESS
     -1 on FAILURE
   i.e.: fd = m2mb_spi_open( "/dev/spidev5.0", 0 );
   Please note that in some modules SPI functionality works only if USIF1 port is not occupied,
   because they share the same pins. In these cases the customer should select a port variant that
   does not contemplate an AT instance on USIF1 port.
*/
INT32 m2mb_spi_open( const CHAR *path, INT32 flags, ... );

/*-----------------------------------------------------------------------------------------------*/
/* m2mb_spi_close:
   description:
     close a SPI device
   arguments:
     fd: file descriptor returned by m2mb_spi_open
   return:
     0 on SUCCESS
    -1 on FAILURE
*/
INT32 m2mb_spi_close( INT32 fd );

/*-----------------------------------------------------------------------------------------------*/
/* m2mb_spi_ioctl:
   description:
     configure a SPI device
   arguments:
     fd: file descriptor returned by m2mb_spi_open
     request: required operation (see M2MB_SPI_IOCTL_REQUEST)
     cfg_ptr: pointer to the set/get val, casted to void*
   return:
     0 on SUCCESS
    -1 on FAILURE
   i.e. m2mb_spi_ioctl(fd, M2MB_SPI_IOCTL_SET_CFG, (void *)&config)
        where config is a M2MB_SPI_CFG_T struct
   Please note that in some modules SPI functionality works only if USIF1 port is not occupied,
   because they share the same pins. In these cases the customer should select a port variant that
   does not contemplate an AT instance on USIF1 port.
*/
INT32 m2mb_spi_ioctl( INT32 fd, INT32 request, ... );

/*-----------------------------------------------------------------------------------------------*/
/* m2mb_spi_read:
   description:
     read nbyte Bytes from a SPI device into the array pointed by buf
   arguments:
     fd: file descriptor returned by m2mb_spi_open
     buf: destination buffer, previously allocated
     nbyte: length of destination buffer in Bytes
   returns:
     number of read Bytes on SUCCESS
     -1 on FAILURE
*/
SSIZE_T m2mb_spi_read( INT32 fd, void *buf, SIZE_T nbyte );

/*-----------------------------------------------------------------------------------------------*/
/* m2mb_spi_write:
   description:
     write nbyte Bytes from the array pointed by buf to a SPI device
   arguments:
     fd: file descriptor returned by m2mb_spi_open
     buf: source buffer, previously allocated
     nbyte: length of source buffer in Bytes
   returns:
     number of written Bytes on SUCCESS
     -1 on FAILURE
*/
SSIZE_T m2mb_spi_write( INT32 fd, const void *buf, SIZE_T nbyte );

/*-----------------------------------------------------------------------------------------------*/
/* m2mb_spi_write_read:
   description:
     perform a bi-directional (full duplex) transfer.
     Read nbyte Bytes from a SPI device into the buffer bufRd
     and write nbyte Bytes from the buffer bufWr to a SPI device
   arguments:
     fd: file descriptor returned by m2mb_spi_open
     bufWr: source buffer, previously allocated
     bufRd: destination buffer, previously allocated
     nbyte: length of source and destination buffers in Bytes
   returns:
     number of written/read Bytes on SUCCESS
     -1 on FAILURE
*/
SSIZE_T m2mb_spi_write_read( INT32 fd, const void *bufWr, void *bufRd, SIZE_T nbyte );

#endif  /* M2M_M2MB_SPI_API_H */


