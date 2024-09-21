/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_gnss.h

  @brief
    m2m base api for gnss functionalities

  @details
    m2m base gnss api prototype

  @notes
    Dependencies: math.h, m2mb_types.h

  @author
    Davide Cubeddu

  @date
    07/03/2017
*/


#ifndef  M2M_M2MB_GNSS_API_H
#define  M2M_M2MB_GNSS_API_H


/* Global declarations ==========================================================================*/


/* Global typedefs ==============================================================================*/
typedef struct M2MB_GNSS_HANDLE_TAG *M2MB_GNSS_HANDLE;

/* Indications */
typedef enum
{
  /*
    Indication M2MB_GNSS_INDICATION_POSITION_REPORT
      Event related to M2MB_GNSS_SERVICE_POSITION_REPORT service
  */
  M2MB_GNSS_INDICATION_POSITION_REPORT,
  /*
    Indication M2MB_GNSS_INDICATION_NMEA_REPORT
      Event related to M2MB_GNSS_SERVICE_NMEA_REPORT service
  */
  M2MB_GNSS_INDICATION_NMEA_REPORT
} M2MB_GNSS_IND_E;

/*
  Services
  Not all services are available on all products.
*/
typedef enum
{
  /*
    Service M2MB_GNSS_SERVICE_POSITION_REPORT
      Once the receiver acquires the position fix a callback (previously registered
      in the m2mb_gnss_init) will be called showing the position report
  */
  M2MB_GNSS_SERVICE_POSITION_REPORT,
  /*
    Service M2MB_GNSS_SERVICE_NMEA_REPORT
      A callback (previously registered in the m2mb_gnss_init) reporting the NMEA sentences
      will be called
  */
  M2MB_GNSS_SERVICE_NMEA_REPORT
} M2MB_GNSS_SERVICE_E;

/* Struct for M2MB_GNSS_SERVICE_POSITION_REPORT service */
typedef struct
{
  UINT8 codingType;  /* Coding of Velocity Type (see 3GPP GAD spec.) */

  FLOAT64 speed_horizontal;
  FLOAT64 bearing;
} M2MB_GNSS_POS_REP_VELOCITY_T;

typedef struct
{
  FLOAT64 speed_horizontal;
  FLOAT64 speed_vertical;

  FLOAT64 speed;  /* The result */
} M2MB_GNSS_POS_REP_SPEED_T;

/*
  M2MB_GNSS_POSITION_REPORT_INFO_T struct
    Struct containing the position report passed by the callback when the
    M2MB_GNSS_SERVICE_POSITION_REPORT service has been started
*/
typedef struct
{
  BOOLEAN latitude_valid;
  FLOAT64 latitude;

  BOOLEAN longitude_valid;
  FLOAT64 longitude;

  BOOLEAN altitude_valid;
  FLOAT64 altitude;

  BOOLEAN uncertainty_valid;
  FLOAT64 uncertainty;

  BOOLEAN velocity_valid;
  M2MB_GNSS_POS_REP_VELOCITY_T velocity;

  BOOLEAN timestamp_valid;
  UINT64 timestamp;  /* UTC timestamp - Units: Milliseconds since Jan. 1, 1970 */

  BOOLEAN speed_valid;
  M2MB_GNSS_POS_REP_SPEED_T speed;  /* NOT PRESENT! =>
                                       I need to calculate it
                                       from speed_horizontal and speed_vertical */
} M2MB_GNSS_POSITION_REPORT_INFO_T;

/* Struct for M2MB_GNSS_SERVICE_GTP service */
typedef struct
{
  UINT64 timestamp;                 /* UTC timestamp for a location fix; milliseconds since Jan. 1, 1970. */
  FLOAT64 latitude;                 /* Latitude in degrees. */
  FLOAT64 longitude;                /* Longitude in degrees. */
  FLOAT64 altitude;                 /* Altitude in meters above the WGS 84 reference ellipsoid. */
  FLOAT64 altitudeMeanSeaLevel;     /* Altitude in meters with respect to mean sea level. */
  FLOAT32 speed;                    /* Speed in meters per second. */
  FLOAT32 bearing;                  /* Bearing in degrees; range: 0 to 360. */
  FLOAT32 accuracy;                 /* Accuracy in meters. */
  FLOAT32 verticalAccuracy;         /* Vertical accuracy in meters. */
  FLOAT32 speedAccuracy;            /* Speed accuracy in meters/second. */
  FLOAT32 bearingAccuracy;          /* Bearing accuracy in degrees (0 to 359.999). */
} M2MB_GNSS_GTP_INFO_T;

