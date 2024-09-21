/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_appMng.h

  @brief
    App manager prototypes
  @details
    Basic functions for App manager are provided

  @notes
    Dependencies:
        m2mb_types.h
        m2m_os_types.h

  @author
    Goran Paic

  @date
    29/01/2019
*/
#ifndef M2MB_APPMNG_H
#define M2MB_APPMNG_H
/* Include ======================================================================================*/


/* Global declarations ==========================================================================*/

#define  M2MB_APPMNG_INVALID                0

#define  M2MB_APPMNG_AUTOSTART              1
#define  M2MB_APPMNG_NOT_AUTOSTART          0

/* max length for each argument */
#define M2MB_APPMNG_MAX_ARG_LEN             32
/* max numbers of args per app allowed */
#define M2MB_APPMNG_MAX_ARG_NUM             10
/* max delay start that can be set */
#define M2MB_APPMNG_MAX_DELAY               60
/* application file extension must be APP_FILE_EXTENSION */
#define APP_FILE_EXTENSION    ".bin"
/* max and minimum filename lenght (extension included) for application file */
#define M2MB_APPMNG_MIN_FILEBIN_LEN         5
#define M2MB_APPMNG_MAX_FILEBIN_LEN         64

#define M2MB_APPMNG_MOD_DIR                 "/data/azc/mod/"
/* the main application manager directory */

/* Global typedefs ==============================================================================*/

/* error for App Manager */
typedef enum M2MB_APPMNG_RESULT
{
  M2MB_APPMNG_RESULT_SUCCESS,
  M2MB_APPMNG_RESULT_ERROR,
  M2MB_APPMNG_RESULT_PTR_ERROR,
  M2MB_APPMNG_RESULT_NO_MEMORY,
  M2MB_APPMNG_RESULT_START_ERROR,
  M2MB_APPMNG_RESULT_STOP_ERROR,
  M2MB_APPMNG_RESULT_SET_ERROR,
  M2MB_APPMNG_RESULT_GET_ERROR,
  M2MB_APPMNG_RESULT_INVALID_STOP,
  M2MB_APPMNG_RESULT_INVALID_START,
  /* generic error to analyse */
  M2MB_APPMNG_RESULT_GENERIC_ERROR,
  /* provided args on API do not match expected values */
  M2MB_APPMNG_RESULT_INVALID_ARG,
  /* virtual address of application is not valid or already in use */
  M2MB_APPMNG_RESULT_INVALID_ADDRESS,
  /* not enough RAM memory available */
  M2MB_APPMNG_RESULT_ERR_AVAILABLE_RAM,
  /* app file not found in FS: maybe deleted after being inserted in execution list */
  M2MB_APPMNG_RESULT_ERR_NO_ENTRY,
  /* internal init error */
  M2MB_APPMNG_RESULT_ERR_FILEINI,
  /* no applications in execution list */
  M2MB_APPMNG_RESULT_ERR_EMPTY_LIST,
  /* application not in execution list as executable */
  M2MB_APPMNG_RESULT_NOT_EXE,
  /* integrity check on binary file fails */
  M2MB_APPMNG_RESULT_ERR_CRC,
  M2MB_APPMNG_RESULT_DIFF_CRC_PREAMBLE,
  /* error during update parameter procedure */
  M2MB_APPMNG_RESULT_UPDATE_ERROR,
  /* wrong arg format */
  M2MB_APPMNG_RESULT_ARG_FORMAT_ERROR,
  /* arglen exceeded maximum value or 0 */
  M2MB_APPMNG_RESULT_LEN_ARG_ERROR,
  /* command not supported, e.g app stop for old version in place */
  M2MB_APPMNG_RESULT_NOT_SUPPORTED,

  M2MB_APPMNG_RESULT_ERR_UNKNOWN = 255,
  M2MB_APPMNG_RESULT_END,

  ENUM_TO_INT( M2MB_APPMNG_RESULT_E )
} M2MB_APPMNG_RESULT_E;

