/**
 * @file sms_handler.h
 * @version 1.0.0
 * @date 05.05.2022
 * @author: DKozenkov from Telit
 * @brief app description
 */

#ifndef HDR_SMS_HANDLER_H_
#define HDR_SMS_HANDLER_H_

BOOLEAN init_sms_handler(void);
// Блокирующий метод
BOOLEAN send_sms_pdu(INT32 pdulen, UINT8 *pdu, BOOLEAN send_respond);

void delete_all_SMSs(void);
#endif /* HDR_SMS_HANDLER_H_ */
