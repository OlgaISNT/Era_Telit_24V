/*
 * gnss.h
 * Created on: 28.03.2022, Author: DKozenkov     
 */

#ifndef HDR_GNSS_H_
#define HDR_GNSS_H_

//typedef enum {
//	START_GNSS,
//	STOP_GNSS
//
//} GNSS_COMMAND;
//
//typedef struct {
//	UINT32 utc;			// timestamp UTC, секунды
//	INT32 latitude;		// широта в угловых мС
//	INT32 longitude;	// долгота в угловых мС
//	float hdop;			// горизонтальная точность HDOP
//	float altitude;		// высота над уровнем моря, метры
//	char fix;			// признак достоверности геоданных
//	float course;		// курс (угол), градусы.минуты
//	UINT16 speed;		// скорость, км/ч
//	UINT8 gps_sat;		// кол-во спутников GPS
//	UINT8 glonass_sat;	// кол-во спутников GPS
//} gnss_data;
//
///*!
//	@brief
//		Парсинг ответа АТ-команды AT$GPSACP
//	@details
//		Парсинг ответа АТ-команды AT$GPSACP.
//		Если какой-то параметр в ответе не имеет значения, то в возвращаемой структуре он должен быть равен 0
//	@param[in] *message
//    	Указатель на строку с ответом АТ-команды AT$GPSACP (может содержать спецсимволы и управляющие символы)
//    	Пример 1:
//    	"$GPSACP: 082055.000,5332.2471N,04918.9815E,5.6,107.5,2,355.2,1.9,1.0,070422,01,02
//    	OK"
//    	Пример 2:
//    	"$GPSACP: ,,,,,,,,,,
//    	OK"
//    	Пример 3:
//    	"ERROR"
//	@return
//    	gnss_data заполненная структура
//  @note
//
//  @b
//    Example
//  @code
//    <C code example>
//  @endcode
// */
//gnss_data parse_gnss_mess(char *message);
//
//
//INT32 GNSS_task(INT32 type, INT32 param1, INT32 param2);
//
//// Сброс GNSS приёмника, reset_type - тип сброса (см. AT$GPSR)
//int reset_GNSS(int reset_type);

#endif /* HDR_GNSS_H_ */
