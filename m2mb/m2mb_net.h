/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_common/m2mb_inc/m2mb_net.h

  @brief
    Header file for m2mb_net APIs

  @details
    m2mb_net APIs provide actions and events for network events management.

  @author
    S. Passiu

  @date
    06/07/2017
*/

#ifndef  M2M_M2MB_NET_API_H
#define  M2M_M2MB_NET_API_H


/* Global declarations ==========================================================================*/

/* Global typedefs ==============================================================================*/

typedef HANDLE M2MB_NET_HANDLE;

typedef enum
{
  M2MB_NET_SUCCESS,
  M2MB_NET_GENERIC_FAILURE
} M2MB_NET_ERRNO_E;

// see ETSI TS 27.007 V14 - commands +CREG, +CGREG and +CEREG
typedef enum
{
  M2MB_NET_RAT_UNKNOWN = -1,
  M2MB_NET_RAT_GSM,
  M2MB_NET_RAT_GSM_Compact,
  M2MB_NET_RAT_UTRAN,
  M2MB_NET_RAT_GSM_wEGPRS,
  M2MB_NET_RAT_UTRAN_wHSDPA,
  M2MB_NET_RAT_UTRAN_wHSUPA,
  M2MB_NET_RAT_UTRAN_wHSDPAandHSUPA,
  M2MB_NET_RAT_EUTRAN,
  M2MB_NET_RAT_EC_GSM_IoT,
  M2MB_NET_RAT_EUTRAN_NB_S1
} M2MB_NET_RAT_E;

typedef enum
{
  M2MB_NET_STAT_NOT_REGISTERED,
  M2MB_NET_STAT_REGISTERED_HOME,
  M2MB_NET_STAT_SEARCHING,
  M2MB_NET_STAT_REGISTRATION_DENIED,
  M2MB_NET_STAT_UNKNOWN,
  M2MB_NET_STAT_REGISTERED_ROAMING
} M2MB_NET_STAT_E;

typedef enum
{
  M2MB_NET_AVAILABILITY_UNKNOWN,
  M2MB_NET_AVAILABILITY_AVAILABLE,
  M2MB_NET_AVAILABILITY_CURRENT,
  M2MB_NET_AVAILABILITY_FORBIDDEN
} M2MB_NET_AVAILABILITY_E;

typedef enum
{
  M2MB_NET_NULL_RESP,
  M2MB_NET_GET_SIGNAL_INFO_RESP,
  M2MB_NET_GET_BER_RESP,
  M2MB_NET_GET_CURRENT_OPERATOR_INFO_RESP,
  M2MB_NET_GET_CURRENT_CELL_LIST_RESP,
  M2MB_NET_GET_AVAILABLE_NW_LIST_RESP,
  M2MB_NET_GET_REG_STATUS_INFO_RESP,
  M2MB_NET_REG_STATUS_IND,                 /* Network registration state change indication */
  M2MB_NET_GET_EDRX_INFO_RESP,
  M2MB_NET_SET_EDRX_INFO_RESP,
  M2MB_NET_ENABLE_IND_RESP,
  M2MB_NET_GET_MODE_PREFERENCE_RESP,       /* Network Mode Preference Get response */
  M2MB_NET_SET_MODE_PREFERENCE_RESP,       /* Network Mode Preference Set response */
  M2MB_NET_MODE_PREFERENCE_IND             /* Network Mode Preference indication */
} M2MB_NET_IND_E;

typedef enum
{
  M2MB_NET_DUMMY,
  M2MB_NET_SET_ENABLE_IND_ASYNC,
  M2MB_NET_GET_ENABLE_IND_ASYNC
} M2MB_NET_ACTION_E;

typedef enum
{
  M2MB_NET_RESP_SET_NONE,
  M2MB_NET_RESP_SET_FULL,
  M2MB_NET_RESP_SET_SUBSET1,
  M2MB_NET_RESP_SET_SUBSET2,
  M2MB_NET_RESP_SET_SUBSET3,
  M2MB_NET_RESP_SET_SUBSET4,
  M2MB_NET_RESP_SET_SUBSET5
} M2MB_NET_RESP_SET_E;

typedef enum
{
  M2MB_NET_LTE_HANDOVER_SERVING,
  M2MB_NET_LTE_HANDOVER_INTRA_FREQ,
  M2MB_NET_LTE_HANDOVER_INTER_FREQ,
} M2MB_NET_LTE_HANDOVER_E;

typedef enum
{
  M2MB_NET_SRV_DOMAIN_NO_SRV,
  M2MB_NET_SRV_DOMAIN_CS_ONLY,
  M2MB_NET_SRV_DOMAIN_PS_ONLY,
  M2MB_NET_SRV_DOMAIN_CS_PS,
  M2MB_NET_SRV_DOMAIN_CAMPED
} M2MB_NET_SRV_DOMAIN_E;

