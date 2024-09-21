//
// Created by klokov on 05.04.2022.
//

#ifndef DATA_STRUCTURE_CHAR_SERVICE_H
#define DATA_STRUCTURE_CHAR_SERVICE_H

extern char *multConc(int count, ...);

extern char *strconc(char *str1, char *str2);

extern void trim(char *s);

extern BOOLEAN isEmpty(char *str);

extern BOOLEAN maybeThisFloat(char *str);

extern BOOLEAN maybeThisInt(char *str);

extern void clearString(char *str);

extern char** str_split(char* a_str, const char a_delim);

extern int compare_strings (const void *a, const void *b);

extern void deleteChar(char *str,int pos);
#endif //DATA_STRUCTURE_CHAR_SERVICE_H
