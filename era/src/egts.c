/**
 * @file egts.c
 * @version 1.0.0
 * @date 05.05.2022
 * @author: DKozenkov from RMerdeev
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "sms.h"
#include "egts.h"
#include "app_cfg.h"
#include "msd_storage.h"
#include "asn1.h"
#include "params.h"
#include "tasks.h"
#include "atc_era.h"
#include "sys_utils.h"
#include "auxiliary.h"
#include "log.h"
#include "era_helper.h"
/* Local defines ================================================================================*/
#define EGTS_PRV          0x01
#define EGTS_SKID         0x00
#define EGTS_BEB_TR       0x00
#define EGTS_HE           0x00
#define EGTS_ENA          0x00
#define EGTS_RFL          0x80
#define EGTS_BEB_SS1      0x00
#define EGTS_ADR          0x00
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static AZX_TIMER_ID sms_fn_t = NO_AZX_TIMER_ID;
#ifdef __CRYPTO_SIGN
static bsp_gsm_enum_events egts_command;
static UINT8 egts_bsp_attempt_timer;
static UINT8 egts_bsp_attempt;
#endif
static UINT16 egts_id;
#ifndef __CRYPTO_SIGN
static UINT8 egts_buffer[MSD_LENGTH];
#else
static UINT8 egts_buffer[MSD_LENGTH*2];
#endif
static UINT8 egts_buffer_len;
static T_egts_states egts_state_prev;
static UINT8 egts_captured;
static char sms_fbn[30] = {0};
/* Local function prototypes ====================================================================*/
//static UINT8 egts_operate_4b_par ( UINT16 *par, UINT16 coef, INT16 offset );
//static UINT8 egts_operate_1b_par ( UINT8* par, UINT8 key );
static UINT8 egts_operate_string_par ( char *par, UINT8 num );
static UINT8 egts_operate_number_par ( char *par, UINT8 num );
static void timer_handler(void* ctx, AZX_TIMER_ID id);
/* Static functions =============================================================================*/

/***************************************************************************/
/*  Function   : egts_operate_4b_par                                       */
/*-------------------------------------------------------------------------*/
/*  Contents   : Operate 4 bytes parameter command                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

// set: par = cmd*coef + offset
// get: cmd = (par - offset)/coef
// TODO адаптировать обработчик EGTS-команды записи/чтения параметра
//static UINT8 egts_operate_4b_par( UINT16 *par, UINT16 coef, INT16 offset ) {
//	UINT8 ind = 0;
//	T_act act = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.beb_ss.single.act;
//	UINT8 dtl = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dtl;
//
//	if ( act == ACT_SET ) {
//		if ( dtl == 4 ) {
//			INT16 temp = (INT16)(auxiliary_read_u32(egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt, &ind) * coef) - (INT16)offset;
//			if ( temp < 0 ) return 0;
//			*par = (UINT16)(temp);
//			return 1;
//		}
//	} else if ( act == ACT_GET ) {
//		INT32 temp = ((INT32)(*par) - (INT32)offset)/(INT32)coef;
//		auxiliary_write_u32(egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt, ind, temp);
//		egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dtl = 4;
//		return 1;
//	}
//
//	return 0;
//}

/***************************************************************************/
/*  Function   : egts_operate_1b_par                                       */
/*-------------------------------------------------------------------------*/
/*  Contents   : Operate 1 byte parameter command                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

//static UINT8 egts_operate_1b_par ( UINT8* par, UINT8 key ) {
//	T_act act = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.beb_ss.single.act;
//	UINT8 dtl = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dtl;
//
//	if ( act == ACT_SET ) {
//		if ( dtl == 1 ) {
//			if ( eeprom_param_set(par, key, egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[0]) ) return 1;
//		}
//	}
//	else if ( act == ACT_GET ) {
//		egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[0] = eeprom_param_get(par, key);
//		egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dtl = 1;
//		return 1;
//	}
//	return 0;
//}

/***************************************************************************/
/*  Function   : egts_operate_string_par                                   */
/*-------------------------------------------------------------------------*/
/*  Contents   : Operate string parameter command                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 egts_operate_string_par ( char *par, UINT8 num ) {
    T_act act = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.beb_ss.single.act;

    if ( act == ACT_SET ) {
        LOG_DEBUG("EGST ACT_SET, num=%i", num);
        if ( num < 20 ) {
            UINT8 i;
            for (i = 0; i < num; i++) {
                par[i] = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[i];
            }
            par[i] = 0;
            return 1;
        }
    } else if ( act == ACT_GET ) {
        LOG_DEBUG("EGST ACT_GET");
        UINT8 i = 0;
        while ( (par[i] != 0) && (i < 20) ) {
            egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[i] = par[i];
            i++;
        }
        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dtl = i;
        return 1;
    }
    return 0;
}
/***************************************************************************/
/*  Function   : egts_operate_number_par                                   */
/*-------------------------------------------------------------------------*/
/*  Contents   : Operate number parameter command                          */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 egts_operate_number_par ( char *par, UINT8 num ) {
    T_act act = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.beb_ss.single.act;

    if ( act == ACT_GET ) {
        for ( UINT8 i=0; i<num; i++ ) {
            egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[i] = par[i];
        }
        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dtl = num;
        return 1;
    }
    return 0;
}



