/**
 * @file indication.h
 * @version 1.0.0
 * @date 26.04.2022
 * @author: DKozenkov
 */
#ifndef HDR_INDICATION_H_
#define HDR_INDICATION_H_

typedef enum {
	SHOW_IGN_ON,           // Включение зажигания
	SHOW_IGN_OFF,          // Выключение зажигания
	SHOW_ECALL,            // Начало экстренного/тестового вызова.
	SHOW_ECALL_DIALING,    // Начался дозвон в режиме экстренный/тестовый вызов
	SHOW_SENDING_MSD,      // Началась передача МНД
	SHOW_RING_SIGNAL,	   // Отобразить входящий сигнал RING ("красный индикатор" согласно требованию А.Архипова от 21.02.24)
	CANCEL_SHOW_RING_SIGNAL,// Прекратить отображать входящий сигнал RING и вернуться к отображению предыдущего состояния ("зелёный индикатор" согласно требованию А.Архипова от 21.02.24)
	SHOW_VOICE_CHANNEL,    // Подключение голосового канала
	SHOW_ECALL_IMPOSSIBLE, // Экстренный/тестовый вызов невозможен
	SHOW_TEST_STARTED,     // Запуск тестирования
	CANCEL_FLASHING,       // Отмена мигания индикаторов
	OFF_SOS,               // Безусловное выключение всех индикаторов
	SHOW_FAILURE,          // Появилась неисправность
	SHOW_NO_FAILURE,       // Неисправностей нет
    NO_TASK,               // Отсутствие задачи
    ACTIVATED,             // БЭГ активирован
    NO_ACTIVATED,          // БЭГ не активирован
    ENABLE_SPEC_FUNC,      // Специальные функции включены (только для Cherry с каном)
    DISABLE_SPEC_FUNC      // Специальные функции отключены (Только для Cherry с каном)
} IND_TASK_COMMAND; // Задачи для сервиса индикации

typedef enum {
    RED,
    GREEN,
    YELLOW,
    NO_LIGHT
} COLOUR;

extern IND_TASK_COMMAND getCurrenTask(void);
/**
 * @brief ! Не вызывать эту функцию напрямую, она запускается в отдельном потоке
 */
INT32 INDICATION_task(INT32 type, INT32 param1, INT32 param2);

#endif /* HDR_INDICATION_H_ */

