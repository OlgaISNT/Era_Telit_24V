/**
 * @file asn1.c
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
#include "app_cfg.h"
#include "m2mb_os_api.h"
#include "msd.h"
#include "params.h"
#include "log.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static UINT8 asn1_byte_cnt;
static UINT8 asn1_bit_cnt;
/* Local function prototypes ====================================================================*/
static void asn1_add ( UINT8 num, UINT64 data, UINT8 * buffer );
/* Static functions =============================================================================*/
static void asn1_add ( UINT8 num, UINT64 data, UINT8 * buffer ) {
    if ( num > 32 || !num ) return;
    if ( buffer == NULL ) return;

    UINT8 left = 40 - asn1_bit_cnt - num;

    data <<= left;

    (*(buffer+asn1_byte_cnt)) = (*(buffer+asn1_byte_cnt)) | (UINT8)(data >> 32);
    (*(buffer+asn1_byte_cnt+1)) = (*(buffer+asn1_byte_cnt+1)) | (UINT8)(data >> 24);
    (*(buffer+asn1_byte_cnt+2)) = (*(buffer+asn1_byte_cnt+2)) | (UINT8)(data >> 16);
    (*(buffer+asn1_byte_cnt+3)) = (*(buffer+asn1_byte_cnt+3)) | (UINT8)(data >> 8);
    (*(buffer+asn1_byte_cnt+4)) = (*(buffer+asn1_byte_cnt+4)) | (UINT8)(data & 0x00000000000000FF) ;

    asn1_bit_cnt += num;
    asn1_byte_cnt += asn1_bit_cnt / 8;
    asn1_bit_cnt = asn1_bit_cnt % 8;
}

/* Global functions =============================================================================*/

void asn1_init_variables ( void ) {
    asn1_byte_cnt = 0;
    asn1_bit_cnt = 0;
}

/***************************************************************************/
/*  Function   : asn1_encode_msd                                           */
/*-------------------------------------------------------------------------*/
/*  Contents   : Encode MSD data set to ASN1                               */
/*  return     : 255 - no encode, other - ok                               */
/*-------------------------------------------------------------------------*/
/***************************************************************************/
UINT8 asn1_encode_msd ( T_msd * msd, UINT8 * buffer ) {
    memset(buffer, 0, MSD_LENGTH);
    asn1_byte_cnt = 0;
    asn1_bit_cnt = 0;

    // ID
    asn1_add( 8, (UINT64)MSD_FORMAT_VERSION, buffer);
    if ( MSD_FORMAT_VERSION == 2 ) {
        UINT64 len = 22 + 102 + 15 + 104;
        if ( msd->msdMessage.msdStructure.recentVehicleLocationN1_presence ) len = len + 20;
        if ( msd->msdMessage.msdStructure.recentVehicleLocationN2_presence ) len = len + 20;
        if ( msd->msdMessage.msdStructure.numberOfPassangers_presence ) len = len + 8;
        if ( msd->msdMessage.additionalData_presence ) {
            len = len + 32 + 8 + 4 + 21 + 3;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.micConnectionFailure_presence )      len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.micFailure_presence )                len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.rightSpeakerFailure_presence )       len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.leftSpeakerFailure_presence )        len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.speakersFailure_presence )           len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.ignitionLineFailure_presence )       len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.uimFailure_presence )                len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.statusIndicatorFailure_presence )    len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.batteryFailure_presence )            len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.batteryVoltageLow_presence )         len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.crashSensorFailure_presence )        len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.firmwareImageCorruption_presence )   len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.commModuleInterfaceFailure_presence )len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.gnssReceiverFailure_presence )       len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.raimProblem_presence )               len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.gnssAntennaFailure_presence )        len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.commModuleFailure_presence )         len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.eventsMemoryOverflow_presence )      len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.crashProfileMemoryOverflow_presence )len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.otherCriticalFailures_presence )     len++;
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.otherNotCriticalFailures_presence )  len++;
        }
        if (len%8) len+=8;
        len = len/8;
        asn1_add( 8, len, buffer);
    }
    asn1_add( 1, (UINT64)0, buffer);
    asn1_add( 1, (UINT64)msd->msdMessage.additionalData_presence, buffer);
    asn1_add( 1, (UINT64)0, buffer);
    asn1_add( 1, (UINT64)msd->msdMessage.msdStructure.recentVehicleLocationN1_presence, buffer);
    asn1_add( 1, (UINT64)msd->msdMessage.msdStructure.recentVehicleLocationN2_presence, buffer);
    asn1_add( 1, (UINT64)msd->msdMessage.msdStructure.numberOfPassangers_presence, buffer);
    asn1_add( 8, (UINT64)msd->msdMessage.msdStructure.messageIdentifier, buffer);
    asn1_add( 1, (UINT64)msd->msdMessage.msdStructure.controlType.automaticActivation, buffer);
    asn1_add( 1, (UINT64)msd->msdMessage.msdStructure.controlType.testCall, buffer);
    asn1_add( 1, (UINT64)msd->msdMessage.msdStructure.controlType.positionCanBeTrusted, buffer);
    asn1_add( 1, (UINT64)0, buffer);

    int val = get_vehicle_type();
