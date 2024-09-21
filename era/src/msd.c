/**
 * @file msd.c
 * @version 1.0.0
 * @date 17.04.2022
 * @author: DKozenkov from RMerdeev
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_sys.h"
#include "failures.h"
#include "m2mb_os_api.h"
#include "msd.h"

#include <codec.h>

#include "u_time.h"
#include "../../hal/hdr/gnss_rec.h"
#include "auxiliary.h"
#include "params.h"
#include "log.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static UINT8 msd_once;
T_msd_mirror msd_mirror;
/* Local function prototypes ====================================================================*/
//static UINT8 msd_check ( T_msd * msd );

/* Static functions =============================================================================*/


/* Global functions =============================================================================*/
T_msd_counters msd_counter;
T_msd_memorized msd_memorized;
T_msd msd_test;

UINT8 msd_check( T_msd *msd ) {
    if ( msd == NULL ) return 0;
    static T_msd msd_temp;
    //print_hex("msd", (char *) msd, sizeof(T_msd));
    memcpy( &msd_temp, msd, sizeof(T_msd) );
    UINT8 cs = msd_temp.checkSum;
    msd_temp.checkSum = 0;
    //print_hex("msd_temp", (char *) &msd_temp, sizeof(T_msd));
    if ( auxiliary_crc8((UINT8 *)(&msd_temp), sizeof(T_msd)) != cs) return 0;
    return 1;
}

/***************************************************************************/
/*  Function   : msd_init_variables                                        */
/*-------------------------------------------------------------------------*/
/*  Contents   : Init variables                                            */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
void msd_init_variables (void) {
    memset( &msd_counter, 0, sizeof(T_msd_counters) );
    memset( &msd_memorized, 0, sizeof(T_msd_memorized) );
    memset( &msd_test, 0, sizeof(T_msd) );
    memset( &msd_mirror, 0, sizeof(T_msd_mirror) );
    msd_once = 0;
}


/***************************************************************************/
/*  Function   : msd_fill                                                  */
/*-------------------------------------------------------------------------*/
/*  Contents   : Fill msd data set                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
/***************************************************************************/

