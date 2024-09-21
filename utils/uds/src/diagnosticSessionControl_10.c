//
// Created by klokov on 24.10.2022.
//

#include "../hdr/diagnosticSessionControl_10.h"
#include "../hdr/uds.h"
#include "../hdr/securityAccess_27.h"
#include "tasks.h"

#define DEFAULT_SESSION_ONE 0x01
#define DEFAULT_SESSION_TWO 0x81
#define PROGRAMMING_SESSION 0x02
#define EXTENDED_DIAGNOSTIC_SESSION_ONE 0x03
#define EXTENDED_DIAGNOSTIC_SESSION_TWO 0xC0

static char currentSession = DEFAULT_SESSION_ONE;

//static UINT16 P2CAN = 0x0032;
//static UINT16 P2CAN_STAR = 0x1388;

static UINT16 pos_resp_2f(char session, char *buf, UINT16 buf_size);

extern UINT16 handler_service10(char *data, char *buf, UINT16 buf_size){
    (void) buf_size;

    // Проверка пакета
    if ((data == NULL) | (data[1] != DIAGNOSTIC_SESSION_CONTROL)){
        return negative_resp(buf, DIAGNOSTIC_SESSION_CONTROL, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Проверка размера (всегда 2 байта)
    if (data[0] != 0x02){
        return negative_resp(buf, DIAGNOSTIC_SESSION_CONTROL, INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT);
    }

    // Проверка неиспользуемых байт
//    for (UINT8 i = 3; i < 8;i++){
//        if (data[i] != 0xAA){
//            return negative_resp(buf, DIAGNOSTIC_SESSION_CONTROL, CONDITIONS_NOT_CORRECT);
//        }
//    }

    return pos_resp_2f(data[2], buf, buf_size);
}

static UINT16 pos_resp_2f(char session, char *buf, UINT16 buf_size){
    (void) buf_size;
    UINT16 size = 0;
    buf[0] = 0x50;
    buf[1] = session & 0x7F;
    switch (session) {
        // Сеанс по умолчанию
        // Этот диагностический сеанс включает диагностический сеанс по умолчанию на сервере
        // и не поддерживает какие-либо условия обработки времени ожидания диагностического приложения (например,
        // для поддержания сеанса в активном состоянии не требуется служба TesterPresent).
        // Если на сервере был активен какой-либо сеанс, отличный от defaultSession, и снова запущен defaultSession,
        // то должны соблюдаться следующие правила реализации.
        // - Сервер должен остановить текущий диагностический сеанс, когда он отправил сообщение положительного ответа
        // DiagnosticSessionControl и должен запустить вновь запрошенный диагностический сеанс после этого.
        // - Если сервер отправил положительное ответное сообщение DiagnosticSessionControl, он должен повторно
        // заблокировать сервер, если клиент разблокировал его во время сеанса диагностики.
        // - Если сервер отправляет отрицательное ответное сообщение c идентификатором службы
        // запроса DiagnosticSessionControl, активный сеанс должен быть продолжен.
        case DEFAULT_SESSION_ONE:
        case DEFAULT_SESSION_TWO:
            size = 2;
            return size;
            // Сессия программирования
            // Этот диагностический сеанс включает все диагностические службы, необходимые для поддержки
            // программирования памяти сервера. Если сервер запускает ProgramSession в загрузочном ПО,
            // ProgramSession должен быть оставлен только через службу ECURset (11 hex),
            // инициированную клиентом, службу DiagnosticSessionControl (10 hex) с типом sessionType, равным defaultSession,
            // или тайм-аут уровня сеанса на сервере.
            // Если сервер работает в загрузочном программном обеспечении, когда он получает
            // Служба DiagnosticSessionControl (10 hex) с типом сеанса, равным
            // defaultSession или тайм-аут сеансового уровня, и в обоих случаях присутствует действительное прикладное
            // программное обеспечение, тогда сервер должен перезапустить прикладное программное обеспечение.
            // Для этого сеанса требуется Security Acess.
        case PROGRAMMING_SESSION:
            // Если нет доступа
            if (!isAccessLevel()){
                return negative_resp(buf, DIAGNOSTIC_SESSION_CONTROL, SECURITY_ACCESS_DENIED);
            }

            // Расширенный сеанс диагностики
            // Этот диагностический сеанс может, использоваться для включения всех диагностических служб, необходимых
            // для поддержки настройки функций в памяти сервера. Также его можно использовать для включения
            // диагностических сервисов, которые специально не привязаны к настройке функций.
        case EXTENDED_DIAGNOSTIC_SESSION_ONE:
        case EXTENDED_DIAGNOSTIC_SESSION_TWO:
//            buf[2] = P2CAN >> 8;
//            buf[3] = P2CAN;
//            buf[4] = P2CAN_STAR >> 8;
//            buf[5] = P2CAN_STAR;
//            size = 6;
            size = 2;
            break;
        default:
            return negative_resp(buf, DIAGNOSTIC_SESSION_CONTROL, SUBFUNCTION_NOT_SUPPORTED);
    }
    // Доступ security acess не сбрасывается через 10 секунд. Он сбрасывается при смене DiagnosticSessionControl (Алексей Николаев)
    if(currentSession != session){
        resetProcedureSA();
    }
    return size;
}