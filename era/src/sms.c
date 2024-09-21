/**
 * @file sms.c
 * @version 1.0.0
 * @date 04.05.2022
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
#include "sms_handler.h"
#include "atc_era.h"
#include "auxiliary.h"
#include "egts.h"
#include "log.h"

/* Local defines ================================================================================*/
#define SMS_NPI                          NPI_ISDN
#define SMS_RP                           0x00
#define SMS_UDHI                         0x00
#define SMS_SRR                          0x00
#define SMS_VPF                          VPF_NO
#define SMS_RD                           0x00
#define SMS_MTI                          0x01
#define SMS_PID                          0x00
#define SMS_DCS                          0x04

/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static bsp_gsm_enum_events sms_command;
//static UINT8 sms_attempts;
#define BUF_CHAR_SIZE 300
static UINT8 sms_buffer_hex[140];
static char sms_buffer_char[BUF_CHAR_SIZE];
static UINT8 sms_mr;
static UINT8 sms_additional_send_flag;
static UINT32 sms_additional_send_timer;
static bsp_gsm_reg_enum_states sms_registration_prev;
static T_sms_states sms_state_prev;
static UINT8 sms_captured;
static UINT8 sms_bsp_attempt_timer;
static UINT8 sms_bsp_attempt;
static UINT8 gsm_smsc = 0;
/* Local function prototypes ====================================================================*/
static UINT8 sms_split_octet( UINT8 octet, char *buff );
static UINT8 sms_extract_sms_center_number( UINT8 *buff, UINT8* ind );
static void sms_extract_first_octet( UINT8 * buff, UINT8 *ind );
static UINT8 sms_extract_destination_address( UINT8 * buff, UINT8* ind );
static void sms_extract_packet_id( UINT8 * buff, UINT8 *ind );
static void sms_extract_data_coding_scheme( UINT8 * buff, UINT8* ind );
static void sms_extract_validity_period( UINT8 * buff, UINT8* ind );
static void sms_extract_user_data_length( UINT8 * buff, UINT8* ind );
static void sms_extract_user_data( UINT8 * buff, UINT8* ind );
static UINT8 sms_add_sms_center_number( UINT8 *buff, UINT8* ind );
static UINT8 sms_pack_sms_center_number( UINT8* buff, UINT8* ind );
static UINT8 sms_add_first_octet( UINT8 * buff, UINT8* ind );
static UINT8 sms_add_message_reference( UINT8 *buff, UINT8* ind );
static UINT8 sms_pack_destination_address( UINT8* buff, UINT8* ind );
static UINT8 sms_add_destination_address( UINT8 *buff, UINT8* ind );
static UINT8 sms_add_packet_id( UINT8 *buff, UINT8 *ind );
static UINT8 sms_add_data_coding_scheme( UINT8 * buff, UINT8* ind );
static UINT8 sms_add_user_data_length( UINT8 *buff, UINT8 *ind, UINT8 *data_length );
static UINT8 sms_add_user_data( UINT8 *buff, UINT8 *ind, UINT8 *data, UINT8 *data_length );
/* Static functions =============================================================================*/
/***************************************************************************/
/*  Function   : sms_split_octet                                           */
/*-------------------------------------------------------------------------*/
/*  Contents   : Split octet to 2 char                                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 sms_split_octet( UINT8 octet, char *buff ) {
    UINT8 nibble;
    const char table[10] = {'0','1','2','3','4','5','6','7','8','9'};

    nibble = octet & 0x0F;
    if ( nibble > 9 ) {
        return 0;
    }

    *buff = table[nibble];

    nibble = (UINT8)( octet & 0xF0 ) >> 4;
    if ( nibble != 0x0F ) {
        if ( nibble > 9 ) {
            return 0;
        }
        *(buff+1) = table[nibble];
    }
    return 1;
}

/***************************************************************************/
/*  Function   : sms_extract_sms_center_number                             */
/*-------------------------------------------------------------------------*/
/*  Contents   : Extract sms center number                                 */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/**************************************************************************/
static UINT8 sms_extract_sms_center_number( UINT8 *buff, UINT8* ind ) {
    sms_incoming.smsc.smsa_l = auxiliary_read_u8 ( buff, ind );

    if ( sms_incoming.smsc.smsa_l == 0 ) {
        return 1;
    }

    if ( sms_incoming.smsc.smsa_l > 7 ) {
        return 0;
    }

    sms_incoming.smsc.smsa_t.all = auxiliary_read_u8 ( buff, ind );

    UINT8 octet;
    UINT8 res;
    for ( UINT8 i=0; i<(sms_incoming.smsc.smsa_l-1); i++ ) {
        octet = auxiliary_read_u8 ( buff, ind );

        if ( ( octet & 0x0F ) == 0x0F ) {
            return 0;
        }

        res = sms_split_octet ( octet, sms_incoming.smsc.smsa + 2*i);
        if ( res == 0 ) {
            return 0;
        }
    }

    return 1;
}