typedef enum
{
  /* lenght name of app */
  M2MB_APPMNG_SEL_CMD_LNAME_APP,
  /* app name */
  M2MB_APPMNG_SEL_CMD_NAME,
  /* not allowed set in user space: checks automatically also integrity: fail in case of integrity error
     it is current virtual address if app is in running, otherwise it is the address the application will request to run
  */
  M2MB_APPMNG_SEL_CMD_ADDRESS,
  /* only get: total RAM occupied by app in case of running; RAM that will be required to run in case of app not
     running with about 8K boundary tolerance. Automatic check of integrity is performed: fail in case of integrity error
  */
  M2MB_APPMNG_SEL_CMD_RAM,
  /* number of total args */
  M2MB_APPMNG_SEL_CMD_TOT_ARGS,
  /* N arg selected, in get or set, it starts from index 1, index 0 is reserved for name of app ( index 0 => arg[0] )
  the max length of arg[0] is M2MB_APPMNG_MAX_FILEBIN_LEN */
  M2MB_APPMNG_SEL_CMD_ARG_N,
  /* Only for get len of specified arg N: arglen of arg N. Args must be passed as string terminating with NULL carachter'\0'.
     MAX ARG LEN IS M2MB_APPMNG_MAX_ARG_LEN.
  */
  M2MB_APPMNG_SEL_CMD_ARGLEN_N,
  /* current delay, volatile value, used to delay start or stop of the application */
  M2MB_APPMNG_SEL_CMD_DELAY,
  /* permanent saved value of delay in  *ini file */
  M2MB_APPMNG_SEL_CMD_DELAY_INI,
  /* run flag in file *.ini : permanent  */
  M2MB_APPMNG_SEL_CMD_SET_EXE_AUTO_INI,
  M2MB_APPMNG_SEL_CMD_STATE,
  /* set or get more arguments separated by comma, at once as unique string */
  M2MB_APPMNG_SEL_CMD_ARGS,
  /* delete all arguments: only for set */
  M2MB_APPMNG_SEL_CMD_ARGS_DELETE,

  /* ---- */
  M2MB_APPMNG_SEL_CMD_END,
  ENUM_TO_INT( M2MB_APPMNG_SEL_CMD_E )
} M2MB_APPMNG_SEL_CMD_E;

typedef enum
{
  /* application ready to start */
  M2MB_APPMNG_STATE_READY,
  /* application is in the starting process, but it's not yet in the run state( start with delay ) */
  M2MB_APPMNG_STATE_STARTING,
  /* application is in the run state */
  M2MB_APPMNG_STATE_RUN,
  /* application is in the stopping process, but it's not yet in the stop state( stop with delay ) */
  M2MB_APPMNG_STATE_STOPPING,
  /* application is in the stop state */
  M2MB_APPMNG_STATE_STOP,

  ENUM_TO_INT( M2MB_APPMNG_STATE_E )
} M2MB_APPMNG_STATE_E;

typedef struct M2MB_APPMNG_HANDLE_TAG      *M2MB_APPMNG_HANDLE;

/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
 /*!

  @brief
    m2mb_appMng_add

  @details
    Add an application file as executable in the permanent list and save the properties.
    Used to add an application as executable, provided it is present in file system in default folder
    /mod

  @param [in] pAppMngHandle
    pointer to handle to get
  @param [in] appName
    filename of the application to add as executable
  @param [in] delayStart
    delay start after reboot

  @return
    M2MB_APPMNG_RESULT_SUCCESS in case of success
  @return
    see M2MB_APPMNG_RESULT_E in case of error

  @note
    File is first checked for integrity, and provide error in case of fail
    Adding a new valid app, automatically the run property
    is M2MB_APPMNG_SEL_CMD_SET_EXE_AUTO_INI = M2MB_APPMNG_AUTOSTART
    so the app starts after reboot. To avoid that, set M2MB_APPMNG_NOT_AUTOSTART using
    m2mb_appMng_setItem
  @b

  @code

  M2MB_APPMNG_HANDLE appMngHandle;
  M2MB_APPMNG_RESULT_E res;

  res = m2mb_appMng_add( &appMngHandle, "app1.bin",5 );

  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_add failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_add success" );
  }


  @endcode

  */
