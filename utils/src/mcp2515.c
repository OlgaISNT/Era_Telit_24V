//
// Created by klokov on 14.09.2022.
//
#include "../hdr/mcp2515.h"
#include "log.h"
#include <string.h>
#include <m2mb_os_types.h>
#include <m2mb_os_api.h>
#include "m2mb_types.h"
#include "m2mb_spi.h"
#include "azx_log.h"
#include "azx_utils.h"
#include "tasks.h"
#include "can_util.h"
#include "factory_param.h"
#include "at_command.h"
#include "sys_param.h"
#include <time.h>
#include <unistd.h>
#include <mntent.h>
#include <bzlib.h>

// Mutex
static M2MB_OS_MTX_ATTR_HANDLE can_mtx_attr_handle;
static M2MB_OS_MTX_HANDLE can_mtx_handle = NULL;
static M2MB_OS_RESULT_E mut_init(void);
static void get_mutex(void);
static void put_mutex(void);


// SPI
static INT32 spi_fd;                    // Файловый дескриптор подключения по SPI
static M2MB_SPI_CFG_T cfg;              // Конфигурация подключения по SPI
static char send_buff[16];              // Буфер для отправки
static char recv_buff[16];              // Буфер для чтения
static BOOLEAN initSPI(void);           // Инициализация SPI
static BOOLEAN getSpiConnection(void);  // Получить подключение по SPI
static BOOLEAN setSpiConfig(void);      // Сконфигурировать подключение по SPI
static BOOLEAN closeSPI(void);          // Деинициализация SPI
static void clearBuffers(void);         // Очистка буферов приема и передачи
static void printSendBuff(void);        // Вывести в консоль содержимое буфера для отправки
static void printRecvBuff(void);        // Вывести в консоль содержимое буфера для приема
static BOOLEAN writeToMultipleRegisters(char start, char end, char data); // Записать один и тот же байт в несколько регистров подряд
static BOOLEAN setSpeed(SPEED speed);   // Установить необходимую скорость работы CAN шины
static void writeReadSpi(char *out, char *in); // Записать/считать массив
static void mcp_read(char regName);     // Прочитать данные регистра
static void mcp_write(char regName, char data);     // Записать в регистр (с чтением)
static BOOLEAN mcp_writeByte(char regName, char data); // Записать в регистр (без чтения)
static void sendTransOneBite(char byte); // Отправить байт и прочитать ответ
static void stopSendMess(void); // Остановить передачу любых сообщений
static void clear_rx_buffer(void);

// CAN
static char TEC;                        // Счетчик ошибок передачи
static char REC;                        // Счетчик ошибок приема
static BOOLEAN needSend = TRUE;         // Есть ли сообщения к отправке
static BOOLEAN work = FALSE;            // Обрабатывается ли сейчас сообщение
static STATUS statusContent;            // Содержимое READ STATUS
static CANINTF_CONT canintfCont;        // Содержимое регистра CANINTF
static RX_STATUS_CONT rxStatusContent;  // Содержимое RX STATUS
static EFLG_CONT eflgContent;           // Содержимое регистра EFLG
static BOOLEAN isInit = FALSE;          // Запущен или нет CAN сервис
static BOOLEAN startCAN(void);          // Запуск сервиса взаимодействия с CAN шиной
static BOOLEAN initCAN(OPMOD mode);           // Инициализация CAN
static void resetCAN(void);             // Программный сброс MCP2515. Возвращает содержимое всех регистров к состоянию по умолчанию
static OPMOD getCurrentMode(void);      // Чтение текущего режима MCP2515
static void readCanintfContent(void);   // Чтение содержимого CANINTF
static void readRxStatusContent(void);  // Считать RX STATUS
static BOOLEAN sendCan(MESSAGE message);  // Отправить сообщение
static MESSAGE checkAttributes(MESSAGE message);  // Определение идентификатора сообщения
static void readStatusContent(void);    // Чтение READ STATUS
static void readTEC(void);              // Чтение ошибок передачи
static void readREC(void);                               // Чтение ошибок приема
static BOOLEAN setOneShot(BOOLEAN state);  // Управление режимом ONE_SHOT
static BOOLEAN setFilter(CAN_FILTERING_MODE mode); // Управление фильтрами can фреймов
static BOOLEAN set_airbags_filt(CAN_FILTERING_MODE mode);
static void wd_without_airbags(void);
static void wd_with_airbags(void);
static INT32 readCan(void);
static BOOLEAN foodForDog = FALSE;
static UINT32 WATCHDOG_TIMER_INTERVAL_RESET_MCP = 30000;
static UINT32 THRESHOLD_FOR_SLEEP = 5000;
static UINT32 time_cnt = 0; // Время в секундах
static BOOLEAN need_pause = FALSE;
static CAN_FILTERING_MODE current_filtering_mode = SEARCH_AIRBAGS;

extern INT32 SPI_CAN_task(INT32 type, INT32 param1, INT32 param2) {
    (void) param1;
    (void) param2;
    switch (type) {
        case START_CAN_SERV:
            LOG_DEBUG("CAN: START_CAN_SERV");
            return startCAN() ? M2MB_OS_SUCCESS : M2MB_OS_TASK_ERROR;
        default:
            LOG_DEBUG("CAN: UNKNOWN CMD.");
            return M2MB_OS_TASK_ERROR;
    }
}

extern INT32 SPI_CAN_event_task(INT32 type, INT32 param1, INT32 param2) {
    (void) param2;
    (void) param1;
    switch (type) {
        case READ_MESSAGE: // Сюда попадают все прочитанные сообщения
            readCan();
            break;
        case WRITE_MESSAGE: // Сюда попадают все сообщения требуемые к отправке
        default:
            LOG_ERROR("CAN: Undefined event");
            break;
    }
    return M2MB_OS_SUCCESS;
}


static void readTEC(void){
    mcp_read(TEC_ADR);
    TEC = recv_buff[2];
    LOG_DEBUG("TEC %02X", TEC);
}

