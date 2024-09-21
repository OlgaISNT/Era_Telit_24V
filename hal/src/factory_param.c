/**
 * @file factory_param.c
 * @version 1.0.0
 * @date 06.05.2022
 * @author: DKozenkov     
 * @brief app description
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_sys.h"
#include "m2mb_os_api.h"
#include "at_command.h"
#include "factory_param.h"
#include "params.h"
#include "prop_manager.h"
#include "log.h"
#include "sys_param.h"
#include "app_cfg.h"
#include "m2mb_fs_stdio.h"
#include "m2mb_os_types.h"
#include "m2mb_os.h"
#include "atc_era.h"
#include "sys_utils.h"
#include <zconf.h>
#include <m2mb_fs_posix.h>
#include <stdio.h>
#include <fcntl.h>
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static PropManager *fac_p;
static char *serial_number; // серийный номер изделия
static char sw_version[10];
static int model_id = 0;
static int simSlot = 0;     // Текущий выбранный слот
static int tm = 0;          // Флаг признака тестового режима
static int audio_prof_pos;  // Номер аудио профиля
static int can_filt_type;   // Тип фильтра для can
static int rev;
/**
 приложение при первом старте будет создавать файл factory.cfg, содержащий две строки:
serial=
MD5=<..>
Строка MD5 является служебной и при считывании параметров из файла отображаться не будет.
Команд записи в этот файл из приложения не будет.
Считать параметры из файла позволит команда READ_FACTORY_CFG.
Сам файл можно будет создавать вручную вне модема и записывать его в модем стандартным образом.
 */
/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
/* Global functions =============================================================================*/
static BOOLEAN rewriteFile(char *destination, char *source, int numb);
static BOOLEAN deleteFile(char *path, char *fileName, char *extension);
static BOOLEAN copyFile(char *destination, char *path, char *filename, int numb);

BOOLEAN get_serial(char *dest, UINT32 size) {
	return get_str_par(dest, size, serial_number);
}

BOOLEAN get_sw_version(char *dest, UINT32 size) {
    return get_str_par(dest, size, sw_version);
}

BOOLEAN init_factory_params(void) {
	fac_p = newPropManager(FACTORY_PARAMS_FILE_NAME);
	if (!readAll(fac_p)) {
		if (!saveAllToFileManager(fac_p)) {
			LOG_CRITICAL("Can't save %s", FACTORY_PARAMS_FILE_NAME);
			return FALSE;
		}
	}
	serial_number = getString(fac_p, SERIAL_NUMBER_NAME, SERIAL_NUMBER_DEFAULT);
    model_id = getInt(fac_p, MODEL_ID_NAME, MODEL_ID_MIN, MODEL_ID_MAX, MODEL_ID_DEFAULT);
    simSlot = getInt(fac_p, CURRENT_SIM_SLOT_NAME, 0,1,0);
    tm = 1;
    audio_prof_pos = getInt(fac_p, AUDIOPROFILE_POSITION_NAME, AUDIOPROFILE_POSITION_MIN, AUDIOPROFILE_POSITION_MAX, AUDIOPROFILE_POSITION_DEFAULT);
    can_filt_type = getInt(fac_p, CAN_FILTER_NAME, CAN_FILTER_MIN, CAN_FILTER_MAX, CAN_FILTER_DEFAULT);
    rev = getInt(fac_p, REV_NAME, REV_MIN, REV_MAX, REV_DEFAULT);
    sprintf(&sw_version[0], "1.0%i", rev);
    LOG_DEBUG(">>>>>>>>>>>>>>>>>> sw %s", sw_version);
  //  memcpy(&sw_version[0], &serial_number[16], 4);
    LOG_INFO("audprpos:%i", get_audio_prof_pos());
    LOG_INFO("board revision %i", rev);
    if (isNeedSave(fac_p)) saveAllToFileManager(fac_p);
    return TRUE;
}

BOOLEAN with_can(void){
    return (model_id >= MODEL_ID11) ? TRUE : FALSE;
}