/*-----------------------------------------------------------------------------------------------*/

M2MB_APPMNG_RESULT_E m2mb_appMng_add( M2MB_APPMNG_HANDLE *pAppMngHandle, CHAR* appName, UINT32 delayStart );

/*-----------------------------------------------------------------------------------------------*/
  /*!

  @brief
    m2mb_appMng_del

  @details
    immediate stop of application (delay 0) and delete the app from permanent list of executables

  @param [in] appMngHandle
    handle of the application to be stopped and deleted from the permanent list as executable

  @return
    M2MB_APPMNG_RESULT_SUCCESS in case of success
  @return
    see M2MB_APPMNG_RESULT_E in case of error

  @note
    It is up to the user application responsibility not usage of appMngHandle after it is deleted.

  @b

  @code

  M2MB_APPMNG_HANDLE appMngHandle;
  M2MB_APPMNG_RESULT_E res;

  appMngHandle = m2mb_appMng_getHandleByName( "m2mapz.bin" );
  if( appMngHandle!= M2MB_APPMNG_INVALID )
  {
    // immediate stop app and delete from executable.
    m2mb_appMng_del( appMngHandle );
  }

  @endcode

  */
/*-----------------------------------------------------------------------------------------------*/

M2MB_APPMNG_RESULT_E m2mb_appMng_del( M2MB_APPMNG_HANDLE appMngHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!

  @brief
    m2mb_appMng_start

  @details
    This service starts the specified application with appMngHandle and its properties.
    The delay must be set carefully, it uses the last volatile delay.
    If the volatile delay is not set, it uses delay to start from permanent value.
    To start immediately, set delay volatile to 0 before

  @param [in] appMngHandle
    handle of application

  @return
    M2MB_APPMNG_RESULT_SUCCESS in case of success
  @return
    see M2MB_APPMNG_RESULT_E in case of error

  @note

  @b

  @code

  M2MB_APPMNG_HANDLE appMngHandle;
  M2MB_APPMNG_RESULT_E res;

  // get handle appMngHandle with getHandle API

  res = m2mb_appMng_start( appMngHandle );

  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_start failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_start success" );
  }

  @endcode

*/

/*-----------------------------------------------------------------------------------------------*/

M2MB_APPMNG_RESULT_E m2mb_appMng_start( M2MB_APPMNG_HANDLE appMngHandle );

/*-----------------------------------------------------------------------------------------------*/

/*!

  @brief
    m2mb_appMng_stop

  @details
    This service stops the specified application. If the app is already stopped,
    this service has no effect and return error.
    The application will be stopped after delay specified in last volatile delay
    ( see M2MB_APPMNG_SEL_CMD_DELAY command that set volatile delay).
    In case of success, application is deregistered and the relative resources released.

  @param [in] appMngHandle
    handle of application

  @return
    M2MB_APPMNG_RESULT_SUCCESS in case of success
  @return
    see M2MB_APPMNG_RESULT_E in case of error

  @note

  @b

  @code

  M2MB_APPMNG_HANDLE appMngHandle;
  M2MB_APPMNG_RESULT_E res;

  // get handle appMngHandle with getHandle API

  res = m2mb_appMng_stop( appMngHandle );

  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_stop failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_stop success" );
  }

  @endcode

*/

/*-----------------------------------------------------------------------------------------------*/

M2MB_APPMNG_RESULT_E m2mb_appMng_stop( M2MB_APPMNG_HANDLE appMngHandle );

