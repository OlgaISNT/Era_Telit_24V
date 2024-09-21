/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_pdp.h

  @brief
    Header file for m2mb_pdp APIs

  @details
    m2m_pdp API actions and related parameters

  @notes
    Dependencies:

  @author
    Andrea Pirnetti

  @date
    01/03/2017
*/

#ifndef  M2M_M2MB_PDP_API_H
#define  M2M_M2MB_PDP_API_H


/* Global declarations ==========================================================================*/


/* Global typedefs ==============================================================================*/

typedef HANDLE M2MB_PDP_HANDLE;

typedef enum
{
  M2MB_PDP_IPV4,   /* PDP IP type IPV4 */
  M2MB_PDP_IPV6,   /* PDP IP type IPV6 */
  M2MB_PDP_IPV4V6  /* PDP IP type IPV4V6 */
} M2MB_PDP_IP_TYPE_E;

typedef enum
{
  M2MB_PDP_TYPE_IPV4,   /* PDP type IPV4 */
  M2MB_PDP_TYPE_PPP,   /* PDP  type PPP -> NOTE: supported only for products supporting WCDMA*/
  M2MB_PDP_TYPE_IPV6,   /* PDP type IPV6 */
  M2MB_PDP_TYPE_IPV4V6  /* PDP type IPV4V6 */
} M2MB_PDP_TYPE_E;

typedef enum
{
  M2MB_PDP_AUTHTYPE_NOAUTH,   /* PDP auth type No Auth */
  M2MB_PDP_AUTHTYPE_PAP,   /* PDP auth type PAP */
  M2MB_PDP_AUTHTYPE_CHAP,   /* PDP auth type CHAP */
  ENUM_TO_INT( M2MB_PDP_AUTHTYPE_E )
} M2MB_PDP_AUTHTYPE_E;

typedef enum
{
  M2MB_PDP_UP,
  M2MB_PDP_DOWN
} M2MB_PDP_IND_E;

typedef enum
{
  M2MB_PDP_DATAVOL_RESET        = 0,
  M2MB_PDP_DATAVOL_GET_SESSION  = 1,
  M2MB_PDP_DATAVOL_GET_LIFE     = 2,
  ENUM_TO_INT( M2MB_PDP_DATAVOL_ACTION_E )
} M2MB_PDP_DATAVOL_ACTION_E;

/**
  * @enum
  *   M2MB_PDP_STATS_ACTION_E
  * @brief
  *   List of available actions related to the m2mb_pdp_stats
  */
typedef enum
{
  M2MB_PDP_STATS_RESET,                     /**< Reset stats counter */
  M2MB_PDP_STATS_GET_SESSION,               /**< Get stats */
  ENUM_TO_INT( M2MB_PDP_STATS_ACTION_E )
} M2MB_PDP_STATS_ACTION_E;

/**
  * @struct
  *   M2MB_PDP_STATS_T
  * @brief
  *   Struct containing the stats
  */
typedef struct
{
  UINT64 byteTX;                            /**< Transmitted Bytes */
  UINT64 byteRX;                            /**< Received Bytes */
  UINT64 pktTX;                             /**< Transmitted Packets */
  UINT64 pktRX;                             /**< Received Packets */
  UINT32 mtu;                               /**< Link MTU */
} M2MB_PDP_STATS_T;

typedef void ( *m2mb_pdp_ind_callback )( M2MB_PDP_HANDLE h, M2MB_PDP_IND_E pdp_event, UINT8 cid, void *userdata );