int get_current_rev(void){
    return rev;
}

BOOLEAN is_two_wire_bip(void){
    return (rev == 1 || rev == 2) ? TRUE : FALSE;
}

BOOLEAN is_airbags_disable(void){
    return (can_filt_type != 0) ? TRUE : FALSE;
}

int get_can_filt_type(void){
    return can_filt_type;
}

BOOLEAN is_enable_accel_crush_detect(void){
    return (can_filt_type == -2) ? TRUE : FALSE;
}

int get_audio_prof_pos(void) {
	return get_int_par(&audio_prof_pos);
}

BOOLEAN set_audio_prof_pos(int value) {
    // Если предыдущее значения профиля равно текущему, то ACDB же переписаны ранее
    if(audio_prof_pos == value) {
        LOG_DEBUG("ACDB files already overwritten");
        return TRUE;
    }

    // Если же оно различно, то переписываем файлы
    if(!rewriteFile(DESTINATION_ACDB_NAME, SOURCE_ACDB_NAME, value)) goto critical;

    // Применяем новый комплект ACDB
    if (!at_era_sendCommandExpectOk(3000, "AT#ACDBEXT=2,%d\r", 0)) goto critical;

    // Переписываем текущий аудио профиль
    if(!set_int_par(AUDIOPROFILE_POSITION_NAME, &audio_prof_pos, value, "S_AUD_P", fac_p)) goto critical;

    LOG_DEBUG("Set audioprofile success.");
    return TRUE;

    critical:
    // При переписывании ACDB файлов произошла ошибка.
    LOG_CRITICAL("An error occurred while rewriting acdb files.......");
//    restart(5000);
    return FALSE;
}

static BOOLEAN rewriteFile(char *destination, char *source, int numb){
    // Удаляем имеющиеся файлы
    deleteFile(ADDR_PREFIX_ACDB_NAME, BLUETOOTH_ACDB_NAME, ACDB_FILE_EXTENSION_NAME);
    deleteFile(ADDR_PREFIX_ACDB_NAME,GENERAL_ACDB_NAME, ACDB_FILE_EXTENSION_NAME);
    deleteFile(ADDR_PREFIX_ACDB_NAME, GLOBAL_ACDB_NAME, ACDB_FILE_EXTENSION_NAME);
    deleteFile(ADDR_PREFIX_ACDB_NAME, HANDSET_ACDB_NAME, ACDB_FILE_EXTENSION_NAME);
    deleteFile(ADDR_PREFIX_ACDB_NAME, HDMI_ACDB_NAME, ACDB_FILE_EXTENSION_NAME);
    deleteFile(ADDR_PREFIX_ACDB_NAME,HEADSET_ACDB_NAME, ACDB_FILE_EXTENSION_NAME);
    deleteFile(ADDR_PREFIX_ACDB_NAME,SPEAKER_ACDB_NAME, ACDB_FILE_EXTENSION_NAME);

    // Копируем файлы
    UINT8 cnt = 0;
    if (copyFile(destination, source, BLUETOOTH_ACDB_NAME, numb)) cnt++; // 1
    if (copyFile(destination, source, GENERAL_ACDB_NAME, numb)) cnt++;  // 2
    if (copyFile(destination, source, GLOBAL_ACDB_NAME, numb)) cnt++;   // 3
    if (copyFile(destination, source, HANDSET_ACDB_NAME, numb)) cnt++;  // 4
    if (copyFile(destination, source, HDMI_ACDB_NAME, numb)) cnt++;     // 5
    if (copyFile(destination, source, HEADSET_ACDB_NAME, numb)) cnt++;  // 6
    if (copyFile(destination, source, SPEAKER_ACDB_NAME, numb)) cnt++;  // 7

    if (cnt == 7){
        LOG_DEBUG("Change audioprofile success.");
        return TRUE;
    }
    LOG_DEBUG("Copy ACDB failed...");
    return FALSE;
}