/*-----------------------------------------------------------------------------------------------*/

/*!

  @brief
    m2mb_appMng_setItem

  @details
    This service set value for specified parameter of the specified application if available.
    Selecting cmd, a parameter to be set can be selected.
    It's possible to set:
        - delay of app[ seconds ]
        - auto start [ M2MB_APPMNG_NOT_AUTOSTART/M2MB_APPMNG_AUTOSTART ( 0/1 ) ]
        - length of argument [ number of characters ]
        - argument [ with pOption you can choose the argument specified, it's possible to change already existing arg
          or adding only one new arg ]. The argument are separated by commas so the comma is not supported inside a single agument.
          The first argument starts with the index 1.
        - set more than one argument at once, the arguments are separated by comma.The empty space and the quotes are seen as normal characters.
        - delete all arguments with one command

  @param [in] appMngHandle
    handle of application
  @param [in] selCmd
    Select parameter to be set (see M2MB_APPMNG_SEL_CMD_E)
  @param [in] pIn
    It is used generic pointer to void to cover all cases; can be used pointer ( name ) or direct value  ( delay, auto start, length of argument )
    depending by command
  @param [in] pOption

  @return
    M2MB_APPMNG_RESULT_SUCCESS in case of success
  @return
    see M2MB_APPMNG_RESULT_E in case of error

  @note
    Not all the M2MB_APPMNG_SEL_CMD_E are always available.
    Allowed:
      M2MB_APPMNG_SEL_CMD_DELAY
      M2MB_APPMNG_SEL_CMD_DELAY_INI
      M2MB_APPMNG_SEL_CMD_SET_EXE_AUTO_INI
      M2MB_APPMNG_SEL_CMD_ARG_N: pOption start from 1 as first arg
      M2MB_APPMNG_SEL_CMD_ARGS
      M2MB_APPMNG_SEL_CMD_ARGS_DELETE

  @b

  @code

  M2MB_APPMNG_HANDLE appMngHandle;
  M2MB_APPMNG_RESULT_E res;

  // get handle appMngHandle with getHandle API

  res = m2mb_appMng_setItem( appMngHandle, M2MB_APPMNG_SEL_CMD_DELAY, ( void* )5, NULL );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_setItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_setItem success " );
  }

  res = m2mb_appMng_setItem( appMngHandle, M2MB_APPMNG_SEL_CMD_DELAY_INI, ( void* )5, NULL );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_setItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_setItem success " );
  }

  res = m2mb_appMng_setItem( appMngHandle, M2MB_APPMNG_SEL_CMD_SET_EXE_AUTO_INI, ( void* )M2MB_APPMNG_AUTOSTART, NULL );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_setItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_setItem success " );
  }

  res = m2mb_appMng_setItem( appMngHandle, M2MB_APPMNG_SEL_CMD_ARG_N, ( void* )"HelloWorld",( void* ) 2 );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_setItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_setItem success " );
  }

  res = m2mb_appMng_setItem( appMngHandle, M2MB_APPMNG_SEL_CMD_ARGS, ( void* )"one,two,three", NULL );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_setItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_setItem success " );
  }

  res = m2mb_appMng_setItem( appMngHandle, M2MB_APPMNG_SEL_CMD_ARGS_DELETE, NULL, NULL );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_setItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_setItem success " );
  }

  @endcode

*/

/*-----------------------------------------------------------------------------------------------*/

M2MB_APPMNG_RESULT_E m2mb_appMng_setItem( M2MB_APPMNG_HANDLE appMngHandle,
                                          M2MB_APPMNG_SEL_CMD_E selCmd,
                                          void *pIn,
                                          void *pOption
                                        );

/*-----------------------------------------------------------------------------------------------*/