static void readREC(void){
    mcp_read(REC_ADR);
    REC = recv_buff[2];
    LOG_DEBUG("REC %02X", REC);
}

static void get_mutex(void) {
    if (can_mtx_handle == NULL) mut_init();
    M2MB_OS_RESULT_E res = m2mb_os_mtx_get(can_mtx_handle, M2MB_OS_WAIT_FOREVER);
    if (res != M2MB_OS_SUCCESS) LOG_ERROR("CAN: init mutex error");
}

static void put_mutex(void) {
    if (can_mtx_handle == NULL) {
        LOG_ERROR("CAN: put mutex error");
        return;
    }
    M2MB_OS_RESULT_E res = m2mb_os_mtx_put(can_mtx_handle);
    if (res != M2MB_OS_SUCCESS) LOG_ERROR("CAN: apmtx");
}

static M2MB_OS_RESULT_E mut_init(void) {
    M2MB_OS_RESULT_E os_res = m2mb_os_mtx_setAttrItem( &can_mtx_attr_handle,
                                                       CMDS_ARGS(
                                                               M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
                                                               M2MB_OS_MTX_SEL_CMD_NAME, "can_name",
                                                               M2MB_OS_MTX_SEL_CMD_USRNAME, "can_username",
                                                               M2MB_OS_MTX_SEL_CMD_INHERIT, 3
                                                       )
    );
    if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("CAN: ape1");
    os_res = m2mb_os_mtx_init( &can_mtx_handle, &can_mtx_attr_handle);
    if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("CAN: ape2");
    return os_res;
}

static void readRxStatusContent(void){
    sendTransOneBite(RX_STATUS);
    rxStatusContent.bit7 = recv_buff[2] & 0x80;
    rxStatusContent.bit6 = recv_buff[2] & 0x40;
    rxStatusContent.bit5 = recv_buff[2] & 0x20;
    rxStatusContent.bit4 = recv_buff[2] & 0x10;
    rxStatusContent.bit3 = recv_buff[2] & 0x08;
    rxStatusContent.bit2 = recv_buff[2] & 0x04;
    rxStatusContent.bit1 = recv_buff[2] & 0x02;
    rxStatusContent.bit0 = recv_buff[2] & 0x01;

//    if(rxStatusContent.bit7)AZX_LOG_DEBUG("RX STATUS: BIT7\r\n");
//    if(rxStatusContent.bit6)AZX_LOG_DEBUG("RX STATUS: BIT6\r\n");
//    if(rxStatusContent.bit5)AZX_LOG_DEBUG("RX STATUS: BIT5\r\n");
//    if(rxStatusContent.bit4)AZX_LOG_DEBUG("RX STATUS: BIT4\r\n");
//    if(rxStatusContent.bit3)AZX_LOG_DEBUG("RX STATUS: BIT3\r\n");
//    if(rxStatusContent.bit2)AZX_LOG_DEBUG("RX STATUS: BIT2\r\n");
//    if(rxStatusContent.bit1)AZX_LOG_DEBUG("RX STATUS: BIT1\r\n");
//    if(rxStatusContent.bit0)AZX_LOG_DEBUG("RX STATUS: BIT0\r\n");
}

static void readCanintfContent(void){
    mcp_read(CANINTF);

    canintfCont.MERRF = recv_buff[2] & 0x80;
    canintfCont.WAKIF = recv_buff[2] & 0x40;
    canintfCont.ERRIF = recv_buff[2] & 0x20;
    canintfCont.TX2IF = recv_buff[2] & 0x10;
    canintfCont.TX1IF = recv_buff[2] & 0x08;
    canintfCont.TX0IF = recv_buff[2] & 0x04;
    canintfCont.RX1IF = recv_buff[2] & 0x02;
    canintfCont.RX0IF = recv_buff[2] & 0x01;

//    if(canintfCont.MERRF)AZX_LOG_DEBUG("CANINTF: MERRF\r\n");
//    if(canintfCont.WAKIF)AZX_LOG_DEBUG("CANINTF: WAKIF\r\n");
//    if(canintfCont.ERRIF)AZX_LOG_DEBUG("CANINTF: ERRIF\r\n");
//    if(canintfCont.TX2IF)AZX_LOG_DEBUG("CANINTF: TX2IF\r\n");
//    if(canintfCont.TX1IF)AZX_LOG_DEBUG("CANINTF: TX1IF\r\n");
//    if(canintfCont.TX0IF)AZX_LOG_DEBUG("CANINTF: TX0IF\r\n");
//    if(canintfCont.RX1IF)AZX_LOG_DEBUG("CANINTF: RX1IF\r\n");
//    if(canintfCont.RX0IF)AZX_LOG_DEBUG("CANINTF: RX0IF\r\n");
}

static void clearCanintfContent(void){
    mcp_write(CANINTF, 0x00);
    readCanintfContent();
}

static void readEFLGContent(void){
    mcp_read(EFLG);
    eflgContent.RX1OVR = recv_buff[2] & 0x80;
    eflgContent.RX0OVR = recv_buff[2] & 0x40;
    eflgContent.TXBO = recv_buff[2] & 0x20;
    eflgContent.TXEP = recv_buff[2] & 0x10;
    eflgContent.RXEP = recv_buff[2] & 0x08;
    eflgContent.TXWAR = recv_buff[2] & 0x04;
    eflgContent.RXWAR = recv_buff[2] & 0x02;
    eflgContent.EWARN = recv_buff[2] & 0x01;

//    if(eflgContent.RX1OVR)AZX_LOG_DEBUG("EFLG: RX1OVR\r\n");
//    if(eflgContent.RX0OVR)AZX_LOG_DEBUG("EFLG: RX0OVR\r\n");
//    if(eflgContent.TXBO)AZX_LOG_DEBUG("EFLG: TXBO\r\n");
//    if(eflgContent.TXEP)AZX_LOG_DEBUG("EFLG: TXEP\r\n");
//    if(eflgContent.RXEP)AZX_LOG_DEBUG("EFLG: RXEP\r\n");
//    if(eflgContent.TXWAR)AZX_LOG_DEBUG("EFLG: TXWAR\r\n");
//    if(eflgContent.RXWAR)AZX_LOG_DEBUG("EFLG: RXWAR\r\n");
//    if(eflgContent.EWARN)AZX_LOG_DEBUG("EFLG: EWARN\r\n");
}