// Copy.
// fullName:  /data/acdb/Bluetooth_cal_1.acdb
// fullDest:  /data/acdb/acdbdata/Bluetooth_cal.acdb
static BOOLEAN copyFile(char *destination, char *path, char *filename, int numb){
    char fullName[256];
    char fullDest[256];

    sprintf(fullName, "%s%s_%d%s", path, filename,numb, ACDB_FILE_EXTENSION_NAME);
    sprintf(fullDest, "%s/%s%s", destination,filename,ACDB_FILE_EXTENSION_NAME);

    int fd_in = 0, fd_out = 0;
    ssize_t ret_in = 0,ret_out = 0;

    fd_in = open(fullName, O_RDONLY);
    if(fd_in == -1) {
        LOG_ERROR("Error with open file >%s< for source", fullName);
        return FALSE;
    }

    fd_out = open(fullDest,O_WRONLY | O_CREAT, 0644);
    if (fd_out == -1) {
        LOG_ERROR("Error with open file >%s< for destenation", fullName);
        return FALSE;
    }

    char buffer[256];

    while ((ret_in = read(fd_in, &buffer, 256)) > 0)
    {
        ret_out = write(fd_out,  &buffer, (ssize_t) ret_in);
        if (ret_out != ret_in){
            LOG_ERROR("Error with write >%s< to dest %s.", fullName, fullDest);
            return FALSE;
        }
    }

    LOG_INFO("Copy file successfully. Src: %s Dest: %s ", fullName, fullDest);

    close(fd_in);
    close(fd_out);
    return TRUE;
}

static BOOLEAN deleteFile(char *path, char *fileName, char *extension) {
    char fullName[512];
    sprintf(fullName, "%s%s%s", path, fileName, extension);

    INT32 res = -1;
    res = m2mb_fs_unlink(fullName);
    if (res == 0) {
        LOG_TRACE("FACTORY: Deleted successfully. Filename = %s", fullName);
        return TRUE;
    } else {
        LOG_ERROR("FACTORY: Deletion failed. Filename = %s", fullName);
        return FALSE;
    }
}

int getTM(void){
    return tm;
}

int getSimSlotNumber(void){
    return simSlot;
}

int get_model_id(void) {
	return model_id;
}

extern BOOLEAN isAutoVaz(void){
	return get_model_id() == MODEL_ID1 || // АвтоВАЗ, 4х4 (Niva Legend 8450082753)
		   get_model_id() == MODEL_ID2 || // АвтоВАЗ, Гранта (8450101149)
		   get_model_id() == MODEL_ID3 || // АвтоВАЗ, Нива Travel GM (8450085827);
		   get_model_id() == MODEL_ID4 || // АвтоВАЗ, Ларгус ф.2 (8450092491)
		   get_model_id() == MODEL_ID5;   // АвтоВАЗ, Веста ф.2 (8450042687)
}

BOOLEAN needIndWhenIsNotActivated(void){
	return (get_model_id() == MODEL_ID9 || with_can())
    & (!is_activated());
}

BOOLEAN is_model_serv_butt(void) {
	return get_model_id() == MODEL_ID3 ||
		   get_model_id() == MODEL_ID5 ||
		   get_model_id() == MODEL_ID6 ||
		   get_model_id() == MODEL_ID7 ||
		   get_model_id() == MODEL_ID8 ||
		   get_model_id() == MODEL_ID9;
}

BOOLEAN is_diag_sos_disable(void) {
	return get_model_id() == MODEL_ID4 ||
		   get_model_id() == MODEL_ID10 ||
           with_can();
}

BOOLEAN is_diag_mic_disable(void) {
	return get_model_id() == MODEL_ID6 ||
		   get_model_id() == MODEL_ID7 ||
		   get_model_id() == MODEL_ID8;
}

void get_factory_params(char *buf, UINT32 buf_size) {
	char *str = propToString(fac_p->prop);
	memset(buf, 0, buf_size);
	if (str == NULL || strlen(str) >= buf_size) {
		LOG_ERROR("Can't gfp");
	} else {
		strcpy(buf, str);
	}
	free(str);
}
