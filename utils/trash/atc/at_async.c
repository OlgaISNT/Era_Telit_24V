/*Copyright (C) 2020 Telit Communications S.p.A. Italy - All Rights Reserved.*/
/*    See LICENSE file in the project root for full license information.     */

/**
  @file
    m2mb_async.c

  @brief
    The file contains the asynchronous implementation of AT example

  @details

  @description
    Asynchronous implementation of AT instance communication (using a callback function)
  @version 
    1.0.1 DKozenkov
  @note
    

  @author
	Fabio Pintus, DKozenkov
  @date
    18/07/2019
*/
/* Include files ================================================================================*/

#include <stdio.h>
#include <string.h>
#include "m2mb_os_api.h"
#include "m2mb_ati.h"
#include "app_cfg.h"
#include "../utils/hdr/log.h"


/* Local defines ================================================================================*/
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX_BUF_SIZE 4096 // максимальный размер приёмного буфера ответа

/* Local typedefs ===============================================================================*/

/* Local statics ================================================================================*/
static M2MB_OS_MTX_ATTR_HANDLE mtxAttrHandle;
static M2MB_OS_MTX_HANDLE      mtxHandle;
static char err_init_msg[] = "Can't at_init";
static char gl_buf[MAX_BUF_SIZE];
static UINT32 rec_data = 0; // кол-во байт, принятых в промежуточный буфер
static M2MB_ATI_HANDLE ati_handle;

static M2MB_OS_SEM_HANDLE at_rsp_sem = NULL;

static int ati_state = M2MB_STATE_IDLE_EVT;

static struct {
	char *rsp;
	UINT32 rsp_max_len;
	void (*rsp_func) (int, char *);
	int cmd_id;
} cbd;

static INT16 inst;

/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
static void get_mutex(void);
static void put_mutex(void);

static void at_cmd_async_callback( M2MB_ATI_HANDLE h, M2MB_ATI_EVENTS_E ati_event, UINT16 resp_size, void *resp_struct, void *userdata ) {
  (void)h;
  (void)userdata;
  
  INT32 resp_len;
  INT16 resp_len_short;
  LOG_TRACE("ati callback! Event: %d; resp_size: %u", ati_event, resp_size);
  get_mutex();
  switch(ati_event) {
  	  case M2MB_RX_DATA_EVT:
  		  LOG_DEBUG("ATI_RX_DATA_EVT");
  		  if(ati_state == M2MB_STATE_IDLE_EVT) {
  			  LOG_TRACE("This is an UNSOLICITED");
  			  //read and discard data
  			  //m2mb_ati_rcv_resp(h, tmp_buf, 100);
  			  //LOG_TRACE("UNSOLICITED: %s", tmp_buf);
  		  } else { /*Normal data reception, read and append into global buffer*/
  			  if (resp_size == 2) {
  				  resp_len_short = *(INT16*)resp_struct;
  				  resp_len = resp_len_short;
  			  } else {
  				  resp_len = *(INT32*)resp_struct;
  			  }
  			  LOG_DEBUG("Callback - available bytes: %d", resp_len);
  		  }
  		  break;
  	  case M2MB_STATE_IDLE_EVT:
  		  LOG_DEBUG("ATI_STATE_IDLE_EVT");
  		  m2mb_os_taskSleep( M2MB_OS_MS2TICKS(20)); // DKozenkov

  		  memset(cbd.rsp, 0x00, cbd.rsp_max_len);
		  m2mb_ati_rcv_resp(ati_handle, cbd.rsp, cbd.rsp_max_len - 1);
		  LOG_INFO("Receive response: <%s>\r\n", cbd.rsp);
		  cbd.rsp_func(cbd.cmd_id, cbd.rsp);
  		  LOG_TRACE("UNLOCKING AT semaphore");
		  m2mb_os_sem_put(at_rsp_sem);  //Release CS
  		  break;
  	  case M2MB_STATE_RUNNING_EVT:
  		  LOG_DEBUG("ATI_STATE_RUNNING_EVT");
  		  break;
  	  case M2MB_STATE_CMD_MODE_EVT: /**< AT parser entered in COMMAND mode */
  		  LOG_DEBUG("ATI_STATE_CMD_MODE_EVT");
  		  break;
  	  case M2MB_STATE_ONLINE_MODE_EVT: /**< AT parser entered in ONLINE mode */
  		  LOG_DEBUG("ATI_STATE_ONLINE_MODE_EVT");
  		  break;
  }
  ati_state = ati_event;
  put_mutex();
}

static void get_mutex(void) {
	M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtxHandle, M2MB_OS_MS2TICKS(5000));
	if (res != M2MB_OS_SUCCESS) LOG_CRITICAL("Can't get mtx:%i", res);
}

static void put_mutex(void) {
	M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtxHandle);
	if (res != M2MB_OS_SUCCESS) LOG_CRITICAL("Can't put mtx:%i", res);
}

