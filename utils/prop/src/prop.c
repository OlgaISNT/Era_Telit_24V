/*
 * prop.c
 *
 *  Created on: 26 мар. 2022 г.
 *      Author: klokov
 */
#include "../utils/prop/hdr/prop.h"
#include "string.h"
#include "app_cfg.h"
#include "m2mb_types.h"
#include "m2mb_fs_stdio.h"
#include "m2mb_fs_posix.h"
#include "log.h"
#include "md5_utils.h"
#include "m2mb_os_types.h"
#include "m2mb_os.h"
#include "../utils/hdr/char_service.h"
#include <stdio.h>
#include <zconf.h>

static BOOLEAN addToTree(Tree *tree, char *content);
static BOOLEAN deleteFile(char *fileName);
static BOOLEAN writeToFile(char *fileName, char *content);
static status readParam(Prop *prop, char *fileName);
static BOOLEAN checkBackupFile(Prop *prop);
static char *propGetContent(Prop *prop);
/**
 * Конструктор экземпляра структуры Prop
 * @param fName адрес файла
 * @return адрес экземпляра
 */
extern Prop *newProp(char *fName) {
    // Проверка имени файла
    if (isEmpty(fName)) {
        LOG_DEBUG("PROP: File name is NULL or his size leass than 1.");
        return NULL;
    }
    // Выделение памяти для prop
    Prop *prop = (Prop *) m2mb_os_malloc(sizeof(Prop));
    prop->tree = newTree(STRING_ELEM, STRING_ELEM);
    prop->filename = multConc(3, LOCALPATH, "/", fName);
    prop->resFileName = multConc(4, LOCALPATH, "/", RES_POSTFIX, fName);
    prop->propStatus = UNDEFINED_PROP;
    return prop;
}

/**
 * Распечатать содержимое экземпляра prop
 * @param prop адрес экземпляра
 */
extern char *printProp(Prop *prop) {
    if (prop == NULL) {
        return multConc(1, "");
    }
    return toString(prop->tree->root);
}

/**
 * Достать из prop значение параметра по его ключу.
 * Может вернуть NULL
 * @param prop адрес экземпляра prop
 * @param key адрес ключа ячейки
 * @return адрес значения ячейки
 */
extern char *getParam(Prop *prop, char *key) {
    if (prop == NULL || isEmpty(key)) {
        LOG_DEBUG("PROP: (getParam) prop is NULL or key is empty");
        return NULL;
    }
    // Ссылка на значение внутри ячейки
    char *proxy = getElementTree(prop->tree, key).string;
    char *res = m2mb_os_malloc(strlen(proxy) + 1);
    strcpy(res, proxy);
    // Возвращаем копию, чтобы нельзя было удалить напрямую и оставить ячейку без значения
    return res;
}

extern void clearAllRamProp(Prop *prop) {
    freeTree(prop->tree);
    prop->tree = newTree(STRING_ELEM, STRING_ELEM);
}

extern BOOLEAN isPropertiesExist(Prop *prop) {
    INT32 fileStream = m2mb_fs_open(prop->filename, M2MB_O_RDONLY);
    if (fileStream == -1) {
        LOG_ERROR("PROP: File does not exist. File name = %s", prop->filename);
        return FALSE;
    }
    LOG_DEBUG("PROP: File %s is exists", prop->filename);
    m2mb_fs_close(fileStream);
    return TRUE;
}

extern BOOLEAN readAllFromFile(Prop *prop) {
    if (prop == NULL) {
        LOG_DEBUG("PROP: Prop is NULL");
        return FALSE;
    }
    status stat = readParam(prop, prop->filename);
    Prop *backupFile = newProp("test");
    backupFile->filename = strdup(prop->filename);
    backupFile->resFileName = strdup(prop->resFileName);
    switch (stat) {
        case OK:
            // Если основной файл прочитан успешно, то необходимо проверить целостность резервного файла
            // Он может отличаться содержимым от основного
            checkBackupFile(backupFile);
            return TRUE;
        case NO_MD5:  /* Если в основном файле отсутствует строка с md5, то в этот файл будет автоматически добавлено значение md5. Такой файл устройство может получить,
             например, при удалённом обновлении конфигурации */
            writeToFile(prop->filename, propGetContent(prop));
            checkBackupFile(backupFile); // Проверка резервного файла
            return TRUE;
        case ERROR: // Если при чтении основного файла обнаружен именно неправильный дайджест
        default:
            clearAllRamProp(prop); // Очищаем все, что считали
            // Для чтения резервного файла, он должен существовать
            stat = readParam(prop, prop->resFileName); // Считываем резервный фаил
            if (stat == OK) { // Если резервный фаил считан успешно
                return saveAllToFile(prop); // Перезаписываем оба файла содержимым из резервного
            } else{
                // Если резервный файл оказывается поврежден
                clearAllRamProp(prop); // Очищаем все, что считали
            }
            // Оба файла содержат ошибки
            // Оба файла будут переписаны со значениями из памяти
            return FALSE;
    }
}

