//
// Created by klokov on 08.04.2022.
//
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "m2mb_os_types.h"
#include "log.h"
#include "../utils/prop/hdr/prop.h"
#include "../utils/hdr/char_service.h"
#include "../utils/prop/hdr/prop_manager.h"

static int setInt(PropManager *propManager, char *paramName, int defaultValue);
static float setFloat(PropManager *propManager, char *paramName, float defaultValue);

/**
 * Конструктор экземпляра PropManagers
 * @param fileName адрес имени файла
 * @return экземпляр
 */
extern PropManager *newPropManager(char *fileName) {
    if (isEmpty(fileName)) {
        LOG_DEBUG("PROP_M: File name is NULL or his size leass than 1.");
        return NULL;
    }
    // Выделение памяти для prop
    PropManager *propManager = (PropManager *) m2mb_os_malloc(sizeof(PropManager));
    propManager->prop = newProp(fileName);
    propManager->needSave = TRUE;
    return propManager;
}
/**
 * Удаление экземпляра PropManager
 * @param propManager адрес менеджера
 */
extern void clearPropManager(PropManager *propManager) {
    clearProp(propManager->prop);
    m2mb_os_free(propManager);
    LOG_DEBUG("PROP_M: Clear success.");
}

/**
 * Требуется ли сохранение в фаил
 * @param propManager адрес менеджера
 * @return TRUE/FALSE
 */
extern BOOLEAN isNeedSave(PropManager *propManager) {
    if (propManager == NULL) {
        return FALSE;
    }
    return propManager->needSave;
}

/** Удалить все параметры, находящиеся в RAM.
 * Может потребоваться, если нужно удалить файл со свойствами и пересоздать свойства
 * с новыми значениями
 */
extern void clearAllRam(PropManager *propManager) {
    clearAllRamProp(propManager->prop);
    LOG_DEBUG("PROP_M: Data has been cleared.");
}

extern BOOLEAN isPropertiesExistManger(PropManager *propManager) {
    return isPropertiesExist(propManager->prop);
}

/** Удаление параметра из RAM, для удаления из файла необходимо выполнить сохранение в файл
   * @param paramName - имя удаляемого параметра
   * @param saveToFile - признак необходимости сохранения изменений в файле
  */
extern void removeParam(PropManager *propManager, char *paramName, BOOLEAN saveToFile) {
    if (isEmpty(paramName) || propManager == NULL) {
        LOG_DEBUG("PROP_M: Prop manager is NULL or parameter name is empty.");
        return;
    }
    deleteParam(propManager->prop, paramName);
    if (saveToFile) {
        saveAllToFile(propManager->prop);
    }
}

// Считать все свойства из файла в RAM
extern BOOLEAN readAll(PropManager *propManager) {
    if (propManager != NULL && isPropertiesExistManger(propManager)) {
        BOOLEAN result = readAllFromFile(propManager->prop);
        if(result == TRUE){ // Если чтение файла прошло успешно, то сохранять ничего не нужно
            propManager->needSave = FALSE;
        } else{
            propManager->needSave = TRUE;
        }
        return result;
    }
    return FALSE;
}

// +
extern char *getString(PropManager *propManager, char *paramName, char *defaultValue) {
    if (propManager == NULL || isEmpty(paramName)) {
        LOG_DEBUG("PROP_M: (getString) Prop manager is NULL or parameter name is empty.");
        return multConc(1, "");
    } else {
        char *result = getParam(propManager->prop, paramName);
        if (isEmpty(result)) {
            char *resx = m2mb_os_malloc(strlen(defaultValue) + 1);
            strcpy(resx, defaultValue);
            setString(propManager, paramName, resx, FALSE);
            m2mb_os_free(result);
            propManager->needSave = TRUE;
            return resx;
        }
        return result;
    }
}

// +
extern BOOLEAN setString(PropManager *propManager, char *paramName, char *value, BOOLEAN writeFile) {
//    LOG_DEBUG("ADD %s=%s",paramName, value);
    if (propManager == NULL || isEmpty(paramName)) {
        LOG_DEBUG("PROP_M: Prop manager is NULL or parameter name is empty.");
        return FALSE;
    }
    setParam(propManager->prop, paramName, value);
    if (writeFile == TRUE) {
        return saveAllToFile(propManager->prop);
    }
    propManager->needSave = TRUE;
    return TRUE;
}

