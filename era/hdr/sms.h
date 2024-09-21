/**
 * @file sms.h
 * @version 1.0.0
 * @date 04.05.2022
 * @author: DKozenkov from RMerdeev
 * @brief app description
 */

#ifndef HDR_SMS_H_
#define HDR_SMS_H_

#include "m2mb_types.h"

typedef enum {
	SMS_IDLE			= 0,
	SMS_GSM_INITIALIZING,
	SMS_MIDLET_DEINITIALIZING,
	SMS_ERA_PROFILE_SETTING,
	SMS_REGISTRATION,
	SMS_SEND,
	SMS_TIMEOUT,
	SMS_WAIT_REQUEST,
	SMS_TIMEOUT_ADD,
	SMS_DEREGISTRATION,
	SMS_COMM_PROFILE_SETTING,
	SMS_MIDLET_INITIALIZING,
	SMS_GSM_DEINITIALIZING,
} T_sms_states;

typedef enum {
	TON_UNDEFINED		= 0,
	TON_INTERNATIONAL,
	TON_NATIONAL,
	TON_SPECIAL,
	TON_SUBSCRIBER,
	TON_ALPHANUMERIC,
	TON_SHORT,
	TON_RESERVED,
} T_ton;

typedef enum {
	NPI_UNDEFINED		= 0,
	NPI_ISDN,
	NPI_DATA_TRANSFER	= 3,
	NPI_TELEGRAPH,
	NPI_NATIONAL		= 8,
	NPI_PRIVAT,
	NPI_RESERVED		= 15,
} T_npi;

typedef enum {
	VPF_NO		= 0,
	VPF_EXTENDED,
	VPF_RELATIVE,
	VPF_ABSOLUTE,
} T_vpf;

typedef union {
	UINT8 all;
	struct {
		T_npi npi : 4;
		T_ton ton : 3;
		UINT8 not_used : 1;
	} single;
} T_smsa_t;

typedef struct {
	UINT8 smsa_l;
	T_smsa_t smsa_t;
	char smsa[12];
} T_smsa;

typedef union {
	UINT8 all;
	struct {
		UINT8 mti : 2;
		UINT8 rd : 1;
		T_vpf vpf : 2;
		UINT8 srr : 1;
		UINT8 udhi : 1;
		UINT8 rp : 1;
	} single;
} T_fo;

typedef struct {
	T_smsa smsc;
	T_fo fo;
	T_smsa da;
	UINT8 pid;
	UINT8 dcs;
	UINT8 vp[7];

	UINT8 udl;
	UINT8 * ud;
} T_sms_incoming;



//#define SMS_LEVEL "sms"
//#else
//#define SMS_LEVEL 0
//#endif


typedef enum {
//for negative return
	GSM_INVALID_PARAMETER		= -128,			//some parameter has an invalid format or value
	GSM_NULL_POINER,							//pointer parameter equal to NULL
	GSM_NO_CONDITIONS,							//no conditions to perform action
	GSM_ERR_RESERV_MEM,							//impossible to reserve memory to perform action
	GSM_TIMEOUT_AXCESSED,
	GSM_DON_T_STARTED,							//action do not started (without explanation)
	GSM_INIT_ERROR,								//initialize before using
	GSM_ERROR,									//general error (without explanation)
	GSM_UNKNOWN_ANSWER,							//unknown answer

//for handler
	GSM_SWITCH_ERROR			= -64,			//GSM-modem switching on/off do not occur
//	GSM_SIM_INIT_ERROR,							//SIM-initializing do not occur
//	GSM_REGISTER_ERROR,							//registration/deregistration do not occur or registration lost
//	GSM_PROFILE_ERROR,							//SIM-profile changing do not occur
	GSM_MSD_START_ERROR,						//starting of MSD sending failed
	GSM_MSD_SEND_ERROR,							//MSD sending failed
	GSM_MSD_SEND_OK,							//MSD sending ok
	GSM_NO_CARRIER,								//no carrier
	GSM_RING,									//ring
	GSM_LOST_CONNECTION,						//lost connection
//	GSM_INBAND_ERROR,							//data sending through inband/cancellation do not occur
	GSM_SMS_ERROR,								//SMS sending/cancellation do not occur
//	GSM_AUDIO_ERROR,							//audio playing/recording/cancellation do not occur
	GSM_PARAMETER_ERROR,
//	GSM_MUTE_ERROR,								// mute activation/deactivation do not occur
	GSM_CALLBACK_END,							// end of callback waiting
	GSM_SWITCHED_OFF,							// GSM spontaneously switched off

//for positive return
	GSM_NOT_ERROR				= 0,
	GSM_BUSY,
	GSM_WAIT_RESTART_TIMEOUT_1S,

	GSM_UNDEFINED				= 127,
} bsp_gsm_enum_events;

typedef enum {											//registration state changed
	GSM_DEREGISTERED			= 0,
	GSM_REGISTERED_HOME,
	GSM_REGISTERED_ROAMING,
} bsp_gsm_reg_enum_states;


/***************************************************************************/
/*  Declarations                                                           */
/***************************************************************************/
void sms_task ( void );
void sms_receive_handler ( UINT8 *data, UINT16 data_len );
void sms_sending (UINT8 changed, UINT32* timer, UINT8* captured);
void sms_init_variables ( void );
UINT8 sms_disable_all_dormancy ( void );
T_sms_states sms_state;
T_sms_incoming sms_incoming;
UINT32 sms_timer;
UINT8 sms_changed;
UINT8 sms_error;
UINT8 sms_request;
UINT8 sms_deregistration;
BOOLEAN sms_send (BOOLEAN send_respond);
#endif /* HDR_SMS_H_ */