/***************************************************************************/
/*  Function   : sms_extract_first_octet                                   */
/*-------------------------------------------------------------------------*/
/*  Contents   : Extract first octet                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

static void sms_extract_first_octet( UINT8 * buff, UINT8 *ind ) {
    sms_incoming.fo.all = auxiliary_read_u8 ( buff, ind );
}

/***************************************************************************/
/*  Function   : sms_extract_destination_address                           */
/*-------------------------------------------------------------------------*/
/*  Contents   : Extract destination address                               */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

static UINT8 sms_extract_destination_address( UINT8 * buff, UINT8* ind ) {
    sms_incoming.da.smsa_l = auxiliary_read_u8 ( buff, ind );

    if ( sms_incoming.da.smsa_l > 12 ) {
        return 0;
    }

    sms_incoming.da.smsa_t.all = auxiliary_read_u8 ( buff, ind );

    UINT8 smsa_l_oct = ( sms_incoming.da.smsa_l % 2 ) ?
                       ( ( sms_incoming.da.smsa_l + 1 ) >> 1 ) : ( sms_incoming.da.smsa_l >> 1 );

    UINT8 octet;
    UINT8 res;
    for ( UINT8 i=0; i<smsa_l_oct; i++ ) {
        octet = auxiliary_read_u8 ( buff, ind );

        if ( ( octet & 0x0F ) == 0x0F ) {
            return 0;
        }

        res = sms_split_octet ( octet, sms_incoming.da.smsa + 2*i);
        if ( res == 0 )
        {
            return 0;
        }
    }
    return 1;
}

/***************************************************************************/
/*  Function   : sms_extract_packet_id                                     */
/*-------------------------------------------------------------------------*/
/*  Contents   : Extract packet id                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static void sms_extract_packet_id( UINT8 * buff, UINT8 *ind ) {
    sms_incoming.pid = auxiliary_read_u8 ( buff, ind );
}

/***************************************************************************/
/*  Function   : sms_extract_data_coding_scheme                            */
/*-------------------------------------------------------------------------*/
/*  Contents   : Extract data coding scheme                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static void sms_extract_data_coding_scheme( UINT8 * buff, UINT8* ind ) {
    sms_incoming.dcs = auxiliary_read_u8 ( buff, ind );
}

/***************************************************************************/
/*  Function   : sms_extract_validity_period                               */
/*-------------------------------------------------------------------------*/
/*  Contents   : Extract validity period                                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

static void sms_extract_validity_period( UINT8 * buff, UINT8* ind ) {
//	switch ( sms_incoming.fo.single.vpf )
//	{
//		case VPF_RELATIVE:
//		{
//			sms_incoming.vp[0] = auxiliary_read_u8 ( buff, ind );
//		}
//		break;
//
//		case VPF_EXTENDED:
//		case VPF_ABSOLUTE:
//		{
    for (UINT8 i=0; i<7; i++) {
        sms_incoming.vp[i] = auxiliary_read_u8 ( buff, ind );
    }
//		}
//		break;
//
//		default:
//		{
//
//		}
//		break;
//	}
}

/***************************************************************************/
/*  Function   : sms_extract_user_data_length                              */
/*-------------------------------------------------------------------------*/
/*  Contents   : Extract user data length                                  */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

static void sms_extract_user_data_length( UINT8 * buff, UINT8* ind ) {
    sms_incoming.udl = auxiliary_read_u8 ( buff, ind );
}

