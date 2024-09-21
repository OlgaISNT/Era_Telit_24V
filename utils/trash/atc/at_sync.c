/* Include files ================================================================================*/

#include <stdio.h>
#include <string.h>
#include "m2mb_os_api.h"
#include "m2mb_ati.h"
#include "../utils/hdr/log.h"
#include "at_utils.h"
#include "app_cfg.h"
/* Local defines ================================================================================*/
#define MAX_BUF_SIZE 4096 // максимальный размер промежуточного буфера приёма
	/*
	 * Длительность ожидания ответа команды, мС. Её нельзя делать небольшой, т.к. тогда некорректно работает
	 * встроенный в модем API сервис реализации обработки АТ-команд
	 * Default: 5 * 60 * 1000
	 */
#define WAIT 5 * 60 * 1000
/* Local typedefs ===============================================================================*/
typedef enum {
	NOT, INIT, PARAM
} Step;
/* Local statics ================================================================================*/
static M2MB_OS_MTX_ATTR_HANDLE mtx_attr_handle;
static M2MB_OS_MTX_HANDLE      mtx_handle = NULL;
static char err_init_msg[] = "Can't ats_init";
static char gl_buf[MAX_BUF_SIZE];
static volatile UINT32 rec_data = 0; // кол-во байт, принятых в промежуточный буфер
static INT16 inst;
static M2MB_ATI_HANDLE ati_handle;
static M2MB_OS_EV_HANDLE ev_handle   = NULL;
static M2MB_OS_SEM_HANDLE at_rsp_sem = NULL;
static volatile int ati_state = M2MB_STATE_IDLE_EVT;
static struct {
	char *rsp;
	UINT32 rsp_max_len;
	int cmd_id;
	char *mask;
	int par_len;
	Step step; // шаг этапа отправки многокомпонентой АТ-команды
} cbd;
static char *params;
/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
static void get_mutex(void);
static void put_mutex(void);
static void reinit(void);
static void at_cmd_sync_callback( M2MB_ATI_HANDLE h, M2MB_ATI_EVENTS_E ati_event, UINT16 resp_size, void *resp_struct, void *userdata ) {
  (void)h;
  (void)userdata;

  INT32 resp_len;
  INT16 resp_len_short;
  get_mutex();
  LOG_TRACE("ats callback! Event: %d; resp_size: %u", ati_event, resp_size);
  switch(ati_event) {
  	  case M2MB_RX_DATA_EVT:
  		  LOG_TRACE("ATI_RX_DATA_EVT");
  		  if (ati_state == M2MB_STATE_IDLE_EVT && resp_size != 2) {
  			  LOG_TRACE("This is an UNSOLICITED");
  			  //read and discard data
  			  static char tmp_buf[1000];
  			  m2mb_ati_rcv_resp(ati_handle, tmp_buf, sizeof(tmp_buf));
  			  LOG_TRACE("UNSOLICITED: %s", tmp_buf);
  		  } else { /*Normal data reception, read and append into global buffer */
  			  if (resp_size == 2) {
  				  resp_len_short = *(INT16*)resp_struct;
  				  resp_len = resp_len_short;
  			  } else {
  				  resp_len = *(INT32*)resp_struct;
  			  }
  			  LOG_TRACE("Callback - available bytes: %d", resp_len);
  			  char *end;
  			  end = gl_buf + rec_data;
  			  rec_data += resp_len;
  			  m2mb_ati_rcv_resp(ati_handle, end, resp_len);
  			  LOG_TRACE("Part of response: %s\r\n", gl_buf);
  			  print_hex("HEX", gl_buf, rec_data);
  			  switch (cbd.step) {
  			  	  case INIT:
  		  			  //LOG_DEBUG("   ----cbd.mask:%s", cbd.mask);
  		  			  //print_hex("HEX", cbd.mask, strlen(cbd.mask));
  			  		  if ((cbd.mask != NULL) && (params != NULL) && (strstr(gl_buf, cbd.mask) != NULL) ) {
  			  			  LOG_DEBUG("send params");
  			  			  cbd.step = PARAM;
  			  			  print_hex("f", params, cbd.par_len + 1);
  			  			  m2mb_ati_send_cmd(ati_handle, params, cbd.par_len + 1);
  			  			  //char ctrl_z[1] = {CTRL_Z};
  			  			  //m2mb_ati_send_cmd(ati_handle, ctrl_z, 1);
  			  		  }
  			  		  break;
  			  	  case PARAM: break;
  			  	  default:
  			  	  case NOT: break;
  			  }
  		  }
  		  break;
  	  case M2MB_STATE_IDLE_EVT:
  		  LOG_TRACE("ATI_STATE_IDLE_EVT");
  		  memset(cbd.rsp, 0x00, cbd.rsp_max_len);
		  memcpy(cbd.rsp, gl_buf, cbd.rsp_max_len - 1);
  		  LOG_TRACE("Receive response: <%s>\r\n", cbd.rsp);
  		  rec_data = 0;
  		  m2mb_os_ev_set(ev_handle, AT_SYNC_BIT, M2MB_OS_EV_SET);
  		  break;
  	  case M2MB_STATE_RUNNING_EVT:
  		  LOG_TRACE("ATI_STATE_RUNNING_EVT");
  		  break;
  	  case M2MB_STATE_CMD_MODE_EVT: /**< AT parser entered in COMMAND mode */
  		  LOG_TRACE("ATI_STATE_CMD_MODE_EVT");
  		  break;
  	  case M2MB_STATE_ONLINE_MODE_EVT: /**< AT parser entered in ONLINE mode */
  		  LOG_TRACE("ATI_STATE_ONLINE_MODE_EVT");
  		  break;
  }
  ati_state = ati_event;
  put_mutex();
}

