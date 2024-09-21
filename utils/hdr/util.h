#ifndef UTIL_H
#define UTIL_H

#include "m2mb_types.h"

	BOOLEAN get_int(char *eq, char *end, INT32 *var);
	BOOLEAN get_uint(char *eq, char *end, UINT32 *var);
	BOOLEAN get_str(char *eq, char *end, char *arg);

	int mul(int *a, int *mul);
	void str_to_hex(char *p_src, int src_len, char *p_dest);
	int hex_to_string(char *p_src, int src_len, char *p_dest);
	/** Склеивание 3-х строк и помещение их в dest */
	BOOLEAN concat3(const char *str1, const char *str2, const char *str3, char *dest, UINT32 size_dest);
	BOOLEAN concat(char *dest, UINT32 size_dest, UINT8 number, ...);

	char *strstr_rn(char *pos);

	/**
	 * @brief
	 * @param[in] app_v массив, который заполняется строкой вида <V.V.V_DD.MM.YY_HH:MM:SS>, содержащей версию приложения,
	 * дату и время его компиляции
	 * @return
	 */
	void set_app_version(char *app_v);

	/**
	 * @brief
	 * @param[in] dt массив, который заполняется строкой вида <DD.MM.YY_HH:MM:SS>, содержащей дату и время компиляции приложения
	 * @return
	 */
	void set_build_time(char *dt);

#endif