/*
  Parameters to set/get by m2mb_gnss_setcfg/m2mb_gnss_getcfg APIs.
  Please see the description of these APIs for further information.
*/
typedef enum
{
  M2MB_GNSS_PRIORITY,        /* GNSS or WWAN as App priority */
  M2MB_GNSS_TBF,             /* Time Between Fix */
  M2MB_GNSS_CONSTELLATION    /* Constellations selection */
} M2MB_GNSS_CFG_E;

/*
  AGNSS providers list to be set/get by m2mb_gnss_set_agnss_enable/m2mb_gnss_get_agnss_enable
  Please see the description of these APIs for further information.
*/
typedef enum
{
  M2MB_AGNSS_XTRA        /* AGNSS provider XTRA */
} M2MB_AGNSS_PROVIDERS_E;

/*
  Callback for GNSS m2mb APIs
    The callback has to be registered in the m2mb_gnss_init.

    Service M2MB_GNSS_SERVICE_POSITION_REPORT
      Once the receiver acquires the position fix, the callback will be received, including the
      GNSS position report in the "resp" field, as a M2MB_GNSS_POSITION_REPORT_INFO_T struct.
      Please see the following example:
      void gnssCallback( M2MB_GNSS_HANDLE handle,
                         M2MB_GNSS_IND_E event,
                         UINT16 resp_size,
                         void *resp,
                         void *userdata )
      {
        if( event == M2MB_GNSS_INDICATION_POSITION_REPORT )
        {
          my_function_to_collect_GNSS_data( ( M2MB_GNSS_POSITION_REPORT_INFO_T * ) resp );
        }
      }

    Service M2MB_GNSS_SERVICE_NMEA_REPORT
      The callback reporting the NMEA sentences, as a string, will be called.
      Please see the following example:
      void gnssCallback( M2MB_GNSS_HANDLE handle,
                         M2MB_GNSS_IND_E event,
                         UINT16 resp_size,
                         void *resp,
                         void *userdata )
      {
        if( event == M2MB_GNSS_INDICATION_NMEA_REPORT )
        {
          PRINT( "NMEA Sentences: %s", ( CHAR * ) resp );
        }
      }
*/
typedef void ( *m2mb_gnss_callback )( M2MB_GNSS_HANDLE handle,
                                          M2MB_GNSS_IND_E event,
                                          UINT16 resp_size,
                                          void *resp,
                                          void *userdata );