static BOOLEAN checkBackupFile(Prop *prop){
    // Читаем резервный файл
    status backStatus = readParam(prop, prop->resFileName);
    switch (backStatus) {
        case OK:
            return TRUE;
        case NO_MD5:
            /* Если в файле отсутствует строка с md5, то в резервный файл будет автоматически добавлено значение md5.
             * Такой файл устройство может получить, например, при удалённом обновлении конфигурации
             * Считанное содержимое записывается обратно в резервный файл вместе с дайджестом
             * */
            deleteFile(prop->resFileName);
            writeToFile(prop->resFileName, propGetContent(prop));
            return TRUE;
        case ERROR:
            // Если при чтении резервного файла был обнаружен неправильный дайджест
            // или отсутствие файла, то записываем в него содержимое из основного файла
            clearAllRamProp(prop);
            readParam(prop, prop->filename);
            deleteFile(prop->resFileName);
            writeToFile(prop->resFileName, propGetContent(prop));
            return writeToFile(prop->resFileName, propGetContent(prop));; // Записываем в резервный файл данные из основного
        default:
            return FALSE;
    }
}

/**
 * Считать все проперти из файла в таблицу
 * @param prop адрес экземпляра Prop
 * @return возвращает статус
 */
static status readParam(Prop *prop, char *fileName) {
    char content[100];
    char *currentData = NULL;
    char *prevData = NULL;
    BOOLEAN hasMd5 = 0;
    M2MB_FILE_T *fileStream = m2mb_fs_fopen(fileName, "r");
    if (fileStream == NULL) {
        prop->propStatus = ERROR;
        LOG_DEBUG("PROP: Cannot open file to read. File name = %s", fileName);
    } else {
        LOG_DEBUG("PROP: The file is open and will be read. File name = %s", fileName);
        memset(content, 0, sizeof(content));
        while (m2mb_fs_fgets(content, sizeof(content), fileStream)) {
            if (strstr(content, MD5) != NULL) {
                hasMd5 = TRUE;
                goto down;
            }

            if (currentData == NULL && prevData == NULL) {
                currentData = strconc("", content);
                prevData = m2mb_os_malloc(strlen(currentData) + 1);
                strcpy(prevData, currentData);
            } else {
                m2mb_os_free(currentData);                          // освобождаем память currentData
                currentData = strconc(prevData, content);   // переписываем currentData
                m2mb_os_free(prevData);                             // Освобождаем prevData
                prevData = m2mb_os_malloc(strlen(currentData) + 1); // Переписываем prevData
                strcpy(prevData, currentData);              // В prevData переписывается currentData
            }
            down:
            trim(content);
            if (addToTree(prop->tree, content) == FALSE) {
                LOG_DEBUG("PROP: Parameter specified with an error. %s", content);
            }
        }
        m2mb_fs_fclose(fileStream);
        if (hasMd5 == TRUE) {
            UINT8 computedHash[16] = {0};
            if(isEmpty(currentData) == TRUE){
                currentData = multConc(1,"");
            }
            md5_computeSum(computedHash, (UINT8 *) currentData, strlen(currentData));
            value_tree_t v = getElementTree(prop->tree, MD5);
            if (md5_compareHashWithString(computedHash, sizeof(computedHash), v.string)) {
                prop->propStatus = OK;
                LOG_DEBUG("PROP: File read successfully. MD5 successfully verified.");
            } else {
                prop->propStatus = ERROR;
                LOG_DEBUG("PROP: File read successfully. MD5 does not match.");
            }
            deleteByKeyTree(prop->tree, MD5);
        } else {
            prop->propStatus = NO_MD5;
            LOG_DEBUG("PROP: The file does not contain MD5.");
        }
        m2mb_os_free(currentData);
        m2mb_os_free(prevData);
    }
    return prop->propStatus;
}