/*
 * Очистить содержимое регистра EFLG
 * (нормально работает)
 */
static void clearEFLGContent(void){
    mcp_write(EFLG, 0x00);
    readEFLGContent();
}

static void printSendBuff(void) {
    LOG_DEBUG("CAN: Send buffer: ");
    for (UINT8 i = 0; i < 16; ++i) {
        AZX_LOG_INFO("%02x", send_buff[i]);
    }
    AZX_LOG_INFO("\r\n");
}

static void printRecvBuff(void) {
    LOG_DEBUG("CAN: Recv buffer: ");
    for (UINT8 i = 0; i < 16; ++i) {
        AZX_LOG_INFO("%02x", recv_buff[i]);
    }
    AZX_LOG_INFO("\r\n");
}

static void resetCAN(void) {
    clearBuffers();
    mcpReset(TRUE);
    azx_sleep_ms(300);
    mcpReset(FALSE);
    azx_sleep_ms(100);
}

static void clearBuffers(void) {
    memset(send_buff, 0, sizeof send_buff);
    memset(recv_buff, 0, sizeof recv_buff);
}

static BOOLEAN getSpiConnection(void) {
    spi_fd = m2mb_spi_open("/dev/spidev5.0", 0);
    if (spi_fd == -1) {
        LOG_ERROR("CAN: Cannot open SPI channel!");
        return FALSE;
    }
    LOG_DEBUG("CAN: SPI chanel was opened!");
    return TRUE;
}

static BOOLEAN setSpiConfig(void){
    memset(&cfg, 0, sizeof(cfg));
    cfg.spi_mode = M2MB_SPI_MODE_3; //clock idle LOW, data driven on falling edge and sampled on rising edge
    cfg.cs_polarity = M2MB_SPI_CS_ACTIVE_LOW;
    cfg.cs_mode = M2MB_SPI_CS_DEASSERT;
    cfg.endianness = M2MB_SPI_NATIVE; //M2MB_SPI_LITTLE_ENDIAN; //M2MB_SPI_BIG_ENDIAN;
    cfg.callback_fn = NULL;
    cfg.callback_ctxt = NULL;
    cfg.clk_freq_Hz = 10000000; // 5MHz
    cfg.bits_per_word = 8;
    cfg.cs_clk_delay_cycles = 0;
    cfg.inter_word_delay_cycles = 0;
    cfg.loopback_mode = FALSE;

    if (m2mb_spi_ioctl(spi_fd, M2MB_SPI_IOCTL_SET_CFG, (void *) &cfg) == -1) {
        LOG_ERROR("CAN: Cannot set SPI channel configuration!");
        return FALSE;
    }
    LOG_DEBUG("CAN: Set configuration for SPI channel has been success!");
    return TRUE;
}

static BOOLEAN closeSPI(void) {
    if (spi_fd != -1) {
        m2mb_spi_close(spi_fd);
        LOG_DEBUG("CAN: SPI close success!");
    }
    return TRUE;
}

static BOOLEAN setMode(OPMOD mode) {
    UINT8 loop = 10;
    do {
        switch (mode) {
            case NORMAL_MODE:
                mcp_write(CANCTRL, 0x00);
                break;
            case SLEEP_MODE:
                mcp_write(CANCTRL, 0x20);
                break;
            case LOOPBACK_MODE:
                mcp_write(CANCTRL, 0x40);
                break;
            case LISTEN_ONLY_MODE:
                mcp_write(CANCTRL, 0x60);
                break;
            case CONFIGURATION_MODE:
                mcp_write(CANCTRL, 0x80);
                break;
            default:
                LOG_ERROR("CAN: Undefined mode selected...");
                break;
        }
        if (getCurrentMode() == mode) {
            return TRUE;
        } else {
            loop--;
        }
    } while (loop > 0);
    mcp_read(CANSTAT);
    LOG_ERROR("CAN: Error switch to need mode %02x. Current mode %02x", mode, recv_buff[2]);
    return FALSE;
}

static BOOLEAN initSPI(void) {
    if (!getSpiConnection()) {  // Открыть подключение по SPI
        return FALSE;
    }
    if (!setSpiConfig()) {
        closeSPI();
        return FALSE;
    }
    LOG_DEBUG("CAN: SPI chanel initialization success!");
    return TRUE;
}

// CAN
static BOOLEAN startCAN(void) {
    resetCAN(); // Сброс
    initSPI();
    initCAN(NORMAL_MODE);
    isInit = TRUE;

    send_to_can_tt_task(READ_MESSAGE,0,0);

    // Наблюдатель за MCP2515.
    // Задача цикла определять повисла ли микросхема.
    // Нельзя мешать обмену данных.
    // И если требуется, то микросхему нужно перезагрузить.
    if(is_airbags_disable()){
        // Вочдог для блоков где не используются подушки
        wd_without_airbags();
    } else {
        // Вочдог для блоков с подушками
        wd_with_airbags();
    }

    // Функции которые оказались не нужны
    if(FALSE){
        printRecvBuff();
        printSendBuff();
        get_mutex();
        put_mutex();
        readRxStatusContent();
        readStatusContent();
        readEFLGContent();
        readREC();
        readTEC();
        clearEFLGContent();
        stopSendMess();
    }

    return TRUE;
}