typedef enum
{
  M2MB_NET_CELL_TYPE_SERVING,
  M2MB_NET_CELL_TYPE_NEIGHBOR
} M2MB_NET_CELL_TYPE_E;

typedef enum
{
  M2MB_NET_ACC_TECH_TYPE_NO_SRV,
  M2MB_NET_ACC_TECH_TYPE_CDMA_1X,
  M2MB_NET_ACC_TECH_TYPE_CDMA_1XEVDO,
  M2MB_NET_ACC_TECH_TYPE_AMPS,
  M2MB_NET_ACC_TECH_TYPE_GSM,
  M2MB_NET_ACC_TECH_TYPE_UMTS,
  M2MB_NET_ACC_TECH_TYPE_WLAN,
  M2MB_NET_ACC_TECH_TYPE_GPS,
  M2MB_NET_ACC_TECH_TYPE_LTE,
  M2MB_NET_ACC_TECH_TYPE_TDSCDMA,
  M2MB_NET_ACC_TECH_TYPE_LTE_M1,
  M2MB_NET_ACC_TECH_TYPE_LTE_NB1,
  M2MB_NET_ACC_TECH_TYPE_MAX
} M2MB_NET_ACC_TECH_TYPE_E;

typedef enum
{
  M2MB_NET_ERR_OK,
  M2MB_NET_ERR_UNKNOWN
} M2MB_NET_ERR_CAUSE_E;

typedef enum
{
  M2MB_NET_ENABLE_IND_ASYNC_DISABLED,
  M2MB_NET_ENABLE_IND_ASYNC_ENABLED
} M2MB_NET_ENABLE_IND_ASYNC_E;

typedef enum
{
  M2MB_NET_SELECTED_WIRELESS_GERAN = 12,
  M2MB_NET_SELECTED_WIRELESS_UTRAN = 22,
  M2MB_NET_SELECTED_WIRELESS_GERAN_UTRAN_EUTRAN = 25,
  M2MB_NET_SELECTED_WIRELESS_EUTRAN = 28,
  M2MB_NET_SELECTED_WIRELESS_GERAN_UTRAN = 29,
  M2MB_NET_SELECTED_WIRELESS_GERAN_EUTRAN = 30,
  M2MB_NET_SELECTED_WIRELESS_UTRAN_EUTRAN = 31,
} M2MB_NET_SELECTED_WIRELESS_NETWORK_E;

typedef enum
{
  M2MB_NET_LTE_CIOT_PREFERENCE_CAT_M1 = 0,        /* CAT-M exclusive */
  M2MB_NET_LTE_CIOT_PREFERENCE_NB_IOT = 1,        /* NB-IoT exclusive */
  M2MB_NET_LTE_CIOT_PREFERENCE_CAT_M1_NB_IOT = 2, /* Both CAT-M and NB-IoT, CAT-M preferred */
  M2MB_NET_LTE_CIOT_PREFERENCE_NB_IOT_CAT_M1 = 3  /* Both CAT-M and NB-IoT, NB-IoT preferred */
} M2MB_NET_LTE_CIOT_PREFERENCE_E;

typedef struct
{
  M2MB_NET_ERR_CAUSE_E err;
} M2MB_NET_ERR_CAUSE_T;

typedef struct
{
  M2MB_NET_ERR_CAUSE_E err;
  M2MB_RESULT_E response;
} M2MB_NET_GENERIC_RESP_T;

typedef struct
{
  M2MB_NET_ERR_CAUSE_E err;
  M2MB_NET_STAT_E stat; //Network status
  M2MB_NET_RAT_E rat; //Radio Access Technology
  M2MB_NET_SRV_DOMAIN_E srvDomain;
  UINT16 areaCode; //LAC or TAC, depending on the current RAT
  UINT32 cellID;
} M2MB_NET_REG_STATUS_T;

typedef struct
{
  M2MB_NET_ERR_CAUSE_E err;
  M2MB_NET_ACC_TECH_TYPE_E acc_tech;
  UINT8 edrx;
  UINT8 enabled;
} M2MB_NET_GET_EDRX_INFO_RESP_T;

typedef struct
{
  INT16 ecio;
} M2MB_NET_SIGNAL_INFO_UTRAN_T;

typedef struct
{
  INT8 rsrq; // Reference Signal Received Quality
  INT16 rsrp; // Reference Signal Received Power
  INT16 snr;
} M2MB_NET_SIGNAL_INFO_EUTRAN_T;

typedef struct
{
  M2MB_NET_ERR_CAUSE_E err;
  M2MB_NET_RAT_E rat;
  INT8 rssi; //Received signal strength indication
  UINT16 sigInfo_size;
  void *sigInfo;
} M2MB_NET_GET_SIGNAL_INFO_RESP_T;

typedef struct
{
  M2MB_NET_ERR_CAUSE_E err;
  M2MB_NET_RAT_E rat;
  INT16 ber; //Bit error rate
} M2MB_NET_GET_BER_RESP_T;