/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Initializes gnss functionalities.

  @details
    m2mb_gnss_init initializes gnss service for current client,
    returning the handle that must be passed as first parameter for all gnss actions.
    Calling the m2mb_gnss_init is mandatory before using the gnss service.

  @param[in] pHandle
    pointer to the handle for the gnss functionalities (M2MB_GNSS_HANDLE *).
    Memory pointed by the pointer must be allocated by the user.
  @param[in] callback
    callback for the gnss indications (m2mb_gnss_callback)
  @param[in] userdata
    user data to be returned by callback function (void *)

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @b
    m2mb_gnss_init( &handle, callback, userdata );
  @code
    void gnssCallback( M2MB_GNSS_HANDLE handle, M2MB_GNSS_IND_E event, UINT16 resp_size, void *resp, void *userdata )
    {
      if(event == M2MB_GNSS_INDICATION_POSITION_REPORT)
      {
        // Success => you can use GNSS data arrived:
        my_function_to_collect_GNSS_data( ( M2MB_GNSS_POSITION_REPORT_INFO_T * ) resp );
      }
      else
      {
        // Fail
      }
    }

    void M2MB_main( int argc, char **argv )
    {
      M2MB_GNSS_HANDLE handle;
      void *userdata;

      userdata = ( void * )m2mb_os_malloc( sizeof( UINT8 ) );
      memset( userdata, 12, sizeof( UINT8 ) );  // Test value: 12

      if( M2MB_RESULT_SUCCESS != m2mb_gnss_init( &handle, gnssCallback, userdata ) )
      {
        PRINT("m2mb_gnss_init Fail");
        return;
      }
      PRINT("m2mb_gnss_init Success");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_init( M2MB_GNSS_HANDLE *pHandle,
                                   m2mb_gnss_callback callback,
                                   void *userdata );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Deinitializes gnss functionalities.

  @details
    Deinitializes gnss functionalities for current client.

  @param[in] handle
    handle for the gnss functionalities (M2MB_GNSS_HANDLE).

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @b
    m2mb_gnss_deinit( handle );
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_deinit( M2MB_GNSS_HANDLE handle );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Enables a gnss service.

  @details
    m2mb_gnss_enable, for each service to activate, configures the system
    in order to manage the specified service.
    Not all services are available on all products.

  @param[in] handle
    handle for the gnss functionalities (M2MB_GNSS_HANDLE)
  @param[in] service
    service to activate (M2MB_GNSS_SERVICE_E)

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @b
    m2mb_gnss_enable( handle, M2MB_GNSS_SERVICE_POSITION_REPORT );
  @code
    M2MB_GNSS_HANDLE handle = NULL;
    M2MB_RESULT_E res;

    res = m2mb_gnss_init( &handle, gnssCallback, NULL );
    if( M2MB_RESULT_SUCCESS != res )
    {
      PRINT("m2mb_gnss_init FAIL");
      return;
    }
    PRINT("m2mb_gnss_init SUCCESS");

    if( M2MB_RESULT_SUCCESS != m2mb_gnss_enable( handle, M2MB_GNSS_SERVICE_POSITION_REPORT ) )
    {
      PRINT("m2mb_gnss_enable FAIL");
      return;
    }
    PRINT("m2mb_gnss_enable SUCCESS");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_enable( M2MB_GNSS_HANDLE handle, M2MB_GNSS_SERVICE_E service );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Disables a gnss service.

  @details
    m2mb_gnss_disable, for each service to deactivate, configures the system
    in order to don't manage the specified service.

  @param[in] handle
    handle for the gnss functionalities (M2MB_GNSS_HANDLE)
  @param[in] service
    service to deactivate (M2MB_GNSS_SERVICE_E)

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @b
    m2mb_gnss_disable( handle, M2MB_GNSS_SERVICE_POSITION_REPORT );
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_disable( M2MB_GNSS_HANDLE handle, M2MB_GNSS_SERVICE_E service );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Starts a service.

  @details
    m2mb_gnss_start starts all the services, previously enabled through the
    m2mb_gnss_enable function, with the proper configuration.

  @param[in] handle
    handle for the gnss functionalities (M2MB_GNSS_HANDLE)

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @b
    m2mb_gnss_start( handle );
  @code
    M2MB_GNSS_HANDLE handle = NULL;
    M2MB_GNSS_POSITION_REPORT_INFO_T *testDataStructure;
    M2MB_RESULT_E res;

    res = m2mb_gnss_init( &handle, gnssCallback, NULL );
    if( M2MB_RESULT_SUCCESS != res )
    {
      PRINT("m2mb_gnss_init FAIL");
      return;
    }
    PRINT("m2mb_gnss_init SUCCESS");

    if( M2MB_RESULT_SUCCESS != m2mb_gnss_enable( handle, M2MB_GNSS_SERVICE_POSITION_REPORT ) )
    {
      PRINT("m2mb_gnss_enable FAIL");
      return;
    }
    PRINT("m2mb_gnss_enable SUCCESS");

    if( M2MB_RESULT_SUCCESS != m2mb_gnss_start( handle ) )
    {
      PRINT("m2mb_gnss_start FAIL");
      return;
    }
    PRINT("m2mb_gnss_start SUCCESS");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_start( M2MB_GNSS_HANDLE handle );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Stops a service.

  @details
    m2mb_gnss_stop stops all the services previously enabled through the m2mb_gnss_enable function.

  @param[in] handle
    handle for the gnss functionalities (M2MB_GNSS_HANDLE)

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @b
    m2mb_gnss_stop( handle );
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_stop( M2MB_GNSS_HANDLE handle );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set the GNSS configuration (priority, TBF, constellation). This is not supported on all products.

  @details
    If the functionality is not supported the API returns error.

    The new GNSS configuration will be set in NVM.
    A reboot is required, after the m2mb_gnss_setcfg calls, to load the new values in RAM
    and make them effective.

    M2MB_GNSS_PRIORITY:
      If concurrent WWAN and GNSS operations are not supported an arbitrator is designed
      to arbitrate concurrencies.
      Priority 0: GNSS as App priority.
                  GNSS fix request succeeds in all WWAN states except when WWAN is loaded
                  due to high priority procedures.
      Priority 1: WWAN as App priority.
                  GNSS fix request succeeds only in WWAN SLEEP state.
    M2MB_GNSS_TBF:
      Time Between Fix: 1÷4294967 [sec]
    M2MB_GNSS_CONSTELLATION:
      If feature supported: –All GNSS constellations supported
                            –Two constellations at a time
                            –GPS + any 2nd-constellation
      0 - The constellation is selected based on Mobile Country Code (MCC) of camped network
      1 - GPS+GLO
      2 - GPS+GAL
      3 - GPS+BDS
      4 - GPS+QZSS

  @param[in] handle
    handle for the gnss functionalities (M2MB_GNSS_HANDLE)
  @param[in] param
    CFG parameter to set (M2MB_GNSS_CFG_E)
  @param[in] value
    pointer to the value to set for the specified CFG parameter (void *)

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @b
    m2mb_gnss_setcfg( handle, M2MB_GNSS_PRIORITY, &priority );
  @code
    M2MB_GNSS_HANDLE handle;
    UINT8 priority;
    UINT32 TBF;
    UINT8 constellation;

    m2mb_gnss_init( &handle, gnssCallback_test, NULL );

    priority=1;
    TBF=4321;
    constellation=4;
    m2mb_gnss_setcfg( handle, M2MB_GNSS_PRIORITY, &priority );
    m2mb_gnss_setcfg( handle, M2MB_GNSS_TBF, &TBF );
    m2mb_gnss_setcfg( handle, M2MB_GNSS_CONSTELLATION, &constellation );
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_setcfg( M2MB_GNSS_HANDLE handle, M2MB_GNSS_CFG_E param, void * value );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get the GNSS configuration (priority, TBF, constellation). This is not supported on all products.

  @details
    If the functionality is not supported the API returns error.

    The GNSS configuration get by this API is the current configuration, read from RAM.

    M2MB_GNSS_PRIORITY:
      If concurrent WWAN and GNSS operations are not supported an arbitrator is designed
      to arbitrate concurrencies.
      Priority 0: GNSS as App priority.
                  GNSS fix request succeeds in all WWAN states except when WWAN is loaded
                  due to high priority procedures.
      Priority 1: WWAN as App priority.
                  GNSS fix request succeeds only in WWAN SLEEP state.
    M2MB_GNSS_TBF:
      Time Between Fix: 1÷4294967 [sec]
    M2MB_GNSS_CONSTELLATION:
      If feature supported: –All GNSS constellations supported
                            –Two constellations at a time
                            –GPS + any 2nd-constellation
      0 - The constellation is selected based on Mobile Country Code (MCC) of camped network
      1 - GPS+GLO
      2 - GPS+GAL
      3 - GPS+BDS
      4 - GPS+QZSS

  @param[in] handle
    handle for the gnss functionalities (M2MB_GNSS_HANDLE)
  @param[in] param
    CFG parameter to get (M2MB_GNSS_CFG_E)
  @param[in] value
    pointer to the memory to be filled with the value of the specified CFG parameter read (void *)

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @b
    m2mb_gnss_getcfg( handle, M2MB_GNSS_PRIORITY, &priority );
  @code
    M2MB_GNSS_HANDLE handle;
    UINT8 priority;
    UINT32 TBF;
    UINT8 constellation;

    m2mb_gnss_init( &handle, gnssCallback_test, NULL );

    m2mb_gnss_getcfg( handle, M2MB_GNSS_PRIORITY, &priority );
    m2mb_gnss_getcfg( handle, M2MB_GNSS_TBF, &TBF );
    m2mb_gnss_getcfg( handle, M2MB_GNSS_CONSTELLATION, &constellation );

    PRINT("Priority: %d", priority);
    PRINT("TBF: %d", TBF);
    PRINT("Constellation: %d", constellation);
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_getcfg( M2MB_GNSS_HANDLE handle, M2MB_GNSS_CFG_E param, void * value );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Start a GTP session.

  @details
    Starts the GTP session and when the call is completed, user callback is triggered
    for M2MB_GNSS_INDICATION_POSITION_REPORT event
    Prerequisites for running GTP
      - GTP needs to be enabled before running it.
      - m2mb_gnss_EnableGTP() enables the GTP and reboots the device.

  @param[in] handle
    handle for the gnss functionalities (M2MB_GNSS_HANDLE)

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @b
    m2mb_gnss_GTP( handle );
  @code
    void gnssCallback( M2MB_GNSS_HANDLE handle, M2MB_GNSS_IND_E event, UINT16 resp_size, void *resp, void *userdata )
    {
      if(event == M2MB_GNSS_INDICATION_POSITION_REPORT)
      {
        // Success => you can use gnss data arrived.
      }
      else if(event == M2MB_GNSS_INDICATION_NMEA_REPORT)
      {
        // Success => you can use gnss data arrived.
      }
      else
      {
        // Fail
      }
    }
    void main(void)
    {
      M2MB_GNSS_HANDLE gnss_handle;
      M2MB_RESULT_E result = 0;

      result = m2mb_gnss_init( gnss_handle, gnssCallback, NULL );
      if(result != M2MB_RESULT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_gnss_init succeeded");
      }

      result = m2mb_gnss_GetGTPstatus( gnss_handle, &status);
      if ( result == M2MB_RESULT_SUCCESS )
      {
        printf( "m2mb_gnss_GetGTPstatus succeeded");
      }

      if(status == 0)
      {
        result = m2mb_gnss_EnableGTP( gnss_handle, enablegtp);
        if ( result == M2MB_RESULT_SUCCESS )
        {
          printf( "m2mb_gnss_EnableGTP succeeded");
        }
      }

      result = m2mb_gnss_GTP( gnss_handle );
      if ( result == M2MB_RESULT_SUCCESS )
      {
        printf( "m2mb_gnss_GTP succeeded");
      }

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_GTP( M2MB_GNSS_HANDLE handle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Enables the GTP feature and reboot the device

  @details
    Enables the GTP feature and reboot the device

  @param[in] handle
    handle for the gnss functionalities (M2MB_GNSS_HANDLE)

  @param[in] enablegtp
    enablegtp it can be either 1(enable) or 0(disable)

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, negative value on error or if platform isn't supported

  @b
    m2mb_gnss_EnableGTP( handle, enablegtp );
  @code
    void gnssCallback( M2MB_GNSS_HANDLE handle, M2MB_GNSS_IND_E event, UINT16 resp_size, void *resp, void *userdata )
    {
      if(event == M2MB_GNSS_INDICATION_POSITION_REPORT)
      {
        // Success => you can use gnss data arrived.
      }
      else if(event == M2MB_GNSS_INDICATION_NMEA_REPORT)
      {
        // Success => you can use gnss data arrived.
      }
      else
      {
        // Fail
      }
    }
    void main(void)
    {
      M2MB_GNSS_HANDLE gnss_handle;
      M2MB_RESULT_E result = 0;
      UINT8 enablegtp;

      result = m2mb_gnss_init( gnss_handle, gnssCallback, NULL );
      if(result != M2MB_RESULT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_gnss_init succeeded");
      }
      enablegtp = 1;
      result = m2mb_gnss_EnableGTP( gnss_handle, enablegtp);
      if ( result == M2MB_RESULT_SUCCESS )
      {
        printf( "m2mb_gnss_EnableGTP succeeded");
      }

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_EnableGTP( M2MB_GNSS_HANDLE handle, UINT8 enablegtp );



/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Checks if GTP is enabled or disabled

  @details
    Checks if GTP feature is enabled or disabled

  @param[in] handle
    handle for the gnss functionalities (M2MB_GNSS_HANDLE)

  @param[in] status
    status gets updated with either 1(enabled) or 0(disabled)

  @return
    M2MB_RESULT_E: it returns M2MB_RESULT_SUCCESS on success, negative value on error or if platform isn't supported.

  @b
    m2mb_gnss_GetGTPstatus( handle, &status );
  @code
    void gnssCallback( M2MB_GNSS_HANDLE handle, M2MB_GNSS_IND_E event, UINT16 resp_size, void *resp, void *userdata )
    {
      if(event == M2MB_GNSS_INDICATION_POSITION_REPORT)
      {
        // Success => you can use gnss data arrived.
      }
      else if(event == M2MB_GNSS_INDICATION_NMEA_REPORT)
      {
        // Success => you can use gnss data arrived.
      }
      else
      {
        // Fail
      }
    }
    void main(void)
    {
      M2MB_GNSS_HANDLE gnss_handle;
      M2MB_RESULT_E result = 0;
      UINT8 status;

      result = m2mb_gnss_init( gnss_handle, gnssCallback, NULL );
      if(result != M2MB_RESULT_SUCCESS)
      {
        return;
      }
      else
      {
        printf("m2mb_gnss_init succeeded");
      }

      result = m2mb_gnss_GetGTPstatus( gnss_handle, &status);
      if ( result == M2MB_RESULT_SUCCESS )
      {
        printf( "m2mb_gnss_GetGTPstatus succeeded");
      }

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_gnss_GetGTPstatus( M2MB_GNSS_HANDLE h, UINT8* status );

#endif  /* M2M_M2MB_GNSS_API_H */