static BOOLEAN ign = FALSE;
static void wd_without_airbags(void) {
    ign = isIgnition();
    LOG_DEBUG("CAN WATCHDOG: WD for era without airbags was started");
    while (isInit){
        azx_sleep_ms(1000); // Засыпаем на 1 сек

        // Если зажигание выключено, то спим еще
        while (!isIgnition()){
            no_ign:
            time_cnt = 0;
            ign = FALSE;
            // При выключенном зажигании вочдог ничего не делает
            azx_sleep_ms(1000);
        }

        if(!ign) {
            // Если КЛ15 была выключена и ее включили, то сброс MCP
            LOG_DEBUG("Reset MCP after switch ignition");
            time_cnt = 0;
            ign = isIgnition();
            goto rst;
        }

        if(!foodForDog){
            if(time_cnt >= THRESHOLD_FOR_SLEEP){ // Если порог времени отсутствия кадров превышен, то переход в режим сна
                while (need_pause) { azx_sleep_ms(10); }
                need_pause = TRUE;
                if(getCurrentMode() != SLEEP_MODE){
                    LOG_DEBUG("CAN WATCHDOG: MCP go to sleep...");
                    resetCAN();
                    setMode(SLEEP_MODE);

                    // Прерывание для пробуждения от активности шины (Bus Activity Wake-up).
                    // Когда MCP2515 находится в режиме сна (Sleep mode), и разрешено прерывание пробуждения
                    // по активности шины (CANINTE.WAKIE=1), будет сгенерировано прерывание выводом /INT, и
                    // будет установлен бит CANINTF.WAKIF, если была детектирована активность шины CAN.
                    // Это прерывание приведет к выходу MCP2515 из режима сна.
                    // Это прерывание сбрасывается очисткой бита WAKIF.
                    mcp_write(CANINTE, 0x40);
                }

                time_cnt = 0;
                do{
                    if(!isIgnition()) goto no_ign;
                    azx_sleep_ms(10);
                    readCanintfContent();
                    time_cnt += 10;
                } while (!canintfCont.WAKIF);
                goto rst;
            } else{
                time_cnt += 1000; // Если новых кадров нет, то инкрементируем счетчик
            }
        } else{
            if(getCurrentMode() != NORMAL_MODE) {
                rst:
                LOG_DEBUG("CAN WATCHDOG: MCP go to work...");
                resetCAN();
                initCAN(NORMAL_MODE);
                clearCanintfContent();
                need_pause = FALSE;
                ign = isIgnition();
            }
            time_cnt = 0;
            foodForDog = FALSE;
        }
    }
}

static void wd_with_airbags(void) {
    LOG_DEBUG("CAN WATCHDOG: WD for era with airbags was started");
    while (isInit){
        azx_sleep_ms(WATCHDOG_TIMER_INTERVAL_RESET_MCP);
        // Если за 30 Сек не было получено/отправлено ни одного сообщения, то это подозрительно.
        if (!foodForDog){
            while (need_pause) { usleep(10); }
            need_pause = TRUE;
            LOG_DEBUG("CAN WATCHDOG: NEED RESET");
            resetCAN();
            initCAN(NORMAL_MODE);
            clearCanintfContent();
            need_pause = FALSE;
        }
        foodForDog = FALSE;
    }
}

extern BOOLEAN is_init(void){
    return isInit;
}

extern void finishCAN(void) {
    isInit = FALSE;
    setMode(SLEEP_MODE);
    closeSPI();
}

static BOOLEAN initCAN(OPMOD mode) {
    needSend = FALSE;
    work = FALSE;

    UINT8 cnt = 0;
    clearCanintfContent();
    // Переход в режим конфигурирования
    if(setMode(CONFIGURATION_MODE)) cnt++; // 1

    // Настройка фильтров.
    switch (get_can_filt_type()) {
        case -2:
        case -1:
            if (setFilter(UDS_WITHOUT_AIRBAGS)) cnt++;
            break;
        case 0:
            if (setFilter(current_filtering_mode)) cnt++;
            break;
    }

    // Настройка прерываний
    if(mcp_writeByte(CANINTE, 0x01)) cnt++; // 3

    // Настройка скорости
    if (with_can()){
        if(setSpeed(MCP_16MHz_500kBPS)) cnt++; // 4
    } else {
        LOG_ERROR("CAN: Unable to set speed. Invalid vehicle type.");
    }

    // Приоритет сообщений
    mcp_writeByte(TXB0CTRL,0x03);
    mcp_writeByte(TXB1CTRL,0x03);
    mcp_writeByte(TXB2CTRL,0x03);

    // Переход в требуемый режим
    if(setMode(mode)) cnt++; // 5

    // Отключение ONE SHOT MODE
    if (setOneShot(FALSE)) cnt++; // 6

    clearCanintfContent();
    clear_rx_buffer();
    stopSendMess();
    if (cnt == 6){
        LOG_DEBUG("CAN: MCP2515 configuration success!");
        return TRUE;
    }
    LOG_ERROR("CAN: MCP2515 configuration failed!");
    return FALSE;
}

extern CAN_FILTERING_MODE get_current_filter(void) {
    return current_filtering_mode;
}

extern BOOLEAN set_can_filter(CAN_FILTERING_MODE mode) {
    if ((current_filtering_mode == mode) | (isInit == FALSE)) return TRUE;
    while(need_pause) {usleep(10);}
    need_pause = TRUE;
    UINT8 bad_cnt = 0;
    current_filtering_mode = mode;
    if(!setMode(CONFIGURATION_MODE)) bad_cnt++;
    if(!set_airbags_filt(mode)) bad_cnt++;
    if(!setMode(NORMAL_MODE)) bad_cnt++;
    if(!setOneShot(FALSE)) bad_cnt++;

    if (bad_cnt != 0){
        // Если при переключении возникла ошибка
        LOG_DEBUG("CAN: Switch filtering mode failure!");
        resetCAN();
        initCAN(NORMAL_MODE);
        clearCanintfContent();
        bad_cnt = 0;
    }
    need_pause = FALSE;
    return (bad_cnt == 0) ? TRUE : FALSE;
}