static void get_mutex(void) {
	if (mtx_handle == NULL) {
		LOG_ERROR("mtxN");
		return;
	}
	M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtx_handle, M2MB_OS_MS2TICKS(5000));
	if (res != M2MB_OS_SUCCESS) LOG_ERROR("Can't get mtx:%i", res);
}

static void put_mutex(void) {
	if (mtx_handle == NULL) {
		LOG_ERROR("mtxN");
		return;
	}
	M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtx_handle);
	if (res != M2MB_OS_SUCCESS) LOG_ERROR("Can't put mtx:%i", res);
}

/* Global functions =============================================================================*/
M2MB_RESULT_E at_cmd_sync_init(void) {
	inst = 1;

    M2MB_OS_RESULT_E        osRes;
    M2MB_OS_EV_ATTR_HANDLE  evAttrHandle;

    osRes  = m2mb_os_ev_setAttrItem( &evAttrHandle,
                                     CMDS_ARGS
                                     (
                                       M2MB_OS_EV_SEL_CMD_CREATE_ATTR, NULL,
                                       M2MB_OS_EV_SEL_CMD_NAME, "atsync_ev"
                                     )
                                   );

    osRes = m2mb_os_ev_init( &ev_handle, &evAttrHandle );
    if ( osRes != M2MB_OS_SUCCESS ) {
    	m2mb_os_ev_setAttrItem( &evAttrHandle, M2MB_OS_EV_SEL_CMD_DEL_ATTR, NULL );
    	return M2MB_RESULT_FAIL;
    }

    M2MB_OS_SEM_ATTR_HANDLE semAttrHandle;
    if (NULL == at_rsp_sem) {
    				osRes = m2mb_os_sem_setAttrItem( &semAttrHandle,
      				CMDS_ARGS(
      						M2MB_OS_SEM_SEL_CMD_CREATE_ATTR, NULL,
  							M2MB_OS_SEM_SEL_CMD_COUNT, 1 /*CS*/,
  							M2MB_OS_SEM_SEL_CMD_TYPE, M2MB_OS_SEM_BINARY,
  							M2MB_OS_SEM_SEL_CMD_NAME, "ATSRSem"));
  	  if (osRes != M2MB_OS_SUCCESS) {
  		  LOG_CRITICAL(err_init_msg);
  		  return M2MB_RESULT_FAIL;
  	  }
  	  osRes = m2mb_os_sem_init( &at_rsp_sem, &semAttrHandle );
  	  if (osRes != M2MB_OS_SUCCESS) {
  		  LOG_CRITICAL(err_init_msg);
  		  return M2MB_RESULT_FAIL;
  	  }
    }

	osRes = m2mb_os_mtx_setAttrItem( &mtx_attr_handle,
	                                      CMDS_ARGS(
	                                        M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
	                                        M2MB_OS_MTX_SEL_CMD_NAME, "ats1",
	                                        M2MB_OS_MTX_SEL_CMD_USRNAME, "ats",
	                                        M2MB_OS_MTX_SEL_CMD_INHERIT, 1
	                                      )
	                                );
	if (osRes != M2MB_OS_SUCCESS) {
		LOG_ERROR("ATMsF");
		return M2MB_RESULT_FAIL;
	}
	osRes = m2mb_os_mtx_init( &mtx_handle, &mtx_attr_handle);
	if (osRes != M2MB_OS_SUCCESS) {
		LOG_ERROR("ATMiF");
		return M2MB_RESULT_FAIL;
	}

	LOG_TRACE("ati_sync_init on instance %d", inst);
	M2MB_RESULT_E res = m2mb_ati_init(&ati_handle, inst, at_cmd_sync_callback, at_rsp_sem);
	if (res != M2MB_RESULT_SUCCESS) {
		LOG_ERROR("Can't init at sync");
		return res;
	}

	return M2MB_RESULT_SUCCESS;
}

