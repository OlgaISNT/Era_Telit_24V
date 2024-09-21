/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_gpio.h

  @brief
    GPIO library implementation.

  @details
    The following functions are implemented:
    m2mb_gpio_ioctl
    m2mb_gpio_multi_ioctl
    m2mb_gpio_open
    m2mb_gpio_read
    m2mb_gpio_write
    m2mb_gpio_close

  @notes
    Dependencies: m2m/m2m_generic/common/m2mb_inc/m2mb_types.h

  @author
    Morgan Deidda

  @changes
    Dino Apostoli

  @date
    31/08/2017
*/

#ifndef M2M_M2MB_GPIO_API_H
#define M2M_M2MB_GPIO_API_H


/* Global declarations ==========================================================================*/
#ifndef NULL
#define NULL  0
#endif

/* Global defines ==============================================================================*/


/*************************************************************************************************************/
/* use of gpio old interface is DEPRECATED for new project: enable it only for old already deployed project  */
/* If an application from APPZONE is used, a good solution for using old GPIO is to create a                 */
/*  makefile.in file and write in it the define: CPPFLAGS=-DENABLE_GPIO_OLD_INTERFACE                        */
/* instead of using the #define ENABLE_GPIO_OLD_INTERFACE below                                              */
/*************************************************************************************************************/
/* #define ENABLE_GPIO_OLD_INTERFACE */


/**
 * @brief LE910 products have 10 GPIOs
 */

#define MAX_GPIO_PIN_NBR  11

/* Global enums ==============================================================================*/

typedef enum
{
  M2MB_GPIO_IOCTL_SET_DIR,          /**< Set gpio device direction configuration */
  M2MB_GPIO_IOCTL_GET_DIR,          /**< Get gpio device direction configuration */
  M2MB_GPIO_IOCTL_SET_PULL,         /**< Set gpio device pull configuration */
  M2MB_GPIO_IOCTL_GET_PULL,         /**< Get gpio device pull configuration */
  M2MB_GPIO_IOCTL_SET_DRIVE,        /**< Set gpio pin drive strength */
  M2MB_GPIO_IOCTL_GET_DRIVE,        /**< Get gpio pin drive strength */
  M2MB_GPIO_IOCTL_INIT_INTR,        /**< Registers a callback/counter for a GPIO interrupt */
  M2MB_GPIO_IOCTL_DEINIT_INTR,      /**< Deregisters a callback/counter Interrupt service */
  M2MB_GPIO_IOCTL_SET_INTR_TYPE,    /**< Set the interrupt type: INTR_CB_SET, INTR_COUNTER_SET or
                                         INTR_COUNTER_SET_HP, see M2M_GPIO_INTR_TYPE_E */
  M2MB_GPIO_IOCTL_GET_INTR_TYPE,    /**< Get the interrupt type: INTR_CB_SET or INTR_COUNTER_SET */
  M2MB_GPIO_IOCTL_SET_INTR_CB,      /**< Set the callback function for the interrupt service */
  M2MB_GPIO_IOCTL_GET_INTR_CB,      /**< Get the callback function used for the interrupt service */
  M2MB_GPIO_IOCTL_SET_INTR_ARG,     /**< Argument to be passed in the callback function of the interrupt */
  M2MB_GPIO_IOCTL_GET_INTR_ARG,     /**< Get the Argument of the callback function of the interrupt  */
  M2MB_GPIO_IOCTL_SET_INTR_TRIGGER, /**< Set trigger. N.B.Can dynamically sets the trigger type of
                                         a registered GPIO interrupt */
  M2MB_GPIO_IOCTL_GET_INTR_TRIGGER, /**< Get the trigger type of a registered GPIO interrupt, as in
                                         M2MB_GPIO_TRIGGER_E */
  M2MB_GPIO_IOCTL_SET_ISR_COUNTER,  /**< Set ISR interrupt counter value */
  M2MB_GPIO_IOCTL_GET_ISR_COUNTER,  /**< Read ISR interrupt counter value, counter has UINT32 size,
                                         is auto reset to 0 in case of overflow */
  ENUM_TO_INT( M2MB_GPIO_IOCTL_REQUEST_E )
} M2MB_GPIO_IOCTL_REQUEST_E;