/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_pdp_init initializes PDP service for current client.

  @details
    m2mb_pdp_init initialize PDP service for current client returning the handle that must be passed as first parameter
    for all PDP actions. Calling the m2mb_pdp_init is mandatory before using the PDP service.

  @param[in] *h
    handle to the PDP service, that will be initialized by the function.
  @param[in] callback
    callback that will be called if an event or a response happens.
  @param[in] userdata
    pointer to generic user data that will be returned as it is in the callback.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_pdp_init(&h, myCallback, myUserdata);
  @code
   void mycallback(M2MB_PDP_HANDLE h, M2MB_PDP_IND_E pdp_event, UINT8 cid, void *userdata)
   {
      //my code
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_PDP_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_pdp_init(&h, myCallback, (void*)myUserdata);

      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_pdp_init succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_RESULT_E m2mb_pdp_init( M2MB_PDP_HANDLE *h, m2mb_pdp_ind_callback callback, void *userdata );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_pdp_deinit deinitializes PDP service for current client.

  @details
    m2mb_pdp_deinit deinitialize PDP service for current client freeing the handle passed
    as first parameter. Calling the m2mb_pdp_deinit is useful after using the PDP service in order to free memory space.

  @param[in] h
    parameter is the handle to the PDP service, that will be deinitialized by the function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_pdp_deinit(h);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_PDP_HANDLE h;
    struct myStruct myUserdata;
    retVal = m2mb_pdp_init(&h, myCallback, (void*)myUserdata);

    //... use PDP APIs ...

    retVal = m2mb_pdp_deinit(h);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "m2mb_pdp_deinit succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_deinit( M2MB_PDP_HANDLE h );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_pdp_activate activates PDP context(or PDN connection)

  @details
    m2mb_pdp_activate permits to activate PDP context(or PDN connection) for a specific cid

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] APN
     access point name
  @param[in] user
     username if required by the APN
  @param[in] password
     password if required by the APN
  @param[in] pdpIPver
     pdp IP version(M2MB_PDP_IPV4V6 or M2MB_PDP_IPV6 or M2MB_PDP_IPV4)
  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note: if profile with the same cid is deleted with +CGDCONT m2mb_pdp_activate will not work

  @b
    m2mb_pdp_activate(h,cid,apn,user,password,pdpIPver);
  @code
    void mycallback(M2MB_PDP_HANDLE h, M2MB_PDP_IND_E pdp_event, UINT8 cid, void *userdata)
    {

      //............
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_PDP_HANDLE hndl;
      struct myStruct myUserdata;
      retVal = m2mb_pdp_init(&hndl, myCallback, (void*)myUserdata);

      retVal = m2mb_pdp_activate(hndl,3,"testAPN","","",M2MB_PDP_IPV4); //activates cid 3 with APN "testAPN" and IP type IPV4

      if ( retVal == M2MB_RESULT_SUCCESS )
          printf( "m2mb_pdp_activate  succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_RESULT_E m2mb_pdp_activate( M2MB_PDP_HANDLE h, UINT8 cid, CHAR *APN, CHAR *user, CHAR *password, M2MB_PDP_IP_TYPE_E pdpIPver );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_pdp_deactivate deactivates PDP context(or PDN connection)

  @details
    m2mb_pdp_deactivate permits to deactivate PDP context(or PDN connection) for a specific cid

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_pdp_deactivate(h,cid);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_deactivate( M2MB_PDP_HANDLE h, UINT8 cid );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_pdp_get_my_ip permits to get IP address

  @details
      m2mb_pdp_get_my_ip permits to get IP address obtained at PDP(or PDN) context activation for a specific cid

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] pdpIPver
    IP version(M2MB_PDP_IPV4 or M2MB_PDP_IPV6)
  @param[out] addr
    pointer to IP address struct to be filled in:
    in case of IPV4 address struct has to be M2MB_SOCKET_BSD_IN_ADDR,
    in case of IPv6 address struct has to be M2MB_SOCKET_BSD_IPV6_ADDR

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note: cannot be called directly inside callback loaded with m2mb_pdp_init

  @b
    m2mb_pdp_get_my_ip(h,cid,pdpIPver,addr);
  @code
    M2MB_SOCKET_BSD_IN_ADDR addrv4;
    M2MB_SOCKET_BSD_IPV6_ADDR addrv6;

      ret = m2mb_pdp_get_my_ip(hndl,cid,M2MB_PDP_IPV4,&addrv4);

      // ..........

      ret = m2mb_pdp_get_my_ip(hndl,cid,M2MB_PDP_IPV6,&addrv6);

      // ..........

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_get_my_ip( M2MB_PDP_HANDLE h, UINT8 cid, M2MB_PDP_IP_TYPE_E pdpIPver, void *addr );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to get address of the DNS

  @details
     permits to get IPV4 primary and secondary DNS or IPv6 primary and secondary DNS, for a specific cid

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] pdpIPver
    IP version(M2MB_PDP_IPV4 or M2MB_PDP_IPV6)
  @param[out] pDNS
    pointer to IP address struct of the primary DNS to be filled in:
    in case of IPV4 address struct has to be M2MB_SOCKET_BSD_IN_ADDR,
    in case of IPv6 address struct has to be M2MB_SOCKET_BSD_IPV6_ADDR
  @param[out] sDNS
    pointer to IP address struct of the secondary DNS to be filled in:
    in case of IPV4 address struct has to be M2MB_SOCKET_BSD_IN_ADDR,
    in case of IPv6 address struct has to be M2MB_SOCKET_BSD_IPV6_ADDR

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_pdp_get_DNS(h,cid,pDNS,sDNS);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_RESULT_E m2mb_pdp_get_DNS( M2MB_PDP_HANDLE h, UINT8 cid, M2MB_PDP_IP_TYPE_E pdpIPver, void *pDNS, void *sDNS );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to get PDP context(or PDN connection) status

  @details

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[out]
    pointer to PDP(or PDN) connection status for specific cid
    status 0: PDP inactive
    status 1: PDP active

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_pdp_get_status(h,cid,&status);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_get_status( M2MB_PDP_HANDLE h, UINT8 cid, UINT8 *p_status );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to set APN

  @details
    permits to set APN for specific context identifier

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] APN
    access point name

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note: if profile with the same cid is deleted with +CGDCONT m2mb_pdp_APN_set will not work

  @b
    m2mb_pdp_APN_set(h,cid,apn);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_APN_set( M2MB_PDP_HANDLE h, UINT8 cid, CHAR *APN );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to get APN

  @details
    permits to get APN for specific context identifier

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] APN
    buffer to retrieve APN
   @param[in] len
    APN buffer length

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note: if profile with the same cid is deleted with +CGDCONT m2mb_pdp_APN_get will not work

  @b
    m2mb_pdp_APN_get(h,cid,apn,len);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_APN_get( M2MB_PDP_HANDLE h, UINT8 cid, CHAR *APN, UINT8 len );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to get cid related to specific APN class

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] apnclass
   apnclass to be given to get corresponding cid
  @param[out] p_cid
   pointer to cid to be retrieved

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_pdp_apnclass2cid(h,apnclass,&cid);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_apnclass2cid( M2MB_PDP_HANDLE h, UINT8 apnclass, UINT8 *p_cid );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to get PDP Type

  @details
    permits to get PDP Type for specific context identifier

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] pdp_type
    to retrieve pdp IP Type

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note: if profile with the same cid is deleted with +CGDCONT m2mb_pdp_type_get will not work

  @b
    m2mb_pdp_type_get(h,cid,&pdp_type);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_type_get( M2MB_PDP_HANDLE h, UINT8 cid, M2MB_PDP_TYPE_E *pdp_type );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to set PDP Type

  @details
    permits to set PDP Type for specific context identifier

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] pdp_type
    to set pdp IP Type

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note: if profile with the same cid is deleted with +CGDCONT m2mb_pdp_type_set will not work

  @b
    m2mb_pdp_type_set(h,cid,pdp_type);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_type_set( M2MB_PDP_HANDLE h, UINT8 cid, M2MB_PDP_TYPE_E pdp_type );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to get Profile Name

  @details
    permits to get Profile Name for specific context identifier

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] profname
    buffer to retrieve Profile Name
  @param[in] len
    Profile Name buffer length

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
    if profile with the same cid is deleted with +CGDCONT m2mb_pdp_profile_name_get will not work
  @note
    if not supported by the platform it returns M2MB_RESULT_FAIL

  @b
    m2mb_pdp_profile_name_get(h, cid, profname, len);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_profile_name_get( M2MB_PDP_HANDLE h, UINT8 cid, CHAR *profname, UINT8 len );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to set Profile Name

  @details
    permits to set Profile Name for specific context identifier

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] profname
    buffer to set Profile Name

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
    if profile with the same cid is deleted with +CGDCONT m2mb_pdp_profile_name_set will not work
  @note
    if not supported by the platform it returns M2MB_RESULT_FAIL

  @b
    m2mb_pdp_profile_name_set(h, cid, profname);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_profile_name_set( M2MB_PDP_HANDLE h, UINT8 cid, CHAR *profname );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to get AUTH Type

  @details
    permits to get AUTH Type for specific context identifier

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] pdp_authtype
    to retrieve AUTH Type

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note: if profile with the same cid is deleted with +CGDCONT m2mb_pdp_authtype_get will not work

  @b
    m2mb_pdp_authtype_get(h,cid,pdp_authtype);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_authtype_get( M2MB_PDP_HANDLE h, UINT8 cid, M2MB_PDP_AUTHTYPE_E *pdp_authtype );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    permits to set AUTH Type

  @details
    permits to set AUTH Type for specific context identifier

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    context identifier
  @param[in] pdp_authtype
    to set AUTH Type

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note: if profile with the same cid is deleted with +CGDCONT m2mb_pdp_authtype_set will not work

  @b
    m2mb_pdp_authtype_set(h,cid,pdp_authtype);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_authtype_set( M2MB_PDP_HANDLE h, UINT8 cid, M2MB_PDP_AUTHTYPE_E pdp_authtype );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    allows to get data traffic statistics (in bytes) on a given PDP

  @details
    allows to get data traffic statistics (in bytes) on a given PDP

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    PDP context identifier (value of 0 with action RESET, resets all counters).
  @param[in] datavol_action
    to set the action (RESET / GET current session counters / GET life counters).
  @param[out] tx
    pointer to retreive sent bytes statistics
  @param[out] rx
    pointer to retreive received bytes statistics

  @return
    returns M2MB_RESULT_SUCCESS on success, M2MB_RESULT_FAIL on error.

  @note
    if not supported by the platform it returns M2MB_RESULT_FAIL

  @b
    m2mb_pdp_datavol_get(h,cid,datavol_action,tx,rx);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_datavol_get( M2MB_PDP_HANDLE h, UINT8 cid, M2MB_PDP_DATAVOL_ACTION_E datavol_action, UINT32 *tx, UINT32 *rx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    allows to get bytes and packets statistics (in bytes) on a given PDP

  @details
    allows to get bytes and packets statistics (in bytes) on a given PDP

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[in] cid
    PDP context identifier.
  @param[in] stats_action
    to set the action.
  @param[out] stats
    pointer to retrieve bytes and packets statistics.

  @return
    returns M2MB_RESULT_SUCCESS on success, M2MB_RESULT_FAIL on error.

  @note

  @b
    m2mb_pdp_stats(h,cid,datavol_action,stats);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_stats( M2MB_PDP_HANDLE h, UINT8 cid, M2MB_PDP_STATS_ACTION_E stats_action, M2MB_PDP_STATS_T* stats );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    function used to get the dfefault PDP context number

  @details
  the functions allows to get the default PDP context number based on the customization variant of the module

  @param[in] h
    handle to the PDP service, previously initialized by the m2mb_pdp_init function.
  @param[out] cid
    pointer to PDP context identifier.

  @return
    returns M2MB_RESULT_SUCCESS on success, M2MB_RESULT_FAIL on error.

  @note

  @b
    m2mb_pdp_default_cid_get(h,cid);
  @code
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_pdp_default_cid_get(M2MB_PDP_HANDLE h, UINT8* cid);


/*! \endcond */



#endif  /* M2M_M2MB_PDP_API_H */

