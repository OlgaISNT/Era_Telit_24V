/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_power.h

  @brief
    Power library implementation.

  @details
    The following functions are implemented:


  @note
    Dependencies:
    m2mb_types.h

  @author
    Alessandro Bonetti

  @date
    24/05/2018
*/
#ifndef M2MB_POWER_H
#define M2MB_POWER_H

/**
   @defgroup POWER M2MB_POWER
   @addtogroup POWER
   @brief Module power related functionalities.<br>
   @{
*/

/* Global declarations ==========================================================================*/
/* Global enums ==============================================================================*/
/* Global typedefs ==============================================================================*/

/*!
  @brief
    Enumeration representing all power related events
*/
typedef enum
{
  M2MB_POWER_SYSHALT_EVT,
  M2MB_POWER_DUMMY_EVT    /**<Dummy power event */
} M2MB_POWER_EVT_E;

/*!
  @brief
    Enumeration representing all power related actions
*/
typedef enum
{
  M2MB_POWER_DO_RBT,        /**<Performs a reboot */
  M2MB_POWER_DO_SHDN,       /**<Performs a shutdown */
} M2MB_POWER_ACTION_E;

/*!
  @brief
    Generic handle type for power operations
*/
typedef HANDLE M2MB_POWER_HANDLE;


/*!
   @brief Power events indication callback type.

   This type defines the callback functions that the client can set in the
   m2mb_power_init API.
*/
typedef void ( *m2mb_power_ind_callback )( M2MB_POWER_HANDLE h, M2MB_POWER_EVT_E power_event, UINT16 resp_size, void *resp_struct, void *userdata );
/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Initializes the power system

  @details
    Initializes the power handle and the power state machine

  @param[in] pHandle
    Pointer to the handle to the power service, that will be initialized by the function.
  @param[in] callback
    The callback that will be called if an event or a response happens.
  @param[in] userdata
    A pointer to generic user data that will be returned as it is in the callback.

  @return
    M2MB_RESULT_SUCCESS on success, a different value on error.


  @code
    int M2MB_main( int argc, char **argv )
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_POWER_HANDLE h = NULL;

      retVal = m2mb_power_init(&h, (m2mb_power_ind_callback) NULL, NULL );
      if ( retVal == M2MB_RESULT_SUCCESS )
      {
         LOG( "m2mb_power_init succeeded");
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_power_init( M2MB_POWER_HANDLE *pHandle, m2mb_power_ind_callback callback, void *userdata );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Deinitializes the power management system

  @details
    Releases resources related to the passed handle

  @param[in] handle
    The handle to the power service initialized by m2mb_power_init(), that will be freed by the function.

  @return
    M2MB_RESULT_SUCCESS on success, a different value on error.

 @code
    int M2MB_main( int argc, char **argv )
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_POWER_HANDLE h = NULL;

      retVal = m2mb_power_init(&h, (m2mb_power_ind_callback) NULL, NULL );
      if ( retVal == M2MB_RESULT_SUCCESS )
      {
        LOG( "m2mb_power_init succeeded");
        m2mb_power_deinit(h);
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_power_deinit( M2MB_POWER_HANDLE handle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Restarts all the system

  @details
    Performs a modem reboot, only after all conditions are met.
    In case of success, this function does not return.

  @param[in] handle
    The handle to the power service initialized by m2mb_power_init()

  @return
    M2MB_RESULT_FAIL on error.

 @code
    int M2MB_main( int argc, char **argv )
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_POWER_HANDLE h = NULL;

      retVal = m2mb_power_init(&h, (m2mb_power_ind_callback) NULL, NULL );
      if ( retVal == M2MB_RESULT_SUCCESS )
      {
        LOG( "m2mb_power_init succeeded");
        m2mb_power_reboot(h);
        m2mb_power_deinit(h);
        return;
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_power_reboot( M2MB_POWER_HANDLE handle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Turns off the system

  @details
    Performs a modem switch off, only after all conditions are met (reboot is not immediate, so code flow should
    stop after this call to avoid unwanted behaviours)

  @param[in] handle
    The handle to the power service initialized by m2mb_power_init()

  @return
    M2MB_RESULT_SUCCESS on success, a different value on error.

@code
    int M2MB_main( int argc, char **argv )
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_POWER_HANDLE h = NULL;

      retVal = m2mb_power_init(&h, (m2mb_power_ind_callback) NULL, NULL );
      if ( retVal == M2MB_RESULT_SUCCESS )
      {
        LOG( "m2mb_power_init succeeded");
        m2mb_power_shutdown(h);
        m2mb_power_deinit(h);
        return;
      }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_power_shutdown( M2MB_POWER_HANDLE handle );

/** @} */
#endif // M2MB_POWER_H