/***************************************************************************/
/*  Function   : sms_extract_user_data                                     */
/*-------------------------------------------------------------------------*/
/*  Contents   : Extract user data                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static void sms_extract_user_data( UINT8 * buff, UINT8* ind ) {
    if ( sms_incoming.fo.single.udhi ) {
        UINT8 ludh = auxiliary_read_u8 ( buff, ind );

        *ind += ludh;
        sms_incoming.udl -= (ludh+1);
    }

    sms_incoming.ud = buff+(*ind);
}

/***************************************************************************/
/*  Function   : sms_pack_sms_center_number                                */
/*-------------------------------------------------------------------------*/
/*  Contents   : Pack sms center number                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 sms_pack_sms_center_number( UINT8* buff, UINT8* ind ) {
    UINT8 temp_buff[20];
    UINT8 temp_ind = 0;
    UINT8 temp_ind_1 = 0;
    char sms_cn[50] = {0};
    get_sms_center_number(sms_cn, sizeof(sms_cn));
    while ( (temp_ind < 19) && (sms_cn[temp_ind] != 0) ) {
        char sign = sms_cn[temp_ind];
        if(sign != '+')
        {
            if((sign >= 0x30) && (sign <= 0x39))
            {
                temp_buff[temp_ind_1++] = sign - 0x30;
            }
            else return 0;
        }
        temp_ind++;
    }
    if (temp_ind > 13) return 0;
    if (temp_ind_1 % 2 != 0) temp_buff[temp_ind_1++] = 0x0F;
    (*ind) = 0;
    for (temp_ind=0; temp_ind != temp_ind_1; temp_ind += 2) {
        buff[(*ind)++] = (UINT8)(temp_buff[temp_ind+1] << 4) | temp_buff[temp_ind];
    }
    return 1;
}

/***************************************************************************/
/*  Function   : sms_add_sms_center_number                                 */
/*-------------------------------------------------------------------------*/
/*  Contents   : Add sms center number                                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 sms_add_sms_center_number( UINT8 *buff, UINT8* ind ) {
    UINT8 temp_buff[10];
    UINT8 temp_ind = 0;
    UINT8 symbol;

    if (!sms_pack_sms_center_number( temp_buff, &temp_ind )) return 0;

    symbol = temp_ind + 1;

    *ind = auxiliary_write_u8 ( buff, *ind, symbol );

    if (temp_ind > 2) symbol = TON_INTERNATIONAL;
    else symbol = TON_UNDEFINED;

    symbol = (UINT8)((symbol << 4) | SMS_NPI | 0x80);
    *ind = auxiliary_write_u8 ( buff, *ind, symbol );
    for ( UINT8 i=0; i != temp_ind; i++ ) {
        *ind = auxiliary_write_u8 ( buff, *ind, temp_buff[i] );
    }
    return 1;
}

/***************************************************************************/
/*  Function   : sms_add_first_octet                                       */
/*-------------------------------------------------------------------------*/
/*  Contents   : Add first octet                                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 sms_add_first_octet( UINT8 * buff, UINT8* ind ) {
    T_fo symbol = {0};
    symbol.single.rp = SMS_RP;
    symbol.single.udhi = SMS_UDHI;
    symbol.single.srr = SMS_SRR;
    symbol.single.vpf = SMS_VPF;
    symbol.single.rd = SMS_RD;
    symbol.single.mti = SMS_MTI;
    *ind = auxiliary_write_u8 ( buff, *ind, symbol.all );
    return 1;
}

/***************************************************************************/
/*  Function   : sms_add_message_reference                                 */
/*-------------------------------------------------------------------------*/
/*  Contents   : Add message reference                                     */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 sms_add_message_reference( UINT8 *buff, UINT8* ind ) {
    *ind = auxiliary_write_u8 ( buff, *ind, sms_mr );
    return 1;
}