/*!
 * @brief enum used for gpio direction
 */

typedef enum
{
  M2MB_GPIO_MODE_INPUT = 0,               /**< GPIO mode : input   */
  M2MB_GPIO_MODE_OUTPUT = 1,              /**< GPIO mode : output  */
  ENUM_TO_INT( M2MB_GPIO_DIRECTION_E )
} M2MB_GPIO_DIRECTION_E;

typedef enum
{
  M2MB_GPIO_INTR_POSEDGE,         /**< Rising edge triggered, to trigger when the pin goes from low to high, */
  M2MB_GPIO_INTR_NEGEDGE,         /**< Falling edge triggered. */
  M2MB_GPIO_INTR_ANYEDGE,         /**< Dual edge triggered, to trigger the interrupt whenever the pin changes value*/
  ENUM_TO_INT( M2MB_GPIO_TRIGGER_E )
} M2MB_GPIO_TRIGGER_E;

typedef enum
{
    M2MB_GPIO_NO_PULL,                  /**< Specify no pull        */
    M2MB_GPIO_PULL_DOWN,                /**< Gpio pull down         */
    M2MB_GPIO_PULL_KEEPER,              /**< Keep the GPIO as it is */
    M2MB_GPIO_PULL_UP,                  /**< Gpio pull up           */
    ENUM_TO_INT( M2MB_GPIO_PULL_MODE_E )
} M2MB_GPIO_PULL_MODE_E;

/*!
 * @brief GPIO pin drive strength.
 *
 * @details This enumeration specifies the drive strength to use when specifying
 *          the configuration of a GPIO pin.
*/
typedef enum
{
  M2MB_GPIO_LOW_DRIVE      = 0,    /**< Specify a low drive. */
  M2MB_GPIO_MEDIUM_DRIVE   = 0x1, /**< Specify a medium drive. */
  M2MB_GPIO_HIGH_DRIVE     = 0x2, /**< Specify a high drive. */
  ENUM_TO_INT( M2MB_GPIO_DRIVE_E )

}M2MB_GPIO_DRIVE_E;

/*!
  @brief GPIO output state specification.
*/

typedef enum
{
  M2MB_GPIO_LOW_VALUE = 0,     /**< Drive the output LOW. */
  M2MB_GPIO_HIGH_VALUE = 1,    /**< Drive the output HIGH. */
  ENUM_TO_INT( M2MB_GPIO_VALUE_E )

}M2MB_GPIO_VALUE_E;

/*!
  @brief GPIO ISR type: callback or counter
*/

typedef enum
{
  INTR_CB_SET = 0,                /**< Interrupt  set with a callback function. */
  INTR_COUNTER_SET,               /**< Interrupt  set a counter for interrupt and no callback,
                                       counter has UINT32 size, is auto reset to 0 in case of overflow */
  INTR_COUNTER_SET_HP,            /**< No support. This interrupt type is not available on LE910Cx Linux products */
  ENUM_TO_INT( M2M_GPIO_INTR_TYPE_E )
}M2M_GPIO_INTR_TYPE_E;



/*!
 * @brief GPIO interrupt callback function definition.
 *
 *  GPIO interrupt clients will pass a function pointer of
 *       this format in M2MB_GPIO_CFG_T callback function
 *
*/

typedef void ( *m2mb_gpio_intr_callback )( UINT32 fd,  void *userdata );

/* Global functions =============================================================================*/


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    open a GPIO device

  @details
    This function open a GPIO device (from 1 to 10)

  @param[in] path
    the path has a string of this type: /dev/GPIO# where # is in decimal format
  @param[in] flags
    currently unused

  @return
    file descriptor on SUCCESS, 1 on FAILURE

  @b
    Example
  @code
    fd = m2mb_gpio_open( "/dev/GPIO7", 0 );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

INT32 m2mb_gpio_open( const CHAR *path, INT32 flags, ... );


/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     close a GPIO device

   @details
     This function close a GPIO device

   @param[in] fd
    file descriptor returned by m2mb_gpio_open

   @return
     0 on SUCCESS, -1 on FAILURE
*/
/*-----------------------------------------------------------------------------------------------*/


INT32 m2mb_gpio_close( INT32 fd );


