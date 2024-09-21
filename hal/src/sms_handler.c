/**
 * @file sms_handler.c
 * @version 1.0.0
 * @date 05.05.2022
 * @author: DKozenkov from Telit
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_types.h"
#include "m2mb_os_api.h"
#include "m2mb_fs_posix.h"
#include "m2mb_sms.h"
#include "sms_callbacks.h"
#include "sms.h"
#include "tasks.h"
#include "auxiliary.h"
#include "era_helper.h"
#include "azx_utils.h"
#include "atc_era.h"
#include "log.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static UINT32  curEvBits;
static M2MB_SMS_HANDLE h_sms_handle;
/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
static M2MB_OS_RESULT_E sending(INT32 pdulen, UINT8 *pdu, INT32 timeout);
/* Global functions =============================================================================*/
M2MB_OS_EV_HANDLE sms_evHandle = NULL;
UINT8 *pdu_provv, *pdu;
INT32 pdulen;
extern M2MB_SMS_STORAGE_E memory;

BOOLEAN init_sms_handler(void) {
	  //char PhoneNumber[32];
	  M2MB_OS_EV_ATTR_HANDLE  evAttrHandle;
	  //LOG_DEBUG("Init SMS PDU handler");
	  M2MB_OS_RESULT_E  osRes;

	  osRes  = m2mb_os_ev_setAttrItem( &evAttrHandle, CMDS_ARGS(M2MB_OS_EV_SEL_CMD_CREATE_ATTR, NULL, M2MB_OS_EV_SEL_CMD_NAME, "sms_ev"));
	  osRes = m2mb_os_ev_init( &sms_evHandle, &evAttrHandle );

	  if ( osRes != M2MB_OS_SUCCESS ) {
	    m2mb_os_ev_setAttrItem( &evAttrHandle, M2MB_OS_EV_SEL_CMD_DEL_ATTR, NULL );
	    LOG_CRITICAL("m2mb_os_ev_init failed!");
	    return FALSE;
	  } else {
	    //LOG_DEBUG("m2mb_os_ev_init success");
	  }

	  M2MB_RESULT_E retVal = m2mb_sms_init(&h_sms_handle, Sms_Callback, NULL);
	  if ( retVal == M2MB_RESULT_SUCCESS ) {
	      //LOG_DEBUG( "m2mb_sms_init() succeeded");
	  } else {
	      LOG_ERROR("m2mb_sms_init()failed");
	      return FALSE;
	  }

	  retVal = m2mb_sms_enable_ind(h_sms_handle, M2MB_SMS_INCOMING_IND, 1);
	  if ( retVal != M2MB_RESULT_SUCCESS ) //LOG_DEBUG("M2MB_SMS_INCOMING_IND indication enabled");
	  LOG_ERROR("M2MB_SMS_INCOMING_IND indication failed");

	  retVal = m2mb_sms_enable_ind(h_sms_handle, M2MB_SMS_MEMORY_FULL_IND, 1);
	  if ( retVal != M2MB_RESULT_SUCCESS ) //LOG_DEBUG("M2MB_SMS_INCOMING_IND MEMORY FULL indication enabled");
	  LOG_ERROR("M2MB_SMS_INCOMING_IND MEMORY FULL indication failed");

	  memory = M2MB_SMS_STORAGE_SM;

	  retVal = m2mb_sms_set_storage(h_sms_handle, memory);
	  if ( retVal != M2MB_RESULT_SUCCESS ) LOG_ERROR( "Set storage to %d failed!", memory);
	  //else LOG_DEBUG("Storage set to %d\r\n", memory);

	  /*
	    M2MB_SMS_DISCARD         -> incoming SMS will be discarded
	    M2MB_SMS_STORE_AND_ACK   -> incoming SMS will be stored and ack managed by Modem -> transactionID = -1
	    M2MB_SMS_FORWARD_AND_ACK -> incoming SMS will be forwarded to app and ack managed by Modem -> transactionID = -1
	    M2MB_SMS_FORWARD_ONLY    -> incoming SMS will be forwarded to app and ack NOT managed by Modem
	                               -> transactionID >= 0 to demand ack management to application logic.
	  */

	    // m2mb_sms_set_route: set M2MB_SMS_FORWARD_AND_ACK for all SMS classes
	    retVal = m2mb_sms_set_route(h_sms_handle, M2MB_SMS_CLASS_0, memory, M2MB_SMS_STORE_AND_ACK);
	    if ( retVal != M2MB_RESULT_SUCCESS ) LOG_ERROR( "Set route for M2MB_SMS_CLASS_0 setting failed!");

	    retVal = m2mb_sms_set_route(h_sms_handle, M2MB_SMS_CLASS_1, memory, M2MB_SMS_STORE_AND_ACK);
	    if ( retVal != M2MB_RESULT_SUCCESS ) LOG_ERROR( "Set route for M2MB_SMS_CLASS_1 setting failed!");

	    retVal = m2mb_sms_set_route(h_sms_handle, M2MB_SMS_CLASS_2, memory, M2MB_SMS_STORE_AND_ACK);
	    if ( retVal != M2MB_RESULT_SUCCESS ) LOG_ERROR( "Set route for M2MB_SMS_CLASS_2 setting failed!");

	    retVal = m2mb_sms_set_route(h_sms_handle, M2MB_SMS_CLASS_3, memory, M2MB_SMS_STORE_AND_ACK);
	    if ( retVal != M2MB_RESULT_SUCCESS ) LOG_ERROR( "Set route for M2MB_SMS_CLASS_3 setting failed!");

	    retVal = m2mb_sms_set_route(h_sms_handle, M2MB_SMS_CLASS_NONE, memory, M2MB_SMS_STORE_AND_ACK);
	    if ( retVal != M2MB_RESULT_SUCCESS ) LOG_ERROR( "Set route for M2MB_SMS_CLASS_NONE setting failed!");

	  return TRUE;
}

