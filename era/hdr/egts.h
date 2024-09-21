/**
 * @file egts.h
 * @version 1.0.0
 * @date 05.05.2022
 * @author: DKozenkov from RMerdeev
 * @brief app description
 */

#ifndef HDR_EGTS_H_
#define HDR_EGTS_H_

#include "m2mb_types.h"
#include "msd.h"

typedef enum {
	EGTS_IDLE			= 0,
#ifdef __CRYPTO_SIGN
	EGTS_GET_SIGNATURE,
#endif
	EGTS_SMS_SENDING,
} T_egts_states;

typedef enum {
	PT_RESPONSE			= 0,
	PT_APPDATA,
} T_pt;

typedef enum {
	PR_OK					= 0,
	PR_UNS_PROTOCOL			= 128,
	PR_DECRYPT_ERROR,
	PR_INC_HEADERFORM		= 131,
	PR_INC_DATAFORM,
	PR_HEADERCRC_ERROR		= 137,
	PR_DATACRC_ERROR,
	PR_TTLEXPIRED			= 144,
} T_pr;

typedef enum {
	SOD_UNIT			= 0,
	SOD_TELEMATIC_PLATFORM,
} T_sod;

typedef enum {
	ST_AUTH			= 1,
	ST_TELEDATA		= 2,
	ST_COMMANDS		= 4,
	ST_FIRMWARE		= 9,
	ST_ECALL,
} T_st;

typedef enum {
	SRT_RECORD_RESPONSE		= 0,
	SRT_ACCEL_DATA			= 20,
	SRT_RAW_MSD_DATA		= 40,
	SRT_COMMAND_DATA		= 51,
	SRT_TRACK_DATA			= 62,
} T_srt;

typedef enum {
	CT_COMCONF				= 1,
	CT_MSGCONF,
	CT_MSGFROM,
	CT_MSGTO,
	CT_COM,
	CT_DELCOM,
	CT_SUBREQ,
	CT_DELIV,
} T_ct;

typedef enum {
	CCT_OK			= 0,
	CCT_ERROR,
	CCT_ILL,
	CCT_DEL,
	CCT_NFOUND,
	CCT_NCONF,
	CCT_INPROG,
} T_cct;

typedef enum {
	FM_UNDEFINED		= 0,
	FM_GOST_R_54620,
} T_fm;

typedef enum {
	ACT_PAR			= 0,
	ACT_GET,
	ACT_SET,
	ACT_ADD,
	ACT_DEL,
} T_act;

typedef enum {
	CCD_RAW_DATA					= 0x0000,
	CCD_TEST_MODE,
	CCD_CONFIG_RESET				= 0x0006,
	CCD_SET_AUTH_CODE,
	CCD_RESTART,
	CCD_ECALL_REQ					= 0x0112,
	CCD_ECALL_MSD_REQ,
	CCD_ACCEL_DATA,
	CCD_TRACK_DATA,
	CCD_ECALL_DEREGISTRATION,
	CCD_RADIO_MUTE_DELAY			= 0x0201,
	CCD_RADIO_UNMUTE_DELAY,
	CCD_GPRS_APN,
	CCD_SERVER_ADDRESS,
	CCD_SIM_PIN,
	CCD_INT_MEM_TRANSMIT_INTERVAL,
	CCD_INT_MEM_TRANSMIT_ATTEMPTS,
	CCD_TEST_MODE_END_DISTANCE		= 0x020A,
	CCD_GARAGE_MODE_END_DISTANCE,
	CCD_GARAGE_MODE_PIN,
	CCD_ECALL_TEST_NUMBER,
	CCD_ECALL_ON					= 0x0210,
	CCD_ECALL_CRASH_SIGNAL_INTERNAL,
	CCD_ECALL_CRASH_SIGNAL_EXTERNAL,
	CCD_ECALL_SOS_BUTTON_TIME,
	CCD_ECALL_NO_AUTOMATIC_TRIGGERING,
	CCD_ASI15_TRESHOLD,
	CCD_ECALL_MODE_PIN,
	CCD_ECALL_CCFT,
	CCD_ECALL_INVITATION_SIGNAL_DURATION,
	CCD_ECALL_SEND_MSG_PERIOD,
	CCD_ECALL_AL_ACK_PERIOD = 0x021A,
	CCD_ECALL_MSD_MAX_TRANSMISSION_TIME,
	CCD_ECALL_NAD_DEREGISTRATION_TIME = 0x021D,
	CCD_ECALL_DIAL_DURATION,
	CCD_ECALL_AUTO_DIAL_ATTEMPTS,
	CCD_ECALL_MANUAL_DIAL_ATTEMPTS,
	CCD_ECALL_MANUAL_CAN_CANCEL = 0x0222,
	CCD_ECALL_SMS_FALLBACK_NUMBER,
	CCD_IGNITION_OFF_FOLLOW_UP_TIME1,
	CCD_IGNITION_OFF_FOLLOW_UP_TIME2,
	CCD_TEST_REGISTRATION_PERIOD	= 0x0242,
	CCD_CRASH_RECORD_TIME			= 0x0251,
	CCD_CRASH_RECORD_RESOLUTION,
	CCD_CRASH_PRE_RECORD_TIME,
	CCD_CRASH_PRE_RECORD_RESOLUTION,
	CCD_TRACK_RECORD_TIME			= 0x025A,
	CCD_TRACK_PRE_RECORD_TIME,
	CCD_TRACK_RECORD_RESOLUTION,
	CCD_GNSS_POWER_OFF_TIME			= 0x0301,
	CCD_GNSS_DATA_RATE,
	CCD_GNSS_MIN_ELEVATION,
	CCD_VEHICLE_VIN					= 0x0311,
	CCD_VEHICLE_TYPE,
	CCD_VEHICLE_PROPULSION_STORAGE_TYPE,
	CCD_UNIT_ID						= 0x0404,
	CCD_UNIT_IMEI,
	CCD_UNIT_RS485_BAUD_RATE,
	CCD_UNIT_RS485_STOP_BITS,
	CCD_UNIT_RS485_PARITY,
	CCD_UNIT_HOME_DISPATCHER_ID		= 0x0411,
	CCD_SERVICE_AUTH_METHOD,
	CCD_SERVER_CHECK_IN_PERIOD,
	CCD_SERVER_CHECK_IN_ATTEMPTS,
	CCD_SERVER_PACKET_TOUT,
	CCD_SERVER_PACKET_RETRANSMIT_ATTEMPTS,
	CCD_UNIT_MIC_LEVEL,
	CCD_UNIT_SPK_LEVEL,
} T_ccd;

