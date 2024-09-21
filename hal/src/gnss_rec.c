/*
 * gnss.c
 * Created on: 28.03.2022, Author: DKozenkov
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "log.h"
#include "gnss_util.h"
#include "azx_timer.h"
#include "failures.h"
#include "at_command.h"
#include "tasks.h"
#include "app_cfg.h"
#include "u_time.h"
#include "sys_param.h"
#include "char_service.h"
#include "sys_utils.h"
#include "azx_ati.h"
#include "m2mb_os.h"
#include "m2mb_os_types.h"
#include "../hdr/gnss_rec.h"
#include "atc_era.h"
#include "params.h"

/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static volatile BOOLEAN stop = FALSE;
static volatile BOOLEAN GNSS_resetted = TRUE;
static volatile int t_interval = 1000; // интервал срабатывания таймера опроса GNSS-приёмника, мС
static T_GnssData g_data;
static M2MB_OS_MTX_ATTR_HANDLE mtx_attr_handle;
static M2MB_OS_MTX_HANDLE      mtx_handle = NULL;
// Значения ниже обновляются сразу же при получении очередных достоверных геоданных:
static INT32 m_last_latitude =  UNKNOWN_COORDINATE;  // последнее известное достоверное значение широты
static INT32 m_last_longitude = UNKNOWN_COORDINATE;  // последнее известное достоверное значение долготы
// Максимальный интервал, по достижении которого происходит сохранение достоверной широты и долготы в файловую систему модема, мСек
static int save_interval = 120*1000;
// Время, прошедшее с момента последнего сохранения достоверной широты и долготы в файловую систему модема, мСек
static int elapsed_time = 0;
static volatile BOOLEAN inited = FALSE;
static volatile BOOLEAN req_started = FALSE;
/* Local function prototypes ====================================================================*/
static void init_gnss(int interval);
//static M2MB_OS_RESULT_E stop_gnss(void);
static M2MB_OS_RESULT_E reset_GNSS(INT32 reset_type);
static void get_mutex(void);
static void put_mutex(void);
static M2MB_OS_RESULT_E mut_init(void);
static void print_recent_location(T_Location r_loc, UINT8 n);
/* Static functions =============================================================================*/

static void get_mutex(void) {
    if (mtx_handle == NULL) mut_init();
    M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtx_handle, M2MB_OS_WAIT_FOREVER);
    if (res != M2MB_OS_SUCCESS) LOG_ERROR("gGmtx");
}

static void put_mutex(void) {
    if (mtx_handle == NULL) {
        LOG_ERROR("mtG");
        return;
    }
    M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtx_handle);
    if (res != M2MB_OS_SUCCESS) LOG_ERROR("pGmtx");
}
static M2MB_OS_RESULT_E mut_init(void) {
    M2MB_OS_RESULT_E os_res = m2mb_os_mtx_setAttrItem( &mtx_attr_handle,
                                                       CMDS_ARGS(
                                                               M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
                                                               M2MB_OS_MTX_SEL_CMD_NAME, "gn",
                                                               M2MB_OS_MTX_SEL_CMD_USRNAME, "gn",
                                                               M2MB_OS_MTX_SEL_CMD_INHERIT, 3
                                                       )
    );
    if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("GNe1");
    os_res = m2mb_os_mtx_init( &mtx_handle, &mtx_attr_handle);
    if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("GNe2");
    return os_res;
}