/**
 * Разбирает считанную строку на ключ и значение и добавляет в таблицу
 * @param tab адрес таблице
 * @param content адрес строки
 */
static BOOLEAN addToTree(Tree *tree, char *content) {
    if (strchr(content, '=') == NULL || content[0] == '=') {
        return FALSE;
    }
    char *last;
    addElementTree(tree, strtok_r(content, SEPARATOR, &last), strtok_r(NULL, SEPARATOR, &last));
    return TRUE;
}

extern void clearProp(Prop *prop) {
    freeTree(prop->tree);
    m2mb_os_free(prop->filename);
    m2mb_os_free(prop->resFileName);
    memset(prop, 0, (int) sizeof(prop));
    m2mb_os_free(prop);
    prop = NULL;
}

extern char* propToString(Prop *prop){
    return toStringSortByAlph(prop->tree->root);
}

static char *propGetContent(Prop *prop){
    char *data = toStringSortByAlph(prop->tree->root);
    UINT8 computedHash[16] = {0};
    char hashString[33] = {0};
    md5_computeSum(computedHash, (UINT8 *) data, strlen(data));
    md5_hashToString(computedHash, hashString);
    return multConc(4, data, MD5, "=", hashString);
}

extern BOOLEAN saveAllToFile(Prop *prop) {
    char *content = propGetContent(prop);
    char *data = toStringSortByAlph(prop->tree->root);
    deleteFile(prop->filename);                        // Удаляем исходный файл
    // Если при переписывании исходного файла была ошибка, то резервный не трогаем
    if (writeToFile(prop->filename, content) == TRUE) { // Переписываем исходный файл
        deleteFile(prop->resFileName);                 // Удаляем резервный файл
        writeToFile(prop->resFileName, content);       // Переписываем резервный файл
    } else {
        m2mb_os_free(content);
        m2mb_os_free(data);
        return FALSE;
    }
    m2mb_os_free(content);
    m2mb_os_free(data);
    return TRUE;
}

static BOOLEAN deleteFile(char *fileName) {
    unsigned int res = -1;
    res = m2mb_fs_unlink(fileName);
    if (res == 0) {
        LOG_TRACE("PROP: Deleted successfully. Filename = %s", fileName);
        return TRUE;
    } else {
        LOG_ERROR("PROP: Deletion failed. Filename = %s", fileName);
        return FALSE;
    }
}

static BOOLEAN writeToFile(char *fileName, char *content) {
    unsigned int res = -1;
    FILE *fileStream = fopen(fileName, "w+");
    if (fileStream == NULL){
        LOG_ERROR("PROP: Error writing to file. "
                  "The current number of elements = %d. "
                  "Expected number of elements = %d.\n", res, strlen(content));
        return FALSE;
    }

    res = fwrite(content, 1, strlen(content), fileStream);
    if (res != strlen(content)) {
        LOG_ERROR("PROP: Error writing to file. "
                  "The current number of elements = %d. "
                  "Expected number of elements = %d.\n", res, strlen(content));
        fclose(fileStream);
        return FALSE;
    } else {
        LOG_TRACE("PROP: Writing to file completed successfully.");
        fflush(fileStream);
        int fd = fileno(fileStream);
        fsync(fd);
        fclose(fileStream);
        return TRUE;
    }
}

/**
 * Добавляем параметр в prop. Если параметр с таким ключем уже есть, то он будет переписан
 * @param prop адрес пропса
 * @param key адрес ключа
 * @param value адрес значения
 */
extern void setParam(Prop *prop, char *key, char *value) {
    if (prop == NULL || isEmpty(key)) {
        LOG_ERROR("PROP: Prop is NULL or key is empty");
        return;
    }
    addElementTree(prop->tree, key, value);
}

extern void deleteParam(Prop *prop, char *key) {
    if (prop == NULL || key == NULL) {
        LOG_DEBUG("PROP: Prop is NULL or key is empty");
        return;
    }
    deleteByKeyTree(prop->tree, key);
}
