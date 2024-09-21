/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_fwautosim.h

  @brief
    Header file for m2mb_fwautosim APIs

  @details
    m2m base fwautosim api prototype

  @notes
    Dependencies: m2mb_types.h
    This module defines APIs change firmware image automatically based on the inserted SIM

  @author
  Giusy Vallascas


  @date

*/

#ifndef  M2M_M2MB_FWAUTOSIM_API_H
#define  M2M_M2MB_FWAUTOSIM_API_H

/* Global declarations ==========================================================================*/


/* Global typedefs ==============================================================================*/

typedef HANDLE M2MB_FWAUTOSIM_HANDLE;
typedef enum
{
  M2MB_FWAUTOSIM_DISABLED             = 0,
  M2MB_FWAUTOSIM_ENABLED              = 1,
  M2MB_FWAUTOSIM_ONESHOT              = 2,
  M2MB_FWAUTOSIM_ONESHOT_IN_PROGRESS  = 3,
  ENUM_TO_INT(M2MB_FWAUTOSIM_E)
} M2MB_FWAUTOSIM_E;

typedef enum
{
  M2MB_FWAUTOSIM_INDICATION_GENERIC  /* WARNING: at the moment not supported */
} M2MB_FWAUTOSIM_IND_E;

/*
  Callback for fwautosim m2mb APIs.
    The callback has to be registered in the m2mb_fwautosim_init.

    WARNING: at the moment not supported/required: pass NULL in the m2mb_fwautosim_init.
*/
typedef void ( *m2mb_fwautosim_callback )(
  M2MB_FWAUTOSIM_HANDLE handle,
  M2MB_FWAUTOSIM_IND_E event,
  UINT16 resp_size,
  void *resp,
  void *userdata );

/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_fwautosim_init initializes FWAUTOSIM service for current client.

  @details
    m2mb_fwautosim_init initialize FWAUTOSIM service for current client returning the handle
    that must be passed as first parameter for all FWAUTOSIM actions.
    Calling the m2mb_fwautosim_init is mandatory before using the FWAUTOSIM service.

  @param[in] *h
    is the pointer to the handle to the FWAUTOSIM service, that will be initialized by the function.
  @param[in] callback
    callback for the fwautosim indications (m2mb_fwautosim_callback)
    WARNING: at the moment not supported/required: pass it as NULL.
  @param[in] userdata
    is a pointer to generic user data

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_fwautosim_init(
  M2MB_FWAUTOSIM_HANDLE *h,
  m2mb_fwautosim_callback callback,
  void *userdata );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_fwautosim_set enable/disable fwautosim

  @details
    m2mb_fwautosim_set allows to set value of fwautosim (0-1)

  @param[in] h
   is the handle to the FWAUTOSIM service
  @param[in] type
   is fwautosim value assigned by at cmd

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
   type can be only 0 or 1, 0 is to disable fwautosim and 1 is to enable fwautosim.
   If fwautosim is enable, the inserted SIM is checked to establish if the operator is ATT o VZW and to change firmware
   image automatically.
   When firmware image is changed automatically the system is automatically rebooted

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_RESULT_E m2mb_fwautosim_set (  M2MB_FWAUTOSIM_HANDLE h, M2MB_FWAUTOSIM_E type);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_fwautosim_get returns fwautosim current state (enable/disable)

  @details
    m2mb_fwautosim_get allows to read value of fwautosim (0-1)

  @param[in] h
   is the handle to the FWAUTOSIM service
  @param[in] *value
   is the pointer to the fwautosim value
   it returns the current state of fwautosim (0: disabled, 1: enabled)

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
    this function is used to check a SIM switch, for example

  @b

  @code

  @endcode
*/
M2MB_RESULT_E m2mb_fwautosim_get (  M2MB_FWAUTOSIM_HANDLE h, M2MB_FWAUTOSIM_E *value);
/*-----------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_fwautosim_deinit deinitializes FWAUTOSIM service for current client.

  @details
    m2mb_fwautosim_deinit deinitialize FWAUTOSIM service for current client, freeing the handle passed
    as first parameter. Calling the m2mb_fwautosim_deinit is useful after using the FWAUTOSIM service in order to free memory space.

  @param[in] h
    is the handle to the FWAUTOSIM service, that will be deinitialized by the function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b

  @code

  @endcode
*/
M2MB_RESULT_E m2mb_fwautosim_deinit( M2MB_FWAUTOSIM_HANDLE h );

#endif
