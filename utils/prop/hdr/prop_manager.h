//
// Created by klokov on 08.04.2022.
//

#ifndef ERA_PROP_MANAGER_H
#define ERA_PROP_MANAGER_H

#include "../utils/prop/hdr/prop.h"
#include "m2mb_types.h"

/**
 * Менеджер хранения параметров конфигурации.
 * Хранит все параметры в hash-таблице в RAM, что позволяет минимизировать обращения к файловой системе
 * При сохранении в файл создаёт два файла: основной и резервный, в оба прописывает значение md5
 * Ключевые особенности:
 * 1.После создания менеджера необходимо считать все свойства из файла в RAM
 * 2.Затем все параметры будут считываться уже из RAM без обращения к файловой системе
 * 3.При изменении параметра можно либо сразу все параметры сохранить файл, поставив соответствующий флаг в методе сохранения
 * нового значения параметра, либо изменив нужные параметры сохранить их все один раз при изменении последнего параметра из списка
 * 4.Если в файле отсутствует строка с md5, то в этот файл будет автоматически добавлено значение md5. Такой файл устройство может получить,
 * например, при удалённом обновлении конфигурации
 * @author D.Kozenkov
 */
typedef struct PropManager {
    char *filename;   // имя файла, с которым работает данный менеджер
    Prop *prop;       // низкоуровневая обработка данных
    BOOLEAN needSave; // признак необходимости сохранить значения в файл
} PropManager;

/**
 * @brief Создает экземпляр структуры PropManager и возвращает адрес на него
 * @private
 *
 * **Example**
 *
 *     PropManager *propManager = newPropManager("test.txt");
 *
 * @warning может вернуть NULL. Для удаления экземпляра и очистки памяти используйте функцию clearPropManager(PropManager *propManager);
 *
 * @param[in] fileName имя файла + расширение. Без слешей (пример: "test.txt")
 *
 * @return экземпляр структуры PropManager. Может вернуть NULL если имя файла пустая строка или NULL.
 */
extern PropManager *newPropManager(char *fileName);

/**
 * @brief Требуется ли сохранить в фаил данные propManager.
 *
 * @param[in] propManager адрес экземпляра propManager.
 *
 * @return TRUE если нужно сохранить. В противном случае FALSE.
 */
extern BOOLEAN isNeedSave(PropManager *propManager);

/**
 * @brief Удаляет экземпляр PropManager. Очищает память
 *
 * @param[in] propManager адрес экземпляра PropManager который нужно удалить
 */
extern void clearPropManager(PropManager *propManager);

/**
 * @brief Удаляет все имеющиеся проперти для данного propManager.
 *
 * @param[in] propManager адрес экземпляра propManager.
 */
extern void clearAllRam(PropManager *propManager);

/**
 * @brief Удаляет параметр из propManager по его ключу.
 *
 * @param[in] propManager адрес экземпляра propManager.
 * @param[in] paramName адрес имени параметра.
 * @param[in] saveToFile требуется ли сохранять файл после удаления TRUE/FALSE.
 */
extern void removeParam(PropManager *propManager, char *paramName, BOOLEAN saveToFile);

/**
 * @brief Считывает проперти из файла.
 *
 * @param[in] propManager адрес экземпляра propManager.
 *
 * @return TRUE если чтение прошло успешно. В противном случае FALSE.
 */
extern BOOLEAN readAll(PropManager *propManager);

/**
 * @brief Проверяет существование файла для данного propManager.
 *
 * @param[in] propManager адрес экземпляра propManager.
 *
 * @return TRUE если файл существует. В противном случае FALSE.
 */
extern BOOLEAN isPropertiesExistManger(PropManager *propManager);

/**
 * @brief Достает параметр из propManager по его ключу.
 *
 * @warning если значение параметра оказалось пустой строкой,
 * то оно заменяется значением по умолчанию. Требуется сохранить изменения в файл.
 *
 * @param[in] propManager адрес экземпляра propManager.
 * @param[in] paramName адрес имени параметра.
 * @param[in] defaultValue адрес значения параметра по умолчанию.
 *
 * @return значение параметра.
 */
extern char *getString(PropManager *propManager, char *paramName, char *defaultValue);