typedef struct
{
  INT32 nARFCN;
  INT32 nBSIC;
  INT32 nSignalStrength;
} M2M_T_NETWORK_CELL_NEIGHBOR_T;

typedef struct
{
  UINT32 cell_id;
  UINT16 mcc;
  UINT16 mnc;
  UINT16 lac;
  UINT16 arfcn;
  UINT8 bsic;
  UINT32 timing_advance;
  UINT16 rx_lev;
} M2MB_NET_CELL_GERAN_FULL_T;

typedef struct
{
  UINT32 cell_id;
  UINT16 lac;
  UINT16 arfcn;
  UINT8 bsic;
  UINT16 rx_lev;
} M2MB_NET_CELL_GERAN_SUBSET1_T;

typedef struct
{
  UINT16 arfcn;
  UINT8 bsic;
  UINT16 rx_lev;
  INT16 srxlev; //Cell selection Rx level
} M2MB_NET_CELL_GERAN_SUBSET2_T;

typedef union
{
  M2MB_NET_CELL_GERAN_FULL_T full;
  M2MB_NET_CELL_GERAN_SUBSET1_T subset1;
  M2MB_NET_CELL_GERAN_SUBSET2_T subset2;
} M2MB_NET_CELL_GERAN_U;

typedef struct
{
  UINT32 cell_id;
  UINT16 mcc;
  UINT16 mnc;
  UINT16 lac;
  UINT16 uarfcn;
  UINT16 psc;
  INT16 rscp;
  INT16 ecio;
} M2MB_NET_CELL_UTRAN_FULL_T;

typedef struct
{
  UINT16 uarfcn;
  UINT16 psc;
  INT16 rscp;
  INT16 ecio;
  INT16 srxlev; //Cell selection Rx level
} M2MB_NET_CELL_UTRAN_SUBSET1_T;

typedef union
{
  M2MB_NET_CELL_UTRAN_FULL_T full;
  M2MB_NET_CELL_UTRAN_SUBSET1_T subset1;
} M2MB_NET_CELL_UTRAN_U;

typedef struct
{
  M2MB_NET_LTE_HANDOVER_E lteType;
  UINT32 cell_id;
  UINT16 mcc;
  UINT16 mnc;
  UINT16 tac;
  UINT16 earfcn;
  UINT16 pci;
  INT16 rsrq;
  INT16 rsrp;
  INT16 rx_lev;
  INT16 srxlev; //Cell selection Rx level
} M2MB_NET_CELL_EUTRAN_FULL_T;

typedef struct
{
  M2MB_NET_LTE_HANDOVER_E lteType;
  UINT16 earfcn;
  UINT16 pci;
  INT16 rsrq;
  INT16 rsrp;
  INT16 rx_lev;
  INT16 srxlev; //Cell selection Rx level
} M2MB_NET_CELL_EUTRAN_SUBSET1_T;

typedef union
{
  M2MB_NET_CELL_EUTRAN_FULL_T full;
  M2MB_NET_CELL_EUTRAN_SUBSET1_T subset1;
} M2MB_NET_CELL_EUTRAN_U;

typedef struct M2MB_NET_CELL_NEIGHBOR_T
{
  M2MB_NET_CELL_TYPE_E cellType;
  M2MB_NET_RAT_E cellRat;
  M2MB_NET_RESP_SET_E respSet;
  UINT16 cell_size;
  void *cell;
  struct M2MB_NET_CELL_NEIGHBOR_T *next;
  BOOLEAN lte_ue_in_idle; //Radio Status
} M2MB_NET_CELL_NEIGHBOR_T;

typedef struct
{
  M2MB_NET_ERR_CAUSE_E err;
  UINT16 neighbors_size;
  M2MB_NET_CELL_NEIGHBOR_T *neighbors;
} M2MB_NET_GET_CURRENT_CELL_LIST_RESP_T;

typedef struct
{
  M2MB_NET_ERR_CAUSE_E err;
  UINT16 mcc;
  UINT16 mnc;
  M2MB_NET_RAT_E rat; //Radio Access Technology
} M2MB_NET_GET_CURRENT_OPERATOR_INFO_RESP_T;

typedef struct M2MB_NET_DESCRIPTION_T
{
  UINT16 mcc;
  UINT16 mnc;
  M2MB_NET_AVAILABILITY_E networkAv;
  M2MB_NET_RAT_E rat;
  struct M2MB_NET_DESCRIPTION_T *next;
} M2MB_NET_DESCRIPTION_T;

typedef struct
{
  M2MB_NET_ERR_CAUSE_E err;
  UINT16 availableNetworks_size;
  M2MB_NET_DESCRIPTION_T *availableNetworks;
} M2MB_NET_GET_AVAILABLE_NW_LIST_RESP_T;