/***************************************************************************/
/*  Function   : sms_pack_destination_address                              */
/*-------------------------------------------------------------------------*/
/*  Contents   : Pack destination address                                  */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 sms_pack_destination_address( UINT8* buff, UINT8* ind ) {
    UINT8 temp_buff[20];
    UINT8 temp_ind = 0;
    UINT8 temp_ind_1 = 0;
    UINT8 res = 0;

    char data[50] = {0};
    get_ecall_to(data, sizeof(data));
    if ( strstr(data, ECALL_TO_DEBUG) != NULL )	{
        get_sms_fallback_debug_number(data, sizeof(data));
        while ( (temp_ind < 19) && (data[temp_ind] != 0) ) {
            char sign = data[temp_ind];
            if(sign != '+')
            {
                if((sign >= 0x30) && (sign <= 0x39))
                {
                    temp_buff[temp_ind_1++] = sign - 0x30;
                }
                else return 0;
            }
            temp_ind++;
        }
    } else {
        get_sms_fallback_number(data, sizeof(data));
        while ( (temp_ind < 19) && (data[temp_ind] != 0) ) {
            char sign = data[temp_ind];
            if(sign != '+') {
                if ((sign >= 0x30) && (sign <= 0x39)) {
                    temp_buff[temp_ind_1++] = sign - 0x30;
                } else return 0;
            }
            temp_ind++;
        }
    }

    res = temp_ind_1;
    if (temp_ind > 13) return 0;
    if (temp_ind_1 % 2 != 0) temp_buff[temp_ind_1++] = 0x0F;
    (*ind) = 0;
    for (temp_ind=0; temp_ind != temp_ind_1; temp_ind += 2) {
        buff[(*ind)++] = (UINT8)(temp_buff[temp_ind+1] << 4) | temp_buff[temp_ind];
    }
    (*ind) = res;
    return 1;
}



/***************************************************************************/
/*  Function   : sms_add_destination_address                               */
/*-------------------------------------------------------------------------*/
/*  Contents   : Add destination address                                   */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 sms_add_destination_address( UINT8 *buff, UINT8* ind ) {
    UINT8 temp_buff[10];
    UINT8 temp_ind = 0;
    UINT8 symbol;
    if (!sms_pack_destination_address( temp_buff, &temp_ind)) return 0;
    symbol = temp_ind;
    *ind = auxiliary_write_u8 ( buff, *ind, symbol );
    if (temp_ind > 4)	symbol = TON_INTERNATIONAL;
    else	symbol = TON_UNDEFINED;
    symbol = (UINT8)((symbol << 4) | SMS_NPI | 0x80);
    *ind = auxiliary_write_u8 ( buff, *ind, symbol );
    for ( UINT8 i=0; i != (temp_ind/2); i++ ) {
        *ind = auxiliary_write_u8 ( buff, *ind, temp_buff[i] );
    }
    if (temp_ind % 2 != 0) {
        *ind = auxiliary_write_u8 ( buff, *ind, temp_buff[temp_ind/2] );
    }
    return 1;
}

/***************************************************************************/
/*  Function   : sms_add_packet_id                                         */
/*-------------------------------------------------------------------------*/
/*  Contents   : Add packet id                                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 sms_add_packet_id( UINT8 *buff, UINT8 *ind ) {
    *ind = auxiliary_write_u8 ( buff, *ind, SMS_PID );
    return 1;
}

/***************************************************************************/
/*  Function   : sms_add_data_coding_scheme                                */
/*-------------------------------------------------------------------------*/
/*  Contents   : Add data coding scheme                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 sms_add_data_coding_scheme( UINT8 * buff, UINT8* ind ) {
    *ind = auxiliary_write_u8 ( buff, *ind, SMS_DCS );
    return 1;
}

/***************************************************************************/
/*  Function   : sms_add_user_data_length                                  */
/*-------------------------------------------------------------------------*/
/*  Contents   : Add user data length                                      */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT8 sms_add_user_data_length( UINT8 *buff, UINT8 *ind, UINT8 *data_length ) {
    if(*data_length >= 134) return 0;
    *ind = auxiliary_write_u8 ( buff, *ind, *data_length );
    return 1;
}

/***************************************************************************/
/*  Function   : sms_add_user_data                                         */
/*-------------------------------------------------------------------------*/
/*  Contents   : Add user data                                             */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

static UINT8 sms_add_user_data( UINT8 *buff, UINT8 *ind, UINT8 *data, UINT8 *data_length ) {
    for (UINT8 i=0; i != *data_length; i++) {
        *ind = auxiliary_write_u8 ( buff, *ind, data[i] );
    }
    return 1;
}