extern int getInt(PropManager *propManager, char *paramName, int min, int max, int defaultValue) {
    if (propManager == NULL || isEmpty(paramName)) {
        LOG_DEBUG("PROP_M: Prop manager is NULL or parameter name is empty.");
        return defaultValue;
    }
    char *content = getParam(propManager->prop, paramName);
    if (maybeThisInt(content)) {
        int val = atoi(content);
        if (isOutRangeInt(val, min, max)) {
            return setInt(propManager, paramName, defaultValue);
        }
        return val;
    } else {
        return setInt(propManager, paramName, defaultValue);
    }
}

static int setInt(PropManager *propManager, char *paramName, int defaultValue) {
    if (propManager == NULL || isEmpty(paramName)) {
        LOG_DEBUG("PROP_M: Prop manager is NULL or parameter name is empty.");
        return defaultValue;
    }
    saveIntProp(propManager, paramName, defaultValue, FALSE);
    propManager->needSave = TRUE;
    return defaultValue;
}

// Допустимый диапазон [-2147483648,2147483647]
extern BOOLEAN saveIntProp(PropManager *propManager, char *paramName, int val, BOOLEAN writeToFile) {
    if (propManager == NULL || isEmpty(paramName)) {
        LOG_DEBUG("PROP_M: Prop manager is NULL or parameter name is empty.");
        return FALSE;
    }
    char buffer[50];
    snprintf(buffer, 50, "%d", val);
    setParam(propManager->prop, paramName, buffer);
    if (writeToFile) {
        return saveAllToFile(propManager->prop);
    }
    propManager->needSave = TRUE;
    return TRUE;
}

extern BOOLEAN isOutRangeInt(int value, int min, int max) {
    if (value < min || value > max) {
        return TRUE;
    }
    return FALSE;
}

extern float getFloat(PropManager *propManager, char *paramName, float min, float max, float defaultValue) {
    if (propManager == NULL || isEmpty(paramName)) {
        LOG_DEBUG("PROP_M: Prop manager is NULL or parameter name is empty.");
        return defaultValue;
    }
    char *content = getParam(propManager->prop, paramName);
    LOG_DEBUG("float content %s", content);
    if (maybeThisFloat(content)) {
        float val = atof(content);
        if (isOutRangeFloat(val, min, max)) {
            return setFloat(propManager, paramName, defaultValue);
        }
        return val;
    } else {
        return setFloat(propManager, paramName, defaultValue);
    }
}

static float setFloat(PropManager *propManager, char *paramName, float defaultValue) {
    if (propManager == NULL || isEmpty(paramName)) {
        LOG_DEBUG("PROP_M: Prop manager is NULL or parameter name is empty.");
        return defaultValue;
    }
    saveFloatProp(propManager, paramName, defaultValue, FALSE);
    propManager->needSave = TRUE;
    return defaultValue;
}

extern BOOLEAN isOutRangeFloat(float value, float min, float max) {
    if (value < min || value > max) {
        return TRUE;
    }
    return FALSE;
}

extern BOOLEAN saveFloatProp(PropManager *propManager, char *paramName, float val, BOOLEAN writeToFile) {
    if (propManager == NULL || isEmpty(paramName)) {
        LOG_DEBUG("PROP_M: Prop manager is NULL or parameter name is empty.");
        return FALSE;
    }
    char floatV[200];
    sprintf(floatV, "%g", val);
    setParam(propManager->prop, paramName, floatV);
    if (writeToFile == TRUE) {
        return saveAllToFile(propManager->prop);
    }
    propManager->needSave = TRUE;
    return TRUE;
}

extern BOOLEAN saveAllToFileManager(PropManager *propManager){
    BOOLEAN res = saveAllToFile(propManager->prop);
    if (res == TRUE){
        propManager->needSave = FALSE;
    }
    return res;
}