/**
 * @brief Добавляет в propManager новый параметр с типом значения (char*).
 *
 * @param[in] propManager адрес экземпляра propManager.
 * @param[in] paramName адрес имени параметра.
 * @param[in] value адрес значение параметра по умолчанию.
 * @param[in] writeFile нужно ли сохранять файл после добавления.
 *
 * @return TRUE - если параметр успешно добавлен. Иначе - FALSE.
 */
extern BOOLEAN setString(PropManager *propManager, char *paramName, char *value, BOOLEAN writeFile);

/**
 * @brief Достает из проперти значение типа int по его ключу. (Для int)
 *
 * @warning если значение не может быть полученно как int, то оно заменяется значением defaultValue
 * если полученное значение больше чем max или меньше чем min, то оно заменяется значением defaultValue
 *
 * @param[in] propManager адрес экземпляра propManager.
 * @param[in] paramName адрес имени параметра.
 * @param[in] min нижняя граница диапазона.
 * @param[in] max верхняя граница диапазона.
 * @param[in] defaultValue значение по умолчанию.
 *
 * @return TRUE - если параметр успешно добавлен. Иначе - FALSE.
 */
extern int getInt(PropManager *propManager, char *paramName, int min, int max, int defaultValue);

/**
 * @brief Проверяет что value находится в диапазоне от min до max (не включительно). (Для int)
 *
 * @param[in] value проверяемое значение.
 * @param[in] min нижняя граница диапазона.
 * @param[in] max верхняя граница диапазона.
 *
 * @return TRUE - если проверяемое значение находится вне диапазона. Иначе - FALSE.
 */
extern BOOLEAN isOutRangeInt(int value, int min, int max);

/**
 * @brief Достает из проперти значение типа float по его ключу. (Для float)
 *
 * @warning если значение не может быть полученно как float, то оно заменяется значением defaultValue
 * если полученное значение больше чем max или меньше чем min, то оно заменяется значением defaultValue
 *
 * @param[in] propManager адрес экземпляра propManager.
 * @param[in] paramName адрес имени параметра.
 * @param[in] min нижняя граница диапазона.
 * @param[in] max верхняя граница диапазона.
 * @param[in] defaultValue значение по умолчанию.
 *
 * @return TRUE - если параметр успешно добавлен. Иначе - FALSE.
 */
extern float getFloat(PropManager *propManager, char *paramName, float min, float max, float defaultValue);

/**
 * @brief Проверяет что value находится в диапазоне от min до max (не включительно). (Для float)
 *
 * @param[in] value проверяемое значение.
 * @param[in] min нижняя граница диапазона.
 * @param[in] max верхняя граница диапазона.
 *
 * @return TRUE - если проверяемое значение находится вне диапазона. Иначе - FALSE.
 */
extern BOOLEAN isOutRangeFloat(float value, float min, float max);

/**
 * @brief Сохраняет все проперти для указанного propManager в файл.
 *
 * @param[in] propManager адрес propManager.
 *
 * @return TRUE - если сохранение прошло успешно. Иначе - FALSE.
 */
extern BOOLEAN saveAllToFileManager(PropManager *propManager);

/**
 * @brief Добавляет в propManager параметр со значением типа int.
 *
 * @param[in] propManager адрес propManager.
 * @param[in] paramName имя параметра.
 * @param[in] val значение параметра.
 * @param[in] writeToFile требуется ли запись в файл.
 *
 * @return TRUE - если параметр успешно добавлен. Иначе - FALSE.
 */
extern BOOLEAN saveIntProp(PropManager *propManager, char *paramName, int val, BOOLEAN writeToFile);

/**
 * @brief Добавляет в propManager параметр со значением типа float.
 *
 * @param[in] propManager адрес propManager.
 * @param[in] paramName имя параметра.
 * @param[in] val значение параметра.
 * @param[in] writeToFile требуется ли запись в файл.
 *
 * @return TRUE - если параметр успешно добавлен. Иначе - FALSE.
 */
extern BOOLEAN saveFloatProp(PropManager *propManager, char *paramName, float val, BOOLEAN writeToFile);
#endif //ERA_PROP_MANAGER_H