typedef struct {
	UINT16 adr;
	union {
		UINT8 all;
		struct {
			T_act act : 4;
			UINT8 sz : 4;
		} single;
	} beb_ss;
	T_ccd ccd;
	UINT8 dtl;
	UINT8 dt[20];
} T_cd;

typedef union {
	struct {
		union {
			UINT8 all;
			struct {
				T_cct cct : 4;
				T_ct ct : 4;
			} single;
		} beb_ss;
		UINT32 cid;
		UINT32 sid;
		union {
			UINT8 all;
			struct {
				UINT8 acfe : 1;
				UINT8 chsfe : 1;
			} single;
		} beb_ss1;
		UINT8 chs;
		UINT8 acl;
		UINT8 ac[1];
		T_cd cd;
	} srd_command;
	//	struct
	//	{
	//		T_fm fm;
	//		U08 msdl;
	//		U08 msd[116];
	//	} srd_msd;
		struct {
			UINT16 crn;
			T_pr rst;
		} srd_response;
	} T_srd;

	typedef struct {
		T_srt srt;
		UINT16 srl;
		T_srd srd;
	} T_rd;

	typedef struct {
		UINT16 rl;
		UINT16 rn;
		union {
			UINT8 all;
			struct {
				UINT8 obfe : 1;
				UINT8 evfe : 1;
				UINT8 tmfe : 1;
				UINT8 rpp : 3;
				T_sod rsod : 1;
				T_sod ssod : 1;
			} single;
		} rfl;
		UINT32 oid;
		UINT32 evid;
		UINT32 tm;
		T_st sst;
		T_st rst;
		T_rd rd[1];
	} T_sdr;

	typedef struct
	{
		UINT16 rpid;
		T_pr pr;
		T_sdr sdr[1];
	} T_sfrd;

	typedef struct
	{
		UINT8 prv;
		UINT8 skid;
		union {
			UINT8 all;
			struct {
				UINT8 pr : 2;
				UINT8 cmp : 1;
				UINT8 ena : 2;
				UINT8 rte : 1;
				UINT8 prf : 2;
			} single;
		} beb_tr;
		UINT8 hl;
		UINT8 he;
		UINT16 fdl;
		UINT16 pid;
		T_pt pt;
		UINT16 pra;
		UINT16 pca;
		UINT8 ttl;
		UINT8 hcs;
		T_sfrd sfrd;
		UINT16 sfrcs;
	} T_egts_incoming;

	typedef struct {
		T_pt pt;
		T_pr pr;
		T_st st;
		T_srt srt;
		T_cct cct;
		T_ccd ccd;
		UINT8 dt[20];
		T_pr rst;

		T_MSDFlag flag;
		UINT8 len;
		UINT8 data[140];
	} T_egts_outgoing;

void egts_task ( void );
void egts_request ( UINT8 *data );
void egts_init_variables ( void );
T_egts_states egts_state;
UINT32 egts_timer;
UINT8 egts_changed;
UINT8 egts_error;
T_egts_incoming egts_incoming;
T_egts_outgoing egts_outgoing;
UINT8 egts_proceed;
BOOLEAN egts_config_msd ( T_MSDFlag flag );
BOOLEAN egts_config_answ ( void );
#endif /* HDR_EGTS_H_ */