/* Global functions =============================================================================*/
/***************************************************************************/
/*  Function   : egts_init_variables                                       */
/*-------------------------------------------------------------------------*/
/*  Contents   : Init variables                                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
void egts_init_variables ( void ) {
    egts_state = EGTS_IDLE;
    egts_timer = 0;
    egts_changed = 0;
    egts_error = 0;
    memset( &egts_incoming, 0, sizeof(T_egts_incoming) );
    memset( &egts_outgoing, 0, sizeof(T_egts_outgoing) );
    egts_proceed = 0;
#ifdef __CRYPTO_SIGN
    egts_command = GSM_UNDEFINED;
	egts_bsp_attempt_timer = 0;
	egts_bsp_attempt = 0;
#endif
    egts_id = 0;
#ifndef __CRYPTO_SIGN
    memset( egts_buffer, 0, MSD_LENGTH );
#else
    memset( egts_buffer, 0, MSD_LENGTH*2 );
#endif
    egts_buffer_len = 0;
    egts_state_prev = EGTS_IDLE;
    egts_captured = 0;
}

/***************************************************************************/
/*  Function   : egts_request                                              */
/*-------------------------------------------------------------------------*/
/*  Contents   : Perform egts request                                      */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

void egts_request ( UINT8 *data ) {
    UINT8 ind = 0;
    egts_proceed = 0;

    // transport
    egts_incoming.prv = auxiliary_read_u8(data, &ind);
    egts_incoming.skid = auxiliary_read_u8(data, &ind);
    egts_incoming.beb_tr.all = auxiliary_read_u8(data, &ind);
    if ( (egts_incoming.prv != EGTS_PRV) || (egts_incoming.beb_tr.all != EGTS_BEB_TR) ) {
        egts_incoming.pt = PT_RESPONSE;
        egts_incoming.sfrd.pr = PR_UNS_PROTOCOL;
        egts_proceed = 1;
        LOG_ERROR("egts_request: error - uns protocol: prv = %d; beb_tr = %d", egts_incoming.prv, egts_incoming.beb_tr.all);
        return;
    }
    egts_incoming.hl = auxiliary_read_u8(data, &ind);
    if ( egts_incoming.hl != 11 ) {
        egts_incoming.pt = PT_RESPONSE;
        egts_incoming.sfrd.pr = PR_INC_HEADERFORM;
        egts_proceed = 1;
        LOG_ERROR("egts_request: error - inc headerform: hl = %d", egts_incoming.hl);
        return;
    }
    egts_incoming.he = auxiliary_read_u8(data, &ind);
    egts_incoming.fdl = auxiliary_read_u16(data, &ind);
    egts_incoming.pid = auxiliary_read_u16(data, &ind);
    egts_incoming.pt = auxiliary_read_u8(data, &ind);
    if ( egts_incoming.pt != PT_APPDATA ) {
        egts_incoming.pt = PT_RESPONSE;
        egts_incoming.sfrd.pr = PR_UNS_PROTOCOL;
        egts_proceed = 1;
        LOG_ERROR("egts_request: error - uns protocol 1: pt = %d", egts_incoming.pt);
        return;
    }
    egts_incoming.hcs = auxiliary_read_u8(data, &ind);
    UINT8 hcs_c = auxiliary_crc8( data, ind-1);
    if ( egts_incoming.hcs != hcs_c ) {
        egts_incoming.pt = PT_RESPONSE;
        egts_incoming.sfrd.pr = PR_HEADERCRC_ERROR;
        egts_proceed = 1;
        LOG_ERROR("egts_request: error - headercrc: hcs = %d; hcs_c = %d", egts_incoming.hcs, hcs_c);
        return;
    }
    if ( egts_incoming.fdl == 0 ) {
        egts_incoming.pt = PT_RESPONSE;
        egts_incoming.sfrd.pr = PR_OK;
        egts_proceed = 1;
        return;
    }
    UINT8 ind_t = egts_incoming.fdl + 11;
    egts_incoming.sfrcs = auxiliary_read_u16(data, &ind_t);
    UINT16 sfrcs_c = auxiliary_crc16( data+11, (UINT8)(egts_incoming.fdl));
    if ( egts_incoming.sfrcs != sfrcs_c ) {
        egts_incoming.pt = PT_RESPONSE;
        egts_incoming.sfrd.pr = PR_DATACRC_ERROR;
        egts_proceed = 1;
        LOG_ERROR("egts_request: error - datacrc: sfrcs = %d; sfrcs_c = %d", egts_incoming.sfrcs, sfrcs_c);
        return;
    }
    egts_incoming.sfrd.pr = PR_OK;

    // service support
    // record
    if ( ind != egts_incoming.hl ) {
        LOG_ERROR("egts_request: error - header length \r\n");
        return;
    }
    egts_incoming.sfrd.sdr[0].rl = auxiliary_read_u16(data, &ind);
    egts_incoming.sfrd.sdr[0].rn = auxiliary_read_u16(data, &ind);
    egts_incoming.sfrd.sdr[0].rfl.all = auxiliary_read_u8(data, &ind);
    if ( egts_incoming.sfrd.sdr[0].rfl.single.obfe ) egts_incoming.sfrd.sdr[0].oid = auxiliary_read_u32(data, &ind);
    if ( egts_incoming.sfrd.sdr[0].rfl.single.evfe ) egts_incoming.sfrd.sdr[0].evid = auxiliary_read_u32(data, &ind);
    if ( egts_incoming.sfrd.sdr[0].rfl.single.tmfe ) egts_incoming.sfrd.sdr[0].tm = auxiliary_read_u32(data, &ind);
    egts_incoming.sfrd.sdr[0].sst = auxiliary_read_u8(data, &ind);
    egts_incoming.sfrd.sdr[0].rst = auxiliary_read_u8(data, &ind);
    if ( (egts_incoming.sfrd.sdr[0].sst != ST_COMMANDS) || (egts_incoming.sfrd.sdr[0].rst != ST_COMMANDS) ) {
        LOG_ERROR("egts_request: error - st not commands: sst = %d; rst = %d", egts_incoming.sfrd.sdr[0].sst, egts_incoming.sfrd.sdr[0].rst);
        return;
    }

    // subrecord
    egts_incoming.sfrd.sdr[0].rd[0].srt = auxiliary_read_u8(data, &ind);
    egts_incoming.sfrd.sdr[0].rd[0].srl = auxiliary_read_u16(data, &ind);
    if ( egts_incoming.sfrd.sdr[0].rd[0].srt == SRT_COMMAND_DATA ) {
        UINT8 ind_sr = 0;
        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss.all = auxiliary_read_u8(data, &ind);
        ind_sr++;
        if ( egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss.single.ct != CT_COM ) {
            LOG_ERROR("egts_request: error - ct not com: ct = %d", egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss.single.ct);
            return;
        }
        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cid = auxiliary_read_u32(data, &ind);
        ind_sr += 4;
        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.sid = auxiliary_read_u32(data, &ind);
        ind_sr += 4;
        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss1.all = auxiliary_read_u8(data, &ind);
        ind_sr++;
        if ( egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss1.single.chsfe ) {
            egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.chs = auxiliary_read_u8(data, &ind);
            ind_sr++;
        }
        if ( egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss1.single.acfe ) {
            egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.acl = auxiliary_read_u8(data, &ind);
            ind_sr++;
            for (UINT8 i=0; i<egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.acl; i++) {
                egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.ac[0] = auxiliary_read_u8(data, &ind);
                ind_sr++;
            }
        }
        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.adr = auxiliary_read_u16(data, &ind);
        ind_sr += 2;
        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.beb_ss.all = auxiliary_read_u8(data, &ind);
        ind_sr++;
        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.ccd = auxiliary_read_u16(data, &ind);
        ind_sr += 2;
        if ( ind_sr > egts_incoming.sfrd.sdr[0].rd[0].srl ) {
            LOG_ERROR("egts_request: error - subrecord length");
            return;
        }
        UINT16 dtl = egts_incoming.sfrd.sdr[0].rd[0].srl - ind_sr;
        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dtl = (UINT8)dtl;
        for ( UINT8 i=0; i<dtl; i++) {
            egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[i] = auxiliary_read_u8(data, &ind);
        }

        T_act act = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.beb_ss.single.act;
        UINT16 ccd = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.ccd;
        egts_proceed = 1;

        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss.single.cct = CCT_OK;
        T_cct cct = CCT_ERROR;
        LOG_DEBUG("EGTS ccd: %i", ccd);
        switch ( ccd ) {
            case CCD_ECALL_REQ: {
                if ( act == ACT_PAR ) {
                    if ( dtl == 1 ) {
                        if ( egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[0] <= 1 ) {
                            //if (is_ecall_in_progress()) {
                            //	LOG_WARN("EGTS request error - already ecall");
                            //	return;
                            //} else {
                            to_log_info_uart("egts_request: ecall request: cd = %d", egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[0]);
                            m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1000));
                            send_to_ecall_task(ECALL_SMS_REQUEST, ECALL_NO_REPEAT_REASON, 0);
                            return;
                            //}
                        }
                    }
                }
            }
                break;

            case CCD_ECALL_MSD_REQ: {
                if ( act == ACT_PAR ) {
                    if ( dtl == 5 ) {
                        if ( (egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[4] == 0) ||
                             (egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[4] == 2) ) {
                            egts_proceed = 0;
                            ind = 0;
                            to_log_info_uart("EGTS request: ecall msd request: mid = %d, trans = %d",(UINT16)(auxiliary_read_u32(egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt, &ind)), egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[4]);
                            LOG_INFO("ECALL_MSD_REQ, waiting RAIM"); // по просьбе А.Архипова от 09.02.24 для прохождения теста 6.20
                            for (int i = 0; i < 50; i++) {
                            	UINT8 fix = get_gnss_data().fix;
                            	LOG_INFO("GNSS (sms) fix:%i", fix);
                            	if (fix > 0) break;
                            	pause_gnss_request(FALSE);
                            	m2mb_os_taskSleep( M2MB_OS_MS2TICKS(500));
                            }
                            increment_msd_id(FALSE);
                            if (egts_config_msd(MSD_INBAND)) {
                                sms_send(FALSE);
                            } else {
                                LOG_ERROR("Can't prepare egts");
                            }
                            return;
                        }
                    }
                }
            }
                break;

            case CCD_ECALL_DEREGISTRATION:
            {
                if ( act == ACT_PAR )
                {
                    if ( dtl == 0 ) {
                        egts_proceed = 0;
                        ind = 0;
                        azx_sleep_ms(5000); // Need dalay!!!!
                        LOG_DEBUG("egts_request: ecall deregistration");
                        send_to_ecall_task(ECALL_DEREGISTRATION, 0, 0); // это только для управления индикацией
                        // deregistration();
                        if(!at_era_sendCommandExpectOk(10 * 1000, "AT+COPS=2\r")) LOG_DEBUG("Can't deregist");
                        if(!at_era_sendCommandExpectOk(10 * 1000, "AT+COPS=0\r")) LOG_DEBUG("Can't set AT+COPS=0");
                        return;
                    }
                }
            }
                break;

            case CCD_INT_MEM_TRANSMIT_INTERVAL: {
                // TODO здесь и далее сделать корректную обработку команд чтения/записи параметров
                //if ( egts_operate_4b_par(&(setup.int_mem_transmit_interval),1,0) ) return;
            }
                break;

            case CCD_INT_MEM_TRANSMIT_ATTEMPTS: {
                //if ( egts_operate_4b_par(&(setup.int_mem_transmit_attempts),1,0) ) return;
            }
                break;

            case CCD_TEST_MODE_END_DISTANCE: {
                //if ( egts_operate_4b_par(&(setup.test_mode_end_distance),1,0) ) return;
            }
                break;

            case CCD_ECALL_TEST_NUMBER: {
                char test_number[30];
                get_test_number(test_number, sizeof(test_number));
                LOG_DEBUG("Rec CCD_ECALL_TEST_NUMBER, before: %s", test_number);
                char new_number[30] = {0};
                if ( egts_operate_string_par(new_number, (UINT8)dtl) ) {
                    char numb[30] = {0};
                    numb[0] = '+';
                    memcpy(numb + 1, new_number, 29);
                    LOG_DEBUG("Rec CCD_ECALL_TEST_NUMBER, will be: %s", numb);
                    set_test_number(numb);
                    return;
                }
            }
                break;

            case CCD_ECALL_ON: {
                //if ( egts_operate_1b_par(&(setup.ecall_on), ECALL_ON_KEY) ) return;
            }
                break;

            case CCD_ECALL_NO_AUTOMATIC_TRIGGERING: {
                //if ( egts_operate_1b_par(&(setup.ecall_no_automatic_triggering), ECALL_NO_AUTOMATIC_TRIGGERING_KEY) ) return;
            }
                break;

            case CCD_ECALL_CCFT: {
                //if ( egts_operate_4b_par(&(setup.ecall_ccft),1,0) ) return;
            }
                break;

            case CCD_ECALL_INVITATION_SIGNAL_DURATION: {
                //if ( egts_operate_4b_par(&(setup.ecall_invitation_signal_duration),1,0) ) return;
            }
                break;

            case CCD_ECALL_SEND_MSG_PERIOD: {
                //if ( egts_operate_4b_par(&(setup.ecall_send_msg_period),1,0) ) return;
            }
                break;

            case CCD_ECALL_AL_ACK_PERIOD: {
                //if ( egts_operate_4b_par(&(setup.ecall_al_ack_period),1,0) ) return;
            }
                break;

            case CCD_ECALL_MSD_MAX_TRANSMISSION_TIME: {
                //if ( egts_operate_4b_par(&(setup.ecall_msd_max_transmission_time),1,0) ) return;
            }
                break;

            case CCD_ECALL_NAD_DEREGISTRATION_TIME: {
                //if ( egts_operate_4b_par(&(setup.ecall_nad_deregistration_time),60,0) ) return;
            }
                break;

            case CCD_ECALL_DIAL_DURATION: {
                //if ( egts_operate_4b_par(&(setup.ecall_dial_duration),1,0) ) return;
            }
                break;

            case CCD_ECALL_AUTO_DIAL_ATTEMPTS: {
                //if ( egts_operate_4b_par(&(setup.ecall_auto_dial_attempts),1,0) ) return;
            }
                break;

            case CCD_ECALL_MANUAL_DIAL_ATTEMPTS: {
                //if ( egts_operate_4b_par(&(setup.ecall_manual_dial_attempts),1,0) ) return;
            }
                break;

            case CCD_ECALL_MANUAL_CAN_CANCEL: {
                //if ( egts_operate_1b_par(&(setup.ecall_manual_can_cancel), ECALL_MANUAL_CAN_CANCEL_KEY) ) return;
            }
                break;

            case CCD_ECALL_SMS_FALLBACK_NUMBER: {
                //if ( egts_operate_string_par(setup.ecall_sms_fallback_number,(UINT8)dtl) ) return;
                char fall_number[30];
                get_sms_fallback_number(fall_number, sizeof(fall_number));
                LOG_DEBUG("Rec CCD_ECALL_SMS_FALLBACK_NUMBER, before: %s", fall_number);
                char new_number[30] = {0};
                if ( egts_operate_string_par(new_number, (UINT8)dtl) ) {
                    memset(sms_fbn, 0, sizeof(sms_fbn));
                    sms_fbn[0] = '+';
                    memcpy(sms_fbn + 1, new_number, 29);
                    LOG_DEBUG("NEW_ECALL_SMS_FALLBACK_NUMBER, will be: %s", sms_fbn);
                    deinit_timer(&sms_fn_t);
                    sms_fn_t = init_timer("sms_fn_t", 5*1000, timer_handler);
                    return;
                }
            }
                break;

            case CCD_TEST_REGISTRATION_PERIOD:{
                //if ( egts_operate_4b_par(&(setup.test_registration_period),1,0) ) return;
            }
                break;

            case CCD_GNSS_DATA_RATE:
            {
                if ( act == ACT_SET ) {
                    if ( dtl == 4 ) {
                        return;
                    }
                }
                else if ( act == ACT_GET ) {
                    INT32 temp = 1;
                    auxiliary_write_u32(egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt, ind, temp);
                    egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dtl = 4;
                    return;
                }
            }
                break;

            case CCD_VEHICLE_VIN: {
                char vin[50];
                get_vin(vin, sizeof(vin));
                if ( egts_operate_number_par(vin, 17) ) return;
            }
                break;

            case CCD_VEHICLE_TYPE: {
                //if ( egts_operate_4b_par((UINT16*)(&(setup.vehicle_type)),1,(-1)) ) return;
            }
                break;

            case CCD_VEHICLE_PROPULSION_STORAGE_TYPE: {
                if ( act == ACT_SET ) {
                    if ( dtl == 4 ) {
                        UINT32 temp = auxiliary_read_u32(egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt, &ind);
                        if (!set_propulsion_type(temp)) LOG_ERROR("Can't egts set PST");
//						if ( temp & 0x00000001 ) setup.vehicle_propulsion_storage_type.type.gasoline = 1;
//						if ( temp & 0x00000002 ) setup.vehicle_propulsion_storage_type.type.diesel = 1;
//						if ( temp & 0x00000004 ) setup.vehicle_propulsion_storage_type.type.compressedNaturalGas = 1;
//						if ( temp & 0x00000008 ) setup.vehicle_propulsion_storage_type.type.liquidPropaneGas = 1;
//						if ( temp & 0x00000010 ) setup.vehicle_propulsion_storage_type.type.electricEnergy = 1;
//						if ( temp & 0x00000020 ) setup.vehicle_propulsion_storage_type.type.hydrogen = 1;
                        return;
                    }
                } else if ( act == ACT_GET ) {
                    UINT32 temp = get_propulsion_type();
//					if ( setup.vehicle_propulsion_storage_type.type.gasoline ) temp |= 0x00000001;
//					if ( setup.vehicle_propulsion_storage_type.type.diesel ) temp |= 0x00000002;
//					if ( setup.vehicle_propulsion_storage_type.type.compressedNaturalGas ) temp |= 0x00000004;
//					if ( setup.vehicle_propulsion_storage_type.type.liquidPropaneGas ) temp |= 0x00000008;
//					if ( setup.vehicle_propulsion_storage_type.type.electricEnergy ) temp |= 0x00000010;
//					if ( setup.vehicle_propulsion_storage_type.type.hydrogen ) temp |= 0x00000020;
                    auxiliary_write_u32(egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt, ind, temp);
                    egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dtl = 4;
                    return;
                }
            }
                break;

            case CCD_UNIT_IMEI: {
                if ( egts_operate_number_par(get_imei(), 15) ) return;
            }
                break;

            default: {
                LOG_WARN("egts_request: error - undef ccd: %i", ccd);
                cct = CCT_ILL;
            }
                break;
        }

        egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss.single.cct = cct;
    }
}

