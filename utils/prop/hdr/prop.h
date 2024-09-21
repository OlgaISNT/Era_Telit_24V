/*
 * prop.h
 *
 *  Created on: 26 мар. 2022 г.
 *      Author: klokov
 */
#ifndef SRC_COM_ITELMA_UTILS_PROPS_PROP_H_
#define SRC_COM_ITELMA_UTILS_PROPS_PROP_H_

#include "../utils/hdr/tree.h"
#include "m2mb_types.h"

#define RES_POSTFIX "_"
#define MD5 "MD5"
#define SEPARATOR "="

typedef enum {
    OK = 0x00,
    NO_MD5 = 0x01,
    ERROR = 0x02,
    UNDEFINED_PROP = 0x03
} status; // Статус чтения

typedef struct Prop {
    char *filename;  // Ссылка на имя файла
    char *resFileName;  // Ссылка на имя файла
    Tree *tree;
    status propStatus;
} Prop;


/**
 * @brief Создает экземпляр структуры Prop и возвращает адрес на него
 * @private
 *
 * **Example**
 *
 *     Prop *prop = newProp("test.txt");
 *
 * @warning может вернуть NULL. Для удаления экземпляра и очистки памяти используйте функцию clearProp(Prop *prop);
 *
 * @param[in] fName имя файла + расширение. Без слешей (пример: "test.txt")
 *
 * @return экземпляр структуры Prop. Может вернуть NULL если имя файла пустая строка или NULL.
 */
extern Prop *newProp(char *fName);

/**
 * @brief Удаляет экземпляр Prop. Очищает память
 *
 * @param[in] prop адрес экземпляра Prop который нужно удалить
 */
extern void clearProp(Prop *prop);

/**
 * @brief Возвращает содержимое prop в одну строку. (аналог toString() в Java)
 *
 * @warning если полученная строка перестала быть нужной вам,
 * то ее нужно очистить при помощи free() или clearString().
 * Перед использовании данной функции необходимо выполнить чтение из файла при помощи readAllFromFile();
 *
 * @param[in] prop адрес экземпляра prop.
 *
 * @return содержимое prop.
 */
extern char *printProp(Prop *prop);

/**
 * @brief Ищет значение параметра по его ключу.
 *
 * @warning Перед использованием данной функции необходимо выполнить чтение из файла при помощи readAllFromFile().
 * Если по указанному ключу ничего небыло найдено, то возвращается пустая строка - "".
 * Если полученная строка перестала быть нужной вам,
 * то ее нужно очистить при помощи free() или clearString(). Даже если это пустая строка.
 *
 * @param[in] prop адрес экземпляра prop.
 * @param[in] key адрес ключа.
 *
 * @return возвращает копию значения, хранящегося внутри prop или пустую строку.
 */
extern char *getParam(Prop *prop, char *key);

/**
 * @brief Сохраняет текущее содержимое prop в основной и резервный файлы.
 *
 * @param[in] prop адрес экземпляра prop.
 *
 * @return TRUE если сохранение в оба файла прошло успешно. В противном случае FALSE.
 */
extern BOOLEAN saveAllToFile(Prop *prop);

/**
 * @brief Добавляет в prop новый параметр.
 *
 * @warning Если адрес prop и/или ключа - NULL, то ничего не добавляет.
 * Если адрес ключа указывает на пустую строку, то ничего не добавляет.
 * Если prop и ключ оказались допустимы, но адрес значения NULL или указывает на пустую строку,
 * то вместо значения добавляется пустая строка.
 *
 * @param[in] prop адрес экземпляра prop.
 * @param[in] key адрес ключа.
 * @param[in] value адрес значения.
 */
extern void setParam(Prop *prop, char *key, char *value);

/**
 * @brief Удаляет параметр из prop по его ключу.
 *
 * @warning Если адрес prop и/или ключа - NULL, то ничего не удаляет.
 * Если адрес ключа указывает на пустую строку, то ничего не удаляет.
 * Если prop и ключ оказались допустимы, но адрес значения NULL или указывает на пустую строку,
 * то ничего не удаляет.
 * Если по указанному ключу ничего не было найдено, то ничего не удаляет
 *
 * @param[in] prop адрес экземпляра prop.
 * @param[in] key адрес ключа.
 */
extern void deleteParam(Prop *prop, char *key);

/**
 * @brief Считывает в prop все параметры из файла
 *
 * @param[in] prop адрес экземпляра prop.
 *
 * @return TRUE если чтение прошло успешно. В противном случае FALSE.
 */
extern BOOLEAN readAllFromFile(Prop *prop);

/**
 * @brief Удаляет все имеющиеся проперти для данного prop.
 *
 * @param[in] prop адрес экземпляра prop.
 */
extern void clearAllRamProp(Prop *prop);

/**
 * @brief Проверяет наличие файла с пропертями.
 *
 * @param[in] prop адрес экземпляра prop.
 *
 * @return TRUE если файл существует. В противном случае FALSE.
 */
extern BOOLEAN isPropertiesExist(Prop *prop);


/**
 * @brief Возвращает содержимое prop в 1 строку.
 *
 * @param[in] prop адрес экземпляра prop.
 *
 * @return указатель.
 */
extern char* propToString(Prop *prop);

#endif /* SRC_COM_ITELMA_UTILS_PROPS_PROP_H_ */
