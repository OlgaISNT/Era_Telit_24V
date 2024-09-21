/**
 * @file factory_param.h
 * @version 1.0.0
 * @date 06.05.2022
 * @author: DKozenkov     
 * @brief app description
 */

#ifndef HDR_FACTORY_PARAM_H_
#define HDR_FACTORY_PARAM_H_
#include "prop_manager.h"

// Серийный номер изделия
#define SERIAL_NUMBER_NAME		    "SERIAL_NUMBER"
#define SERIAL_NUMBER_DEFAULT		"________________1.00"
#define CURRENT_SIM_SLOT_NAME       "SIM_SLOT"
#define TM_FLAG_NAME                "TM"

#define MODEL_ID_NAME				"MODEL_ID"
#define MODEL_ID_DEFAULT			11
#define MODEL_ID_MIN				11
#define MODEL_ID_MAX				30

#define CAN_FILTER_NAME				"CAN_FILTER"
#define CAN_FILTER_DEFAULT			0
#define CAN_FILTER_MIN				((INT32)-2)
#define CAN_FILTER_MAX				0

#define REV_NAME				    "REV"
#define REV_DEFAULT			        0
#define REV_MIN				        0
#define REV_MAX				        2

// Audioprofile position - номер аудиопрофиля, который должен выбираться в модеме
#define AUDIOPROFILE_POSITION_NAME			"AUDIOPROFILE_POSITION"
#define AUDIOPROFILE_POSITION_DEFAULT		0
#define AUDIOPROFILE_POSITION_MIN			0
#define AUDIOPROFILE_POSITION_MAX			20
#define DESTINATION_ACDB_NAME               "/data/acdb/acdbdata"
#define SOURCE_ACDB_NAME                    "/data/acdb/"

#define ADDR_PREFIX_ACDB_NAME "/data/acdb/acdbdata/"
#define BLUETOOTH_ACDB_NAME "Bluetooth_cal"
#define GENERAL_ACDB_NAME "General_cal"
#define GLOBAL_ACDB_NAME "Global_cal"
#define HANDSET_ACDB_NAME "Handset_cal"
#define HDMI_ACDB_NAME "Hdmi_cal"
#define HEADSET_ACDB_NAME "Headset_cal"
#define SPEAKER_ACDB_NAME "Speaker_cal"
#define ACDB_FILE_EXTENSION_NAME ".acdb"

int get_can_filt_type(void);
int get_audio_prof_pos(void);
BOOLEAN set_audio_prof_pos(int value);
int getSimSlotNumber(void);
BOOLEAN get_serial(char *dest, UINT32 size);
BOOLEAN init_factory_params(void);
int get_model_id(void);
int get_current_rev(void);
BOOLEAN is_two_wire_bip(void);
int getTM(void);
BOOLEAN setTM(BOOLEAN value);
BOOLEAN get_sw_version(char *dest, UINT32 size);
BOOLEAN with_can(void);   // Блок эра содержит CAN



/*!
  @brief
    Проверка, принадлежит модель автомобиля к группе, имеющей кнопку SERVICE
  @details
 	@return TRUE - кнопка SERVICE есть в БИПе, FALSE - нет
 */
BOOLEAN is_model_serv_butt(void);

/*!
  @brief
    Проверка, принадлежит модель автомобиля к группе, для которой отключена реальная диагностика обрыва микрофона
  @details
 	@return TRUE - реальная диагностика обрыва микрофона отключена, FALSE - не отключена
 */
BOOLEAN is_diag_mic_disable(void);

/*!
  @brief
    Проверка, принадлежит модель автомобиля к группе, для которой отключена реальная диагностика обрыва кнопки SOS
  @details
 	@return TRUE - реальная диагностика обрыва кнопки SOS отключена, FALSE - не отключена
 */
BOOLEAN is_diag_sos_disable(void);

/*!
  @brief Проверка, принадлежит модель автомобиля к группе, для которой отключено использование подушек безопасности
  @details
 	@return TRUE/FALSE - отключено/не отключено
 */
BOOLEAN is_airbags_disable(void);

/*!
  @brief
    Чтение всех параметров производственной конфигурации и копирование их в переданный массив символов
  @details
    @param[in] buf 		- буфер, в который будут скопированы все параметры
    @param[in] buf_size - кол-во байт буфера
  	@return
 */
void get_factory_params(char *buf, UINT32 buf_size);


/*!
 @brief Проверка, требуется ли индикация для данного БЭГа при отсутствии активации

 @return TRUE - требуется, FALSE - не требуется
 */
BOOLEAN needIndWhenIsNotActivated(void);

/*!
 @brief Собран ли данный автомобиль Автовазом?
 @return TRUE - да, FALSE - нет
 */
extern BOOLEAN isAutoVaz(void);

/*!
 @brief Использовать фиксацию удара акселерометром
 @return TRUE - да, FALSE - нет
 */
BOOLEAN is_enable_accel_crush_detect(void);

#endif /* HDR_FACTORY_PARAM_H_ */
