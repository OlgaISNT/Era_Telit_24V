/*
 * gnss.c
 * Created on: 21.10.2022, Author: DKozenkov
 * Реализация с помощью API, без использования АТ-команд
 * Ограничения:
 * - нет информации о кол-ве спутников
 * - интервал обновления данных 3 сек. и его нельзя изменить
 */

///* Include files ================================================================================*/
//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include "m2mb_types.h"
//#include "m2mb_os_api.h"
//#include "log.h"
//#include "failures.h"
//#include "at_command.h"
//#include "atc_era.h"
//#include "tasks.h"
//#include "app_cfg.h"
//#include "azx_timer.h"
//#include "u_time.h"
//#include "sys_param.h"
//#include "azx_ati.h"
//#include "m2mb_os.h"
//#include "m2mb_os_types.h"
//#include "m2mb_info.h"
//#include <math.h>
//#include "m2mb_gnss.h"
//#include "gnss_rec.h"
//
///* Local defines ================================================================================*/
///* Local typedefs ===============================================================================*/
///* Local statics ================================================================================*/
//typedef enum {
//	MEX10C1,
//	MEX10G1,
//	LE910CX,
//	LE910CX_X,
//	NONE
//} MODULE_TYPE_E;
//static MODULE_TYPE_E moduleType;
//static M2MB_OS_EV_ATTR_HANDLE  evAttrHandle;
//static UINT32                  curEvBits;
//static M2MB_OS_EV_HANDLE gnss_evHandle = NULL;
//#define GNSS_BIT 1
//void *userdata = NULL;
//
//M2MB_GNSS_HANDLE handle1;
//UINT8 priority;
//UINT32 TBF;
//UINT8 constellation;
//static const CHAR *gnssServ[] = {"POSITION", "NMEA sentences"};
//static M2MB_GNSS_SERVICE_E gnss_service;
//#ifndef M2MB_GNSS_WWAN_GNSS_PRIORITY_E
//  #define GNSS_MEX10G1 0
//#else
//  #define GNSS_MEX10G1 1
//#endif
//
//#define TIMEOUT_GNSS 6000
//static T_GnssData g_data;
//static M2MB_OS_MTX_ATTR_HANDLE mtx_attr_handle;
//static M2MB_OS_MTX_HANDLE      mtx_handle = NULL;
//// Значения ниже обновляются сразу же при получении очередных достоверных геоданных:
//static INT32 m_last_latitude =  UNKNOWN_COORDINATE;  // последнее известное достоверное значение широты
//static INT32 m_last_longitude = UNKNOWN_COORDINATE;  // последнее известное достоверное значение долготы
//static volatile BOOLEAN inited = FALSE;
///* Local function prototypes ====================================================================*/
//static M2MB_OS_RESULT_E reset_GNSS(INT32 reset_type);
//static void get_mutex(void);
//static void put_mutex(void);
//static M2MB_OS_RESULT_E mut_init(void);
//static void print_recent_location(T_Location r_loc, UINT8 n);
//static AZX_TIMER_ID gnss_t = NO_AZX_TIMER_ID;
//static void gnss_timer_handler(void* ctx, AZX_TIMER_ID id);
//static void gnss_callback( M2MB_GNSS_HANDLE handle, M2MB_GNSS_IND_E event, UINT16 resp_size, void *resp, void *userdata);
///* Static functions =============================================================================*/
//static void get_mutex(void) {
//	if (mtx_handle == NULL) mut_init();
//	M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtx_handle, M2MB_OS_WAIT_FOREVER);
//	if (res != M2MB_OS_SUCCESS) LOG_ERROR("gGmtx");
//}
//
//static void put_mutex(void) {
//	if (mtx_handle == NULL) {
//		LOG_ERROR("mtG");
//		return;
//	}
//	M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtx_handle);
//	if (res != M2MB_OS_SUCCESS) LOG_ERROR("pGmtx");
//}
//
//static M2MB_OS_RESULT_E mut_init(void) {
//	M2MB_OS_RESULT_E os_res = m2mb_os_mtx_setAttrItem( &mtx_attr_handle,
//	                                      CMDS_ARGS(
//	                                        M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
//	                                        M2MB_OS_MTX_SEL_CMD_NAME, "gn",
//	                                        M2MB_OS_MTX_SEL_CMD_USRNAME, "gn",
//	                                        M2MB_OS_MTX_SEL_CMD_INHERIT, 3
//	                                      )
//	                                );
//	if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("GNe1");
//	os_res = m2mb_os_mtx_init( &mtx_handle, &mtx_attr_handle);
//	if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("GNe2");
//	return os_res;
//}
//
//static M2MB_OS_RESULT_E reset_GNSS(INT32 reset_type) {
//	if (!ati_sendCommandExpectOkEx(2000, "AT$GPSR=%i\r", reset_type)) {
//		LOG_ERROR("GNSS reset err");
//		return M2MB_OS_APP_GENERIC_ERROR;
//	}
//	return M2MB_OS_SUCCESS;
//}
//
////static void printGnssInfo_test(M2MB_GNSS_POSITION_REPORT_INFO_T *locData ) {
////  locData->speed.speed = sqrt( pow( locData->speed.speed_horizontal,2 ) + pow( locData->speed.speed_vertical,2 ) );
////  LOG_INFO("latitude_valid: %d - latitude: %f", locData->latitude_valid, locData->latitude);
////  LOG_INFO("longitude_valid: %d - longitude: %f", locData->longitude_valid, locData->longitude );
////  LOG_INFO("altitude_valid: %d - altitude: %f", locData->altitude_valid, locData->altitude );
////  LOG_INFO("uncertainty_valid: %d - uncertainty: %f", locData->uncertainty_valid, locData->uncertainty );
////  LOG_INFO("velocity_valid: %d - ", locData->velocity_valid );
////  LOG_INFO("codingType: %d", locData->velocity.codingType );
////  LOG_INFO("speed_horizontal: %f", locData->velocity.speed_horizontal );
////  LOG_INFO("bearing: %f", locData->velocity.bearing );
////  LOG_INFO("timestamp_valid: %d - timestamp: %llu", locData->timestamp_valid, locData->timestamp ); // milliseconds since Jan. 1, 1970
////  LOG_INFO("speed_valid: %d - speed: %f", locData->speed_valid, locData->speed.speed );
////  return;
////}
//
//static void print_recent_location(T_Location r_loc, UINT8 n) {
//	LOG_INFO("recent loc%i:: UTC: %d, lat: %d, long: %d", n, r_loc.utc, r_loc.latitude, r_loc.longitude);
//}
//
//static void clear_m_last_coordinates(void) {
//	get_mutex();
//	m_last_latitude = UNKNOWN_COORDINATE;
//	m_last_longitude = UNKNOWN_COORDINATE;
//	set_last_reliable_coord(UNKNOWN_COORDINATE, UNKNOWN_COORDINATE);
//	put_mutex();
//	LOG_INFO("Last reliable coords was cleared");
//}
//
//static AZX_TIMER_ID init_timer(UINT32 timeout_ms, azx_expiration_cb cb) {
//	AZX_TIMER_ID id = NO_AZX_TIMER_ID;
//	id = azx_timer_initWithCb(cb, NULL, 0);
//	if (id == NO_AZX_TIMER_ID) LOG_ERROR("t creation failed");
//	else {
//		//LOG_INFO("timer_started");
//		azx_timer_start(id, timeout_ms, FALSE);
//	}
//	return id;
//}
//
//static BOOLEAN deinit_timer(AZX_TIMER_ID *id) {
//	BOOLEAN r;
//	if (*id != NO_AZX_TIMER_ID) {
//		//LOG_INFO("deinit_timer");
//		r = azx_timer_deinit(*id);
//		*id = NO_AZX_TIMER_ID;
//	}
//	return r;
//}
//
//static MODULE_TYPE_E getModuleType(void) {
//	M2MB_RESULT_E res;
//	M2MB_INFO_HANDLE hInfo;
//	CHAR *info;
//	MODULE_TYPE_E moduleType = NONE;
//
//	res = m2mb_info_init(&hInfo);
//	if (res != M2MB_RESULT_SUCCESS) {
//		 LOG_ERROR("Impossible init info");
//	} else {
//		res = m2mb_info_get(hInfo, M2MB_INFO_GET_MODEL, &info);
//		if (res != M2MB_RESULT_SUCCESS) LOG_ERROR("Impossible to get model");
//		else {
//			LOG_TRACE("Model: %s", info);
//			if (strstr(info, "G1") != NULL) {
//				LOG_TRACE("Type: %d", MEX10G1);
//				moduleType = MEX10G1;
//			} else if (info[0] == 'M' && strstr(info, "C1")) {
//				LOG_TRACE("type: %d", MEX10C1);
//				moduleType = MEX10C1;
//			} else if ( strstr(info, "LE910C")) {
//				if (info[strlen(info) -1 ] == 'X') {
//					LOG_TRACE("type: %d", LE910CX_X);
//					moduleType = LE910CX_X;
//				} else {
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
//static int start_gnss(void) {
//	if (inited) return 0;
//	get_mutex();
//	memset(&g_data, 0, sizeof(g_data));
//	m_last_latitude = get_last_reliable_latitude();
//	m_last_longitude = get_last_reliable_longitude();
//	put_mutex();
//	M2MB_OS_RESULT_E        osRes;
//	osRes = m2mb_os_ev_setAttrItem(&evAttrHandle,
//							CMDS_ARGS(
//									M2MB_OS_EV_SEL_CMD_CREATE_ATTR,	NULL,
//									M2MB_OS_EV_SEL_CMD_NAME, "gnss_ev")
//			);
//	if (osRes != M2MB_OS_SUCCESS) {
//		LOG_CRITICAL("gnss_SAI");
//		return -1;
//	}
//	osRes = m2mb_os_ev_init( &gnss_evHandle, &evAttrHandle );
//	if (osRes != M2MB_OS_SUCCESS) {
//		m2mb_os_ev_setAttrItem(&evAttrHandle, M2MB_OS_EV_SEL_CMD_DEL_ATTR, NULL);
//		LOG_CRITICAL("m2mb_os_ev_init failed!");
//		return -1;
//	} else LOG_TRACE("m2mb_os_ev_init success");
//
//	if (M2MB_RESULT_SUCCESS != m2mb_gnss_init(&handle1, gnss_callback, userdata)) {
//		LOG_ERROR("m2mb_gnss_enable NMEA REPORT, failed!");
//		return -1;
//	}
//	moduleType = getModuleType();
//	/*
//	 * on MEX10G1 both M2MB_GNSS_SERVICE_NMEA_REPORT and M2MB_GNSS_SERVICE_POSITION_REPORT services are available, while
//	 * on ME910C1 product family only M2MB_GNSS_SERVICE_POSITION_REPORT is available
//	 */
//	gnss_service = M2MB_GNSS_SERVICE_POSITION_REPORT;// M2MB_GNSS_SERVICE_NMEA_REPORT;// M2MB_GNSS_SERVICE_POSITION_REPORT;
//	if (M2MB_RESULT_SUCCESS != m2mb_gnss_enable( handle1, gnss_service)) {
//		LOG_ERROR("m2mb_gnss_enable %s REPORT, failed!", gnssServ[gnss_service]);
//		return -1;
//	}
//	//LOG_INFO("m2mb_gnss_enable, %s OK", gnssServ[gnss_service]);
//	if ( M2MB_RESULT_SUCCESS != m2mb_gnss_start(handle1)) {
//		LOG_ERROR("Failed to start GNSS");
//		return -1;
//	}
//	//LOG_INFO("m2mb_gnss_start OK, waiting for position/nmea sentences...");
//	/*Wait for GPS fix/NMEA sentences event to occur (released in gnssCallbackFN function) */
//	m2mb_os_ev_get(gnss_evHandle, GNSS_BIT, M2MB_OS_EV_GET_ANY_AND_CLEAR, &curEvBits, M2MB_OS_WAIT_FOREVER); // M2MB_OS_MS2TICKS(600*1000)
//	//LOG_INFO("GNSS was inited");
//	inited = TRUE;
//	gnss_t = init_timer(TIMEOUT_GNSS, gnss_timer_handler);
//	return M2MB_OS_SUCCESS;
//}
//
//static void save_rel_coord(void) {
//	if (m_last_latitude != UNKNOWN_COORDINATE &&
//		m_last_longitude != UNKNOWN_COORDINATE &&
//		(m_last_latitude != get_last_reliable_latitude() || m_last_longitude != get_last_reliable_longitude())) {
//		if (isIgnition()) set_last_reliable_coord(m_last_latitude, m_last_longitude);
//	}
//}
//
//static void gnss_callback( M2MB_GNSS_HANDLE handle, M2MB_GNSS_IND_E event, UINT16 resp_size, void *resp, void *userdata) {
//	(void)handle;
//	(void)resp_size;
//	(void)userdata;
//	//LOG_DEBUG("gnssCallback[%d], event[%d]", sentenceNum, event);
//	static UINT32 counter = 0;
//	switch (event) {
//		case M2MB_GNSS_INDICATION_POSITION_REPORT:
//			deinit_timer(&gnss_t);
//			gnss_t = init_timer(TIMEOUT_GNSS, gnss_timer_handler);
//			//printGnssInfo_test( (M2MB_GNSS_POSITION_REPORT_INFO_T *)resp);
//			M2MB_GNSS_POSITION_REPORT_INFO_T *data;
//			data = (M2MB_GNSS_POSITION_REPORT_INFO_T *) resp;
//			T_GnssData gd = {0};
//			gd.altitude = data->altitude;
//			gd.course = (float) data->velocity.bearing;
//			gd.fix = data->latitude_valid & data->longitude_valid & data->timestamp_valid;
//			gd.hdop = (float) data->uncertainty;
//			gd.latitude = data->latitude == 0 ? 0x7FFFFFFF : ((INT32) (data->latitude * 3600000));
//			gd.longitude = data->longitude == 0 ? 0x7FFFFFFF : ((INT32) (data->longitude * 3600000));
//			//gd.latitude_i = ; TODO добавить преобразование для READ_DIAG
//			gd.speed = sqrt( pow( data->speed.speed_horizontal,2 ) + pow( data->speed.speed_vertical,2 ));
//			gd.utc = (UINT32) (data->timestamp / 1000);
//			//send_to_gnss_task(PRINT_GNSS_DATA, 0, 0);
//			get_mutex();
//				if (gd.fix == 0 || gd.utc == g_data.rec_locations[1].utc || gd.latitude == 0 || gd.longitude == 0) {
//					if (get_raim_failure() == F_INACTIVE) {
//						set_raim_failure(F_ACTIVE);
//						set_gnss_failure(F_ACTIVE);
//					}
//				} else if (get_raim_failure() != F_INACTIVE) {
//					set_raim_failure(F_INACTIVE);
//					set_gnss_failure(F_INACTIVE);
//				}
//			// здесь делается подмена UTC, т.к. реальный интервал обновления геоданных в API gnss не 1, а 3 секунды
//				T_Location loc_n1 = {gd.utc == 0 ? 0 : gd.utc - 1, g_data.latitude, g_data.longitude}; //{g_data.utc, g_data.latitude, g_data.longitude};
//				T_Location loc_n2 = g_data.rec_locations[0];
//				loc_n2.utc = loc_n1.utc - 1;
//				g_data = gd;
//				g_data.rec_locations[0] = loc_n1;
//				g_data.rec_locations[1] = loc_n2;
//				if (get_raim_failure() != F_INACTIVE) g_data.fix = 0;
//			put_mutex();
//			if (g_data.fix > 0) {
//				m_last_latitude = g_data.latitude;
//				m_last_longitude = g_data.longitude;
//				updateRTC(g_data.utc, 5);
//			}
//			break;
//		case M2MB_GNSS_INDICATION_NMEA_REPORT:
//			LOG_INFO("NMEA: %s", (CHAR*)resp);
//			break;
//		default:
//			LOG_WARN("unexp GNSS event");
//			break;
//	}
//	counter++;
//	if (counter >= 3 * 20) { // попытка сохранения в файл делается каждую минуту
//		counter = 0;
//		save_rel_coord();
//	}
//	m2mb_os_ev_set(gnss_evHandle,GNSS_BIT, M2MB_OS_EV_SET);
//}
//
//static void gnss_timer_handler(void* ctx, AZX_TIMER_ID id) {
//	(void) ctx;
//	if (gnss_t == id) {
//		LOG_DEBUG("GNSS lost");
//		g_data.fix = 0;
//		get_mutex();
//			if (get_raim_failure() == F_INACTIVE) {
//				set_raim_failure(F_ACTIVE);
//				set_gnss_failure(F_ACTIVE);
//			}
//		put_mutex();
//		save_rel_coord();
//	}
//}
//
//
///* Global functions =============================================================================*/
//T_GnssData get_gnss_data(void) {
//	get_mutex();
//	T_GnssData gd = g_data;
//	put_mutex();
//	return gd;
//}
//
//BOOLEAN is_gnss_valid(void) {
//	return g_data.fix > 0;
//}
//
//void print_gnss_data(T_GnssData gd) {
//    LOG_INFO("GNSS data:\t\t UTC: %d", gd.utc);
//    LOG_INFO("latitude: %d, longitude: %d", gd.latitude, gd.longitude);
//    LOG_INFO("hdop: %f, altitude: %f", gd.hdop, gd.altitude);
//    LOG_INFO("fix: %d", gd.fix);
//    LOG_INFO("course: %f, speed: %d", gd.course, gd.speed);
//    LOG_INFO("sattelites:: GPS: %d, GLONASS: %d", gd.gps_sat, gd.glonass_sat);
//    for (UINT32 i = 0; i < sizeof(gd.rec_locations) / sizeof(T_Location); i++) {
//    	print_recent_location(gd.rec_locations[i], i);
//	}
//}
//
//void deinit_gnss(void) {
//	deinit_timer(&gnss_t);
//	g_data.fix = 0;
//	if ( M2MB_RESULT_SUCCESS != m2mb_gnss_stop(handle1)) 	LOG_ERROR("Failed GNSS_stop");
//	if ( M2MB_RESULT_SUCCESS != m2mb_gnss_deinit(handle1))	LOG_ERROR("Failed GNSS_deinit");
//	//LOG_INFO("GNSS deinited");
//}
//
///***************************************************************************************************
//   GNSS_task handles GNSS init, configuration, start and stop
// **************************************************************************************************/
//INT32 GNSS_task(INT32 type, INT32 param1, INT32 param2) {
//	(void)param1;
//	(void)param2;
//	if (type != GNSS_TIMER) LOG_DEBUG("GNSS_task command: %i", type);
//	switch(type) {
//		case START_GNSS:
//			start_gnss();
//			break;
//		case PAUSE_GNSS_REQUEST:
//			break;
//		case RESTART_GNSS_REQUEST:
//			break;
//		case STOP_GNSS:
////			g_data.fix = 0;
////			if ( M2MB_RESULT_SUCCESS != m2mb_gnss_stop(handle1)) LOG_ERROR("Failed to stop GNSS");
////			break;
//
//		case RESET_GNSS:
//			reset_GNSS(param1);
//			break;
//		case PRINT_GNSS_DATA:	{
//			T_GnssData gd;
//			get_mutex();
//			gd = g_data;
//			put_mutex();
//			print_gnss_data(gd);
//			break;
//		}
//		case CLEAR_LAST_RELIABLE_COORD:
//			clear_m_last_coordinates();
//			break;
//		default:
//			break;
//	}
//	return M2MB_OS_SUCCESS;
//}
//
//INT32 GNSS_TT_task(INT32 type, INT32 param1, INT32 param2) {
//	(void)param1;
//	(void)param2;
//	switch(type) {
//		case GNSS_TIMER:
//			break;
//		default:
//			break;
//	}
//	return M2MB_OS_SUCCESS;
//}
//
//INT32 get_m_last_latitude(void) {
//	return m_last_latitude;
//}
//
//INT32 get_m_last_longitude(void) {
//	return m_last_longitude;
//}