M2MB_RESULT_E at_cmd_sync_deinit(void) {
	LOG_TRACE("ati_sync_deinit on instance %d", inst);
	if (at_rsp_sem != NULL) {
		m2mb_os_sem_deinit( at_rsp_sem);
		at_rsp_sem = NULL;
	}
	if (m2mb_os_ev_deinit(ev_handle) != M2MB_OS_SUCCESS) LOG_ERROR("ERR_DEIN_EV");
	if (mtx_handle != NULL) {
		M2MB_OS_RESULT_E os_res = m2mb_os_mtx_deinit(mtx_handle);
		if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("ERR_DEIN_MTX:%i", os_res);
		mtx_handle = NULL;
	}
	if (m2mb_ati_deinit(ati_handle) == M2MB_RESULT_SUCCESS ) return M2MB_RESULT_SUCCESS;
	else return M2MB_RESULT_FAIL;
}

M2MB_RESULT_E send_sync_at_command(const CHAR *atCmd, CHAR *atRsp, UINT32 atRspMaxLen) {
	INT32 cmd_len = 0;
	M2MB_RESULT_E retVal;

	if (M2MB_OS_SUCCESS != m2mb_os_sem_get(at_rsp_sem, M2MB_OS_MS2TICKS(WAIT))) {  //get critical section
		LOG_ERROR("AT sync timeout: %s", atCmd);
		return M2MB_RESULT_FAIL;
	}
/*
	if (timeout_ms < WAIT || timeout_ms > 60 * 1000) {
		LOG_ERROR("ATSIA:%i, will set:%i", timeout_ms, WAIT);
		timeout_ms = 10 * 1000;
	}
*/
	LOG_DEBUG("Sending AT syncro command: %.*s", strlen(atCmd)-1, atCmd);
	m2mb_os_taskSleep( M2MB_OS_MS2TICKS(20));
	cbd.rsp = atRsp;
	cbd.rsp_max_len = atRspMaxLen;
	cbd.mask = NULL;
	params = NULL;
	cbd.par_len = 0;
	cbd.step = NOT;
	memset(gl_buf, 0x00, atRspMaxLen + 1);
	rec_data = 0;
	cmd_len = strlen(atCmd);
	retVal = m2mb_ati_send_cmd(ati_handle, (void*) atCmd, cmd_len);
	if ( retVal != M2MB_RESULT_SUCCESS ) {
		LOG_ERROR("ati_sync_sending fail");
		//LOG_TRACE("UNLOCKING AT semaphore");
		m2mb_os_sem_put(at_rsp_sem);
		return retVal;
	}

	UINT32 curEvBits;
	M2MB_OS_RESULT_E os_res = m2mb_os_ev_get(ev_handle, AT_SYNC_BIT, M2MB_OS_EV_GET_ANY_AND_CLEAR, &curEvBits, M2MB_OS_MS2TICKS(WAIT));
	LOG_TRACE("--- ev_res: %i", os_res);
	int result;
	if (os_res != M2MB_OS_SUCCESS) {
		LOG_WARN("No AT-response on: %s", atCmd);
		reinit();
		result = M2MB_RESULT_FAIL;
	} else {
		m2mb_os_taskSleep( M2MB_OS_MS2TICKS(20));
		result = M2MB_RESULT_SUCCESS;
	}

	LOG_TRACE("UNLOCKING AT semaphore");
	m2mb_os_sem_put(at_rsp_sem);
	return result;
}