BOOLEAN send_sms_pdu(INT32 pdulen, UINT8 *pdu, BOOLEAN send_respond) {
    //print_hex("Sending message", (char *) pdu, pdulen);

//    M2MB_RESULT_E retVal = m2mb_sms_send(h_sms_handle, pdulen, pdu);
//    if (retVal == M2MB_RESULT_SUCCESS) to_log_info_uart(" m2mb_sms_send() - succeeded, but sms not sent yet!");
//    else {
//    	LOG_ERROR("m2mb_sms_send() - failure");
//    	return FALSE;
//    }
//
//    /*Wait for sms send event to occur (released in Sms_Callback function) */
//    M2MB_OS_RESULT_E osRes = m2mb_os_ev_get(sms_evHandle, EV_SMS_SEND, M2MB_OS_EV_GET_ANY_AND_CLEAR, &curEvBits, M2MB_OS_MS2TICKS( 40000 ));
	M2MB_OS_RESULT_E osRes = sending(pdulen, pdu, (send_respond ? 40000 : 10000));
    INT32 result;
    if (osRes == M2MB_OS_SUCCESS) {
    	to_log_info_uart( "SMS correctly sent!" );
    	result = 0;
    } else if (osRes == M2MB_OS_NO_EVENTS) {
    	to_log_error_uart("SMS not sent! - exit for timeout" );
    	result = 1;
    } else {
    	LOG_WARN("SMS not sent! - unexpected value %d returned", osRes);
    	result = 2;
    }
    if (send_respond) {
    	send_to_sms_task(SMS_SENDING_RESULT, result, 0); // повторная отправка в случае неуспеха будет в sms_manager.c
    } else if (osRes != M2MB_OS_SUCCESS) {
    	osRes = sending(pdulen, pdu, 40000);
    }
    send_to_ecall_task(ECALL_SMS_SENT, result, 0);
    return osRes == M2MB_OS_SUCCESS;
}

static M2MB_OS_RESULT_E sending(INT32 pdulen, UINT8 *pdu, INT32 timeout) {
    LOG_INFO("Trying sending SMS...");
    if (!is_registered_AT() && !is_ecall_in_progress()) { // эта проверка нужна для избежания ошибки прошивки модема, при которой он считает успешной отправку СМС при отсутствии регистрации в сети
        //pause_gnss_request(TRUE); // останов запросов к ГНСС, чтобы не возникало сбоя АТ-команд
        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(3*1000)); // ожидание завершения останова запросов к ГНСС
        if (!is_ecall_in_progress()) {
        	char resp[300] = {0};
        	int r = at_era_sendCommand(15*1000, resp, sizeof(resp), "AT#MONI\r");
        	LOG_INFO("#MONI: %s", resp);
        	if (r < 30) return M2MB_OS_FEATURE_NOT_SUPPORTED;
        	at_era_sendCommandExpectOk(2*1000, "AT+CECALL=0\r");  // инициация экстр. вызова для "ручной" регистрации в сети
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(2*1000)); // небольшая пауза для отработки команды вызова модемом
            at_era_sendCommandExpectOk(2*1000, "ATH\r"); // отмена вызова
            m2mb_os_taskSleep(M2MB_OS_MS2TICKS(5*1000));// ожидание завершения внутренних процессов в модеме до запуска ГНСС
            //if (!is_ecall_in_progress()) pause_gnss_request(FALSE); // возобновление отправки запросов к ГНСС
        }
        return M2MB_OS_FEATURE_NOT_SUPPORTED;
    } else {
        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(2*1000));
        if (!is_registered_AT()) return M2MB_OS_FEATURE_NOT_SUPPORTED;
    }
    M2MB_RESULT_E retVal = m2mb_sms_send(h_sms_handle, pdulen, pdu);
    if (retVal == M2MB_RESULT_SUCCESS) to_log_info_uart(" m2mb_sms_send() - succeeded, but sms not sent yet!");
    else {
        LOG_ERROR("m2mb_sms_send() - failure");
        return M2MB_OS_APP_GENERIC_ERROR;
    }

    /*Wait for sms send event to occur (released in Sms_Callback function) */
    M2MB_OS_RESULT_E osRes = m2mb_os_ev_get(sms_evHandle, EV_SMS_SEND, M2MB_OS_EV_GET_ANY_AND_CLEAR, &curEvBits, M2MB_OS_MS2TICKS( timeout ));
    return osRes;
}

void delete_all_SMSs(void) {
	for (int i = 0; i < 10; i++) {
		M2MB_RESULT_E retVal = m2mb_sms_delete(h_sms_handle, i);
		LOG_DEBUG("del #%i res:%i", i, retVal);
	}
}