//    if (val == 1){}                        // VEHICLE_TYPE = 1 передается как есть
//    if (val == 2){}                        // VEHICLE_TYPE = 2 передается как есть
//    if (val == 3) val = 4;  отстает на 1   // Чтобы в МНД передалось VEHICLE_TYPE = 3
//    if (val == 4) val = 6;  отстает на 2   // Чтобы в МНД передалось VEHICLE_TYPE = 4
//    if (val == 5) val = 8;  отстает на 3   // Чтобы в МНД передалось VEHICLE_TYPE = 5
//    if (val == 6) val = 10; отстает на 4   // Чтобы в МНД передалось VEHICLE_TYPE = 6
//    if (val == 7) val = 12; отстает на 5   // Чтобы в МНД передалось VEHICLE_TYPE = 7
//    и т.д.
    if (val > 2) val = val * 2 - 2; // закономерность
    asn1_add( 4, (UINT64) val >> 1, buffer); // DK: >> 1
    char vin[17 + 1];
    if (!get_vin(vin, sizeof(vin))) LOG_ERROR("gV");
    UINT8 temp;
    for ( UINT8 i=0; i<17; i++ ) {
        temp = (UINT8) *(vin + i);
        if ( (temp >= 0x30) && (temp <= 0x39) ) {
            asn1_add( 6, (UINT64)(temp-0x30) , buffer);
        } else if ( (temp >= 0x41) && (temp < 0x49) ) {
            asn1_add( 6, (UINT64)(temp-0x37) , buffer);
        } else if ( (temp > 0x49) && (temp < 0x4F) ) {
            asn1_add( 6, (UINT64)(temp-0x38) , buffer);
        } else if ( (temp > 0x4F) && (temp < 0x51) ) {
            asn1_add( 6, (UINT64)(temp-0x39) , buffer);
        } else if ( (temp > 0x51) && (temp <= 0x5A) ) {
            asn1_add( 6, (UINT64)(temp-0x3A) , buffer);
        } else return 255;
    }

    asn1_add( 1, (UINT64)0, buffer);

    //LOG_DEBUG(" get_propulsion_type(): %i", get_propulsion_type());
    UINT8 s = get_propulsion_type();
    UINT8 pt = ((s << 7) & 0x80) | ((s << 5) & 0x40) | ((s << 3) & 0x20) | ((s << 1) & 0x10) | ((s >> 1) & 0x08) | ((s >> 3) & 0x04);
    //LOG_DEBUG(" to_asn: %i", pt);

    if ( MSD_FORMAT_VERSION == 2 ) {
        asn1_add( 7, (UINT64)0x7F, buffer);
        asn1_add( 7, (UINT64)(pt >> 1), buffer);
    } else {
        asn1_add( 6, (UINT64)0x3F, buffer);
        asn1_add( 6, (UINT64)(pt >> 2), buffer);
    }
    LOG_INFO("asn_encode timestamp: %i", msd->msdMessage.msdStructure.timestamp);
    asn1_add( 32, (UINT64)msd->msdMessage.msdStructure.timestamp, buffer);
    asn1_add( 32, (UINT64)((msd->msdMessage.msdStructure.vehicleLocation.single.latitude ^ 0x80000000) & 0x00000000FFFFFFFF), buffer);
    asn1_add( 32, (UINT64)((msd->msdMessage.msdStructure.vehicleLocation.single.longitude ^ 0x80000000) & 0x00000000FFFFFFFF), buffer);
    asn1_add( 8, (UINT64)(msd->msdMessage.msdStructure.vehicleDirection), buffer);

    if ( msd->msdMessage.msdStructure.recentVehicleLocationN1_presence )
    {
        asn1_add( 10, (UINT64)((msd->msdMessage.msdStructure.recentVehicleLocationN1.latitudeDelta ^ 0x0200) & 0x00000000000003FF), buffer);
        asn1_add( 10, (UINT64)((msd->msdMessage.msdStructure.recentVehicleLocationN1.longitudeDelta ^ 0x0200) & 0x00000000000003FF), buffer);
    }

    if ( msd->msdMessage.msdStructure.recentVehicleLocationN2_presence )
    {
        asn1_add( 10, (UINT64)((msd->msdMessage.msdStructure.recentVehicleLocationN2.latitudeDelta ^ 0x0200) & 0x00000000000003FF), buffer);
        asn1_add( 10, (UINT64)((msd->msdMessage.msdStructure.recentVehicleLocationN2.longitudeDelta ^ 0x0200) & 0x00000000000003FF), buffer);
    }

    if ( msd->msdMessage.msdStructure.numberOfPassangers_presence )
    {
        asn1_add( 8, (UINT64)(msd->msdMessage.msdStructure.numberOfPassangers), buffer);
    }

    if ( msd->msdMessage.additionalData_presence )
    {
        // OID
        asn1_add( 8, (UINT64)3, buffer);
        asn1_add( 8, (UINT64)1, buffer);
        asn1_add( 8, (UINT64)4, buffer);
        asn1_add( 8, (UINT64)1, buffer);

        // octetLength
        UINT64 len = 4 + 21 + 3;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.micConnectionFailure_presence )      len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.micFailure_presence )                len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.rightSpeakerFailure_presence )       len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.leftSpeakerFailure_presence )        len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.speakersFailure_presence )           len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.ignitionLineFailure_presence )       len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.uimFailure_presence )                len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.statusIndicatorFailure_presence )    len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.batteryFailure_presence )            len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.batteryVoltageLow_presence )         len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.crashSensorFailure_presence )        len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.firmwareImageCorruption_presence )   len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.commModuleInterfaceFailure_presence )len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.gnssReceiverFailure_presence )       len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.raimProblem_presence )               len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.gnssAntennaFailure_presence )        len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.commModuleFailure_presence )         len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.eventsMemoryOverflow_presence )      len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.crashProfileMemoryOverflow_presence )len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.otherCriticalFailures_presence )     len++;
        if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.otherNotCriticalFailures_presence )  len++;

        if (len%8) len+=8;
        len = len/8;
        asn1_add( 8, len, buffer);

        asn1_add( 1, (UINT64)0, buffer);
        asn1_add( 1, (UINT64)msd->msdMessage.additionalData.crashSeverityASI15_presence, buffer);
        asn1_add( 1, (UINT64)msd->msdMessage.additionalData.diagnosticResult_presence, buffer);
        asn1_add( 1, (UINT64)msd->msdMessage.additionalData.crashInfo_presence, buffer);

        if ( msd->msdMessage.additionalData.crashSeverityASI15_presence )
        {

        }

        if ( msd->msdMessage.additionalData.diagnosticResult_presence ) {
            asn1_add( 21, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.presence.all), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.micConnectionFailure_presence )        asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.micConnectionFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.micFailure_presence )                  asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.micFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.rightSpeakerFailure_presence )         asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.rightSpeakerFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.leftSpeakerFailure_presence )          asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.leftSpeakerFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.speakersFailure_presence )             asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.speakersFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.ignitionLineFailure_presence )         asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.ignitionLineFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.uimFailure_presence )                  asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.uimFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.statusIndicatorFailure_presence )      asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.statusIndicatorFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.batteryFailure_presence )              asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.batteryFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.batteryVoltageLow_presence )           asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.batteryVoltageLow), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.crashSensorFailure_presence )          asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.crashSensorFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.firmwareImageCorruption_presence )     asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.firmwareImageCorruption), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.commModuleInterfaceFailure_presence )  asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.commModuleInterfaceFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.gnssReceiverFailure_presence )         asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.gnssReceiverFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.raimProblem_presence )                 asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.raimProblem), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.gnssAntennaFailure_presence )          asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.gnssAntennaFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.commModuleFailure_presence )           asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.commModuleFailure), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.eventsMemoryOverflow_presence )        asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.eventsMemoryOverflow), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.crashProfileMemoryOverflow_presence )  asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.crashProfileMemoryOverflow), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.otherCriticalFailures_presence )       asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.otherCriticalFailures), buffer);
            if ( msd->msdMessage.additionalData.diagnosticResult.presence.single.otherNotCriticalFailures_presence )    asn1_add( 1, (UINT64)(msd->msdMessage.additionalData.diagnosticResult.flag.single.otherNotCriticalFailures), buffer);
        }

        if ( msd->msdMessage.additionalData.crashInfo_presence )
        {

        }

        asn1_add( 1, (UINT64)0, buffer);
        asn1_add( 2, (UINT64)1, buffer);
    }

    return asn1_byte_cnt+1;
}
