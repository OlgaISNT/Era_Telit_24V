/*
 * gnss.c
 * Created on: 28.03.2022, Author: Roberta Galeazzo, DKozenkov
 */

///* Include files ================================================================================*/
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include <time.h>
//#include <math.h>
//
//#include "m2mb_types.h"
//#include "m2mb_os_api.h"
//#include "m2mb_os.h"
//#include "m2mb_os_sem.h"
//#include "m2mb_gnss.h"
//#include "m2mb_info.h"
//
//#include "azx_utils.h"
//#include "azx_tasks.h"
//#include "log.h"
//#include "tasks.h"
//#include "u_time.h"
//#include "app_cfg.h"
//
//#include "../utils/hdr/char_service.h"
///* Local defines ================================================================================*/
//
//
//#ifndef M2MB_GNSS_WWAN_GNSS_PRIORITY_E
//  #define GNSS_MEX10G1 0
//#else
//  #define GNSS_MEX10G1 1
//#endif
///* Local typedefs ===============================================================================*/
//typedef enum {
//	MEX10C1,
//	MEX10G1,
//	LE910CX,
//	LE910CX_X,
//	NONE
//} MODULE_TYPE_E;
//
///* Local statics ================================================================================*/
//void *userdata = NULL;
//
//M2MB_GNSS_HANDLE handle1;
//UINT8 priority;
//UINT32 TBF;
//UINT8 constellation;
//
//const CHAR *gnssServ[] = {"POSITION", "NMEA sentences"};
//
//M2MB_GNSS_SERVICE_E gnss_service;
//MODULE_TYPE_E moduleType;
//static M2MB_OS_EV_HANDLE gnss_evHandle = NULL;
//static UINT32 sentenceNum = 0;
//
//static M2MB_OS_EV_ATTR_HANDLE  evAttrHandle;
//static UINT32                  curEvBits;
///* Local function prototypes ====================================================================*/
//void gnss_callback( M2MB_GNSS_HANDLE handle, M2MB_GNSS_IND_E event, UINT16 resp_size, void *resp, void *userdata);
//static MODULE_TYPE_E getModuleType(void);
//static int start_gnss(void);
//static int stop_gnss(void);
//static UINT32 getUtcFromString(char *str);
//static INT32 getLat(char *str);
//static INT32 getLon(char *str);
//static double degreeToMilSec(double degrees, double minutes);
///* Static functions =============================================================================*/
//
//double degreeToMilSec(double degrees, double minutes){
//    // Конвертация из представления широты/долготы градусы, минуты, доли минут в градусы и доли градусов
//    double result = degrees + (minutes / 60);
//    // Переводим градусы в секунды
//    // 1 угловой градус - 3600 угловых секунд или (3600 * 1000) милисекунд
//    result = result * 3600 * 1000;
//    return result;
//}
//
//
////5332.2471N
////ddmm.mmmm N/S
////N/S => +/-
//static INT32 getLat(char *str){
//    if (strlen(str) != 10){
//        // Некорректная строка
//        return 0;
//    }
//    // Получаем градусы
//    char *y;
//    char *content = substr(str, 0,2);
//    double degrees = strtod(content, &y);
//    clearString(content);
//
//    // Получаем минуты
//    content = substr(str,2,9);
//    char *x;
//    double minutes = strtod(content,&x);
//    clearString(content);
//    double result = degreeToMilSec(degrees, minutes);
//    if(str[9] == 'S'){
//        result *= (-1);
//    }
//    return result;
//}
//
////04918.9815E
////dddmm.mmmm E/W
////E/W => +/-
//static INT32 getLon(char *str){
//    if (strlen(str) != 11){
//        // Некорректная строка
//        return 0;
//    }
//
//    // Получаем градусы
//    char *y;
//    char *content = substr(str, 0,3);
//    double degrees = strtod(content, &y);
//    clearString(content);
//
//    // Получаем минуты
//    content = substr(str,3,10);
//    char *x;
//    double minutes = strtod(content,&x);
//    clearString(content);
//
//    double result = degreeToMilSec(degrees, minutes);
//
//    if(str[10] == 'W'){
//        result *= (-1);
//    }
//    return result;
//}
//
////      hhmmss.sssddmmyy
////<UTC>[082055.000070422]
//static UINT32 getUtcFromString(char *str){
//    if (strlen(str) != 16){
//        // Неверная длина строки
//        return 0;
//    }
//    struct tm m_time;
//    char *subst;
//
//    // int часы (отсчет с 0);
//    subst = substr(str, 0,2);
//    m_time.tm_hour = atoi(subst);
//    clearString(subst);
//
//    // int минуты (отсчет с 0);
//    subst = substr(str, 2,4);
//    m_time.tm_min = atoi(subst);
//    clearString(subst);
//
//    // int секунды (отсчет с 0);
//    subst = substr(str, 4,6);
//    m_time.tm_sec = atoi(subst);
//    clearString(subst);
//
//    // int день месяца (отсчет с 1);
//    subst = substr(str, 10,12);
//    m_time.tm_mday = atoi(subst) + 1;
//    clearString(subst);
//
//    // int месяц (отсчет с 0);
//    subst = substr(str, 12,14);
//    m_time.tm_mon = atoi(subst) - 1;
//    clearString(subst);
//
//    // int год (за начала отсчета принят 1900 год);
//    char* z = substr(str, 14,16);
//    subst = strconc("20",z);
//    m_time.tm_year = atoi(subst) - 1900;
//    clearString(subst);
//    clearString(z);
//
//    m_time.tm_wday = 0;   // день недели (воскресенье - 0);
//    m_time.tm_yday = 0;   // день в году (отсчет с 0);
//    m_time.tm_isdst = -1; // признак "летнее время"
//    return mktime (&m_time);
//}
//
//
//static int start_gnss(void) {
//	M2MB_OS_RESULT_E        osRes;
//
//	osRes = m2mb_os_ev_setAttrItem(&evAttrHandle,
//			CMDS_ARGS(
//					M2MB_OS_EV_SEL_CMD_CREATE_ATTR,	NULL,
//					M2MB_OS_EV_SEL_CMD_NAME, "gnss_ev")
//			);
//	if (osRes != M2MB_OS_SUCCESS) {
//		LOG_CRITICAL("gnss_SAI");
//		return -1;
//	}
//	osRes = m2mb_os_ev_init( &gnss_evHandle, &evAttrHandle );
//	if (osRes != M2MB_OS_SUCCESS) {
//		m2mb_os_ev_setAttrItem(&evAttrHandle,
//				M2MB_OS_EV_SEL_CMD_DEL_ATTR, NULL);
//		LOG_CRITICAL("m2mb_os_ev_init failed!");
//		return -1;
//	} else {
//		LOG_TRACE("m2mb_os_ev_init success");
//	}
//
//	if (M2MB_RESULT_SUCCESS != m2mb_gnss_init(&handle1, gnss_callback, userdata)) {
//		LOG_ERROR("m2mb_gnss_enable NMEA REPORT, failed!");
//		return -1;
//	}
//	moduleType = getModuleType();
//
//		/*this part is available ONLY for MEX10G1 products*/
//#if GNSS_MEX10G1
//		LOG_WARN("MEX10G1");
//		if(moduleType == MEX10G1) {
//			m2mb_gnss_getcfg(handle1, M2MB_GNSS_PRIORITY, &priority);
//
//			LOG_INFO("Priority: %d", priority);
//
//			m2mb_gnss_getcfg(handle1, M2MB_GNSS_TBF, &TBF);
//			LOG_INFO("TBF: %d", TBF);
//			m2mb_gnss_getcfg(handle1, M2MB_GNSS_CONSTELLATION, &constellation);
//			LOG_INFO("constellation: %d", constellation);
//
//			/*To start getting GPS position priority MUST be set to GNSS */
//			res = m2mb_gnss_set_prio_runtime(handle1, GNSS_PRIORITY);
//			if(res != M2MB_RESULT_SUCCESS){
//				LOG_ERROR("Can't change the priority to GNSS_PRIORITY!");
//			} else {
//				LOG_INFO("Priority changed to GNSS_PRIORITY, start GPS");
//			}
//
//		}
//#endif
//		/*
//		 * on MEX10G1 both M2MB_GNSS_SERVICE_NMEA_REPORT and M2MB_GNSS_SERVICE_POSITION_REPORT services are available, while
//		 * on ME910C1 product family only M2MB_GNSS_SERVICE_POSITION_REPORT is available
//		 */
//		gnss_service = M2MB_GNSS_SERVICE_POSITION_REPORT;
//		if (M2MB_RESULT_SUCCESS != m2mb_gnss_enable( handle1, gnss_service)) {
//			LOG_ERROR("m2mb_gnss_enable %s REPORT, failed!", gnssServ[gnss_service]);
//			return -1;
//		}
//
//		LOG_INFO("m2mb_gnss_enable, %s OK", gnssServ[gnss_service]);
//
//		if( M2MB_RESULT_SUCCESS != m2mb_gnss_start( handle1 ) )
//		{
//			LOG_ERROR("Failed to start GPS");
//			return -1;
//		}
//		LOG_INFO("m2mb_gnss_start OK, waiting for position/nmea sentences...");
//		/*Wait for GPS fix/NMEA sentences event to occur (released in gnssCallbackFN function) */
//		m2mb_os_ev_get(gnss_evHandle, GNSS_BIT, M2MB_OS_EV_GET_ANY_AND_CLEAR, &curEvBits, M2MB_OS_WAIT_FOREVER); // M2MB_OS_MS2TICKS(600*1000)
//
//		LOG_INFO("***** Wait 120 seconds and then stop GPS *****");
//
//		azx_sleep_ms(120000);
//
//		send_to_gnss_task(STOP_GNSS, 0, 0);
//
//	return M2MB_OS_SUCCESS;
//}
//
//static int stop_gnss(void) {
//	if (M2MB_RESULT_SUCCESS != m2mb_gnss_stop(handle1) ) {
//		LOG_ERROR("GNSS stop fail");
//		return -1;
//	}
//	LOG_INFO("GNSS stop OK");
//	if( M2MB_RESULT_SUCCESS != m2mb_gnss_disable(handle1, gnss_service)) {
//		LOG_ERROR("GNSS disable fail");
//		return -1;
//	}
//	LOG_INFO("GNSS service was disable OK");
//
//	#if GNSS_MEX10G1
//				/*Restore priority to WWAN */
//				if(moduleType == MEX10G1){
//					res = m2mb_gnss_set_prio_runtime(handle1, WWAN_PRIORITY);
//					if(res != M2MB_RESULT_SUCCESS){
//						LOG_ERROR("Can't change the priority to WWAN_PRIORITY!");
//					} else {
//						LOG_INFO("Priority changed to WWAN_PRIORITY, stop GPS");
//					}
//				}
//	#endif
//	if (M2MB_RESULT_SUCCESS != m2mb_gnss_deinit(handle1 )) {
//		LOG_ERROR("GNSS deinit fail");
//		return -1;
//	}
//	LOG_INFO("GNSS deinit OK");
//	return M2MB_OS_SUCCESS;
//}
//
//MODULE_TYPE_E getModuleType(void) {
//	M2MB_RESULT_E res;
//	M2MB_INFO_HANDLE hInfo;
//	CHAR *info;
//	MODULE_TYPE_E moduleType = NONE;
//
//	res = m2mb_info_init(&hInfo);
//	if (res != M2MB_RESULT_SUCCESS)
//	{
//		 LOG_ERROR("Impossible init info");
//	}
//    else
//	{
//		res = m2mb_info_get(hInfo, M2MB_INFO_GET_MODEL, &info);
//
//		if (res != M2MB_RESULT_SUCCESS)
//		{
//			LOG_ERROR("Impossible to get model");
//		}
//		else
//		{
//			//LOG_INFO("Model: %s", info);
//			if (strstr(info, "G1") != NULL)
//			{
//				LOG_TRACE("Type: %d", MEX10G1);
//				moduleType = MEX10G1;
//			}
//			else if (info[0] == 'M' && strstr(info, "C1"))
//			{
//				LOG_TRACE("type: %d", MEX10C1);
//				moduleType = MEX10C1;
//			}
//			else if ( strstr(info, "LE910C"))
//			{
//				if (info[strlen(info) -1 ] == 'X')
//				{
//					LOG_TRACE("type: %d", LE910CX_X);
//					moduleType = LE910CX_X;
//				}
//				else
//				{
//					LOG_TRACE("type: %d", LE910CX);
//					moduleType = LE910CX;
//				}
//			}
//		}
//		m2mb_info_deinit(hInfo);
//	}
//	return moduleType;
//}
//
///* Global functions =============================================================================*/
//
//void printGnssInfo_test(M2MB_GNSS_POSITION_REPORT_INFO_T *locData )
//{
//  locData->speed.speed = sqrt( pow( locData->speed.speed_horizontal,2 ) + pow( locData->speed.speed_vertical,2 ) );
//
//  LOG_INFO("latitude_valid: %d - latitude: %f", locData->latitude_valid, locData->latitude);
//
//  LOG_INFO("longitude_valid: %d - longitude: %f", locData->longitude_valid, locData->longitude );
//
//  LOG_INFO("altitude_valid: %d - altitude: %f", locData->altitude_valid, locData->altitude );
//
//  LOG_INFO("uncertainty_valid: %d - uncertainty: %f", locData->uncertainty_valid, locData->uncertainty );
//
//  LOG_INFO("velocity_valid: %d - ", locData->velocity_valid );
//  LOG_INFO("codingType: %d", locData->velocity.codingType );
//  LOG_INFO("speed_horizontal: %f", locData->velocity.speed_horizontal );
//  LOG_INFO("bearing: %f", locData->velocity.bearing );
//
//  LOG_INFO("timestamp_valid: %d - timestamp: %llu", locData->timestamp_valid, locData->timestamp ); // milliseconds since Jan. 1, 1970
//
//  if (locData->timestamp_valid != 0) updateRTC_ms(locData->timestamp, 1);
//
//  LOG_INFO("speed_valid: %d - speed: %f", locData->speed_valid, locData->speed.speed );
//
//  return;
//}
//
//
///*-----------------------------------------------------------------------------------------------*/
//
//void gnss_callback( M2MB_GNSS_HANDLE handle, M2MB_GNSS_IND_E event, UINT16 resp_size, void *resp, void *userdata )
//{
//  (void)handle;
//  (void)resp_size;
//  (void)userdata;
//  LOG_DEBUG("gnssCallback[%d]", sentenceNum);
//  switch (event){
//
//	  case M2MB_GNSS_INDICATION_POSITION_REPORT: {
//		  	  printGnssInfo_test( (M2MB_GNSS_POSITION_REPORT_INFO_T *)resp );
//		  	  m2mb_os_ev_set(gnss_evHandle,GNSS_BIT, M2MB_OS_EV_SET);
//	  	  }
//	  	  break;
//#if GNSS_MEX10G1
//	  case M2MB_GNSS_INDICATION_NMEA_REPORT:
//	  {
//		  LOG_INFO("NMEA: %s", (CHAR*)resp);
//		  m2mb_os_ev_set(gnss_evHandle,GNSS_BIT, M2MB_OS_EV_SET);
//	  }
//	  break;
//#endif
//	  default:
//		  LOG_WARN("unexpected event");
//		  break;
//  }
//  sentenceNum++;
//}
//
///*-----------------------------------------------------------------------------------------------*/
///***************************************************************************************************
//   GNSS_task handles GNSS init, configuration, start and stop
// **************************************************************************************************/
//INT32 GNSS_task(INT32 type, INT32 param1, INT32 param2) {
//	(void)param1;
//	(void)param2;
//	LOG_INFO("GNSS command: %i", type);
//	switch(type) {
//		case START_GNSS:	return start_gnss();
//		case STOP_GNSS:		return stop_gnss();
//		default:			return M2MB_OS_SUCCESS;
//	}
//}
//
//gnss_data parse_gnss_mess(char *message) {
//    if (message == NULL || strcmp(message, "ERROR") == 0){
//        LOG_INFO("GNSS message is not valid");
//        goto down;
//    }
//    message = validateBeforeSplit(message, ',',"0");
//    gnss_data gnssData;
//    char **tokens;
//    char fx;
//    tokens = str_split(message, ',');
//    if (tokens) {
//        char *dateTime;
//        int i;
//        for (i = 0; *(tokens + i); i++) {
//            switch (i) {
//                case 0:  // <UTC> part1
//                    dateTime = malloc(10+1);
//                    strcpy(dateTime, *(tokens + i));
//                    break;
//                case 1:  // <latitude>
//                    gnssData.latitude = getLat(*(tokens + i));
//                    break;
//                case 2:  // <longitude>
//                    gnssData.longitude = getLon(*(tokens + i));
//                    break;
//                case 3:  // <hdop>
//                    gnssData.hdop = atof(*(tokens + i));
//                    break;
//                case 4:  // <altitude>
//                    gnssData.altitude = atof(*(tokens + i));
//                    break;
//                case 5:  // <fix>
//                    fx = *(tokens + i)[0];
//                    if(fx == '0' || fx == '1' || fx == '2' || fx == '3'){
//                        gnssData.fix = fx;
//                    } else{
//                        gnssData.fix = '0';
//                    }
//                    break;
//                case 6:  // <cog>
//                    gnssData.course = atof(*(tokens + i));
//                    break;
//                case 7:  // <spkm>
//                    gnssData.speed = atoi(*(tokens + i));
//                    break;
//                case 8:  // <spkn> not use
//                    break;
//                case 9:  // <date> part2
//                    dateTime = strconc(dateTime,*(tokens + i));
//                    gnssData.utc = getUtcFromString(dateTime);
//                    free(dateTime);
//                    break;
//                case 10: // <nsat_gps>
//                    gnssData.gps_sat = atoi(*(tokens + i));
//                    break;
//                case 11: // <nsat_glonass>
//                    gnssData.glonass_sat = atoi(*(tokens + i));
//                    break;
//                default:
//                    break;
//            }
//            free(*(tokens + i));
//        }
//        if (i != 12){
//            goto down;
//        }
//        free(tokens);
//        return gnssData;
//    } else{
//        down:
//        gnssData.utc = 0;
//        gnssData.latitude = 0;
//        gnssData.longitude = 0;
//        gnssData.fix = '0';
//        gnssData.course = 0;
//        gnssData.speed = 0;
//        gnssData.hdop = 0;
//        gnssData.glonass_sat = 0;
//        gnssData.gps_sat = 0;
//        gnssData.altitude = 0;
//        return gnssData;
//    }
//}
