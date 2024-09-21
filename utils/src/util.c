/*
 * util.c
 * Created on: 25.03.2022, Author: DKozenkov
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "util.h"

#include <factory_param.h>

#include "azx_string_utils.h"
#include "buildTime.h"
#include "log.h"

/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/

/* Local statics ================================================================================*/
/* Local function prototypes ====================================================================*/

/* Static functions =============================================================================*/
/* Global functions =============================================================================*/

BOOLEAN get_int(char *eq, char *end, INT32 *var) {
    if (eq >= end - 1) {
        return FALSE;
    }
    int size = end - eq;
    char str[size];
    str[size - 1] = 0;
    memcpy(str, ++eq, size - 1);
    if (azx_str_to_l(str, var) == 0) {
        //LOG_DEBUG("val: %i", *var);
        return TRUE;
    } else return FALSE;
}

BOOLEAN get_uint(char *eq, char *end, UINT32 *var) {
    if (eq >= end - 1) {
        return FALSE;
    }
    int size = end - eq;
    char str[size];
    str[size - 1] = 0;
    memcpy(str, ++eq, size - 1);
    if (azx_str_to_ul(str, var) == 0) {
        //LOG_DEBUG("val: %i", *var);
        return TRUE;
    } else return FALSE;
}

BOOLEAN get_str(char *eq, char *end, char *arg) {
    if (eq >= end - 1) return FALSE;
    int size = end - eq;
    memcpy(arg, ++eq, size - 1);
    return TRUE;
}

int mul(int *a, int *mul) {
    LOG_INFO("mul func for: %i, %i", *a, *mul);
    return (*a) * (*mul);
}

void str_to_hex(char *p_src, int src_len, char *p_dest) {
    char h1, h2;
    char s1, s2;
    int i;

    for (i = 0; i < src_len / 2; i++)	{
        h1 = p_src[2*i];
        h2 = p_src[2*i+1];

        s1 = toupper(h1) - 0x30;
        if (s1 > 9)
            s1 -= 7;
        s2 = toupper(h2) - 0x30;
        if (s2 > 9)
            s2 -= 7;

        p_dest[i] = s1*16 + s2;
    }
}

int hex_to_string(char *p_src, int src_len, char *p_dest) {
    int i = 0;
    for(i = 0; i < src_len; i++) {
        sprintf((char *)(p_dest + i * 2), "%02X", *(p_src + i));
    }
    *(p_dest + i * 2) = '\0';
    return  (i * 2);
}

BOOLEAN concat3(const char *str1, const char *str2, const char *str3, char *dest, UINT32 size_dest) {
    dest[0] = 0;
    if (strlen(str1) + strlen(str2) + strlen(str3) + 1 > size_dest) {
        LOG_ERROR("Errct3");
        return FALSE;
    }
    strcat(dest, str1);
    strcat(dest, str2);
    strcat(dest, str3);
    return TRUE;
}

BOOLEAN concat(char *dest, UINT32 size_dest, UINT8 number, ...) {
    dest[0] = 0;
    va_list ap; // @suppress("Type cannot be resolved")
    va_start(ap, number);
    for (int i = 0; i < number; i++) {
        CHAR* str = va_arg(ap, CHAR*);
        size_dest -= strlen(str);
        if (size_dest <= 0) {
            va_end(ap);
            return FALSE;
        }
        strcat(dest, str);
    }
    va_end(ap);
    return TRUE;
}

char *strstr_rn(char *pos) {
    while (*pos != 0) {
        if (*pos == '\r' && *(pos + 1) == '\n') return pos;
        pos++;
    }
    return NULL;
}

void set_app_version(char *app_v) {
    sprintf(app_v, "%s_%02i%02i%i_%02i%02i%02i", VERSION, BUILD_DAY, BUILD_MONTH, BUILD_YEAR, BUILD_HOUR, BUILD_MIN, BUILD_SEC);
}

void set_build_time(char *dt) {
    char sw[4];
    get_sw_version(sw, 4);
//    sprintf(dt, "%02i%02i%i%02i%02i", 11, 04, 23, 15, 26); // Для R-TEL
    sprintf(dt, "%02i%02i%i%02i%02i %s", BUILD_DAY, BUILD_MONTH, BUILD_YEAR, BUILD_HOUR, BUILD_MIN, sw);
}