/***************************************************************************/
/*  Function   : egts_config_msd                                           */
/*-------------------------------------------------------------------------*/
/*  Contents   : Config egts for msd                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
BOOLEAN egts_config_msd ( T_MSDFlag flag ) {
    T_msd msd;
    BOOLEAN found = TRUE;
    if (flag == MSD_INBAND) {
        msd = get_current_msd();
        msd.msdMessage.msdStructure.controlType.automaticActivation = FALSE; // для прохождения теста 22 - повторная передача МНД
    } else {
        found = find_msd_n(&msd, flag == MSD_SAVED ? MSD_SAVED : MSD_SMS) == -1 ? FALSE : TRUE;
    }
    // Error - no msd
    if ( !found ) {
        LOG_ERROR("egts_config_msd: error - no_msd");
        return FALSE;
    }
    print_hex("MSD from storage", (char *)&msd, sizeof(msd));
    egts_buffer_len = asn1_encode_msd(&msd, egts_buffer);

    // Error - no encode
    if ( egts_buffer_len == 255 ) {
        LOG_ERROR("egts_config_msd: error - no_encode");
        return FALSE;
    }
    UINT8 ind = 0;
#ifndef __CRYPTO_SIGN
    UINT8 fdl = 11+egts_buffer_len;
    UINT8 rl = 4+egts_buffer_len;
    UINT8 srl = 1+egts_buffer_len;
#else
    UINT8 fdl = 11+egts_buffer_len+34;
	UINT8 rl = 4+egts_buffer_len+34;
	UINT8 srl = 1+egts_buffer_len+34;
#endif

    egts_outgoing.data[ind++] = EGTS_PRV;
    egts_outgoing.data[ind++] = EGTS_SKID;
    egts_outgoing.data[ind++] = EGTS_BEB_TR;
    egts_outgoing.data[ind++] = 11;
    egts_outgoing.data[ind++] = EGTS_HE;

    ind = auxiliary_write_u16(egts_outgoing.data, ind, (UINT16)fdl);

    ind = auxiliary_write_u16(egts_outgoing.data, ind, egts_id);

    egts_outgoing.data[ind++] = PT_APPDATA;
    egts_outgoing.pt = PT_APPDATA;

    egts_outgoing.data[ind] = auxiliary_crc8( (UINT8 *)egts_outgoing.data, ind);
    ind++;
    ind = auxiliary_write_u16(egts_outgoing.data, ind, (UINT16)rl);

    ind = auxiliary_write_u16(egts_outgoing.data, ind, egts_id);

    egts_outgoing.data[ind++] = EGTS_RFL;
    egts_outgoing.data[ind++] = ST_ECALL;
    egts_outgoing.data[ind++] = ST_ECALL;
    egts_outgoing.st = ST_ECALL;
    egts_outgoing.data[ind++] = SRT_RAW_MSD_DATA;
    egts_outgoing.srt = SRT_RAW_MSD_DATA;

    ind = auxiliary_write_u16(egts_outgoing.data, ind, (UINT16)srl);

    egts_outgoing.data[ind++] = FM_GOST_R_54620;
#ifndef __CRYPTO_SIGN
    for (UINT8 i=0;i<egts_buffer_len;i++) {
        egts_outgoing.data[ind+i] = egts_buffer[i];
    }
    ind+=egts_buffer_len;

    ind = auxiliary_write_u16(egts_outgoing.data, ind, auxiliary_crc16( (UINT8 *)(egts_outgoing.data + 11), ind-11));
#endif

    egts_outgoing.len = ind;
    egts_outgoing.flag = flag;

#ifndef __CRYPTO_SIGN
    egts_id++;
#endif
    return TRUE;
}

/***************************************************************************/
/*  Function   : egts_config_answ                                          */
/*-------------------------------------------------------------------------*/
/*  Contents   : Config egts for answ                                      */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
BOOLEAN egts_config_answ ( void ) {
    UINT8 ind = 0;

    egts_outgoing.data[ind++] = EGTS_PRV;
    egts_outgoing.data[ind++] = EGTS_SKID;
    egts_outgoing.data[ind++] = EGTS_BEB_TR;
    egts_outgoing.data[ind++] = 11;
    egts_outgoing.data[ind++] = EGTS_HE;

    if ( egts_incoming.pt == PT_RESPONSE ) {
        UINT8 fdl = 3;

        ind = auxiliary_write_u16(egts_outgoing.data, ind, (UINT16)fdl);
        ind = auxiliary_write_u16(egts_outgoing.data, ind, egts_id);

        egts_outgoing.data[ind++] = egts_incoming.pt;
        egts_outgoing.pt = egts_incoming.pt;

        egts_outgoing.data[ind] = auxiliary_crc8( (UINT8 *)egts_outgoing.data, ind);
        ind++;
        ind = auxiliary_write_u16(egts_outgoing.data, ind, egts_incoming.pid);

        egts_outgoing.data[ind++] = egts_incoming.sfrd.pr;
        egts_outgoing.pr = egts_incoming.sfrd.pr;
    } else if ( egts_incoming.pt == PT_APPDATA ) {
        UINT8 len;
        UINT8 fdl;
        UINT8 rl;
        UINT8 srl;

        if ( egts_incoming.sfrd.sdr[0].rd[0].srt == SRT_RECORD_RESPONSE ) {
            fdl = 13;
            rl = 6;
            srl = 3;
        } else if ( egts_incoming.sfrd.sdr[0].rd[0].srt == SRT_COMMAND_DATA ) {
            len = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dtl;
            fdl = 24+len;
            rl = 17+len;
            srl = 14+len;
        } else {
            // Error - no srt
            LOG_ERROR("egts_config_answ: error - no_srt");
            return FALSE;
        }

        ind = auxiliary_write_u16(egts_outgoing.data, ind, (UINT16)fdl);
        ind = auxiliary_write_u16(egts_outgoing.data, ind, egts_id);

        egts_outgoing.data[ind++] = PT_APPDATA;
        egts_outgoing.pt = PT_APPDATA;

        egts_outgoing.data[ind] = auxiliary_crc8( (UINT8 *)egts_outgoing.data, ind);
        ind++;
        ind = auxiliary_write_u16(egts_outgoing.data, ind, (UINT16)rl);
        ind = auxiliary_write_u16(egts_outgoing.data, ind, egts_id);

        egts_outgoing.data[ind++] = EGTS_RFL;
        egts_outgoing.data[ind++] = egts_incoming.sfrd.sdr[0].sst;
        egts_outgoing.data[ind++] = egts_incoming.sfrd.sdr[0].rst;
        egts_outgoing.st = egts_incoming.sfrd.sdr[0].sst;
        egts_outgoing.data[ind++] = egts_incoming.sfrd.sdr[0].rd[0].srt;
        egts_outgoing.srt = egts_incoming.sfrd.sdr[0].rd[0].srt;

        ind = auxiliary_write_u16(egts_outgoing.data, ind, (UINT16)srl);

        if ( egts_incoming.sfrd.sdr[0].rd[0].srt == SRT_RECORD_RESPONSE ) {
            ind = auxiliary_write_u16(egts_outgoing.data, ind, egts_incoming.sfrd.sdr[0].rn);
            egts_outgoing.data[ind++] = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_response.rst;
            egts_outgoing.rst = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_response.rst;
        } else if ( egts_incoming.sfrd.sdr[0].rd[0].srt == SRT_COMMAND_DATA ) {
            if ( egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss.single.ct == CT_COM ) {
                egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss.single.ct = CT_COMCONF;
                egts_outgoing.data[ind++] = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss.all;

                ind = auxiliary_write_u32(egts_outgoing.data, ind, egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cid);
                ind = auxiliary_write_u32(egts_outgoing.data, ind, egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.sid);

                egts_outgoing.data[ind++] = EGTS_BEB_SS1;

                ind = auxiliary_write_u16(egts_outgoing.data, ind, egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.adr);
                ind = auxiliary_write_u16(egts_outgoing.data, ind, egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.ccd);
                egts_outgoing.ccd = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.ccd;
                egts_outgoing.cct = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.beb_ss.single.cct;

                for (UINT8 i=0;i<len;i++) {
                    egts_outgoing.data[ind+i] = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[i];
                    egts_outgoing.dt[i] = egts_incoming.sfrd.sdr[0].rd[0].srd.srd_command.cd.dt[i];
                }
                ind+=len;
            } else {
                // Error - no ct
                LOG_ERROR("egts_config_answ: error - no_ct");
                return FALSE;
            }
        }
    } else {
        // Error - no pt
        LOG_ERROR("egts_config_answ: error - no_pt");
        return FALSE;
    }
    ind = auxiliary_write_u16(egts_outgoing.data, ind, auxiliary_crc16( (UINT8 *)(egts_outgoing.data + 11), ind-11));
    egts_outgoing.len = ind;
    egts_id++;
    return TRUE;
}

static void timer_handler(void* ctx, AZX_TIMER_ID id) {
    (void)ctx;
    if (sms_fn_t == id) {
        set_sms_fallback_number(sms_fbn);
    }
}