static BOOLEAN setFilter(CAN_FILTERING_MODE mode){
    // Очистка масок
    if(!writeToMultipleRegisters(RXM0SIDH, RXM0EID0, 0x00)) return FALSE;
    if(!writeToMultipleRegisters(RXM1SIDH, RXM1EID0, 0x00)) return FALSE;

    // Очистка фильтров
    if(!writeToMultipleRegisters(RXF0SIDH, RXF0EID0, 0x00)) return FALSE;
    if(!writeToMultipleRegisters(RXF1SIDH, RXF1EID0, 0x00)) return FALSE;
    if(!writeToMultipleRegisters(RXF2SIDH, RXF2EID0, 0x00)) return FALSE;
    if(!writeToMultipleRegisters(RXF3SIDH, RXF3EID0, 0x00)) return FALSE;
    if(!writeToMultipleRegisters(RXF4SIDH, RXF4EID0, 0x00)) return FALSE;
    if(!writeToMultipleRegisters(RXF5SIDH, RXF5EID0, 0x00)) return FALSE;

    if (mode == ANY_FRAME){
        LOG_DEBUG("CAN: Set mode ANY_FRAME");
        return mcp_writeByte(RXB0CTRL, 0x60);
    } else {
        // Настройка масок
        if(!mcp_writeByte(RXM0SIDH, UDS_MASK_SIDH)) return FALSE;
        if(!mcp_writeByte(RXM0SIDL, UDS_MASK_SIDL)) return FALSE;
        if(!mcp_writeByte(RXM1SIDH, UDS_MASK_SIDH)) return FALSE;
        if(!mcp_writeByte(RXM1SIDL, UDS_MASK_SIDL)) return FALSE;

        // Настройка фильтров
        if(!mcp_writeByte(RXF0SIDH, UDS_FILT_SIDH_TERM)) return FALSE;
        if(!mcp_writeByte(RXF0SIDL, UDS_FILT_SIDL_TERM)) return FALSE;
        if(!mcp_writeByte(RXF2SIDH, UDS_FILT_SIDH_TERM)) return FALSE;
        if(!mcp_writeByte(RXF2SIDL, UDS_FILT_SIDL_TERM)) return FALSE;
        if(!mcp_writeByte(RXF3SIDH, UDS_FILT_SIDH_TERM)) return FALSE;
        if(!mcp_writeByte(RXF3SIDL, UDS_FILT_SIDL_TERM)) return FALSE;
        if(!mcp_writeByte(RXF4SIDH, UDS_FILT_SIDH_TERM)) return FALSE;
        if(!mcp_writeByte(RXF4SIDL, UDS_FILT_SIDL_TERM)) return FALSE;
        if(!mcp_writeByte(RXF5SIDH, UDS_FILT_SIDH_TERM)) return FALSE;
        if(!mcp_writeByte(RXF5SIDL, UDS_FILT_SIDL_TERM)) return FALSE;
        if(!mcp_writeByte(RXB0CTRL, 0x20)) return FALSE;

        switch (mode) {
            case UDS_ONLY_TYPE_0: // ID сигнала подушек безопасности 0x31C
            default:
                if(!mcp_writeByte(RXF1SIDH, UDS_FILT_SIDH_AIRBAGS_0)) return FALSE;
                if(!mcp_writeByte(RXF1SIDL, UDS_FILT_SIDL_AIRBAGS_0)) return FALSE;
             //   LOG_DEBUG("AIRBUGS ID 0x31C in active");
                return TRUE;
            case UDS_ONLY_TYPE_1: // ID сигнала подушек безопасности 0x320
                if(!mcp_writeByte(RXF1SIDH, UDS_FILT_SIDH_AIRBAGS_1)) return FALSE;
                if(!mcp_writeByte(RXF1SIDL, UDS_FILT_SIDL_AIRBAGS_1)) return FALSE;
              //  LOG_DEBUG("AIRBUGS ID 0x320 in active");
                return TRUE;
            case UDS_ONLY_TYPE_2:
                if (!mcp_writeByte(RXF1SIDH, UDS_FILT_SIDH_AIRBAGS_2)) return FALSE;
                if (!mcp_writeByte(RXF1SIDL, UDS_FILT_SIDL_AIRBAGS_2)) return FALSE;
                //LOG_DEBUG("AIRBUGS ID 0x021 in active");
                return TRUE;
            case UDS_ONLY_TYPE_3:
                if (!mcp_writeByte(RXF1SIDH, UDS_FILT_SIDH_AIRBAGS_3)) return FALSE;
                if (!mcp_writeByte(RXF1SIDL, UDS_FILT_SIDL_AIRBAGS_3)) return FALSE;
//                LOG_DEBUG("AIRBUGS ID 0xAB in active");
                return TRUE;
            case UDS_WITHOUT_AIRBAGS:
                LOG_DEBUG("CAN: I don't listen to airbags");
                return  TRUE;
        }
    }
}

static BOOLEAN set_airbags_filt(CAN_FILTERING_MODE mode){
    switch (mode) {
        case UDS_ONLY_TYPE_0: // ID сигнала подушек безопасности 0x31C
        default:
            if(!mcp_writeByte(RXF1SIDH, UDS_FILT_SIDH_AIRBAGS_0)) return FALSE;
            if(!mcp_writeByte(RXF1SIDL, UDS_FILT_SIDL_AIRBAGS_0)) return FALSE;
            return TRUE;
        case UDS_ONLY_TYPE_1: // ID сигнала подушек безопасности 0x320
            if(!mcp_writeByte(RXF1SIDH, UDS_FILT_SIDH_AIRBAGS_1)) return FALSE;
            if(!mcp_writeByte(RXF1SIDL, UDS_FILT_SIDL_AIRBAGS_1)) return FALSE;
            return TRUE;
        case UDS_ONLY_TYPE_2:
            if (!mcp_writeByte(RXF1SIDH, UDS_FILT_SIDH_AIRBAGS_2)) return FALSE;
            if (!mcp_writeByte(RXF1SIDL, UDS_FILT_SIDL_AIRBAGS_2)) return FALSE;
            return TRUE;
        case UDS_ONLY_TYPE_3:
            if (!mcp_writeByte(RXF1SIDH, UDS_FILT_SIDH_AIRBAGS_3)) return FALSE;
            if (!mcp_writeByte(RXF1SIDL, UDS_FILT_SIDL_AIRBAGS_3)) return FALSE;
            return TRUE;
        case UDS_WITHOUT_AIRBAGS:
            LOG_DEBUG("CAN: I don't listen to airbags");
            return  TRUE;
    }
}

