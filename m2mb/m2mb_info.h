/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_info.h

  @brief
    Header file for m2mb_info APIs

  @details
    m2mb_info APIs provide general information about device in use (FW version, SW version, etc...)

  @notes

  @author
    S. Passiu

  @date
    21/02/2017
*/

#ifndef  M2M_M2MB_INFO_API_H
#define  M2M_M2MB_INFO_API_H


/* Global declarations ==========================================================================*/

/* Global typedefs ==============================================================================*/

typedef HANDLE M2MB_INFO_HANDLE;

typedef enum
{
  M2MB_INFO_GET_SERIAL_NUM,
  M2MB_INFO_GET_FW_VERSION,
  M2MB_INFO_GET_SW_VERSION,
  M2MB_INFO_GET_MANUFACTURER,
  M2MB_INFO_GET_MODEL,
  M2MB_INFO_GET_HW_VERSION,
  M2MB_INFO_GET_MSISDN,
  M2MB_INFO_GET_IMSI,
  M2MB_INFO_GET_SVN,
  M2MB_INFO_GET_AZL_VERSION,
  M2MB_INFO_GET_SMSS,
  M2MB_INFO_GET_TID,
  M2MB_INFO_GET_ICCID,
  M2MB_INFO_GET_SERIAL_NUM_FACTORY,
  M2MB_INFO_GET_PKG_VERSION,        /**< entry for retrieving the package version */
  M2MB_INFO_NUM
} M2MB_INFO_E;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_info_init initializes the INFO service.

  @details
    m2mb_info_init initialize SMS INFO for current client returning the handle that must be passed as first parameter
    for all INFO actions. Calling the m2mb_info_init is mandatory before using the INFO service.

  @param[in] *h
    first parameter is the handle to the INFO service, that will be initialized by the function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_info_init(&h);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_INFO_HANDLE h;
    retVal = m2mb_info_init(&h);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "m2mb_info_init succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_info_init( M2MB_INFO_HANDLE *h );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_info_deinit deinitializes the INFO service.

  @details
    m2mb_info_deinit deinitialize INFO service for current client freeing the handle passed as first parameter.
    Calling the m2mb_info_deinit is useful after using the INFO service in order to free memory space.

  @param[in] h
    first parameter is the handle to the INFO service, that will be deinitialized by the function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_info_deinit(&h);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_INFO_HANDLE h;
    retVal = m2mb_info_init(&h);

    //... use INFO APIs ...

    retVal = m2mb_info_deinit(h);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "m2mb_info_deinit succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_info_deinit( M2MB_INFO_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_info_get provides information about device/SIM card in use.

  @details
    m2mb_info_get function provides, depending on the second parameter value, one of the following information
    about the device/SIM card in use: MSISDN, device Serial Number (IMSI), device firmware version,
    device manufacturer name, device model name, device hardware version.

  @param[in] h
    first parameter is the handle to the INFO service, previously initialized by the m2mb_info_init function.
  @param[in] info
    one of the items of type M2MB_INFO_E, according to the information required.
  @param[in] **data
    pointer to a char pointer where the required string value will be created.
    On success: the variable contains the requested string.
    On failure: no modifications are done to the content of the variable.
    NOTE: do not alloc memory for this variable, it must be declared as simple CHAR pointer before be passed
    to the m2mb_info_get function (see source code example below).

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_info_get(h, M2MB_INFO_GET_SERIAL_NUM, &data);
  @code
    CHAR *data;
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_INFO_HANDLE h;
    retVal = m2mb_info_init(&h);
    //...

    retVal = m2mb_info_get(h, M2MB_INFO_GET_SERIAL_NUM, &data);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "Device IMSI is %s", data);

    //...
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_info_get( M2MB_INFO_HANDLE h, M2MB_INFO_E info, CHAR **data );

#endif