/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     configure a GPIO device

   @details
    This function configure a GPIO using the M2MB_GPIO_IOCTL_REQUEST_E enum

   @param[in] fd
      file descriptor returned by m2mb_gpio_open
   @param[in] cmd
      required operation (see M2MB_GPIO_IOCTL_REQUEST_E)
   @param[in] arg
      pointer to the value to set

   @return
     0 on SUCCESS
    -1 on FAILURE

  @note
    In order to use interrupts it is mandatory to initialize the interrupt with M2MB_GPIO_IOCTL_INIT_INTR
    and M2MB_GPIO_IOCTL_INIT_INTR must be the last m2mb_gpio_ioctl() call


  @b
    Example

  @code
    example of usage
    INT32  ret;
    fd = m2mb_gpio_open( "/dev/GPIO1", 0 );

    if( fd != -1 )
    {
      ret = m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_INPUT );
      if ( ret == -1 )
        return ;

      ret = m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_PULL_UP );
      ret = m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_MEDIUM_DRIVE );
    }
  @endcode
  @code
    example of usage for a interrupt callback:
    /// callback ////
    static void myUptrigg_callbk(UINT32 fd,  void *userdata )
    {
      ....
    }

    .....
    ///// code ///
    m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_SET_INTR_TRIGGER,  M2MB_GPIO_INTR_POSEDGE );
    m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_SET_INTR_ARG,  (UINT32)str_gpio );
    m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_SET_INTR_CB,  (UINT32)myUptrigg_callbk );
    m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_SET_INTR_TYPE,  INTR_CB_SET );
    m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_SET_ISR_COUNTER,  0 );
    m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_INIT_INTR,  NULL );

    //In order to use interrupts it is mandatory to initialize the interrupt with M2MB_GPIO_IOCTL_INIT_INTR
    //M2MB_GPIO_IOCTL_INIT_INTR must be the last m2mb_gpio_ioctl


  @endcode

  @code
  example of usage for get data: Must be used with the reference or pointer

  M2MB_GPIO_DIRECTION_E dir;
  M2MB_GPIO_PULL_MODE_E pull;


  ret = m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_GET_DIR, (UINT32)&dir );
  if( ret )
    ......

  ret = m2mb_gpio_ioctl( fd, M2MB_GPIO_IOCTL_GET_PULL, (UINT32)&pull );

  if( ret )
    ......

  @code
  example of usage for set and get interrupt argument
  CHAR str_test[] = "hello";
  CHAR *ptr_str;

  m2mb_gpio_ioctl(fd2, M2MB_GPIO_IOCTL_SET_INTR_ARG, (UINT32)str_test);
  ret = m2mb_gpio_ioctl(fd2, M2MB_GPIO_IOCTL_GET_INTR_ARG, (UINT32)&ptr_str);

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/


INT32 m2mb_gpio_ioctl( INT32 fd, INT32 cmd, UINT32 arg);


/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief:
     multiple configuration of a GPIO device.

   @details
     allow a multiple configuration of a GPIO device. Can not be used in order to get data.
     In order to use interrupts it is mandatory to initialize the interrupt with M2MB_GPIO_IOCTL_INIT_INTR

   @param[in] fd
     file descriptor returned by m2mb_gpio_open
   @param[in] nCmds
     number of commands to be issued at once
   @param[in] ...
     variable inputs, N cmds (see M2MB_GPIO_IOCTL_REQUEST_E) followed by N arguments
     for their respective issued commands. CMDS(...) and CMDS_ARGS(...) macro can be
     used to better insert cmd and parameters and to better control right insertion

   @return
     0 on SUCCESS
    -1 on FAILURE

  @b
    Example

  @code
    example of usage for PMM:
    ret =  m2mb_gpio_multi_ioctl( fd1, CMDS_ARGS( M2MB_GPIO_IOCTL_SET_DIR, M2MB_GPIO_MODE_OUTPUT,
                                    M2MB_GPIO_IOCTL_SET_PULL, M2MB_GPIO_PULL_UP,
                                    M2MB_GPIO_IOCTL_SET_DRIVE, M2MB_GPIO_MEDIUM_DRIVE ) );
    if ( ret )
    {
      m2mb_trace_printf(M2MB_TC_M2M_USER, M2MB_TL_ERROR, "GPIO1 FAIL" );
    }
  @endcode
  @code
    example of usage for a interrupt callback:
    INT32 ret;

    ret =  m2mb_gpio_multi_ioctl( fd1, CMDS_ARGS( M2MB_GPIO_IOCTL_INIT_INTR, NULL,
                                        M2MB_GPIO_IOCTL_SET_INTR_TYPE, INTR_CB_SET,
                                        M2MB_GPIO_IOCTL_SET_INTR_CB, myUptrigg_callbk,
                                        M2MB_GPIO_IOCTL_SET_INTR_ARG, str_gpio,
                                        M2MB_GPIO_IOCTL_SET_INTR_TRIGGER, M2MB_GPIO_INTR_POSEDGE
                                    ) );

    if( ret == -1 )
    {
      m2mb_trace_printf(M2MB_TC_M2M_USER, M2MB_TL_ERROR, "GPIO1 - FAIL" );
    }

  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/


