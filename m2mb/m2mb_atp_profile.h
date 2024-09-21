/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_atp_profile.h

  @brief
    Header file for m2mb_atp profiles related APIs

  @details
    m2mb_atp_profile APIs provide actions for profiles handling.

  @notes

  @author
    F. Sansa

  @date
    02/02/2018
*/

#ifndef  M2M_M2MB_ATP_PROFILE_API_H
#define  M2M_M2MB_ATP_PROFILE_API_H


/* Global declarations ==========================================================================*/

#define M2MB_ATP_PROFILE_PARAM_REGISTER(paramString, paramFlags, paramDefault, paramType) \
m2mb_atp_profile_param_register((CHAR*)paramString, (UINT16)paramFlags, (void*)paramDefault, (UINT16)(sizeof(paramType)))
#define M2MB_ATP_PROFILE_PARAM_SET(h, instanceNum, paramString, paramValue, paramType) \
  m2mb_atp_profile_param_set((M2MB_ATP_HANDLE)h, (UINT16)instanceNum, (CHAR*)paramString, (void*)paramValue, (UINT16)(sizeof(paramType)))
#define M2MB_ATP_PROFILE_PARAM_GET(h, instanceNum, paramString, paramValue, paramType) \
  m2mb_atp_profile_param_get((M2MB_ATP_HANDLE)h, (UINT16)instanceNum, (CHAR*)paramString, (void*)paramValue, (UINT16)(sizeof(paramType)))

#define M2MB_ATP_BASE_PROFILE_FLAG   0x80

typedef enum
{
  M2MB_ATP_PI_FIRST = 0,
  M2MB_ATP_PI_SECOND,
  M2MB_ATP_PI_MAX,
  M2MB_ATP_PI_BASE_FIRST = (M2MB_ATP_BASE_PROFILE_FLAG | M2MB_ATP_PI_FIRST),
  M2MB_ATP_PI_BASE_SECOND,
  M2MB_ATP_PI_BASE_MAX
} M2MB_ATP_PROFILE_INDEX_E;

typedef enum
{
  M2MB_ATP_COMMON_PROFILE = 0,
  M2MB_ATP_INSTANCE_PROFILE
} M2MB_ATP_PROFILE_TYPE_E;