M2MB_RESULT_E send_sync_at_command_params(const CHAR *atCmd, CHAR *atRsp, UINT32 atRspMaxLen, CHAR *mask, char *p, int par_len) {
	INT32 cmd_len = 0;
	M2MB_RESULT_E retVal;

	if (M2MB_OS_SUCCESS != m2mb_os_sem_get(at_rsp_sem, M2MB_OS_MS2TICKS(WAIT))) {  //get critical section
		LOG_ERROR("AT sync timeout: %s", atCmd);
		return M2MB_RESULT_FAIL;
	}

	if (par_len < 1) return M2MB_RESULT_INVALID_ARG;
	m2mb_os_taskSleep( M2MB_OS_MS2TICKS(20));
	LOG_DEBUG("Sending AT syncro command: %.*s with mask: %s", strlen(atCmd) - 1, atCmd, mask);
	cbd.rsp = atRsp;
	cbd.rsp_max_len = atRspMaxLen;
	cbd.mask = mask;
	cbd.par_len = par_len;
	cbd.step = INIT;
	params = (char*) m2mb_os_malloc(cbd.par_len + 1);
	memcpy(params, p, cbd.par_len);
	params[cbd.par_len] = CTRL_Z;
	memset(gl_buf, 0x00, atRspMaxLen + 1);
	rec_data = 0;
	cmd_len = strlen(atCmd);
	retVal = m2mb_ati_send_cmd(ati_handle, (void*) atCmd, cmd_len);
	if ( retVal != M2MB_RESULT_SUCCESS ) {
		LOG_ERROR("ati_sync_sending fail");
		//LOG_TRACE("UNLOCKING AT semaphore");
		m2mb_os_sem_put(at_rsp_sem);
		return retVal;
	}

	UINT32 curEvBits;
	M2MB_OS_RESULT_E os_res = m2mb_os_ev_get(ev_handle, AT_SYNC_BIT, M2MB_OS_EV_GET_ANY_AND_CLEAR, &curEvBits, M2MB_OS_MS2TICKS(WAIT));
	LOG_TRACE("--- ev_res: %i", os_res);
	m2mb_os_free(params);
	params = NULL;
	int result;
	if (os_res != M2MB_OS_SUCCESS) {
		LOG_WARN("No AT-response on: %s", atCmd);
		reinit();
		result = M2MB_RESULT_FAIL;
	} else {
		m2mb_os_taskSleep( M2MB_OS_MS2TICKS(20));
		result = M2MB_RESULT_SUCCESS;
	}

	LOG_TRACE("UNLOCKING AT semaphore");
	m2mb_os_sem_put(at_rsp_sem);
	return result;
}

static void reinit(void) {
	LOG_WARN("Re-init AT-instance %i", inst);
	rec_data = 0;
	UINT8 cancel[1] = {0x1B};
	m2mb_ati_send_cmd(ati_handle, cancel, 1);
	m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1000));
	if (at_cmd_sync_deinit() != M2MB_RESULT_SUCCESS) LOG_ERROR("Can't deinit at_sync");
	if (at_cmd_sync_init() != M2MB_RESULT_SUCCESS) LOG_ERROR("Can't re-init at_sync");
}