INT32 m2mb_gpio_multi_ioctl( INT32 fd, UINT8 nCmds, ... );


/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     Reads the state of a GPIO pin

   @details
     Reads the state of a GPIO pin, used only for M2MB_GPIO_MODE_INPUT

   @param[in] fd
     file descriptor returned by m2mb_gpio_open
   @param[out] value
     get the M2MB_GPIO_VALUE_E LOW or HIGH

   @return
     0 on SUCCESS
     -1 on FAILURE

   @b
     Example

   @code
     M2MB_GPIO_VALUE_E value_read;

     ret = m2mb_gpio_read(fd, &value_read );

   @endcode

*/
/*-----------------------------------------------------------------------------------------------*/

INT32 m2mb_gpio_read( INT32 fd, M2MB_GPIO_VALUE_E *value );



/*-----------------------------------------------------------------------------------------------*/
/*!
   @brief
     write the value of a GPIO device

   @details
     write the value high or low to a GPIO device, used only for M2MB_GPIO_MODE_OUTPUT

   @param[in] fd
     file descriptor returned by m2mb_gpio_open
   @param[in] value
     set high or low

   @return
      0 on SUCCESS
     -1 on FAILURE
   @code
     ret = m2mb_gpio_write( fd, M2MB_GPIO_LOW_VALUE );
   @endcode

*/
/*-----------------------------------------------------------------------------------------------*/

INT32 m2mb_gpio_write( INT32 fd, M2MB_GPIO_VALUE_E value );


#ifdef ENABLE_GPIO_OLD_INTERFACE

/*************************************************************/
/* OLD GPIO INTERFACE                                        */
/*************************************************************/
// #warning "**** WARNING!  ENABLE_GPIO_OLD_INTERFACE defined! "

/* Global enums ==============================================================================*/

enum M2MB_GPIO_IOCTL_REQUEST
{
  M2MB_GPIO_IOCTL_SET_CFG, /* set gpio device configuration */
  M2MB_GPIO_IOCTL_GET_CFG  /* get gpio device configuration */
};

/* Global typedefs ==============================================================================*/

/* GPIO device configuration struct */
typedef struct
{
  UINT8 direction;
  UINT8 save_cfg;
} M2MB_GPIO_CNFG_T;


INT32 m2mb_gpio_ioctl_wrp( INT32 fd, INT32 request, ... );


SSIZE_T m2mb_gpio_read_wrp( INT32 fd, void *buf, SIZE_T nbyte );


SSIZE_T m2mb_gpio_write_wrp( INT32 fd, const void *buf, SIZE_T nbyte );


#define m2mb_gpio_read( fd, value, nbyte )     m2mb_gpio_read_wrp( fd, ( void * )value, nbyte )
#define m2mb_gpio_write( fd, value, nbyte )    m2mb_gpio_write_wrp( fd, ( void * )value, nbyte )
#define m2mb_gpio_ioctl( a... )                m2mb_gpio_ioctl_wrp( a )
#define M2MB_GPIO_CFG_T     M2MB_GPIO_CNFG_T



#endif  /* ENABLE_GPIO_OLD_INTERFACE */



#endif  /* M2M_M2MB_GPIO_API_H */