/*!

  @brief
    m2mb_appMng_getItem

  @details
    This service retrieve value for specified parameter of the specified application
    Selecting cmd, a parameter to be retrieved can be set

  @param [in] appMngHandle
    handle of application
  @param [in] selCmd
    Select parameter to be set (see M2MB_APPMNG_SEL_CMD_E)
  @param [in] pOut
    pointer Output parameter got: its meaning depend on selCmd used See examples
    as general pointer it is set to MEM_W
  @param [in] pIn
    pointer to void, general Parameter to be specified, if any, to get right output See examples
    If parameter is not needed, use NULL

  @return
    M2MB_APPMNG_RESULT_SUCCESS in case of success
  @return
    see M2MB_APPMNG_RESULT_E in case of error

  @note
    All the M2MB_APPMNG_SEL_CMD_E are always available, a part from which
    dedicated to set only ( e.g M2MB_APPMNG_SEL_CMD_ARGS_DELETE )

  @b

  @code

   M2MB_APPMNG_HANDLE appMngHandle;
   M2MB_APPMNG_RESULT_E res;
   MEM_W num_arg, state, auto_start, auto_start_ini, delay, delay_ini, l_name, adress, arg_len;
   CHAR *pArg, name[64];

  // get handle appMngHandle with getHandle API

  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_DELAY, &delay, NULL );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success " );
  }

  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_DELAY_INI, &delay_ini, NULL );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success " );
  }

  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_SET_EXE_AUTO_INI, &auto_start_ini, NULL );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success " );
  }

  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_LNAME_APP, ( MEM_W* )&l_name, NULL );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success " );
  }
  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_NAME, ( MEM_W* )&name, NULL );
  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success " );
  }

  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_ADDRESS, &adress, NULL );

  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success " );
  }

  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_STATE, &state, NULL );

  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success " );
  }

  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_TOT_ARGS, &num_arg, NULL );

  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success, tot args = %d of app", num_arg );
  }

  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_ARGLEN_N, ( MEM_W* )&arg_len, ( void* )1 );

  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success, first arg_len = %d of app", arg_len );
  }

  pArg = ( CHAR * )m2mb_os_calloc( arg_len + 1 );

  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_ARG_N, ( MEM_W* )pArg, ( void* )1 );

  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success, first arg = %s of app", pArg );
  }

  res = m2mb_appMng_getItem( appMngHandle, M2MB_APPMNG_SEL_CMD_ARGS, ( MEM_W* )&pArg, NULL );

  if( res != M2MB_APPMNG_RESULT_SUCCESS )
  {
    printf("\n m2mb_appMng_getItem failure, error %d", res);
  }
  else
  {
    printf( "\n m2mb_appMng_getItem success, args = %s of app", pArg );
  }


  @endcode

*/

/*-----------------------------------------------------------------------------------------------*/

M2MB_APPMNG_RESULT_E m2mb_appMng_getItem( M2MB_APPMNG_HANDLE appMngHandle,
                                          M2MB_APPMNG_SEL_CMD_E selCmd,
                                          MEM_W *pOut,
                                          void *pIn
                                        );

/*-----------------------------------------------------------------------------------------------*/

/*!

  @brief
    m2mb_appMng_appsNumber

  @details
    This service returns the number of total executable applications.

  @return
    number of total executable applications

  @note

  @b

  @code

  INT32 n_apps;

  n_apps = m2mb_appMng_appsNumber( );
  printf("\n Number of apps: %d", n_apps);

  @endcode

*/

/*-----------------------------------------------------------------------------------------------*/

UINT8 m2mb_appMng_appsNumber( void );

/*-----------------------------------------------------------------------------------------------*/

/*!

  @brief
    m2mb_appMng_getHandleByN

  @details
    This service returns the handle of the specified application number, the index starts from 0.

  @param [in] n
    index of app to get the handle

  @return
    M2MB_APPMNG_HANDLE in case of success
  @return
    M2MB_APPMNG_INVALID in case of error

  @note
    Index starts from 0 to (N-1) of apps

  @b

  @code

    M2MB_APPMNG_HANDLE appMngHandle;

    appMngHandle = m2mb_appMng_getHandleByN( 1 );
    printf("\n m2mb_appMng_getHandleByN( 1 ): 0x%x", appMngHandle);

  @endcode

*/

