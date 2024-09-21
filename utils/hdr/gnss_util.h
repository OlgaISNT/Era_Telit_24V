/**
 * @file gnss_util.h
 * @version 1.0.0
 * @date 14.04.2022
 * @author: AKlokov
 * @brief app description
 */

#ifndef HDR_GNSS_UTIL_H_
#define HDR_GNSS_UTIL_H_

#include "gnss_rec.h"

/*!
	@brief
		Парсинг ответа АТ-команды AT$GPSACP
	@details
		Парсинг ответа АТ-команды AT$GPSACP.
		Если какой-то параметр в ответе не имеет значения, то в возвращаемой структуре он должен быть равен 0
	@param[in] *message
    	Указатель на строку с ответом АТ-команды AT$GPSACP (может содержать спецсимволы и управляющие символы)
    	Пример 1:
    	"$GPSACP: 082055.000,5332.2471N,04918.9815E,5.6,107.5,2,355.2,1.9,1.0,070422,01,02
    	OK"
    	Пример 2:
    	"$GPSACP: ,,,,,,,,,,
    	OK"
    	Пример 3:
    	"ERROR"
	@return
    	gnss_data заполненная структура
  @note

  @b
    Example
  @code
    <C code example>
  @endcode
 */
T_GnssData parse_gnss_mess(char *message);


#endif /* HDR_GNSS_UTIL_H_ */
