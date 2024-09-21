/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_bt.h

  @brief
    Header file for m2mb_bt APIs

  @details
    m2mb_bt APIs provide actions and events for BT HFP-AG management.

  @note

  @author
    

  @date
    27/08/2020
*/
/* Include files ================================================================================*/

#ifndef  M2MB_BT_HFP_AG_H
#define  M2MB_BT_HFP_AG_H

typedef void (*m2mb_bt_hfp_ag_event_callback) (M2MB_BT_HANDLE bt_handle, M2MB_BT_USER_EVENTS_T bt_event, UINT16 resp_size, void *resp, void *userdata);

M2MB_BT_STATUS_T m2mb_bt_hfp_ag_init(M2MB_BT_HANDLE bt_handle, m2mb_bt_hfp_ag_event_callback bt_hfp_ag_cb, void *user_data);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_deinit(M2MB_BT_HANDLE bt_handle);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_init_state_get(M2MB_BT_HANDLE bt_handle, 
                                                   M2MB_BT_INIT_STATE_T *state);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_connect(M2MB_BT_HANDLE bt_handle, M2MB_BT_BD_ADDR_T *remote_bd_addr,INT32 *connection_id);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_disconnect(M2MB_BT_HANDLE bt_handle, INT32 conn_id);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_audio_connect(M2MB_BT_HANDLE bt_handle, INT32 conn_id,M2MB_BT_HFP_AG_CODEC_TYPE_T *codec_type);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_audio_disconnect(M2MB_BT_HANDLE bt_handle, INT32 conn_id);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_volume_control (M2MB_BT_HANDLE bt_handle , INT32 conn_id, 
                                            M2MB_BT_HFP_VOL_CTRL_TYPE_T  vol_type, int volume);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_dial_out_call(M2MB_BT_HANDLE bt_handle, INT32 conn_id,
                                            char *number);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_answer_call(M2MB_BT_HANDLE bt_handle, INT32 conn_id);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_discon_call (M2MB_BT_HANDLE bt_handle, INT32 conn_id);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_reject_call (M2MB_BT_HANDLE bt_handle, INT32 conn_id);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_call_ind_enable(M2MB_BT_HANDLE bt_handle, bool enable_flag);
M2MB_BT_STATUS_T m2mb_bt_hfp_ag_dev_event_enable(M2MB_BT_HANDLE bt_handle, bool enable_flag);
#endif /* M2MB_BT_HFP_AG_H */
