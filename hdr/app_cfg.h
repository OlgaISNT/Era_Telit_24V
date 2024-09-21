/**
 * @file app_cfg.h
 * @version 1.0.0
 * @date 05.04.2022
 * @author: DKozenkov
 * @brief Application configuration settings conveniently located here.
 */

#ifndef HDR_APP_CFG_H_
#define HDR_APP_CFG_H_

/**
 * ---- ВАРИАНТЫ КОМПИЛЯЦИИ ПРОЕКТА ----
 * 1.Для тестирования тестировщиками: 	включить RELEASE и LOG_TO_UART0 и в Makefile.in установить: LOGS_ENABLE = 1
 * 2.Для отладки приложения: 			выключить RELEASE, включить LOG_TO_UART0 и в Makefile.in установить: LOGS_ENABLE = 1
 * 3.Для производства:					включить RELEASE, выключить LOG_TO_UART0 и в Makefile.in установить: LOGS_ENABLE = 0
 */


/** Определяет, будет ли отключено логирование, включен вызов на 112 и переключен уровень обслуживания по протоколу на 0
 * При включении этого дефайн нужно также в файле Makefile.in установить: LOGS_ENABLE = 0 */
#define  RELEASE

/** Определяет, будет ли дополнительная информация выводиться в лог UART0 - должно быть отключено при компиляции для конечного потребителя */
//#define LOG_TO_UART0

/** Маски security access для различных MODEL_ID */
#define SA_MASK_0 0x68b05c76	// модель не задана;
#define SA_MASK_1 0x874526bd	// АвтоВАЗ 4х4 (8450110539);
#define SA_MASK_2 0x14db533a	// АвтоВАЗ Гранта (8450110539);
#define SA_MASK_3 0xc0712e3d	// АвтоВАЗ NIVA TRAVEL GM (8450086661), УАЗ (18.3879600-75);
#define SA_MASK_4 0x13609cea	// АвтоВАЗ Ларгус ф.2 (8450092997);
#define SA_MASK_5 0xa9714f63	// АвтоВАЗ Веста ф.2 (?);
#define SA_MASK_6 0x20612a7f	// ГАЗ (18.3879600-70);
#define SA_MASK_7 0x7cf01b01	// ПАЗ (1824.3879600-40);
#define SA_MASK_8 0x6ba772f5	// КАМАЗ (1824.3879600-70);

#define SA_MASK_10 0x23c5b2f1	// Cherry (18.3879600-53);

#define MODEL_ID0 0
#define MODEL_ID1 1
#define MODEL_ID2 2
#define MODEL_ID3 3
#define MODEL_ID4 4
#define MODEL_ID5 5
#define MODEL_ID6 6
#define MODEL_ID7 7
#define MODEL_ID8 8
#define MODEL_ID9 9
#define MODEL_ID10 10
#define MODEL_ID11 11

/** ---- Пути и имена файлов -- */
#define LOCALPATH_HIDE "/data/azc"
#define LOCALPATH "/data/azc/mod"
#define LOCALPATH_S "/data/azc/mod/"
#define AUDIOFILE_PATH "/data/aplay/"
#define ERA_PARAMS_FILE_NAME     				"era.cfg" 		// файл с параметрами конфигурации режима ЭРА
#define SYS_PARAMS_FILE_NAME     				"sysp.cfg"		// файл с системными параметрами приложения
#define FACTORY_PARAMS_FILE_NAME     			"factory.cfg"	// файл с параметрами конфигурации устройства на производстве
/** ---- МНД ------------------------*/
#define MSD_DAT_FILE_NAME "msd.dat"   // название файла, хранящего массивы данных МНД
#define MAX_NUMBER_MSD 100			  // максимальное количество массивов МНД, сохраняемых в файле
#define MSD_LENGTH 140				  // кол-во байт в МНД
#define MSD_FORMAT_VERSION 2
#define SERT_VERS "1.00"
#define HW_VERS "x02"

/**----- Логирование --------------- */
#define SHOW_LOG_DATE 0 // показывать в сообщениях лога дату
#define SHOW_LOG_TIME 1 // показывать в сообщениях лога время
	// !! - осторожно, чтобы не разрушать FLASH память модема не следует всегда использовать эту настройку !!
#define SAVE_LOG_TO_FILE 0 // сохранять сообщения лога в файл

#define LOG_FILE_SIZE 2048000 // максимальный размер в байтах каждого из 2-х лог-файлов

#define MAX_RESTART_NUMBER 3    // максимальное кол-во перезапусков модема для выполнения попытки вызова ЭРА

/**----- Параметры АТ-интерфейса --- */
/** atInstance MUST be not reserved to a physical port: use atInstance=0 with PORTCFG=8 or atInstance=1 with PORTCFG=13 */
#define AT_INSTANCE_GENERAL 0 	// номер экземпляра АТ-интерфейса для АТ-команд, не связанных с ЭРА
#define AT_INSTANCE_ERA 	1	// номер экземпляра АТ-интерфейса для АТ-команд ЭРА

/**----- Конфигурация системных ивентов ----- */
#define AT_SYNC_BIT      (UINT32)0x2    /*0x0000000000000010*/

/** @cond DEV*/
#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)
/** @endcond*/

#endif /* HDR_APP_CFG_H_ */