/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_profile_param_register registers a command parameter.

  @details
    m2mb_atp_profile_param_register registers a command parameter for subsequent storage in profiles.

  @param[in] h
    first parameter is the handle to the AT Parser.
  @param[in] paramString
    second parameter is the name assigned to the parameter.
  @param[in] paramFlags
    third parameter is a flag to specify whether the parameter is common to all AT parser instances, or just for a specific instance.
  @param[in] paramDefault
    fourth parameter is a pointer to the default value for the parameter.
  @param[in] paramSize
    fifth parameter is a pointer to the size in bytes for the parameter value.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_profile_param_register(h, paramString, paramFlags, paramDefault, paramSize );
  @code
  void m2mb_EasyAT_test_app( void )
  {
    UINT16 atpFlags;
    UINT8 tmp = 0;

    m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "m2mb_EasyAT_test_app" );

    m2mb_atp_init( &atpHandle, atpCallback_test, NULL );

    atptask_init(&appAtptaskHandle, 0, 0);
    atpFlags = M2MB_ATP_NORML | M2MB_ATP_NOPIN | M2MB_ATP_NOSIM;
    atptask_register(&appAtpHandleCMGF, "+CMGF", atpFlags, CMGFCallback, NULL, appAtptaskHandle);
    m2mb_atp_profile_param_register( atpHandle, "CMGFset", M2MB_ATP_INSTANCE_PROFILE, &tmp, sizeof(UINT8) );
    atptask_register(&appAtpHandleCMGW, "+CMGW", atpFlags, CMGWCallback, CMGWDelegation, appAtptaskHandle);
  }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_profile_param_register( M2MB_ATP_HANDLE h, CHAR *paramString, M2MB_ATP_PROFILE_TYPE_E paramFlags, void *paramDefault, UINT16 paramSize );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_profile_param_set sets value for a registered command parameter.

  @details
    m2mb_atp_profile_param_set sets value for a registered command parameter for a given instance.

  @param[in] h
    first parameter is the handle to the AT Parser.
  @param[in] instanceNum
    second parameter is the AT parser instance where run the action
  @param[in] paramString
    third parameter is the name assigned to the parameter.
  @param[in] paramValue
    fourth parameter is the value to be assigned to the parameter.
  @param[in] paramSize
    fifth parameter is a pointer to the size in bytes for the parameter value.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_profile_param_set(h, instanceNum, paramString, paramValue, paramSize );
  @code
  void CMGFCallback( M2MB_ATP_HANDLE h, M2MB_ATP_PARAM_T *atpParam )
  {
    UINT8 temp = 0;
    CHAR tmp_str[50 + 1];
    UINT16 Instance;

    m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "CMGFCallback %d %d", atpParam->type, atpParam->itemNum);

    switch (atpParam->type)
    {
      case M2MB_ATP_CMDTYP_SET:
        temp = atoi( atpParam->item[0]);

        if ( ( temp == 0 ) || ( temp == 1 ) )
        {
            m2mb_atp_profile_param_set(h, atpParam->atpInstance, "CMGFset", &temp, sizeof(UINT8) );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else
          m2mb_atp_release(h, atpParam->atpInstance 1, 0, ATPCME_OPERATION_NOT_SUPPORTED);
        break;

      case M2MB_ATP_CMDTYP_READ:
        m2mb_atp_profile_param_get(h, atpParam->atpInstance, "CMGFset", &temp, sizeof(UINT8) );
        sprintf( tmp_str, "+CMGF: %d", temp);
        m2mb_atp_msgout(h, atpParam->atpInstance tmp_str );
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      case M2MB_ATP_CMDTYP_TEST:
        m2mb_atp_msgout(h, atpParam->atpInstance "+CMGF: (0,1)" );
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      case M2MB_ATP_CMDTYP_NOPARAMS:
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      default:
        m2mb_atp_release(h, atpParam->atpInstance 1, 0, -1);
        break;

    } // end switch
  }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_profile_param_set( M2MB_ATP_HANDLE h, UINT16 instanceNum, CHAR *paramString, void *paramValue, UINT16 paramSize );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_profile_param_get gets value for a registered command parameter.

  @details
    m2mb_atp_profile_param_set gets value for a registered command parameter for a given instance.

  @param[in] h
    first parameter is the handle to the AT Parser.
  @param[in] instanceNum
    second parameter is the AT parser instance where run the action
  @param[in] paramString
    third parameter is the name assigned to the parameter.
  @param[out] paramValue
    fourth parameter is the value of the parameter.
  @param[in] paramSize
    fifth parameter is a pointer to the size in bytes for the parameter value.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_profile_param_get(h, instanceNum, paramString, paramValue, paramSize );
  @code
  void CMGFCallback( M2MB_ATP_HANDLE h, M2MB_ATP_PARAM_T *atpParam )
  {
    UINT8 temp = 0;
    CHAR tmp_str[50 + 1];
    UINT16 Instance;

    m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "CMGFCallback %d %d", atpParam->type, atpParam->itemNum);

    switch (atpParam->type)
    {
      case M2MB_ATP_CMDTYP_SET:
        temp = atoi( atpParam->item[0]);

        if ( ( temp == 0 ) || ( temp == 1 ) )
        {
            m2mb_atp_profile_param_set(h, atpParam->atpInstance, "CMGFset", &temp, sizeof(UINT8) );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else
          m2mb_atp_release(h, atpParam->atpInstance 1, 0, ATPCME_OPERATION_NOT_SUPPORTED);
        break;

      case M2MB_ATP_CMDTYP_READ:
        m2mb_atp_profile_param_get(h, atpParam->atpInstance, "CMGFset", &temp, sizeof(UINT8) );
        sprintf( tmp_str, "+CMGF: %d", temp);
        m2mb_atp_msgout(h, atpParam->atpInstance tmp_str );
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      case M2MB_ATP_CMDTYP_TEST:
        m2mb_atp_msgout(h, atpParam->atpInstance "+CMGF: (0,1)" );
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      case M2MB_ATP_CMDTYP_NOPARAMS:
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      default:
        m2mb_atp_release(h, atpParam->atpInstance 1, 0, -1);
        break;

    } // end switch
  }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_profile_param_get( M2MB_ATP_HANDLE h, UINT16 instanceNum, CHAR *paramString, void *paramValue, UINT16 paramSize );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_profile_param_restore resets all registered command parameters to their default value.

  @details
    m2mb_atp_profile_param_restore resets all registered command parameters to their default value for a given instance.

  @param[in] h
    first parameter is the handle to the AT Parser.
  @param[in] instanceNum
    second parameter is the AT parser instance where run the action
  @param[in] paramString
    third parameter is the name assigned to the parameter, or NULL if all registered parameters should be restored.
  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_profile_param_restore(h, instanceNum, paramString );
  @code
  void PCallback( M2MB_ATP_HANDLE h, M2MB_ATP_PARAM_T *atpParam )
  {
    UINT8 temp = 0;

    m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "PCallback %d %d", atpParam->type, atpParam->itemNum);

    switch (atpParam->type)
    {
      case M2MB_ATP_CMDTYP_SET:
        temp = atoi( atpParam->item[0]);

        if ( temp == 0 )
        {
            m2mb_atp_profile_set_active_profile(h, atpParam->atpInstance, M2MB_ATP_PI_FIRST );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else if ( temp == 1 )
        {
            m2mb_atp_profile_set_active_profile(h, atpParam->atpInstance, M2MB_ATP_PI_SECOND );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else if ( temp == 2 )
        {
            m2mb_atp_profile_param_restore(h, atpParam->atpInstance, NULL );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else
          m2mb_atp_release(h, atpParam->atpInstance 1, 0, ATPCME_OPERATION_NOT_SUPPORTED);
        break;

      case M2MB_ATP_CMDTYP_READ:
        m2mb_atp_profile_get_active_profile(h, atpParam->atpInstance, &temp );
        sprintf( tmp_str, "+P: %d", temp);
        m2mb_atp_msgout(h, atpParam->atpInstance tmp_str );
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      case M2MB_ATP_CMDTYP_TEST:
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      default:
        m2mb_atp_release(h, atpParam->atpInstance 1, 0, -1);
        break;

    } // end switch
  }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_profile_param_restore( M2MB_ATP_HANDLE h, UINT16 instanceNum, CHAR *paramString );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_profile_set_active_profile sets active profile.

  @details
    m2mb_atp_profile_set_active_profile sets the active profile for a given instance from which restore registered parameters.

  @param[in] h
    first parameter is the handle to the AT Parser.
  @param[in] instanceNum
    second parameter is the AT parser instance where run the action
  @param[in] profileID
    third parameter is the profile number.
  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_profile_set_active_profile(h, profileID );
  @code
  void PCallback( M2MB_ATP_HANDLE h, M2MB_ATP_PARAM_T *atpParam )
  {
    UINT8 temp = 0;

    m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "PCallback %d %d", atpParam->type, atpParam->itemNum);

    switch (atpParam->type)
    {
      case M2MB_ATP_CMDTYP_SET:
        temp = atoi( atpParam->item[0]);

        if ( temp == 0 )
        {
            m2mb_atp_profile_set_active_profile(h, atpParam->atpInstance, M2MB_ATP_PI_FIRST );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else if ( temp == 1 )
        {
            m2mb_atp_profile_set_active_profile(h, atpParam->atpInstance, M2MB_ATP_PI_SECOND );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else if ( temp == 2 )
        {
            m2mb_atp_profile_param_restore(h, atpParam->atpInstance, NULL );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else
          m2mb_atp_release(h, atpParam->atpInstance 1, 0, ATPCME_OPERATION_NOT_SUPPORTED);
        break;

      case M2MB_ATP_CMDTYP_READ:
        m2mb_atp_profile_get_active_profile(h, atpParam->atpInstance, &temp );
        sprintf( tmp_str, "+P: %d", temp);
        m2mb_atp_msgout(h, atpParam->atpInstance tmp_str );
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      case M2MB_ATP_CMDTYP_TEST:
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      default:
        m2mb_atp_release(h, atpParam->atpInstance 1, 0, -1);
        break;

    } // end switch
  }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_profile_set_active_profile( M2MB_ATP_HANDLE h, UINT16 instanceNum, M2MB_ATP_PROFILE_INDEX_E profileID );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_profile_get_active_profile gets active profile.

  @details
    m2mb_atp_profile_get_active_profile gets the active profile for a given instance from which restore registered parameters.

  @param[in] h
    first parameter is the handle to the AT Parser.
  @param[in] instanceNum
    second parameter is the AT parser instance where run the action
  @param[out] profileID
    third parameter is the profile number.
  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_profile_get_active_profile(h, &profileID );
  @code
  void PCallback( M2MB_ATP_HANDLE h, M2MB_ATP_PARAM_T *atpParam )
  {
    UINT8 temp = 0;

    m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "PCallback %d %d", atpParam->type, atpParam->itemNum);

    switch (atpParam->type)
    {
      case M2MB_ATP_CMDTYP_SET:
        temp = atoi( atpParam->item[0]);

        if ( temp == 0 )
        {
            m2mb_atp_profile_set_active_profile(h, atpParam->atpInstance, M2MB_ATP_PI_FIRST );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else if ( temp == 1 )
        {
            m2mb_atp_profile_set_active_profile(h, atpParam->atpInstance, M2MB_ATP_PI_SECOND );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else if ( temp == 2 )
        {
            m2mb_atp_profile_param_restore(h, atpParam->atpInstance, NULL );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else
          m2mb_atp_release(h, atpParam->atpInstance 1, 0, ATPCME_OPERATION_NOT_SUPPORTED);
        break;

      case M2MB_ATP_CMDTYP_READ:
        m2mb_atp_profile_get_active_profile(h, atpParam->atpInstance, &temp );
        sprintf( tmp_str, "+P: %d", temp);
        m2mb_atp_msgout(h, atpParam->atpInstance tmp_str );
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      case M2MB_ATP_CMDTYP_TEST:
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      default:
        m2mb_atp_release(h, atpParam->atpInstance 1, 0, -1);
        break;

    } // end switch
  }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_profile_get_active_profile( M2MB_ATP_HANDLE h, UINT16 instanceNum, M2MB_ATP_PROFILE_INDEX_E *profileID );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_atp_profile_param_store stores current value of a command parameter in a permanent (not volatile) storage.

  @details
    m2mb_atp_profile_param_store stores current value of a command parameter for current instance in a permanent (not volatile) storage, in the selected profile.

  @param[in] h
    first parameter is the handle to the AT Parser.
  @param[in] instanceNum
    second parameter is the AT parser instance where run the action
  @param[in] paramString
    third parameter is the name assigned to the parameter, or NULL if all registered parameters should be stored.
  @param[in] profileID
    fourth parameter is the number of the profile for parameter storing.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_atp_profile_param_store(atpInstance, paramString, profileID );
  @code
  void CMGFCallback( M2MB_ATP_HANDLE h, M2MB_ATP_PARAM_T *atpParam )
  {
    UINT8 temp = 0;
    CHAR tmp_str[50 + 1];
    UINT16 Instance;

    m2mb_trace_printf( M2MB_TC_GENERIC, M2MB_TL_ERROR, "CMGFCallback %d %d", atpParam->type, atpParam->itemNum);

    switch (atpParam->type)
    {
      case M2MB_ATP_CMDTYP_SET:
        temp = atoi( atpParam->item[0]);

        if ( ( temp == 0 ) || ( temp == 1 ) )
        {
            m2mb_atp_profile_param_set(h, atpParam->atpInstance, "CMGFset", &temp, sizeof(UINT8) );
            m2mb_atp_profile_param_store(h, atpParam->atpInstance, NULL, 0 );
            m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        }
        else
          m2mb_atp_release(h, atpParam->atpInstance 1, 0, ATPCME_OPERATION_NOT_SUPPORTED);
        break;

      case M2MB_ATP_CMDTYP_READ:
        m2mb_atp_profile_param_get(h, atpParam->atpInstance, "CMGFset", &temp, sizeof(UINT8) );
        sprintf( tmp_str, "+CMGF: %d", temp);
        m2mb_atp_msgout(h, atpParam->atpInstance tmp_str );
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      case M2MB_ATP_CMDTYP_TEST:
        m2mb_atp_msgout(h, atpParam->atpInstance "+CMGF: (0,1)" );
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      case M2MB_ATP_CMDTYP_NOPARAMS:
        m2mb_atp_release(h, atpParam->atpInstance 0, 0, -1);
        break;

      default:
        m2mb_atp_release(h, atpParam->atpInstance 1, 0, -1);
        break;

    } // end switch
  }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_atp_profile_param_store( M2MB_ATP_HANDLE h, UINT16 instanceNum, CHAR *paramString, M2MB_ATP_PROFILE_INDEX_E profileID );



#endif

