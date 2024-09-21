/**
 * @file sms_callbacks.h
 * @version 1.0.0
 * @date 05.05.2022
 * @author: Telit
 * @brief app description
 */

#ifndef HDR_SMS_CALLBACKS_H_
#define HDR_SMS_CALLBACKS_H_

#define EV_SMS_SEND         	(UINT32)0x0001
#define EV_SMS_SEND_FROM_MEM    (UINT32)0x0010
#define EV_SMS_RECV         	(UINT32)0x0011
#define EV_SMS_DELETE       	(UINT32)0x0100
#define EV_SMS_WRITE        	(UINT32)0x0101
#define EV_SMS_READ         	(UINT32)0x0110
#define EV_SMS_SET_TAG        	(UINT32)0x0111
#define EV_SMS_GET_STORAGE_STAT (UINT32)0x1000
#define EV_SMS_GET_SCA        	(UINT32)0x1001
#define EV_SMS_SET_SCA        	(UINT32)0x1010
#define EV_SMS_GET_STORAGE_IDXS	(UINT32)0x1011
#define EV_SMS_MEM_FULL        	(UINT32)0x1100
#define EV_SMS_MEM_FULL_REACHED	(UINT32)0x1101


/* Global typedefs ==============================================================================*/
/* Global functions =============================================================================*/
void Sms_Callback(M2MB_SMS_HANDLE h, M2MB_SMS_IND_E sms_event, UINT16 resp_size, void *resp_struct, void *myUserdata);

#endif /* HDR_SMS_CALLBACKS_H_ */