/*-----------------------------------------------------------------------------------------------*/

M2MB_APPMNG_HANDLE m2mb_appMng_getHandleByN( UINT8 n );

/*-----------------------------------------------------------------------------------------------*/

/*!

  @brief
    m2mb_appMng_getHandleByName

  @details
    This service returns the value handle of the application name saved in file system.

  @param [in] name

  @return
    M2MB_APPMNG_RESULT_SUCCESS in case of success
  @return
    M2MB_APPMNG_INVALID in case of error

  @note

  @b

  @code

    M2MB_APPMNG_HANDLE appMngHandle;

    appMngHandle = m2mb_appMng_getHandleByName( "m2mapz.bin" );
    printf("\n m2mb_appMng_getHandleByName( m2mapz.bin ): 0x%x", appMngHandle);

  @endcode

*/

/*-----------------------------------------------------------------------------------------------*/

M2MB_APPMNG_HANDLE m2mb_appMng_getHandleByName( char *name );

/*-----------------------------------------------------------------------------------------------*/

/*!

  @brief
    m2mb_appMng_getMyHandle

  @details
    This service returns the current running handle of the application that uses the api.

  @param [in] void

  @return
    M2MB_APPMNG_RESULT_SUCCESS in case of success
  @return
    M2MB_APPMNG_INVALID in case of error

  @note

  @b

  @code

    M2MB_APPMNG_HANDLE appMngHandle;

    appMngHandle = m2mb_appMng_getMyHandle( );
    printf("\n m2mb_appMng_getMyHandle( ): 0x%x", appMngHandle);

  @endcode

*/

/*-----------------------------------------------------------------------------------------------*/

M2MB_APPMNG_HANDLE m2mb_appMng_getMyHandle( void );

/*-----------------------------------------------------------------------------------------------*/
 /*!
   @brief
      Function to verify integrity of an application file
   @details
      the function opens the file and checks the integrity. In case of success provides virtual address
      of the application and total ram needed if pointer are provided.
      NULL is\are accepted for pAddress and\or totRam if no value\s is\are needed.

   @param [in] appName
      binary file name, only name not path which is by default "/data/azc/mod"
   @param [out] pAddress
      pointer to location where to write virtual address. Optional, can be NULL
   @param [out] pTotRam
      pointer to location where to write total RAM needed from application. Optional, can be NULL

   @return
      M2MB_APPMNG_RESULT_SUCCESS if integrity is OK
   @return
      see M2MB_APPMNG_RESULT_E for errors

   @note
     This function is not supported and always return M2MB_APPMNG_RESULT_NOT_SUPPORTED.
     
     If the integrity is verified, pointed to pAddress and pointed to pTotRam are updated
     In case of error, they are both set to 0

   @b

   @code
     M2MB_APPMNG_RESULT_E appMngRes;
     MEM_W virtAddress;
     UINT32 totRam;

     appMngRes = m2mb_appMng_checkIntegrity( "m2mapz.bin", &virtAddress, &totRam );
     // or
     appMngRes = m2mb_appMng_checkIntegrity( "m2mapz.bin", NULL, &totRam );
     // or
     appMngRes = m2mb_appMng_checkIntegrity( "m2mapz.bin", &virtAddress, NULL );
     // or
     appMngRes = m2mb_appMng_checkIntegrity( "m2mapz.bin", NULL, NULL );
   @endcode

 */
/*-----------------------------------------------------------------------------------------------*/
M2MB_APPMNG_RESULT_E m2mb_appMng_checkIntegrity( CHAR* appName, MEM_W *pAddress, UINT32 *pTotRam );

/* Global define ================================================================================*/

#endif /* M2MB_APPMNG_H */