static BOOLEAN setOneShot(BOOLEAN state){
    // Определяем в каком режиме сейчас находимся
    mcp_read(CANCTRL);
    char current = recv_buff[2];

    char mask = (state) ? 0xFF : 0xF7; // FF - OSM_ON / F7 - OSM-OFF
    current = current & mask;
    if (mcp_writeByte(CANCTRL, current)){
     //   LOG_DEBUG("CAN: ONE_SHOT MODE is %s", (state) ? "TRUE" : "FALSE");
        return TRUE;
    }
    return FALSE;
}

// Рассчитать содержимое регистров скорости можно здесь:
// https://www.kvaser.com/support/calculators/bit-timing-calculator/
// На MCP2515 настраивается также как на MCP2510
static BOOLEAN setSpeed(SPEED speed) {
    UINT8 cnt = 0;
    // Определяем в каком режиме сейчас находимся
    mcp_read(CANCTRL);
    char current = recv_buff[2];

    // Переходим в режим конфигурации
    setMode(CONFIGURATION_MODE);
    switch (speed) {
        case MCP_16MHz_1000kBPS:
            LOG_DEBUG("CAN: Set speed 1000kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_1000kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_1000kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_1000kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_500kBPS:
            LOG_DEBUG("CAN: Set speed 500kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_500kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_500kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_500kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_250kBPS:
            LOG_DEBUG("CAN: Set speed 250kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_250kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_250kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_250kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_200kBPS:
            LOG_DEBUG("CAN: Set speed 200kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_200kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_200kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_200kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_125kBPS:
            LOG_DEBUG("CAN: Set speed 125kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_125kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_125kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_125kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_100kBPS:
            LOG_DEBUG("CAN: Set speed 100kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_100kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_100kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_100kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_83kBPS:
            LOG_DEBUG("CAN: Set speed 83kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_83kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_83kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_83kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_50kBPS:
            LOG_DEBUG("CAN: Set speed 50kBPS");
            if( mcp_writeByte(CNF1, MCP_16MHz_50kBPS_CFG1)) cnt++;
            if( mcp_writeByte(CNF2, MCP_16MHz_50kBPS_CFG2)) cnt++;
            if( mcp_writeByte(CNF3, MCP_16MHz_50kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_40kBPS:
            LOG_DEBUG("CAN: Set speed 40kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_40kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_40kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_40kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_33kBPS:
            LOG_DEBUG("CAN: Set speed 33kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_33kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_33kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_33kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_20kBPS:
            LOG_DEBUG("CAN: Set speed 20kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_20kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_20kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_20kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_10kBPS:
            LOG_DEBUG("CAN: Set speed 10kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_10kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_10kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_10kBPS_CFG3)) cnt++;
            break;
        case MCP_16MHz_5kBPS:
            LOG_DEBUG("CAN: Set speed 5kBPS");
            if(mcp_writeByte(CNF1, MCP_16MHz_5kBPS_CFG1)) cnt++;
            if(mcp_writeByte(CNF2, MCP_16MHz_5kBPS_CFG2)) cnt++;
            if(mcp_writeByte(CNF3, MCP_16MHz_5kBPS_CFG3)) cnt++;
            break;
        default:
            LOG_ERROR("CAN: invalid speed %d", speed);
            break;
    }
    // Возвращаемся в режим в котором были
    if (mcp_writeByte(CANCTRL, current)) cnt++;
    return (cnt == 4) ? TRUE : FALSE;
}

static BOOLEAN mcp_writeByte(char regName, char data) {
    UINT8 loop = 10;
    do{
        clearBuffers();
        send_buff[0] = WRITE;
        send_buff[1] = regName;
        send_buff[2] = data;
        m2mb_spi_write(spi_fd, send_buff, 3);
        mcp_read(regName);
        if (recv_buff[2] != data){
            loop--;
        } else return TRUE;
    } while (loop > 0);
    LOG_ERROR("CAN: Error writing data to the register %02x Expect: %02x Actual: %02x", regName, data, recv_buff[2]);
    return FALSE;
}

static OPMOD getCurrentMode(void) {
    mcp_read(CANSTAT);
    char state = recv_buff[2] & 0xE0;
//    LOG_DEBUG("CAN: CURRENT MODE: %02x", state);
    switch (state) {
        case 0x00:
            return NORMAL_MODE;
        case 0x20:
            return SLEEP_MODE;
        case 0x40:
            return LOOPBACK_MODE;
        case 0x60:
            return LISTEN_ONLY_MODE;
        case 0x80:
            return CONFIGURATION_MODE;
        default:
            LOG_DEBUG("CAN: UNDEFINED_MODE");
            return UNDEFINED_MODE;
    }
}

static void writeReadSpi(char *out, char *in) {
    if (-1 == m2mb_spi_write_read(spi_fd, (void *) out, (void *) in, sizeof(out))) {
        LOG_ERROR("CAN: Failed sending/reading data.");
    }
}

static void mcp_write(char regName, char data) {
    clearBuffers();
    send_buff[0] = WRITE;
    send_buff[1] = regName;
    send_buff[2] = data;
    writeReadSpi(send_buff, recv_buff);
}

static void mcp_read(char regName) {
    clearBuffers();
    send_buff[0] = READ;
    send_buff[1] = regName;
    send_buff[2] = 0x00;
    writeReadSpi(send_buff, recv_buff);
}

static BOOLEAN writeToMultipleRegisters(char start, char end, char data) {
    UINT8 cnt;
    UINT8 loop = 0;
    do {
        cnt = end - start + 1;
        for (char i = start; i <= end; i++){
            if(mcp_writeByte(i,data)) cnt--;
        }
        if (cnt == 0){
          //  LOG_DEBUG("CAN: Writing data to the registers between %02x and %02x success", start, end);
            return TRUE;
        }
        loop++;
    } while (loop < 10);
    LOG_ERROR("CAN: Error writing data to the registers between %02x and %02x. Data: %02x", start, end, data);
    return FALSE;
}

static void sendTransOneBite(char byte){
    clearBuffers();
    send_buff[0] = byte;
    writeReadSpi(send_buff, recv_buff);
}

static void stopSendMess(void){
    // Определяем в каком режиме сейчас находимся
    mcp_read(CANCTRL);
    char current = recv_buff[2];
    char mask = (1) ? 0xFF : 0xEF; // FF - ABAT_ON / EF - ABAT_OFF
    current = current & mask;
    mcp_writeByte(CANCTRL, current);
}

static void clear_rx_buffer(void){
    clearBuffers();
    send_buff[0] = READ;
    send_buff[1] = RXB0SIDH;
    m2mb_spi_write_read(spi_fd, (void *) send_buff, (void *) recv_buff, 16);
   // LOG_DEBUG("RXB0 was cleared");
}

static INT32 readCan(void){
    while(isInit) {
        while (need_pause) { usleep(10);}// Если некий процесс(например WATCHDOG) занял SPI, то не мешаем ему. Ждем пока освободится
        usleep(100);

        readCanintfContent();
        MESSAGE readMessage = {0};
        // Обрабатываем сообщения только с одного буфера - RX0.
        // Пытаться обрабатывать сообщения сразу с 2х буферов - себе дороже.
        if (canintfCont.RX0IF){ // Если в буфере RX0 есть непрочитанное сообщение
            foodForDog = TRUE; // Покормить собаку
            clearBuffers();
            send_buff[0] = READ;
            send_buff[1] = RXB0SIDH;
            m2mb_spi_write_read(spi_fd, (void *) send_buff, (void *) recv_buff, 16);
            readMessage.SIDH = recv_buff[2];
            readMessage.SIDL = recv_buff[3];
            readMessage.EID8 = recv_buff[4];
            readMessage.EID0 = recv_buff[5];
            readMessage.DLC = recv_buff[6];
            memcpy(readMessage.data, &recv_buff[7], 8);
            readMessage = checkAttributes(readMessage);
           // printMessage(readMessage, TRUE);
            can_cb(readMessage);
            clearCanintfContent(); // Разрешаем прием нового сообщения после того как обработали текущее
        }
    }
    return M2MB_OS_SUCCESS;
}

static MESSAGE checkAttributes(MESSAGE message) {
    // Определяем тип идентификатора
    char x = message.SIDL & 0x08;
    if (x == 0x08) {
        message.typeId = EXTENDED;
        uint32_t ConvertedID = 0;
        uint8_t CAN_standardLo_ID_lo2bits = ((uint8_t) message.SIDL & 0x03);
        uint8_t CAN_standardLo_ID_hi3bits = ((uint8_t) message.SIDL >> 5);
        ConvertedID = ((uint8_t)message.SIDH << 3);
        ConvertedID = ((ConvertedID + CAN_standardLo_ID_hi3bits) << 2);
        ConvertedID = ((ConvertedID + CAN_standardLo_ID_lo2bits) << 8);
        ConvertedID = ((ConvertedID + ((uint8_t)message.EID8)) << 8);
        message.id= ConvertedID + ((uint8_t)message.EID0);
    } else{
        message.typeId = STANDARD;
        message.id = ((uint8_t) message.SIDH<<3) | ((uint8_t) message.SIDL>>5);
    }

    // Определяем тип сообщения
    char y = message.DLC & 0x40;
    if (y == 0x40){
        message.rtr = REMOTE;
    } else message.rtr = NOT_REMOTE;
    return message;
}

extern void printMessage(MESSAGE mess, BOOLEAN isRead){
    char *x = (isRead == TRUE) ? "<<< READ " : ">>> WRITE";
    char *s;
    switch (mess.typeId) {
        case STANDARD:
            s = "STANDARD";
            break;
        case EXTENDED:
            s = "EXTENDED";
            break;
        default:
            s = "";
            break;
    }

    char *c;
    switch (mess.rtr) {
        case REMOTE:
            c = "REMOTE";
            goto down;
        case NOT_REMOTE:
            c = "NOT_REMOTE";
            break;
        default:
            c = "";
            break;
    }
    AZX_LOG_DEBUG("%s %s %s id=%u %02x ", x, s, c, mess.id, mess.DLC);

    UINT8 cnt = mess.DLC & 0x0F;
    for (int i = 0; i < cnt; i++){
        AZX_LOG_INFO("%02x ", mess.data[i]);
    }
    down:
    AZX_LOG_INFO("\r\n");
}

extern BOOLEAN message_to_string(MESSAGE mess, char *buffer, UINT32 buffer_size, char *name){
    if (buffer == NULL || buffer_size == 0 || name == NULL) return FALSE;
    memset(buffer, '\0',buffer_size);

    char *s;
    switch (mess.typeId) {
        case STANDARD:
            s = "STANDARD";
            break;
        case EXTENDED:
            s = "EXTENDED";
            break;
        default:
            s = "UNKNOWN";
            break;
    }

    char *c;
    switch (mess.rtr) {
        case REMOTE:
            c = "REMOTE";
            goto remote;
        case NOT_REMOTE:
            c = "NOT_REMOTE";
            break;
        default:
            c = ", UNKNOWN";
            break;
    }

    sprintf(buffer, "%s[ID=%u, %s, %s, DLC=%d, DATA={",name, mess.id, s, c, mess.DLC);
    UINT8 cnt = mess.DLC & 0x0F;
    for (int i = 0; i < cnt; i++){
        sprintf(&buffer[0], "%s %02X", buffer, mess.data[i]);
    }
    sprintf(&buffer[0], "%s }]\r\n", buffer);
    return TRUE;

    remote:
    sprintf(buffer, "%s[ID=%u, TYPE_ID=%s, TYPE_MESSAGE=%s]\r\n", name, mess.id, s, c);
    return TRUE;
}

extern void sendMessage(UINT32 id, TYPE_FRAME typeFrame, const char *data, size_t size){
    MESSAGE writeMessage = {0};
    // Указываем идентификатор
    if (id > 2047){ // Расширенный идентификатор
        uint8_t wipSIDL = 0;

        // RXBnEID0
        writeMessage.EID0 = 0xFF & (char) id;
        id = id >> 8;

        // RXBnEID8
        writeMessage.EID8 = 0xFF & (char) id;
        id = id >> 8;

        // RXBnSIDL
        wipSIDL = 0x03 & (char) id;
        id = id << 3;
        wipSIDL = (0xE0 & (char) id) + wipSIDL;
        wipSIDL = wipSIDL + 0x08;
        writeMessage.SIDL = 0xEB & wipSIDL;

        // RXBnSIDH
        writeMessage.SIDH = 0xFF & ((char)id >> 8);
    } else { // Стандартный идентификатор
        // RXBnEID8
        writeMessage.EID8 = 0x00;
        // RXBnEID0
        writeMessage.EID0 = 0x00;
        // RXBnSIDL
        id = id << 5;
        writeMessage.SIDL = 0xFF & (char) id;
        // RXBnSIDH
        id = id >> 8;
        writeMessage.SIDH = 0xFF & (char) id;
    }

    // DLC
    if(typeFrame == REMOTE){
        writeMessage.DLC = 0x40 | (char) size;
    } else {
        writeMessage.DLC = (char) size;
    }

    // DATA
    memcpy(&writeMessage.data[0], data, 8);
    writeMessage = checkAttributes(writeMessage);
    sendCan(writeMessage);
}

static void readStatusContent(void){
    sendTransOneBite(READ_STATUS);
    statusContent.RX0IF = recv_buff[2] & 0x80;
    statusContent.RX1IF = recv_buff[2] & 0x40;
    statusContent.TX0REQ = recv_buff[2] & 0x20;
    statusContent.TX0IF = recv_buff[2] & 0x10;
    statusContent.TX1REQ = recv_buff[2] & 0x08;
    statusContent.TX1IF = recv_buff[2] & 0x04;
    statusContent.TX2REQ = recv_buff[2] & 0x02;
    statusContent.TX2IF = recv_buff[2] & 0x01;

//    if(statusContent.RX0IF) AZX_LOG_DEBUG("STATUS: RX0IF\r\n");
//    if(statusContent.RX1IF)AZX_LOG_DEBUG("STATUS: RX1IF\r\n");
//    if(statusContent.TX0REQ)AZX_LOG_DEBUG("STATUS: TX0REQ\r\n");
//    if(statusContent.TX0IF)AZX_LOG_DEBUG("STATUS: TX0IF\r\n");
//    if(statusContent.TX1REQ)AZX_LOG_DEBUG("STATUS: TX1REQ\r\n");
//    if(statusContent.TX1IF)AZX_LOG_DEBUG("STATUS: TX1IF\r\n");
//    if(statusContent.TX2REQ)AZX_LOG_DEBUG("STATUS: TX2REQ\r\n");
//    if(statusContent.TX2IF)AZX_LOG_DEBUG("STATUS: TX2IF\r\n");
}

static BOOLEAN sendCan(MESSAGE message) {
    needSend = TRUE;
    while (needSend){
        readStatusContent();
//        azx_sleep_ms(1);
        usleep(100);
        if (!statusContent.TX0REQ) { // Если буфер TX0 не ожидает передачи
            send_buff[0] = WRITE_TX0;
            send_buff[1] = message.SIDH;
            send_buff[2] = message.SIDL;
            send_buff[3] = message.EID8;
            send_buff[4] = message.EID0;
            send_buff[5] = message.DLC;
            memcpy(&send_buff[6], message.data,8);
            m2mb_spi_write(spi_fd, send_buff, 14);
            sendTransOneBite(RTS_TX0);
            needSend = FALSE;
            foodForDog = TRUE; // Покормить собаку
//            printMessage(message, FALSE);
            return TRUE;
        } else if (!statusContent.TX1REQ){ // Если буфер TX1 не ожидает передачи
            send_buff[0] = WRITE_TX1;
            send_buff[1] = message.SIDH;
            send_buff[2] = message.SIDL;
            send_buff[3] = message.EID8;
            send_buff[4] = message.EID0;
            send_buff[5] = message.DLC;
            memcpy(&send_buff[6], message.data,8);
            m2mb_spi_write(spi_fd, send_buff, 14);
            sendTransOneBite(RTS_TX1);
            needSend = FALSE;
            foodForDog = TRUE; // Покормить собаку
//            printMessage(message, FALSE);
            return TRUE;
        } else if (!statusContent.TX2REQ){ // Если буфер TX2 не ожидает передачи
            send_buff[0] = WRITE_TX2;
            send_buff[1] = message.SIDH;
            send_buff[2] = message.SIDL;
            send_buff[3] = message.EID8;
            send_buff[4] = message.EID0;
            send_buff[5] = message.DLC;
            memcpy(&send_buff[6], message.data,8);
            m2mb_spi_write(spi_fd, send_buff, 14);
            sendTransOneBite(RTS_TX2);
            needSend = FALSE;
            foodForDog = TRUE; // Покормить собаку
//            printMessage(message, FALSE);
            return TRUE;
        }
    }
    return FALSE;
}