static void gnss_requester(void) {
    static UINT32 cycles_restart = 0; // кол-во циклов безуспешного чтения данных до рестарта GNSS
    static UINT32 cycles_ok = 0;	  // кол-во циклов чтения данных до отправки команды синхронизации часов модема
    if (req_started) return;
    req_started = TRUE;
    while (1) {
        m2mb_os_taskSleep( M2MB_OS_MS2TICKS(t_interval));
        if (stop) {
            cycles_restart = 0;
            cycles_ok = 0;
            //		else { // TODO в идеале правильно использовать этот код, но тогда изменится внешнее поведение БЭГа
            //			get_mutex();
            //			g_data.fix = 0;
            //			put_mutex();
            //		}
        } else {
            T_GnssData gd = {0};
            char nmea[200];
            const char *req = "AT$GPSACP\r";
            if (ati_sendCommandEx(2000, nmea, sizeof(nmea), req) == 0) {
                LOG_DEBUG("Not NMEA1");
                m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1000));
                if (ati_sendCommandEx(2000, nmea, sizeof(nmea), req) == 0) {
                    LOG_WARN("Not NMEA2");
                } else gd = parse_gnss_mess(nmea);
            } else {
                gd = parse_gnss_mess(nmea);
                if (gd.utc == 0) {
                    ati_sendCommandEx(1000, nmea, sizeof(nmea), req);
                    gd = parse_gnss_mess(nmea);
                }
            }
            get_mutex();
            if (gd.fix == 0 || gd.utc == g_data.rec_locations[1].utc || gd.latitude == 0 || gd.longitude == 0) {
                if (get_raim_failure() == F_INACTIVE) {
                    set_raim_failure(F_ACTIVE);
                    set_gnss_failure(F_ACTIVE);
                }
            } else if (get_raim_failure() != F_INACTIVE) {
                set_raim_failure(F_INACTIVE);
                set_gnss_failure(F_INACTIVE);
            }
            T_Location loc_n1 = {g_data.utc, g_data.latitude, g_data.longitude};
            T_Location loc_n2 = g_data.rec_locations[0];
            g_data = gd;
            g_data.rec_locations[0] = loc_n1;
            g_data.rec_locations[1] = loc_n2;
            if (get_raim_failure() != F_INACTIVE) g_data.fix = 0;
            if (g_data.fix > 0 && cycles_ok > 10) {
                m_last_latitude = g_data.latitude;
                m_last_longitude = g_data.longitude;
                updateRTC(g_data.utc, 4);
            }
            if (g_data.fix > 0 && GNSS_resetted) {
            	GNSS_resetted = FALSE;
            	LOG_INFO("timestamp will not be 0 since there is a fix");
            }
            put_mutex();
            //print_gnss_data(g_data);
            // LOG_DEBUG("%d / %d", (UINT32) get_gnss_wdt(), cycles_restart);
            if (get_raim_failure() == F_ACTIVE) {
                cycles_ok = 0;
                cycles_restart++;
                if (cycles_restart > (UINT32) get_gnss_wdt()) {
                    cycles_restart = 0;
                    // при необходимости для перезапуска ГНСС можно отправить эти 4 команды:
//        			send_to_gnss_task(STOP_GNSS, 0, 0);
//        			m2mb_os_taskSleep(M2MB_OS_MS2TICKS(5*1000));
//        			send_to_gnss_task(START_GNSS, 1000, 0);
//        			m2mb_os_taskSleep(M2MB_OS_MS2TICKS(2*1000));

                    // либо альтернативный вариант - выполнить горячий рестарт ГНСС:
                    send_to_gnss_task(RESET_GNSS, 3, 0);
                }
            } else {
                cycles_ok++;
                cycles_restart = 0;
            }
            elapsed_time += t_interval;
            if (elapsed_time >= save_interval) {
                elapsed_time = 0;
                //LOG_DEBUG("m_last_latitude:%i", m_last_latitude);
                //LOG_DEBUG("m_last_longitude:%i", m_last_longitude);
                if (m_last_latitude != UNKNOWN_COORDINATE &&
                    m_last_longitude != UNKNOWN_COORDINATE &&
                    (m_last_latitude != get_last_reliable_latitude() || m_last_longitude != get_last_reliable_longitude())) {
                    if (isIgnition()) set_last_reliable_coord(m_last_latitude, m_last_longitude);
                }
            }
        }
    }
}

static void init_gnss(int interval) {
    if (inited) return;
    get_mutex();
    memset(&g_data, 0, sizeof(g_data));
    m_last_latitude = get_last_reliable_latitude();
    m_last_longitude = get_last_reliable_longitude();
    put_mutex();
    if (interval < 1000) {
        LOG_ERROR("Inv t_int gnss:%i", interval);
        interval = 1000;
    }
    t_interval = interval;
    if (!ati_sendCommandExpectOkEx(2000, "AT$GPSP=0\r")) {
        LOG_DEBUG("Can't AT$GPSP=0");
    }
    if (!ati_sendCommandExpectOkEx(2000, "AT$GPSELNA=1\r")) LOG_ERROR("Can't enable LNA");
    if (!ati_sendCommandExpectOkEx(5000, "AT$GPSP=1\r")) {
        LOG_CRITICAL("Can't START GNSS");
        restart(5*1000);
        return;
    }
    if (!req_started) send_to_gnss_tt_task(GNSS_TIMER, 0, 0);
    inited = TRUE;
    stop = FALSE;
    LOG_INFO("___ GNSS INITED");
}

//static M2MB_OS_RESULT_E stop_gnss(void) { // осторожно, отправка команды AT$GPSP=0 почему-то блокирует обмен данными по UART0!
//    get_mutex();
//    stop = TRUE;
//    g_data.fix = 0;
//    put_mutex();
//    if (!ati_sendCommandExpectOkEx(3001, "AT$GPSP=0\r")) {
//        LOG_WARN("GNSS stop err");
//        return M2MB_OS_APP_GENERIC_ERROR;
//    }
//    inited = FALSE;
//    return M2MB_OS_SUCCESS;
//}

