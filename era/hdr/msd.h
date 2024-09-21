/**
 * @file msd.h
 * @version 1.0.0
 * @date 17.04.2022
 * @author: DKozenkov from RMerdeev
 * @brief app description
 */

#ifndef HDR_MSD_H_
#define HDR_MSD_H_
#include "m2mb_types.h"

#include "../../hal/hdr/gnss_rec.h"
#include "app_cfg.h"
#include "params.h"
typedef struct {
	UINT8 inband; // MНД для отправки через голосовой канал
	UINT8 sms;	  // МНД для отправки через СМС
	UINT8 saved;  // МНД, отправка которого оказалась невозможной ни через какой канал и оно было просто сохранено в файловой системе
	UINT16 last_number;
} T_msd_counters;

typedef enum {
	MSD_SAVE_INBAND			= 0,
	MSD_REMOVE_INBAND,
	MSD_MOVE_INBAND_TO_SMS,
	MSD_SAVE_SMS,
	MSD_REMOVE_SMS,
	MSD_MOVE_ALL_SMS_TO_SAVED,
	MSD_REMOVE_SAVED,
	MSD_REMOVE_ALL,
	MSD_READ_ALL,
} T_msd_request;


typedef struct {
	INT16 latitudeDelta;
	INT16 longitudeDelta;
} T_VehicleLocationDelta;

typedef struct {
	UINT8 automaticActivation:  1;
	UINT8 testCall:  1;
	UINT8 positionCanBeTrusted:  1;
} T_ControlType;

typedef struct {
	UINT8 recentVehicleLocationN1_presence:  1;
	UINT8 recentVehicleLocationN2_presence:  1;
	UINT8 numberOfPassangers_presence:  1;

	UINT8 messageIdentifier;
	T_ControlType controlType;

	UINT32 timestamp;
	T_VehicleLocation vehicleLocation;
	UINT8 vehicleDirection;

	T_VehicleLocationDelta recentVehicleLocationN1;
	T_VehicleLocationDelta recentVehicleLocationN2;
	UINT8 numberOfPassangers;
} T_MSDStructure;

typedef struct {
	union {
		UINT32 all;
		struct {
			UINT8 otherNotCriticalFailures_presence:  1;
			UINT8 otherCriticalFailures_presence:  1;
			UINT8 crashProfileMemoryOverflow_presence:  1;
			UINT8 eventsMemoryOverflow_presence:  1;
			UINT8 commModuleFailure_presence:  1;
			UINT8 gnssAntennaFailure_presence:  1;
			UINT8 raimProblem_presence:  1;
			UINT8 gnssReceiverFailure_presence:  1;
			UINT8 commModuleInterfaceFailure_presence:  1;
			UINT8 firmwareImageCorruption_presence:  1;
			UINT8 crashSensorFailure_presence:  1;
			UINT8 batteryVoltageLow_presence:  1;
			UINT8 batteryFailure_presence:  1;
			UINT8 statusIndicatorFailure_presence:  1;
			UINT8 uimFailure_presence:  1;
			UINT8 ignitionLineFailure_presence:  1;
			UINT8 speakersFailure_presence:  1;
			UINT8 leftSpeakerFailure_presence:  1;
			UINT8 rightSpeakerFailure_presence:  1;
			UINT8 micFailure_presence:  1;
			UINT8 micConnectionFailure_presence:  1;
		} single;
	} presence;

	union {
		UINT32 all;
		struct {
			UINT8 otherNotCriticalFailures:  1;
			UINT8 otherCriticalFailures:  1;
			UINT8 crashProfileMemoryOverflow:  1;
			UINT8 eventsMemoryOverflow:  1;
			UINT8 commModuleFailure:  1;
			UINT8 gnssAntennaFailure:  1;
			UINT8 raimProblem:  1;
			UINT8 gnssReceiverFailure:  1;
			UINT8 commModuleInterfaceFailure:  1;
			UINT8 firmwareImageCorruption:  1;
			UINT8 crashSensorFailure:  1;
			UINT8 batteryVoltageLow:  1;
			UINT8 batteryFailure:  1;
			UINT8 statusIndicatorFailure:  1;
			UINT8 uimFailure:  1;
			UINT8 ignitionLineFailure:  1;
			UINT8 speakersFailure:  1;
			UINT8 leftSpeakerFailure:  1;
			UINT8 rightSpeakerFailure:  1;
			UINT8 micFailure:  1;
			UINT8 micConnectionFailure:  1;
		} single;
	} flag;
} T_DiagnosticResult;

typedef struct {
	union {
		UINT8 all;
		struct {
			UINT8 crashAnotherType_presence:  1;
			UINT8 crashFrontOrSide_presence:  1;
			UINT8 crashSide_presence:  1;
			UINT8 crashRollover_presence:  1;
			UINT8 crashRear_presence:  1;
			UINT8 crashRight_presence:  1;
			UINT8 crashLeft_presence:  1;
			UINT8 crashFront_presence:  1;
		} single;
	} presence;

	union {
		UINT8 all;
		struct {
			UINT8 crashAnotherType:  1;
			UINT8 crashFrontOrSide:  1;
			UINT8 crashSide:  1;
			UINT8 crashRollover:  1;
			UINT8 crashRear:  1;
			UINT8 crashRight:  1;
			UINT8 crashLeft:  1;
			UINT8 crashFront:  1;
		} single;
	} flag;
} T_DiagnosticInfo;

typedef struct {
	UINT8 crashSeverityASI15_presence:  1;
	UINT8 diagnosticResult_presence:  1;
	UINT8 crashInfo_presence:  1;

	UINT16 crashSeverityASI15;
	T_DiagnosticResult diagnosticResult;
	T_DiagnosticInfo diagnosticInfo;
} T_AdditionalData;

typedef struct {
	UINT8 additionalData_presence:  1;
	T_MSDStructure msdStructure;
	T_AdditionalData additionalData;
} T_MSDMessage;

typedef enum {
	MSD_EMPTY = 0,
	MSD_INBAND,
	MSD_SMS,
	MSD_SAVED,
	MSD_UNDEFINED,
} T_MSDFlag;

typedef struct {
	T_MSDFlag msdFlag;
	UINT16 number;

	T_MSDMessage msdMessage;

	UINT8 checkSum;
} T_msd;

typedef struct {
	UINT8 automaticActivation:  1;
	UINT8 testCall:  1;
	UINT32 timestamp;
} T_msd_memorized;

typedef struct {
	UINT16 number;
	UINT8 index;
} T_msd_find;

typedef struct {
	T_MSDFlag msdFlag;
	UINT16 number;
} T_msd_mirror_cell;

typedef struct {
	T_msd_mirror_cell cell[MAX_NUMBER_MSD];
} T_msd_mirror;

void msd_init_variables(void);
T_msd msd_fill(T_MSDFlag flag, UINT8 automatic, UINT8 test, UINT8 id, UINT8 memorized, T_GnssData g_data);
UINT8 msd_check(T_msd * msd);
T_msd_counters msd_counter;
#endif /* HDR_MSD_H_ */