typedef struct
{
  M2MB_NET_SELECTED_WIRELESS_NETWORK_E selectedWirelessNetwork;
  M2MB_NET_LTE_CIOT_PREFERENCE_E lte_ciot_preference;
} M2MB_NET_SELECTED_WIRELESS_NETWORK_RESP_T;

typedef M2MB_NET_SELECTED_WIRELESS_NETWORK_RESP_T M2MB_NET_MODE_PREFERENCE_T;

typedef struct
{
  M2MB_NET_ERR_CAUSE_E       err;             /* error status */
  M2MB_NET_MODE_PREFERENCE_T mode_preference; /* mode preference */
} M2MB_NET_MODE_PREFERENCE_STATUS_T;

typedef void ( *m2mb_net_ind_callback )( M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *userdata );

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_init initializes NET service for current client.

  @details
    m2mb_net_init initialize NET service for current client returning the handle that must be passed as first parameter
    for all NET actions. Calling the m2mb_net_init is mandatory before using the NET service.

  @param[in] *h
    It is the handle to the NET service, that will be initialized by the function.
  @param[in] callback
    It is the callback that will be called if an event or a response happens.
  @param[in] userdata
    It is a pointer to generic user data that will be returned as it is in the callback.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_net_init(&h, myCallback, myUserdata);
  @code
    void myCallback(M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      //my code
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_net_init succeeded");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_net_init( M2MB_NET_HANDLE *h, m2mb_net_ind_callback callback, void *userdata );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_deinit deinitializes NET service for current client.

  @details
    m2mb_net_deinit deinitialize NET service for current client freeing the handle passed
    as first parameter. Calling the m2mb_net_deinit is useful after using the NET service in order to free memory space.

  @param[in] h
    It is the handle to the NET service, that will be deinitialized by the function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_net_deinit(&h);
  @code
    M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
    M2MB_NET_HANDLE h;
    struct myStruct myUserdata;
    retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);

    //... use NET APIs ...

    retVal = m2mb_net_deinit(h);
    if ( retVal == M2MB_RESULT_SUCCESS )
      printf( "m2mb_net_deinit succeeded");
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_net_deinit( M2MB_NET_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_conf sets/gets some NET parameters.

  @details
    m2mb_net_conf sets/gets NET parameters listed in the M2MB_NET_ACTION_E enum.

  @param[in] h
    Handle to the NET service, previously initialized by the m2mb_net_init function.
  @param[in] argc
    It determines the number of parameters that follows, plus one (e.g.: if two parameters follow, argc is 3).
  @param[in]
    It determines the action (among the ones listed in M2MB_NET_ACTION_E), while the parameters
    that follow depend on the action (see following note for further details).

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
    Action: M2MB_NET_SET_ENABLE_IND_ASYNC

    Description: choose the behaviour of the m2mb_net_enable_ind function: synchronous (default) or asynchronous.
      The asynchronous mode can be necessary when the calling function can't be locked for long time (eg.: a callback function).
      In asynchronous mode, the result of the m2mb_net_enable_ind is confirmed by the M2MB_NET_ENABLE_IND_RESP event.

    Params(in): h(M2MB_NET_HANDLE), 3, action(M2MB_NET_ACTION_E), syncOrAsync(M2MB_NET_ENABLE_IND_ASYNC_E)

    - example: set the m2mb_net_enable_ind function as asynchronous

    m2mb_net_conf(h, 3, M2MB_NET_SET_ENABLE_IND_ASYNC, M2MB_NET_ENABLE_IND_ASYNC_ENABLED);


    Action: M2MB_NET_GET_ENABLE_IND_ASYNC

    Description: get the current status (synchronous or asynchronous) of the m2mb_net_enable_ind behaviour.

    Params(in): h(M2MB_NET_HANDLE), 3, action(M2MB_NET_ACTION_E), syncOrAsync(M2MB_NET_ENABLE_IND_ASYNC_E*)

    - example: get current status

    M2MB_NET_ENABLE_IND_ASYNC_E currentStatus;
    m2mb_net_conf(h, 3, M2MB_NET_GET_ENABLE_IND_ASYNC, &currentStatus);

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_net_conf( M2MB_NET_HANDLE h, UINT8 argc, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_enable_ind enables the indications of network state change events.

  @details
    m2mb_net_enable_ind enables the indications of network state change events through the callback function
    previously registered with the m2mb_net_init.

  @param[in] h
    It is the handle to the NET service, previously initialized by the m2mb_net_init function.
  @param[in] ind
    Determines the event (among the ones ending with the _IND suffix, listed in M2MB_NET_IND_E) to be
    enabled/disabled.
  @param[in] enable
    It is 0 to disable or 1 to enable the event "ind".

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note
    The indication event enabled with the m2mb_net_enable_ind will be received in the callback function
    previously registerd with the m2mb_net_init, when the event happens.
    If the event is M2MB_NET_REG_STATUS_IND the resp_struct must be casted to the M2MB_NET_REG_STATUS_T type.

  @b
    m2mb_net_enable_ind(h, M2MB_NET_REG_STATUS_IND, 1);
  @code
    void myCallback(M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(net_event)
      {
        //...

        case M2MB_NET_REG_STATUS_IND:
        {
          M2MB_NET_REG_STATUS_T *resp = (M2MB_NET_REG_STATUS_T*)resp_struct;
          printf( M2MB_TC_GENERIC, M2MB_TL_LOG, "Reg Status INDICATION: %d,%d,%d,%X,%lX", resp->rat, resp->stat, resp->srvDomain, resp->areaCode, resp->cellID );
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);

      retVal = m2mb_net_enable_ind(h, M2MB_NET_REG_STATUS_IND, 1);

      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "M2MB_NET_REG_STATUS_IND enabled");
    }
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_net_enable_ind( M2MB_NET_HANDLE h, M2MB_NET_IND_E ind, UINT8 enable );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_get_signal_info gets information about signal strength.

  @details
    m2mb_net_get_signal_info gets information about signal strength.
    The function is asynchronous. This means that the function returns immediately, but the signal information is received
    in the callback previously set by the m2mb_net_init function.
    The response event received in the callback will be the M2MB_NET_GET_SIGNAL_INFO_RESP. The resp_struct must be casted to
    the M2MB_NET_GET_SIGNAL_INFO_RESP_T type.

  @param[in] h
    It is the handle to the NET service, previously initialized by the m2mb_net_init function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_net_get_signal_info(h);
  @code
    void myCallback(M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(net_event)
      {
        //...

        case M2MB_NET_GET_SIGNAL_INFO_RESP:
        {
          M2MB_NET_GET_SIGNAL_INFO_RESP_T *resp = (M2MB_NET_GET_SIGNAL_INFO_RESP_T*)resp_struct;
          printf("GET Signal Info resp is %d, %d", resp->rat, resp->rssi);
          if((resp->sigInfo != NULL) && (resp->rat == M2MB_NET_RAT_UTRAN) || ((resp->rat >= M2MB_NET_RAT_UTRAN_wHSDPA) && (resp->rat <= M2MB_NET_RAT_UTRAN_wHSDPAandHSUPA)))
          {
            M2MB_NET_SIGNAL_INFO_UTRAN_T *tmpSigInfo = (M2MB_NET_SIGNAL_INFO_UTRAN_T*)(resp->sigInfo);
            printf("GET Signal Info resp is %d", tmpSigInfo->ecio);
          }
          else if((resp->sigInfo != NULL) && (resp->rat == M2MB_NET_RAT_EUTRAN))
          {
            M2MB_NET_SIGNAL_INFO_EUTRAN_T *tmpSigInfo = (M2MB_NET_SIGNAL_INFO_EUTRAN_T*)(resp->sigInfo);
            printf("GET Signal Info resp is %d, %d, %d", tmpSigInfo->rsrq, tmpSigInfo->rsrp, tmpSigInfo->snr);
          }
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_net_get_signal_info(h);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_net_get_signal_info request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_net_get_signal_info( M2MB_NET_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_get_ber gets information about bit error rate.

  @details
    m2mb_net_get_ber gets information about bit error rate of current network connection.
    The function is asynchronous. This means that the function returns immediately, but the signal information is received
    in the callback previously set by the m2mb_net_init function.
    The response event received in the callback will be the M2MB_NET_GET_BER_RESP. The resp_struct must be casted to
    the M2MB_NET_GET_BER_RESP_T type.

  @param[in] h
    It is the handle to the NET service, previously initialized by the m2mb_net_init function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_net_get_ber(h);
  @code
    void myCallback(M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(net_event)
      {
        //...

        case M2MB_NET_GET_BER_RESP:
        {
          M2MB_NET_GET_BER_RESP_T *resp = (M2MB_NET_GET_BER_RESP_T*)resp_struct;
          printf("GET BER resp is %d, %d", resp->rat, resp->ber);
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_net_get_ber(h);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_net_get_ber request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_net_get_ber( M2MB_NET_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_get_wireless_network_selection queries the modem for the
    wireless network selection and for the LTE camped IoT preference.
  @details
    m2mb_net_get_wireless_network_selection retrieves the same information of the
    AT commands AT+WS46 and AT#WS46, getting information about the wireless network
    selection and the LTE camped IoT preference, respectively.
    This function in synchronous, and a M2MB_NET_SELECTED_WIRELESS_NETWORK_RESP_T
    structure is populated with the response.
  @param[in] h
    the handle to the NET service.
  @param[out] resp
    pointer where to store the response.
  @return
    M2MB_RESULT_SUCCESS when the function succeeds.
  @return
    M2MB_RESULT_INVALID_ARG if resp is NULL.
  @return
    M2MB_RESULT_FAIL if the function fails.
 */
M2MB_RESULT_E m2mb_net_get_wireless_network_selection( M2MB_NET_HANDLE h, M2MB_NET_SELECTED_WIRELESS_NETWORK_RESP_T* resp );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_get_mode_preference gets information about the mode preference for the network to attach.

  @details
    The function is asynchronous. This means that the function returns immediately,
    but the mode preference information is received in the callback previously set by the m2mb_net_init function.
    The response event received in the callback will be the M2MB_NET_GET_MODE_PREFERENCE_RESP.
    The resp_struct must be casted to the M2MB_NET_MODE_PREFERENCE_STATUS_T type.

  @param[in] h
    It is the handle to the NET service, previously initialized by the m2mb_net_init function.

  @return
    M2MB_RESULT_SUCCESS when the function succeeds.
  @return
    M2MB_RESULT_INVALID_ARG if h is NULL.
  @return
    M2MB_RESULT_FAIL if the function fails.

  @note

  @b
    m2mb_net_get_mode_preference( h );
  @code
    void myCallback( M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata )
    {
      switch( net_event )
      {
        //...

        case M2MB_NET_GET_MODE_PREFERENCE_RESP:
        {
          if( resp_struct )
          {
            M2MB_NET_MODE_PREFERENCE_STATUS_T *status = ( M2MB_NET_MODE_PREFERENCE_STATUS_T* )resp_struct;
            if( status->err == M2MB_NET_ERR_OK )
            {
              printf( "MODE PREFERENCE STATUS is: selectedWirelessNetwork %d, lte_ciot_preference %d",
                      status->mode_preference.selectedWirelessNetwork, status->mode_preference.lte_ciot_preference );
            }
            else
            {
              printf( "M2MB_NET_GET_MODE_PREFERENCE_RESP got error %d", status->err );
            }
          }
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init( &h, myCallback, (void*)myUserdata );
      //...
      retVal = m2mb_net_get_mode_preference( h );
      if ( retVal == M2MB_RESULT_SUCCESS )
      {
        printf( "m2mb_net_get_mode_preference request succeeded" );
      }
      else
      {
        printf( "m2mb_net_get_mode_preference request got error %d", retVal );
      }
    }
  @endcode
*/
M2MB_RESULT_E m2mb_net_get_mode_preference( M2MB_NET_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_get_current_operator_info gets information about current operator.

  @details
    m2mb_net_get_current_operator_info gets information about current registered network operator.
    The function is asynchronous. This means that the function returns immediately, but the signal information is received
    in the callback previously set by the m2mb_net_init function.
    The response event received in the callback will be the M2MB_NET_GET_CURRENT_OPERATOR_INFO_RESP. The resp_struct must be casted to
    the M2MB_NET_GET_CURRENT_OPERATOR_INFO_RESP_T type.

  @param[in] h
    It is the handle to the NET service, previously initialized by the m2mb_net_init function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_net_get_current_operator_info(h);
  @code
    void myCallback(M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(net_event)
      {
        //...

        case M2MB_NET_GET_CURRENT_OPERATOR_INFO_RESP:
        {
          M2MB_NET_GET_CURRENT_OPERATOR_INFO_RESP_T *resp = (M2MB_NET_GET_CURRENT_OPERATOR_INFO_RESP_T*)resp_struct;
          printf("GET curren operator resp is %d, %d", resp->mcc, resp->mnc);
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_net_get_current_operator_info(h);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_net_get_current_operator_info request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_net_get_current_operator_info( M2MB_NET_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_get_current_cell_list gets a list containing current cell neighbors.

  @details
    m2mb_net_get_current_cell_list gets a list containing current cell neighbors, including serving cell.
    The function is asynchronous. This means that the function returns immediately, but the signal information is received
    in the callback previously set by the m2mb_net_init function.
    The response event received in the callback will be the M2MB_NET_GET_CURRENT_CELL_LIST_RESP. The resp_struct must be casted to
    the M2MB_NET_GET_CURRENT_CELL_LIST_RESP_T type.

  @param[in] h
    It is the handle to the NET service, previously initialized by the m2mb_net_init function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_net_get_current_cell_list(h);
  @code
    void myCallback(M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      M2MB_NET_CELL_NEIGHBOR_T *listPnt;
      switch(net_event)
      {
        //...

        case M2MB_NET_GET_CURRENT_CELL_LIST_RESP:
        {
          M2MB_NET_GET_CURRENT_CELL_LIST_RESP_T *resp = (M2MB_NET_GET_CURRENT_CELL_LIST_RESP_T*)resp_struct;
          printf("GET list length is %d", resp->neighbors_size);
          listPnt = resp->neighbors;
          while(listPnt != NULL)
          {
            switch(listPnt->cellRat)
            {
              case M2MB_NET_RAT_GSM:
              case M2MB_NET_RAT_GSM_Compact:
              case M2MB_NET_RAT_GSM_wEGPRS:
              {
                M2MB_NET_CELL_GERAN_U *geranInfo = (M2MB_NET_CELL_GERAN_U*) listPnt->cell;
                switch(listPnt->respSet)
                {
                  M2MB_NET_RESP_SET_FULL:
                    printf("Geran Full info are %ld, %d, %d, %d, %ld, %d", geranInfo->full.cell_id, geranInfo->full.lac, geranInfo->full.arfcn, geranInfo->full.bsic, geranInfo->full.timing_advance, geranInfo->full.rx_lev);
                    break;
                  M2MB_NET_RESP_SET_SUBSET1:
                    printf("Geran sub1 info are %ld, %d, %d, %d, %d", geranInfo->subset1.cell_id, geranInfo->subset1.lac, geranInfo->subset1.arfcn, geranInfo->subset1.bsic, geranInfo->subset1.rx_lev);
                    break;
                  M2MB_NET_RESP_SET_SUBSET2:
                    printf("Geran sub2 info are %d, %d, %d", geranInfo->subset2.arfcn, geranInfo->subset2.bsic, geranInfo->subset2.rx_lev);
                    break;
                  default:
                    break;
                }
                break;
              }
              case M2MB_NET_RAT_UTRAN:
              case M2MB_NET_RAT_UTRAN_wHSDPA:
              case M2MB_NET_RAT_UTRAN_wHSUPA:
              case M2MB_NET_RAT_UTRAN_wHSDPAandHSUPA:
              {
                M2MB_NET_CELL_UTRAN_U *utranInfo = (M2MB_NET_CELL_UTRAN_U*) listPnt->cell;
                switch(listPnt->respSet)
                {
                  M2MB_NET_RESP_SET_FULL:
                    printf("Utran Full info are %ld, %d, %d, %d, %d, %d", utranInfo->full.cell_id, utranInfo->full.lac, utranInfo->full.uarfcn, utranInfo->full.psc, utranInfo->full.rscp, utranInfo->full.ecio);
                    break;
                  M2MB_NET_RESP_SET_SUBSET1:
                    printf("Utran sub1 info are %d, %d, %d, %d", utranInfo->subset1.uarfcn, utranInfo->subset1.psc, utranInfo->subset1.rscp, utranInfo->subset1.ecio);
                    break;
                  default:
                    break;
                }
                break;
              }
              case M2MB_NET_RAT_EUTRAN:
              {
                M2MB_NET_CELL_EUTRAN_U *eutranInfo = (M2MB_NET_CELL_EUTRAN_U*) listPnt->cell;
                switch(listPnt->respSet)
                {
                  M2MB_NET_RESP_SET_FULL:
                    printf("Eutran Full info are %ld, %d, %d, %d, %d, %d, %d", eutranInfo->full.cell_id, eutranInfo->full.tac, eutranInfo->full.earfcn, eutranInfo->full.pci, eutranInfo->full.rsrq, eutranInfo->full.rsrp, eutranInfo->full.rx_lev);
                    break;
                  M2MB_NET_RESP_SET_SUBSET1:
                    printf("Eutran sub1 info are %d, %d, %d, %d", eutranInfo->subset1.earfcn, eutranInfo->subset1.pci, eutranInfo->subset1.rsrq, eutranInfo->subset1.rsrp, eutranInfo->subset1.rx_lev);
                    break;
                  default:
                    break;
                }
                break;
              }
            }
            listPnt = listPnt->next;
          }
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_net_get_current_cell_list(h);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_net_get_current_cell_list request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_net_get_current_cell_list( M2MB_NET_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_get_available_nw_list gets a list containing current availble networks.

  @details
    m2mb_net_get_available_nw_list gets a list containing current availble networks.
    The function is asynchronous. This means that the function returns immediately, but the signal information is received
    in the callback previously set by the m2mb_net_init function.
    The response event received in the callback will be the M2MB_NET_GET_AVAILABLE_NW_LIST_RESP. The resp_struct must be casted to
    the M2MB_NET_GET_AVAILABLE_NW_LIST_RESP_T type.

  @param[in] h
    It is the handle to the NET service, previously initialized by the m2mb_net_init function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_net_get_available_nw_list(h);
  @code
    void myCallback(M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      M2MB_NET_DESCRIPTION_T *listPnt;
      switch(net_event)
      {
        //...

        case M2MB_NET_GET_AVAILABLE_NW_LIST_RESP:
        {
          M2MB_NET_GET_AVAILABLE_NW_LIST_RESP_T *resp = (M2MB_NET_GET_AVAILABLE_NW_LIST_RESP_T*)resp_struct;
          printf("GET list length is %d", resp->availableNetworks_size);
          listPnt = resp->availableNetworks;
          while(listPnt != NULL)
          {
            printf("Available NW info are %d, %d, %d, %d", listPnt->mcc, listPnt->mnc, listPnt->networkAv, listPnt->rat);
            listPnt = listPnt->next;
          }
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_net_get_available_nw_list(h);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_net_get_available_nw_list request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_net_get_available_nw_list( M2MB_NET_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_get_reg_status_info gets information about regitration status.

  @details
    m2mb_net_get_reg_status_info gets information about regitration status to the network.
    The function is asynchronous. This means that the function returns immediately, but the signal information is received
    in the callback previously set by the m2mb_net_init function.
    The response event received in the callback will be the M2MB_NET_GET_REG_STATUS_INFO_RESP. The resp_struct must be casted to
    the M2MB_NET_REG_STATUS_T type.

  @param[in] h
    It is the handle to the NET service, previously initialized by the m2mb_net_init function.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_net_get_reg_status_info(h);
  @code
    void myCallback(M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(net_event)
      {
        //...

        case M2MB_NET_GET_REG_STATUS_INFO_RESP:
        {
          M2MB_NET_REG_STATUS_T *resp = (M2MB_NET_REG_STATUS_T*)resp_struct;
          printf("GET NET status resp is %d, %d, %d, %d, %ld", resp->stat, resp->rat, resp->srvDomain, resp->areaCode, resp->cellID);
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_net_get_reg_status_info(h);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_net_get_reg_status_info request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_net_get_reg_status_info( M2MB_NET_HANDLE h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_get_edrx_info gets information about edrx.

  @details
    m2mb_net_get_edrx_info gets information about edrx.
    The function is asynchronous. This means that the function returns immediately, but the signal information is received
    in the callback previously set by the m2mb_net_init function.
    The response event received in the callback will be the M2MB_NET_GET_EDRX_INFO_RESP. The resp_struct must be casted to
    the M2MB_NET_GET_EDRX_INFO_RESP_T type.

  @param[in] h
    It is the handle to the NET service, previously initialized by the m2mb_net_init function.
  @param[in] acc_tech
    access technology to get eDRX.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_net_get_edrx_info(h, acc_tech);
  @code
    void myCallback(M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(net_event)
      {
        //...

        case M2MB_NET_GET_EDRX_INFO_RESP:
        {
          M2MB_NET_GET_EDRX_INFO_RESP_T *resp = (M2MB_NET_GET_EDRX_INFO_RESP_T*)resp_struct;
          printf("GET NET edrx resp is err %d, acc_tech %d, edrx %d", resp->err, resp->acc_tech, resp->edrx);
        }

        //...
      }
    }

    int main()
    {
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_net_get_edrx_info(h, M2MB_NET_ACC_TECH_TYPE_LTE);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_net_get_edrx_info request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_net_get_edrx_info( M2MB_NET_HANDLE h, M2MB_NET_ACC_TECH_TYPE_E acc_tech );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_net_set_edrx_info sets information about edrx.

  @details
    m2mb_net_set_edrx_info sets information about edrx.
    The function is asynchronous. This means that the function returns immediately, but the signal information is received
    in the callback previously set by the m2mb_net_init function.
    The response event received in the callback will be the M2MB_NET_SET_EDRX_RESP. The resp_struct must be casted to
    the M2MB_NET_GENERIC_RESP_T type.

  @param[in] h
    It is the handle to the NET service, previously initialized by the m2mb_net_init function.
  @param[in] acc_tech
    access technology to set eDRX.
  @param[in] edrx
    the edrx to set.
  @param[in] enabled
    to enable the edrx.

  @return
    returns M2MB_RESULT_SUCCESS on success, a different value on error.

  @note

  @b
    m2mb_net_set_edrx_info(h);
  @code
    void myCallback(M2MB_NET_HANDLE h, M2MB_NET_IND_E net_event, UINT16 resp_size, void *resp_struct, void *myUserdata)
    {
      switch(net_event)
      {
        //...

        case M2MB_NET_SET_EDRX_INFO_RESP:
        {
          M2MB_NET_GENERIC_RESP_T *resp = (M2MB_NET_GENERIC_RESP_T*)resp_struct;
          printf("SET NET edrx resp is %d, %d", resp->err, resp->response);
        }

        //...
      }
    }

    int main()
    {
      UINT8 edrx = 0;
      UINT8 enable = 1;
      M2MB_RESULT_E retVal = M2MB_RESULT_SUCCESS;
      M2MB_NET_HANDLE h;
      struct myStruct myUserdata;
      retVal = m2mb_net_init(&h, myCallback, (void*)myUserdata);
      //...
      retVal = m2mb_net_set_edrx_info(h, M2MB_NET_ACC_TECH_TYPE_LTE, edrx, enable);
      if ( retVal == M2MB_RESULT_SUCCESS )
        printf( "m2mb_net_set_edrx_info request succeeded");
    }
  @endcode
*/
M2MB_RESULT_E m2mb_net_set_edrx_info( M2MB_NET_HANDLE h, M2MB_NET_ACC_TECH_TYPE_E acc_tech, UINT8 edrx, UINT8 enabled );

#endif