static M2MB_OS_RESULT_E reset_GNSS(INT32 reset_type) {
    if (!ati_sendCommandExpectOkEx(2000, "AT$GPSR=%i\r", reset_type)) {
        LOG_ERROR("GNSS reset err");
        return M2MB_OS_APP_GENERIC_ERROR;
    }
    LOG_INFO("GNSS was reset, type:%i", reset_type);
    m2mb_os_taskSleep( M2MB_OS_MS2TICKS(1000));
    if (reset_type == 1) {
    	GNSS_resetted = TRUE;
    	LOG_INFO("timestamp will be 0 due to COLD RESTART");
    }
    return M2MB_OS_SUCCESS;
}

/* Global functions =============================================================================*/
// Получение мьютекса в функции ниже занимает много времени
T_GnssData get_gnss_data_fast(void){
    T_GnssData gd = g_data;
    return gd;
}

T_GnssData get_gnss_data(void) {
    get_mutex();
    T_GnssData gd = g_data;
    put_mutex();
    return gd;
}

BOOLEAN is_gnss_valid(void) {
    return g_data.fix > 0;
}

void print_gnss_data(T_GnssData gd) {
    LOG_INFO("GNSS data:\t\t UTC: %d", gd.utc);
    LOG_INFO("latitude: %d, longitude: %d", gd.latitude, gd.longitude);
    LOG_INFO("hdop: %f, altitude: %f", gd.hdop, gd.altitude);
    LOG_INFO("fix: %d", gd.fix);
    LOG_INFO("course: %f, speed: %d", gd.course, gd.speed);
    LOG_INFO("sattelites:: GPS: %d, GLONASS: %d", gd.gps_sat, gd.glonass_sat);
    for (UINT32 i = 0; i < sizeof(gd.rec_locations) / sizeof(T_Location); i++) {
        print_recent_location(gd.rec_locations[i], i);
    }
}

static void print_recent_location(T_Location r_loc, UINT8 n) {
    LOG_INFO("recent loc%i:: UTC: %d, latit: %d, longit: %d", n, r_loc.utc, r_loc.latitude, r_loc.longitude);
}

static void clear_m_last_coordinates(void) {
    get_mutex();
    m_last_latitude = UNKNOWN_COORDINATE;
    m_last_longitude = UNKNOWN_COORDINATE;
    set_last_reliable_coord(UNKNOWN_COORDINATE, UNKNOWN_COORDINATE);
    put_mutex();
    LOG_INFO("Last reliable coords was cleared");
}

void pause_gnss_request(BOOLEAN value) {
    get_mutex();
    //g_data.fix = 0;
    stop = value;
    put_mutex();
    LOG_INFO("GNSS requester is %s", value ? "stopped" : "continued");
}

BOOLEAN is_gnss_req_stopped(void) {
    return stop;
}

BOOLEAN is_gnss_resetted(void) {
	return GNSS_resetted;
}

/***************************************************************************************************
   GNSS_task handles GNSS init, configuration, start and stop
 **************************************************************************************************/
INT32 GNSS_task(INT32 type, INT32 param1, INT32 param2) {
    (void)param1;
    (void)param2;
    if (type != GNSS_TIMER) LOG_DEBUG("GNSS_task command: %i", type);
    switch(type) {
        case START_GNSS:
            if (inited) stop = FALSE;
            else init_gnss(param1);
            break;
        case RESTART_GNSS_SESSION:
            if(!at_era_sendCommandExpectOk(2*1000, "AT$GPSP=0\r")){
                at_era_sendCommandExpectOk(2*1000, "AT$GPSP=0\r");
            }
            LOG_INFO("GNSS session was stopped");
            if(at_era_sendCommandExpectOk(2*1000, "AT$GPSP=1\r")) {
                at_era_sendCommandExpectOk(2*1000, "AT$GPSP=1\r");
            }
            stop = FALSE;
            LOG_INFO("GNSS session was started");
            break;
//        case STOP_GNSS: // осторожно, отправка команды AT$GPSP=0 почему-то блокирует обмен данными по UART0!
//            stop_gnss();
//            break;
        case RESET_GNSS:
            reset_GNSS(param1);
            break;
        case PRINT_GNSS_DATA:	{
            T_GnssData gd;
            get_mutex();
            gd = g_data;
            put_mutex();
            print_gnss_data(gd);
            break;
        }
        case CLEAR_LAST_RELIABLE_COORD:
            clear_m_last_coordinates();
            break;
        default:
            break;
    }
    return M2MB_OS_SUCCESS;
}

INT32 GNSS_TT_task(INT32 type, INT32 param1, INT32 param2) {
    (void)param1;
    (void)param2;
    switch(type) {
        case GNSS_TIMER:
            gnss_requester();
            break;
        default:
            break;
    }
    return M2MB_OS_SUCCESS;
}

INT32 get_m_last_latitude(void) {
    return m_last_latitude;
}

INT32 get_m_last_longitude(void) {
    return m_last_longitude;
}

void deinit_gnss(void) {
    // empty in AT-command version of gnss_rec
}