/***************************************************************************/
/*  Function   : sms_create_pdu                                            */
/*-------------------------------------------------------------------------*/
/*  Contents   : Create pdu                                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
static UINT16 sms_create_pdu( char *data, UINT8 *smsc_len, UINT8 *buffer, UINT8 *indicator ) {
    UINT8 ind = 0;

    if ( !gsm_smsc ) {
        if (!sms_add_sms_center_number(sms_buffer_hex, &ind)) return 0;
    } else {
        ind = auxiliary_write_u8 ( sms_buffer_hex, ind, 0 );
    }

    *smsc_len = ind;

    if (!sms_add_first_octet(sms_buffer_hex, &ind)) return 0;
    if (!sms_add_message_reference(sms_buffer_hex, &ind)) return 0;
    if (!sms_add_destination_address(sms_buffer_hex, &ind)) return 0;
    if (!sms_add_packet_id(sms_buffer_hex, &ind)) return 0;
    if (!sms_add_data_coding_scheme(sms_buffer_hex, &ind)) return 0;
    if (!sms_add_user_data_length(sms_buffer_hex, &ind, indicator)) return 0;
    if (!sms_add_user_data(sms_buffer_hex, &ind, buffer, indicator)) return 0;

    return auxiliary_hex_to_char(sms_buffer_hex, ind, data, BUF_CHAR_SIZE);
}

/* Global functions =============================================================================*/
/***************************************************************************/
/*  Function   : sms_init_variables                                        */
/*-------------------------------------------------------------------------*/
/*  Contents   : Init variables                                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
void sms_init_variables ( void ) {
    sms_state = SMS_IDLE;
    memset( &sms_incoming, 0, sizeof(T_sms_incoming) );
    sms_timer = 0;
    sms_changed = 0;
    sms_error = 0;
    sms_request = 0;
    sms_deregistration = 0;
    sms_command = GSM_UNDEFINED;
    //sms_attempts = 0;
    memset( sms_buffer_hex, 0, 140 );
    memset( sms_buffer_char, 0, BUF_CHAR_SIZE );
    sms_mr = 0;
    sms_additional_send_flag = 0;
    sms_additional_send_timer = 0;
    sms_registration_prev = GSM_DEREGISTERED;
    sms_state_prev = SMS_IDLE;
    sms_captured = 0;
    sms_bsp_attempt_timer = 0;
    sms_bsp_attempt = 0;
}

/***************************************************************************/
/*  Function   : sms_receive_handler                                       */
/*-------------------------------------------------------------------------*/
/*  Contents   : Perform receive handler for sms                           */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

void sms_receive_handler ( UINT8 *data, UINT16 data_len ) {
    //UINT8 num = auxiliary_char_to_hex(data, data_len, sms_buffer_hex);

    // Error - not even
    //if ( !num ) LOG_WARN("sms_receive_handler: error - not_even");
    memcpy(sms_buffer_hex, data, data_len);
    //print_hex("sms_buff_hex", (char *) sms_buffer_hex, data_len);

    UINT8 ind = 0;

    if (!sms_extract_sms_center_number(sms_buffer_hex, &ind)) {
        LOG_WARN("sms_receive_handler: error - bad_smsc");
    }

    sms_extract_first_octet(sms_buffer_hex, &ind);

    if (!sms_extract_destination_address(sms_buffer_hex, &ind)) {
        LOG_WARN("sms_receive_handler: error - bad_da");
    }

    sms_extract_packet_id(sms_buffer_hex, &ind);
    sms_extract_data_coding_scheme(sms_buffer_hex, &ind);
    sms_extract_validity_period(sms_buffer_hex, &ind);
    sms_extract_user_data_length(sms_buffer_hex, &ind);
    sms_extract_user_data(sms_buffer_hex, &ind);

    if ( sms_incoming.dcs == SMS_DCS ) {
        egts_request(sms_incoming.ud);
        if (egts_proceed) {
            egts_proceed = 0;
            if (egts_config_answ()) {
                LOG_DEBUG("SMS answer prepared");
                sms_send(FALSE);
            }
        }
    }
}

/***************************************************************************/
/*  Function   : sms_send                                                  */
/*-------------------------------------------------------------------------*/
/*  Contents   : Send sms                                                  */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
BOOLEAN sms_send(BOOLEAN send_respond) {
    UINT8 smsc_len = 0;
    UINT16 num = sms_create_pdu(sms_buffer_char, &smsc_len, egts_outgoing.data, &(egts_outgoing.len));

    if ( !num ) {
        LOG_ERROR("sms_send: error - no_create");
        return FALSE;
    }
    to_log_info_uart("SMS for send: %s", sms_buffer_char);
    int length = strlen(sms_buffer_char) / 2;
    //print_hex("SMS hex", (char *)sms_buffer_hex, length);
    return send_sms_pdu(length, sms_buffer_hex, send_respond);
}
