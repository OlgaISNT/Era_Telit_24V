/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_bt_spp.h

  @brief
    Header file for m2mb_bt_spp APIs

  @details
    m2mb_bt_spp APIs provide actions and events for BT SPP.

  @note

  @author
    bhaskar.naik@telit.com

  @date
    12/01/2021
*/
/* Include files ================================================================================*/

#ifndef  M2MB_BT_SPP_H
#define  M2MB_BT_SPP_H

typedef void (*m2mb_bt_spp_event_callback) (M2MB_BT_HANDLE bt_handle, 
    M2MB_BT_USER_EVENTS_T bt_event, UINT16 resp_size, void *resp, void *userdata);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_bt_spp_init - initializes BT SPP module

  @details
    m2mb_bt_spp_init initialize BT SPP module. 
    m2mb_bt_initialize() API must be called before using m2mb_bt_spp_init() API.
    Calling the m2mb_bt_spp_init is mandatory before using the SPP service.

  @param[in] bt_handle
    first parameter is the handle to the BT interface.
    BT handle which was returned during m2mb_bt_initialize() must be used here.
    
  @return
    returns M2MB_BT_STATUS_SUCCESS_E on success, a different value on error.

  @note

  @b
    m2mb_bt_spp_init(M2MB_BT_HANDLE bt_handle,m2mb_bt_spp_event_callback bt_spp_cb, void *user_data);
  @code
    
    int main()
    {
        M2MB_BT_STATUS_T ret;
        M2MB_BT_HANDLE bt_handle;
        
	    //Initilizing BT
        ret = m2mb_bt_initialize(&bt_handle, bt_spp_user_callback, NULL);

        //Initialize SPP module
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_init(bt_handle, bt_spp_demo_app_server_callback, NULL);

        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            printf("SPP init success");
        else
            printf("SPP init failed");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T  m2mb_bt_spp_init(M2MB_BT_HANDLE bt_handle, 
    m2mb_bt_spp_event_callback bt_spp_cb, void *user_data);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_bt_spp_deinit - deinitialize the BT SPP module

  @details
    m2mb_bt_spp_deinit deinitialize the BT SPP module. It will release the resources.
    m2mb_bt_spp_init() API must have been called before using m2mb_bt_spp_deinit() API.

  @param[in] bt_handle
    first parameter is the handle to the BT interface.
    BT handle which was returned during m2mb_bt_initialize() must be used here.
    
  @return
    returns M2MB_BT_STATUS_SUCCESS_E on success, a different value on error.

  @note

  @b
    m2mb_bt_spp_deinit(M2MB_BT_HANDLE bt_handle);

  @code
    
    int main()
    {
        M2MB_BT_STATUS_T ret;
        M2MB_BT_HANDLE bt_handle;
        
	    //Initilizing BT
        ret = m2mb_bt_initialize(&bt_handle, bt_spp_user_callback, NULL);

        //Initialize SPP module
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_init(bt_handle, bt_spp_demo_app_server_callback, NULL);

        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            printf("SPP init success");
        else
            printf("SPP init failed");
       
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
        {
            ret = m2mb_bt_spp_deinit(bt_handle);
            
            if(ret == M2MB_BT_STATUS_SUCCESS_E)
                printf("SPP de-init success");
            else
                printf("SPP de-init failed");
        }
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T  m2mb_bt_spp_deinit(M2MB_BT_HANDLE bt_handle);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_bt_spp_init_state_get - To get the current state of SPP module

  @details
    m2mb_bt_spp_init_state_get will return the current state of SPP module. 
    Status will indicate whether SPP module is initialized or not.

  @param[in] bt_handle
    first parameter is the handle to the BT interface.
    BT handle which was returned during m2mb_bt_initialize() must be used here.
  @param[in] *state 
    second parameter is init state, which will be updated by the function
  @return
    returns M2MB_BT_STATUS_SUCCESS_E on success, a different value on error.

  @note

  @b
   m2mb_bt_spp_init_state_get(M2MB_BT_HANDLE bt_handle,M2MB_BT_INIT_STATE_T *state);
    
  @code
    
    int main()
    {
        M2MB_BT_STATUS_T ret;
        M2MB_BT_HANDLE bt_handle;
        M2MB_BT_INIT_STATE_T spp_init_state;
        
	    //Initilizing BT
        ret = m2mb_bt_initialize(&bt_handle, bt_spp_user_callback, NULL);

        //Initialize SPP module
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_init(bt_handle, bt_spp_demo_app_server_callback, NULL);

        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            printf("SPP init success");
        else
            printf("SPP init failed");

        //Get the SPP module init state
       m2mb_bt_spp_init_state_get(bt_handle,&spp_init_state);
       if(spp_init_state == M2MB_BT_STATE_INITIALIZED)
           printf("SPP module is in init state");
       else
           printf("SPP module not initialized");
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_BT_STATUS_T m2mb_bt_spp_init_state_get(M2MB_BT_HANDLE bt_handle, 
                                                   M2MB_BT_INIT_STATE_T *state);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_bt_spp_server_start - Start SPP server
    
  @details
    m2mb_bt_spp_server_start SPP server start request
    m2mb_bt_spp_init() API must be called before using m2mb_bt_spp_server_start() API.

  @param[in] bt_handle
    first parameter is the handle to the BT interface.
    BT handle which was returned during m2mb_bt_initialize() must be used here.
  @param[in] server_name
    Second parameter is the SPP server name.
    
  @return
    returns M2MB_BT_STATUS_SUCCESS_E on success, a different value on error.

  @note

  @b
    m2mb_bt_spp_server_start(M2MB_BT_HANDLE bt_handle, char *server_name);
  @code
    
    int main()
    {
        M2MB_BT_STATUS_T ret;
        M2MB_BT_HANDLE bt_handle;
        
	    //Initilizing BT
        ret = m2mb_bt_initialize(&bt_handle, bt_spp_user_callback, NULL);

        //Initialize SPP module
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_init(bt_handle, bt_spp_demo_app_server_callback, NULL);
        

        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_server_start(bt_handle, "MySppServer");
        
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            printf("SPP server start success");
        else
            printf("SPP server start failed");
            
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T  m2mb_bt_spp_server_start(M2MB_BT_HANDLE bt_handle,
    char *server_name);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_bt_spp_server_stop - Stop the SPP server
    
  @details
    m2mb_bt_spp_server_stop SPP server start request
    m2mb_bt_spp_server_start() API must be called before using m2mb_bt_spp_server_stop() API.

  @param[in] bt_handle
    first parameter is the handle to the BT interface.
    BT handle which was returned during m2mb_bt_initialize() must be used here.
  @return
    returns M2MB_BT_STATUS_SUCCESS_E on success, a different value on error.

  @note

  @b
    m2mb_bt_spp_server_stop(M2MB_BT_HANDLE bt_handle);
  @code
    
    int main()
    {
        M2MB_BT_STATUS_T ret;
        M2MB_BT_HANDLE bt_handle;
        
	    //Initilizing BT
        ret = m2mb_bt_initialize(&bt_handle, bt_spp_user_callback, NULL);

        //Initialize SPP module
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_init(bt_handle, bt_spp_demo_app_server_callback, NULL);
        
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_server_start(bt_handle, "MySppServer");

        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_server_stop(bt_handle);
            
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            printf("SPP server stop success");
        else
            printf("SPP server stop failed");
            
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T  m2mb_bt_spp_server_stop(M2MB_BT_HANDLE bt_handle);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_bt_spp_send_data - Send data to SPP client
    
  @details
    m2mb_bt_spp_send_data  Data send request.
    Its blocking call.
    m2mb_bt_spp_send_data() API must be called once client device is connected and SPP is in connected state.
    i.e. after recieving M2MB_BT_SPP_PEER_CONNECT_IND event from SPP module.    
  @param[in] bt_handle
    first parameter is the handle to the BT interface.
    BT handle which was returned during m2mb_bt_initialize() must be used here.
  @param[in] con_id
    Second  parameter is connection ID. 
    Connection ID shall be retrieved from M2MB_BT_SPP_PEER_CONNECT_IND
  @param[in] send_data
    Third parameter is pointer to send buffer.
  @param[in] data_length
    Fourth parameter is number of bytes to transfer.

  @return
    returns M2MB_BT_STATUS_SUCCESS_E on success, a different value on error.

  @note

  @b
    m2mb_bt_spp_send_data(M2MB_BT_HANDLE bt_handle ,INT32 con_id, char *send_data, int data_length);
  @code
    
    int main()
    {
        M2MB_BT_STATUS_T ret;
        M2MB_BT_HANDLE bt_handle;
        
	    //Initilizing BT
        ret = m2mb_bt_initialize(&bt_handle, bt_spp_user_callback, NULL);
        //Initialize SPP module
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_init(bt_handle, bt_spp_user_callback, NULL);
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_server_start(bt_handle, "MySppServer");

        //wait for callback event M2MB_BT_SPP_PEER_CONNECT_IND

        //Once we get connect indication, data can be sent
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_send_data(bt_handle,con_id,send_buf,sizeof(send_buf));
        
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            printf("SPP data send success");
        else
            printf("SPP data send failed");
            
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T  m2mb_bt_spp_send_data(M2MB_BT_HANDLE bt_handle , 
    INT32 con_id, char *send_data, int data_length);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_bt_spp_recv_data - To recieve data
    
  @details
    m2mb_bt_spp_recv_data  Data send request.
    Its non-blocking call. 
    Recieved data will be indicated via async callback event M2MB_BT_SPP_SERVER_RX_DATA_EVENT_E.
    m2mb_bt_spp_recv_data() API must be called once client device is connected and SPP is in connected state.
    i.e. after recieving M2MB_BT_SPP_PEER_CONNECT_IND event from SPP module.    
  @param[in] bt_handle
    first parameter is the handle to the BT interface.
    BT handle which was returned during m2mb_bt_initialize() must be used here.
  @param[in] con_id
    Second  parameter is connection ID. 
    Connection ID shall be retrieved from M2MB_BT_SPP_PEER_CONNECT_IND

  @return
    returns M2MB_BT_STATUS_SUCCESS_E on success, a different value on error.

  @note

  @b
    m2mb_bt_spp_recv_data(M2MB_BT_HANDLE bt_handle ,INT32 con_id);
    
  @code
    
    int main()
    {
        M2MB_BT_STATUS_T ret;
        M2MB_BT_HANDLE bt_handle;
        
	    //Initilizing BT
        ret = m2mb_bt_initialize(&bt_handle, bt_spp_user_callback, NULL);
        //Initialize SPP module
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_init(bt_handle, bt_spp_user_callback, NULL);
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_server_start(bt_handle, "MySppServer");

        //wait for callback event M2MB_BT_SPP_PEER_CONNECT_IND

        //Once we get connect indication, data can be sent
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_recv_data(bt_handle,con_id);
         
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            printf("SPP data reception initiated");
        else
            printf("SPP data reception failed");

         //user callback shall handle recieve data indication event
         //wait for callback event M2MB_BT_SPP_SERVER_RX_DATA_EVENT_E
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T  m2mb_bt_spp_recv_data (M2MB_BT_HANDLE bt_handle , 
    INT32 con_id);
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    m2mb_bt_spp_disconnect - To disconnect SPP connection
    
  @details
    m2mb_bt_spp_disconnect disconnect SPP connection
    m2mb_bt_spp_disconnect() API must be called in connected state.
    
  @param[in] bt_handle
    first parameter is the handle to the BT interface.
    BT handle which was returned during m2mb_bt_initialize() must be used here.
  @param[in] con_id
    Second  parameter is connection ID. 
    Connection ID shall be retrieved from M2MB_BT_SPP_PEER_CONNECT_IND

  @return
    returns M2MB_BT_STATUS_SUCCESS_E on success, a different value on error.

  @note

  @b
    m2mb_bt_spp_server_stop(M2MB_BT_HANDLE bt_handle);
  @code
    
    int main()
    {
        M2MB_BT_STATUS_T ret;
        M2MB_BT_HANDLE bt_handle;
        
	    //Initilizing BT
        ret = m2mb_bt_initialize(&bt_handle, bt_spp_user_callback, NULL);

        //Initialize SPP module
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_init(bt_handle, bt_spp_demo_app_server_callback, NULL);
        
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_server_start(bt_handle, "MySppServer");

        //wait for callback event M2MB_BT_SPP_PEER_CONNECT_IND
        
        //Once we get connect indication, server is in connected state.

        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            ret = m2mb_bt_spp_disconnect(bt_handle,con_id);
         
        if(ret == M2MB_BT_STATUS_SUCCESS_E)
            printf("SPP disconnected");
        else
            printf("SPP disconnect failed");
            
    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_BT_STATUS_T  m2mb_bt_spp_disconnect(M2MB_BT_HANDLE bt_handle, 
    INT32 con_id);

#endif /* M2MB_BT_SPP_H */