T_msd msd_fill ( T_MSDFlag flag, UINT8 automatic, UINT8 test, UINT8 id, UINT8 memorized, T_GnssData g_data) {
    LOG_INFO("msd_fill: flag:%i, auto:%i, id:%i, g_data_fix:%i, timestamp:%i", flag, automatic, id, g_data.fix, g_data.utc);
	T_msd msd;
    msd.msdMessage.msdStructure.recentVehicleLocationN1_presence = 1;
    msd.msdMessage.msdStructure.recentVehicleLocationN2_presence = 1;
    msd.msdMessage.msdStructure.numberOfPassangers_presence = 0;
    msd.msdMessage.msdStructure.messageIdentifier = id;

    if ( memorized ) {
        //LOG_INFO("___memorized");
        msd.msdMessage.msdStructure.controlType.automaticActivation = ((msd_memorized.automaticActivation & 0x01) > 0);
        msd.msdMessage.msdStructure.controlType.testCall = ((msd_memorized.testCall & 0x01) > 0);
        msd.msdMessage.msdStructure.timestamp = msd_memorized.timestamp;
    } else {
        //LOG_INFO("___not memorized");
        msd.msdMessage.msdStructure.controlType.automaticActivation = ((automatic & 0x01) > 0);
        msd.msdMessage.msdStructure.controlType.testCall = ((test & 0x01) > 0); // 0x03
        msd.msdMessage.msdStructure.timestamp = g_data.utc;
    }

    msd.msdMessage.msdStructure.controlType.positionCanBeTrusted = g_data.fix == 0 ? 0 : 1;// navi_reliability;
    if (test) {
        msd.msdMessage.msdStructure.vehicleLocation.single.latitude = g_data.fix == 0 ? UNKNOWN_COORDINATE : g_data.latitude;
        msd.msdMessage.msdStructure.vehicleLocation.single.longitude = g_data.fix == 0 ? UNKNOWN_COORDINATE : g_data.longitude;
    } else {
        msd.msdMessage.msdStructure.vehicleLocation.single.latitude = g_data.fix == 0 ? get_m_last_latitude() : g_data.latitude;// navi_recent_pos[0].single.latitude;
        msd.msdMessage.msdStructure.vehicleLocation.single.longitude = g_data.fix == 0 ? get_m_last_longitude() : g_data.longitude;// navi_recent_pos[0].single.longitude;
    }

//	LOG_WARN("gdata_fix:%i", g_data.fix);
//	LOG_WARN("m_last_lat:%i, m_last_lon%i", get_m_last_latitude(), get_m_last_longitude());
//	LOG_WARN("m_cur_lat:%i, m_cur_lon%i", g_data.latitude, g_data.longitude);
//	LOG_WARN("msd_lat:%i, msd_lon%i", msd.msdMessage.msdStructure.vehicleLocation.single.latitude, msd.msdMessage.msdStructure.vehicleLocation.single.longitude);

//	terminal_add_byte('L'); terminal_add_byte('A');terminal_add_byte('T');terminal_add_byte('=');
//	terminal_add_char((msd.msdMessage.msdStructure.vehicleLocation.single.latitude>>24)&0xFF);
//	terminal_add_char((msd.msdMessage.msdStructure.vehicleLocation.single.latitude>>16)&0xFF);
//	terminal_add_char((msd.msdMessage.msdStructure.vehicleLocation.single.latitude>>8)&0xFF);
//	terminal_add_char((msd.msdMessage.msdStructure.vehicleLocation.single.latitude>>0)&0xFF);
//	terminal_add_byte('\n');
//
//	terminal_add_byte('L'); terminal_add_byte('O');terminal_add_byte('N');terminal_add_byte('=');
//	terminal_add_char((msd.msdMessage.msdStructure.vehicleLocation.single.longitude>>24)&0xFF);
//	terminal_add_char((msd.msdMessage.msdStructure.vehicleLocation.single.longitude>>16)&0xFF);
//	terminal_add_char((msd.msdMessage.msdStructure.vehicleLocation.single.longitude>>8)&0xFF);
//	terminal_add_char((msd.msdMessage.msdStructure.vehicleLocation.single.longitude>>0)&0xFF);
//	terminal_add_byte('\n');

    msd.msdMessage.msdStructure.vehicleDirection = g_data.fix == 0 || g_data.speed == 0 ? 0xFF : (UINT8)(g_data.course / 2); // navi_direction

    msd.msdMessage.msdStructure.recentVehicleLocationN1.latitudeDelta = 0;
    msd.msdMessage.msdStructure.recentVehicleLocationN1.longitudeDelta = 0;
    msd.msdMessage.msdStructure.recentVehicleLocationN2.latitudeDelta = 0;
    msd.msdMessage.msdStructure.recentVehicleLocationN2.longitudeDelta = 0;

    INT32 lat_0 = g_data.latitude;							//LOG_DEBUG("lat_0: %i", lat_0);
    INT32 lon_0 = g_data.longitude;							//LOG_DEBUG("lon_0: %i", lon_0);
    if ( (lat_0 != 0) && (lon_0 != 0) ) {
        INT32 lat_1 = g_data.rec_locations[0].latitude; 	//LOG_DEBUG("lat_1: %i", lat_1);
        INT32 lon_1 = g_data.rec_locations[0].longitude;	//LOG_DEBUG("lon_1: %i", lon_1);
        if ( (lat_1 != 0) && (lon_1 != 0) ) {
            msd.msdMessage.msdStructure.recentVehicleLocationN1.latitudeDelta = (INT16)  ((lat_0 - lat_1) / 100);
            msd.msdMessage.msdStructure.recentVehicleLocationN1.longitudeDelta = (INT16) ((lon_0 - lon_1) / 100);
            INT32 lat_2 = g_data.rec_locations[1].latitude; 	//LOG_DEBUG("lat_2: %i", lat_2);
            INT32 lon_2 = g_data.rec_locations[1].longitude;	//LOG_DEBUG("lon_2: %i", lon_2);
            if ( (lat_2 != 0) && (lon_2 != 0) ) {
                msd.msdMessage.msdStructure.recentVehicleLocationN2.latitudeDelta = (INT16)  ((lat_1 - lat_2) / 100);
                msd.msdMessage.msdStructure.recentVehicleLocationN2.longitudeDelta = (INT16) ((lon_1 - lon_2) / 100);
            }
        }
    }

    if ( test || (memorized && msd_memorized.testCall) ) {
        //LOG_DEBUG("____ additionalData_presence");
        msd.msdMessage.additionalData_presence = 1;

        msd.msdMessage.additionalData.crashSeverityASI15_presence = 0;
        msd.msdMessage.additionalData.diagnosticResult_presence = 1;
        msd.msdMessage.additionalData.crashInfo_presence = 0;

        msd.msdMessage.additionalData.diagnosticResult.presence.all = 0;
        //msd.msdMessage.additionalData.diagnosticResult.presence.single.micConnectionFailure_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.micFailure_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.speakersFailure_presence = 1;
        //msd.msdMessage.additionalData.diagnosticResult.presence.single.ignitionLineFailure_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.uimFailure_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.batteryFailure_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.batteryVoltageLow_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.crashSensorFailure_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.firmwareImageCorruption_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.gnssReceiverFailure_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.raimProblem_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.gnssAntennaFailure_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.otherCriticalFailures_presence = 1;
        msd.msdMessage.additionalData.diagnosticResult.presence.single.eventsMemoryOverflow_presence = 1;

        msd.msdMessage.additionalData.diagnosticResult.flag.all = 0;
        //msd.msdMessage.additionalData.diagnosticResult.flag.single.micConnectionFailure = get_microphone_failure() != F_INACTIVE;// | setup.beb.single.md_test_error;
        msd.msdMessage.additionalData.diagnosticResult.flag.single.micFailure = get_microphone_failure() != F_INACTIVE;
        msd.msdMessage.additionalData.diagnosticResult.flag.single.speakersFailure = get_speaker_failure() != F_INACTIVE;// | setup.beb.single.md_test_error;
        msd.msdMessage.additionalData.diagnosticResult.flag.single.uimFailure = get_sos_break_failure() != F_INACTIVE || get_uim_failure() != F_INACTIVE;
        msd.msdMessage.additionalData.diagnosticResult.flag.single.batteryFailure = get_battery_failure() != F_INACTIVE || get_battery_low_voltage_failure() != F_INACTIVE;
        msd.msdMessage.additionalData.diagnosticResult.flag.single.batteryVoltageLow = get_battery_low_voltage_failure() != F_INACTIVE;
        msd.msdMessage.additionalData.diagnosticResult.flag.single.crashSensorFailure = (get_crash_sensor_failure() != F_INACTIVE) | (get_airbags_failure() != F_INACTIVE);
        msd.msdMessage.additionalData.diagnosticResult.flag.single.firmwareImageCorruption = get_audio_files_failure() != F_INACTIVE;
        msd.msdMessage.additionalData.diagnosticResult.flag.single.gnssReceiverFailure = get_gnss_failure() != F_INACTIVE;
        msd.msdMessage.additionalData.diagnosticResult.flag.single.raimProblem = g_data.fix == 0;// !navi_reliability;
        msd.msdMessage.additionalData.diagnosticResult.flag.single.gnssAntennaFailure = g_data.fix == 0;// !navi_reliability;
        msd.msdMessage.additionalData.diagnosticResult.flag.single.otherCriticalFailures = get_other_critical_failure() != F_INACTIVE;
        if ((msd_counter.inband + msd_counter.sms + msd_counter.saved) >= 100) msd.msdMessage.additionalData.diagnosticResult.flag.single.eventsMemoryOverflow = 0;
    } else {
        msd.msdMessage.additionalData_presence = 0;
        //LOG_DEBUG("____ not additionalData_presence");
    }

    msd.number = ++msd_counter.last_number;
    msd.msdFlag = flag;

    msd.checkSum = 0;
    msd.checkSum = auxiliary_crc8((UINT8*)(&msd), sizeof(T_msd));
    return msd;
}
