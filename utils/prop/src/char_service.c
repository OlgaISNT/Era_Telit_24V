//
// Created by klokov on 05.04.2022.
//

#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "m2mb_os_types.h"
#include "log.h"
#include "m2mb_os.h"
#include "m2mb_os_types.h"
#include "../utils/hdr/char_service.h"

/**
 * Склеивает две строки. В куче выделяет память под новую строку.
 * Удалять потом из памяти нужно самому.
 * @param str1 адрес первой строки
 * @param str2 адрес второй строки
 * @return адрес результирующей строки
 */
extern char *strconc(char *str1, char *str2) {
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    char *result = m2mb_os_malloc(len1 + len2 + 1);
    if (!result) {
        return NULL;
    }
    memcpy(result, str1, len1);
    memcpy(result + len1, str2, len2 + 1);
    return result;
}

/**
 * Множественная конкатенация строк.
 * Чистить память после результирующей строки нужно самому.
 * @param count количество строк.
 * @param ... адреса строк.
 * @return адрес результирующей строки.
 */
extern char *multConc(int count, ...) {
    va_list args;
    va_start(args, count);
    char *currentStr;
    char *prevRes = NULL;
    char *currRes = NULL;
    for (int i = count; i--;) {
        currentStr = va_arg(args,
        char *);
        if (currRes == NULL) {
            currRes = strconc("", currentStr);
            prevRes = strconc("", currRes);
        } else {
            if (i == (count - 2)) {
                m2mb_os_free(currRes);
            }
            currRes = strconc(prevRes, currentStr);
            m2mb_os_free(prevRes);                          // Освобождаем prevData
            prevRes = m2mb_os_malloc(strlen(currRes) + 1);  // Переписываем prevData
            strcpy(prevRes, currRes);
            if (i != 0) {
                m2mb_os_free(currRes);
            }
        }
    }
    m2mb_os_free(prevRes);
    va_end(args);
    return currRes;
}

/**
 * Убирает из строки нечитаемые символы
 * @param s адрес строки
 */
extern void trim(char *s) {
    UINT8 i = 0, j;
    while ((s[i] == ' ') || (s[i] == '\t') || (s[i] == '\n') || (s[i] == '\r')) {
        i++;
    }
    if (i > 0) {
        for (j = 0; j < strlen(s); j++) {
            s[j] = s[j + i];
        }
        s[j] = '\0';
    }

    i = strlen(s) - 1;
    while ((s[i] == ' ') || (s[i] == '\t') || (s[i] == '\n') || (s[i] == '\r')) {
        i--;
    }
    if (i < (strlen(s) - 1)) {
        s[i + 1] = '\0';
    }
}

// Проверка пустой строки
extern BOOLEAN isEmpty(char *str){
    if(str == NULL || strlen(str) < 1){
        return TRUE;
    }
    return FALSE;
}

// Можно ли строку полноценно распарсить только во float
extern BOOLEAN maybeThisFloat(char *str){
    if(isEmpty(str)){
        return FALSE;
    }
    trim(str);
    int len;
    float ignore;
    int ret = sscanf(str, "%f %n", &ignore, &len);
    int dotNum = 0;
    for(size_t i = 0; i < strlen(str); i++){
        if (str[i] == '.'){
            dotNum++;
        }
    }
    if(ret==1 && !str[len] && dotNum == 1){
        return TRUE;
    }
    return FALSE;
}

// Можно ли строку полноценно распарсить только во int
extern BOOLEAN maybeThisInt(char *str){
    if(isEmpty(str)){
        return FALSE;
    }
    trim(str);
    int len;
    float ignore;
    int dotNum = 0;
    for(size_t i = 0; i < strlen(str); i++){
        if (str[i] == '.'){
            dotNum++;
        }
    }
    int ret = sscanf(str, "%d %n", &ignore, &len);
    if(ret==1 && !str[len] && dotNum == 0){
        return TRUE;
    }
    return FALSE;
}

extern void clearString(char *str){
//    memset(str,0,strlen(str));
    m2mb_os_free(str);
    str = NULL;
}

extern char** str_split(char* a_str, const char a_delim){
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
    count++;
    result = m2mb_os_malloc(sizeof(char*) * count);
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);
        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    return result;
}

extern int compare_strings (const void *a, const void *b) {
    return strcmp(*(char **)a, *(char **)b);
}

extern void deleteChar(char *str,int pos){
    unsigned int i;
    for(i=pos;i<strlen(str);++i){
        str[i]=str[i+1];
    }
}