/* Global functions =============================================================================*/
//atInstance MUST be not reserved to a physical port: use atInstance=0 with PORTCFG=8 or atInstance=1 with PORTCFG=13
M2MB_RESULT_E at_cmd_async_init(INT16 instance) {
	M2MB_OS_SEM_ATTR_HANDLE semAttrHandle;
	inst = instance;
	M2MB_OS_RESULT_E os_res;
	if (NULL == at_rsp_sem) {
		os_res = m2mb_os_sem_setAttrItem( &semAttrHandle,
    				CMDS_ARGS(
    						M2MB_OS_SEM_SEL_CMD_CREATE_ATTR, NULL,
							M2MB_OS_SEM_SEL_CMD_COUNT, 1 /*CS*/,
							M2MB_OS_SEM_SEL_CMD_TYPE, M2MB_OS_SEM_BINARY,
							M2MB_OS_SEM_SEL_CMD_NAME, "ATRSPSem"));
		if (os_res != M2MB_OS_SUCCESS) {
			LOG_CRITICAL(err_init_msg);
			return M2MB_RESULT_FAIL;
		}
		os_res = m2mb_os_sem_init( &at_rsp_sem, &semAttrHandle );
		if (os_res != M2MB_OS_SUCCESS) {
			LOG_CRITICAL(err_init_msg);
			return M2MB_RESULT_FAIL;
		}
	}

	os_res = m2mb_os_mtx_setAttrItem( &mtxAttrHandle,
	                                      CMDS_ARGS(
	                                        M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
	                                        M2MB_OS_MTX_SEL_CMD_NAME, "ats1",
	                                        M2MB_OS_MTX_SEL_CMD_USRNAME, "ats",
	                                        M2MB_OS_MTX_SEL_CMD_INHERIT, 1
	                                      )
	                                );
	if (os_res != M2MB_OS_SUCCESS) {
		LOG_CRITICAL("ATAMsF");
		return M2MB_RESULT_FAIL;
	}
	os_res = m2mb_os_mtx_init( &mtxHandle, &mtxAttrHandle);
	if (os_res != M2MB_OS_SUCCESS) {
		LOG_CRITICAL("ATAMiF");
		return M2MB_RESULT_FAIL;
	}

	LOG_DEBUG("m2mb_ati_init() on instance %d", instance);
	M2MB_RESULT_E ret = m2mb_ati_init(&ati_handle, instance, at_cmd_async_callback, at_rsp_sem);
	if (ret == M2MB_RESULT_SUCCESS) return M2MB_RESULT_SUCCESS;
	else {
		LOG_ERROR("ati_async_init() returned failure value: %i", ret);
		return M2MB_RESULT_FAIL;
	}
}

M2MB_RESULT_E at_cmd_async_deinit(void) {
	if (m2mb_os_mtx_deinit(mtxHandle) != M2MB_OS_SUCCESS) LOG_CRITICAL("ERR_DEIN_MTX");
	if (NULL != at_rsp_sem) {
		m2mb_os_sem_deinit( at_rsp_sem);
		at_rsp_sem = NULL;
	}
	LOG_DEBUG("ati_async_deinit() on instance %d", inst);
	return m2mb_ati_deinit(ati_handle);
}


M2MB_RESULT_E send_async_at_command(const CHAR *atCmd, CHAR *atRsp, UINT32 atRspMaxLen,
									int cmdId, void (*ati_response) (int, char *)) {

	INT32 cmd_len = 0;
	M2MB_RESULT_E retVal;
	LOG_DEBUG("Sending AT Command: %.*s",strlen(atCmd) -1, atCmd);

	if (atRspMaxLen >= MAX_BUF_SIZE) {
		LOG_ERROR("ATRBO");
		return M2MB_RESULT_INVALID_ARG;
	}

	if (M2MB_OS_SUCCESS != m2mb_os_sem_get(at_rsp_sem, M2MB_OS_MS2TICKS(120000))) {  //get critical section  M2MB_OS_WAIT_FOREVER
		LOG_ERROR("AT async timeout: %s", atCmd);
		return M2MB_RESULT_FAIL;
	}
	cbd.rsp = atRsp;
	cbd.rsp_max_len = atRspMaxLen;
	cbd.rsp_func = ati_response;
	cbd.cmd_id = cmdId;
	memset(gl_buf, 0x00, atRspMaxLen + 1);
	rec_data = 0;
	cmd_len = strlen(atCmd);

	retVal = m2mb_ati_send_cmd(ati_handle, (void*) atCmd, cmd_len);
	if ( retVal != M2MB_RESULT_SUCCESS ) {
		LOG_ERROR("ati_send fail");
		m2mb_os_sem_put(at_rsp_sem);  /*Release CS*/
		return retVal;
	}
	return M2MB_RESULT_SUCCESS;
}

