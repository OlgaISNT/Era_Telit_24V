//
// Created by klokov on 20.07.2022.
//
#include <stdio.h>
#include "../hdr/accel_driver.h"

#include <failures.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "log.h"
#include "m2mb_types.h"
#include "at_command.h"
#include "m2mb_os.h"
#include "tasks.h"
#include "era_helper.h"
#include "sys_param.h"
#include "factory_param.h"
#include <math.h>

#define TIME_SPEC 10000
#define ACCEL_TIMER_INTERVAL 8 // Опрос каждые 8 мСек (~10 мСек)
#define ACCEL_ADDR_LIS3DSH 0x19 // Адрес акселерометра LIS3DSH
#define ONE_RADIAN 57.29577951308f
#define WHO_AM_I_INTERVAL 2000 // Интервал проверки акселерометра
//"<20DF><2100><229B><23B0><2400><3000><3200><3300><3400><3600><3700>"
// Режим работы
static char *CFG1 = "2077"; // 01110111
static char *CFG2 = "2100"; // 00000000
static char *CFG3 = "2200"; // 00000000
static char *CFG4 = "23B8"; // 10111000
static char *CFG5 = "2400"; // 00000000
static char *CFG6 = "3000"; //
static char *CFG7 = "3200";
static char *CFG8 = "3300";
static char *CFG9 = "3400";
static char *CFG10 = "3600";
static char *CFG11 = "3700";

// ACC_X_LSB (0x02), ACC_X_MSB (0x03)
// ACC_Y_LSB (0x04), ACC_Y_MSB (0x05)
// ACC_Z_LSB (0x06), ACC_Z_MSB (0x07)
static char *ACC_YZX = "A80006";  // Получить 6 байт показаний
static char *WHO_AM_I = "0F0001"; // Получить WHO_AM_I
static UINT16 amICnt = 0;
static BOOLEAN needHorizon = TRUE;
static BOOLEAN isStarted = FALSE;
static INT32 i2c_fd;
static UINT8 buffer[6];
static Accel accel;
//static long int leadTime = 0;
//static struct timespec start, end;
static BOOLEAN prevCoupe;
static BOOLEAN currentCoupe;
static UINT8 indCnt;
static BOOLEAN currentStateHit;
static BOOLEAN prevStateHit;
static BOOLEAN calibrated;
static Quaternion qH;
static UINT32 currentRolloverTime;      // Текущее время превышения угла опрокидывания
static UINT16 timeFilt = 0;
static UINT16 timeF2 = 0;
static BOOLEAN one = FALSE;
static BOOLEAN needVHF = FALSE;     // Требуется ли переключение на более чувствительный режим
static UINT16 cntSpecial = 0;
static UINT16 specTime = 0;
static BOOLEAN enableSpec = FALSE;
static char whoAmi = 0x33;

static BOOLEAN accelInit(void);         // Инициализация акселерометра
static BOOLEAN accelConfig(void);       // Конфигурирование акселерометра
static BOOLEAN writeToAccel(char *str); // Отправить строку
static void readToAccel(void);          // Прочитать ответ
static int calcAcc(char h, char l);         // Посчитать ускорение из показаний
static FLOAT32 convertInG(FLOAT32 val);     // Конвертация в G
static void printMeasurements(Accel accel); // Вывод в консоль
static void runAccelService(void);
static void accelProc(void);
static Quaternion getHorizonQuat(Vector vector); // Получение кватерниона поворота в горизонт
static BOOLEAN isNeedHorizon(void); // Требуется ли получение кватерниона поворота в горизонт
static void coupTracking(void);     // Фиксация переворота
static void hitDetection(void);     // Фиксация удара
static void watchFilt(void);        // Автоматический переключатель режимов фильтра
static void enableSpecFunc(void);
static void isWhoAMI(void);

INT32 ACCEL_task(INT32 type, INT32 param1, INT32 param2){
    (void) param1;
    (void) param2;
    switch (type) {
        case START_ACCEL_SERVICE:
            runAccelService();
            return M2MB_OS_SUCCESS;
        case ACCEL_POOL:
            accelProc();
            return M2MB_OS_SUCCESS;
        default:
            LOG_ERROR("ACCEL: Undefined task");
            return M2MB_OS_TASK_ERROR;
    }
}

extern void incrementSpec(void){
    cntSpecial++;
}

extern INT32 getValY(void){
    return (INT16) accel.origVector.gY;
}

extern INT32 getValZ(void){
    return (INT16) accel.origVector.gZ;
}

extern INT32 getValX(void){
    return (INT16) accel.origVector.gX;
}

static void printMeasurements(Accel accel) {
    accel.nonFiltVector = rotateVect(accel.qHor, accel.nonFiltVector);
    char x[100];
    sprintf(x,"ACCEL: x%f y%f z%f\r\n", accel.nonFiltVector.gX, accel.nonFiltVector.gY, accel.nonFiltVector.gZ);
    save_to_file(x, strlen(x));
}

int char2intTo(char input) {
    if (input >= '0' && input <= '9') return input - '0';
    if (input >= 'A' && input <= 'F') return input - 'A' + 10;
    if (input >= 'a' && input <= 'f') return input - 'a' + 10;
    return -1;
}

int hex2binTo(const char *src, char *target) {
    while (src[0] && src[1]) {
        int a = char2intTo(src[0]);
        int b = char2intTo(src[1]);
        if (a < 0 || b < 0) return -1;
        *(target++) = a * 16 + b;
        src += 2;
    }
    return 0;
}

static INT32 calcAcc(char h, char l) {
    int acc = ((h & 0xFF) * 256 + (l & 0xFF));
    if (acc > 32767) {
        acc -= 65536;
    }
    return acc * (-1);
}

static UINT8 cnt = 0;
static void isWhoAMI(void){
    if (amICnt > WHO_AM_I_INTERVAL/ACCEL_TIMER_INTERVAL){
        memset(buffer, 0x00, 6);
        writeToAccel(WHO_AM_I);
        readToAccel();

        if (buffer[0] == 0x33) {
            // Если id совпал, то переписываем сразу и сбрасываем счетчик
            whoAmi = buffer[0];
            cnt = 0;
            set_crash_sensor_failure(F_INACTIVE);
        } else {
            // Если id не совпал более 10 раз
            if(cnt > 10) {
                whoAmi = buffer[0];
                cnt = 0;
                set_crash_sensor_failure(F_ACTIVE);
            } else {
                cnt++; // Инкрементируем счетчик
            }
        }
        amICnt = 0;
    }
    amICnt++;
}

extern char getWhoAmI(void){
    return whoAmi;
}

static FLOAT32 convertInG(FLOAT32 val) {
    return val * 9.81f / 1333;
}

static BOOLEAN accelInit(void) {
    if ((i2c_fd = open("/dev/i2c-4", O_RDWR)) < 0) {
        LOG_ERROR("ACCEL: Unable to open connection");
        return FALSE;
    }

    accel.filtVector = getNewVector(0.0f,0.0f,0.0f);
    accel.nonFiltVector = getNewVector(0.0f,0.0f,0.0f);
    accel.g1 = getNewVector(0.0f,0.0f,0.0f);
    accel.normCurrentVector = getNewVector(0.0f,0.0f,0.0f);
    accel.normGravVector = getNewVector(0.0f,0.0f,-1.0f);
    accel.origVector = getNewVector(0.0f,0.0f,0.0f);
    accel.g1Auto = getNewVector(0.0f,0.0f,0.0f);
    accel.ultraFiltVector = getNewVector(0.0f,0.0f,0.0f);
    Vector vQ = getNewVector(0.0f,0.0f,0.0f);
    qH = getNewQuaternion(vQ, 0.0f);

    accel.maxTippingAngle = (FLOAT32) get_rollover_angle();     // Максимальный угол опрокидывания
    accel.currentTippingAngle = 0;  // Текущий угол опрокидывания
    prevCoupe = FALSE;
    currentCoupe = FALSE;
    prevStateHit = FALSE;
    currentStateHit = FALSE;
    calibrated = FALSE;
    indCnt = 0;
    return TRUE;
}

static BOOLEAN accelConfig(void) {
    if (ioctl(i2c_fd, I2C_SLAVE, ACCEL_ADDR_LIS3DSH) < 0) {
        LOG_ERROR("ACCEL: Unable to set slave addr");
        return FALSE;
    }

    UINT8 cntCfg = 0;
    if (writeToAccel(CFG1)) cntCfg++; // 1
    if (writeToAccel(CFG2)) cntCfg++; // 2
    if (writeToAccel(CFG3)) cntCfg++; // 3
    if (writeToAccel(CFG4)) cntCfg++; // 4
    if (writeToAccel(CFG5)) cntCfg++; // 5
    if (writeToAccel(CFG6)) cntCfg++; // 6
    if (writeToAccel(CFG7)) cntCfg++; // 7
    if (writeToAccel(CFG8)) cntCfg++; // 8
    if (writeToAccel(CFG9)) cntCfg++; // 9
    if (writeToAccel(CFG10)) cntCfg++; // 10
    if (writeToAccel(CFG11)) cntCfg++; // 11
    if (cntCfg != 11) {
        LOG_ERROR("ACCEL: Configuration not loaded");
        return FALSE;
    }
    return TRUE;
}

static BOOLEAN writeToAccel(char *str){
    char data[128];
    memset(data,0,128);
    if (hex2binTo(str, data) < 0) {
        LOG_ERROR("ACCEL: message conversion error");
        return FALSE;
    }
    int len = strlen(str)/2;
    if (write(i2c_fd, data, len) != len) {
        LOG_ERROR("ACCEL: message sending error");
        return FALSE;
    }
    return TRUE;
}

static void readToAccel(void){
    INT32 readCnt = read(i2c_fd, buffer, 8);
    if(readCnt <= 0){
        LOG_ERROR("Read error\r\n");
    }
}

extern Vector getVector(void){
    return accel.filtVector;
}

static void runAccelService(void) {
    // Если уже запущен
    if (isStarted){
        return;
    }

    // Подключение
    if (!accelInit()) {
        LOG_ERROR("ACCEL: initialization error");
        isStarted = FALSE;
        close(i2c_fd);
    }

    // Конфигурирование
    LOG_DEBUG("ADDR: %02x", ACCEL_ADDR_LIS3DSH);
    if (!accelConfig()) {
        LOG_ERROR("ACCEL: configuration error");
        isStarted = FALSE;
        close(i2c_fd);
    }

    isStarted = TRUE;
    send_to_accel_task(ACCEL_POOL,0,0);
    azx_sleep_ms(1000);
    if (!isHasCalibrated()){
        send_to_Kalman_task(CALIBRATION,0,0);
    }
}

static BOOLEAN isNeedHorizon(void){
    char *res = get_hor_quat();
    accel.qHor = parseQuatFromString(res);

    if (strcmp(res, "qw0.0x0.0y0.0z0.0") != 0) {
        LOG_DEBUG("Q_HOR VALID");
        return FALSE;
    }
    LOG_DEBUG("Q_HOR NOT VALID NEED HORIZON");
    return TRUE;
}

extern RAW getRawData(void){
    return accel.rawData;
}

extern BOOLEAN needHorQuat(void){
    return needHorizon;
}

// Мера предосторожности
static void enableSpecFunc(void){
    if(specTime > TIME_SPEC/ACCEL_TIMER_INTERVAL){ // В течение 10 секунд
        if(cntSpecial >= 20){ // Для переключения нужно тыкнуть в терминале кнопку "Звонки на 112!!!" больше 20 раз
            enableSpec = !enableSpec; // Переключение разрешения специальных функции на противоположное
            if(enableSpec){
                send_to_ind_task(ENABLE_SPEC_FUNC,0,0);
            } else {
                send_to_ind_task(DISABLE_SPEC_FUNC,0,0);
            }
            LOG_WARN("ACCEL: special functions was %s", enableSpec ? "enabled" : "disabled");
        }
        cntSpecial = 0;
        specTime = 0;
    } else {
        specTime++;
    }
}

static void accelProc(void){
    needHorizon = isNeedHorizon(); // Нужно ли горизонтирование
    while (isStarted) {
        enableSpecFunc();
        isWhoAMI();

        // Акселерометр опрашивается каждые 10 мСек
        m2mb_os_taskSleep(M2MB_OS_MS2TICKS(ACCEL_TIMER_INTERVAL));
        if (writeToAccel(ACC_YZX)) {
            // Чтение данных
            readToAccel();

            // Сырые данные
            accel.rawData.xL = buffer[4];
            accel.rawData.xH = buffer[5];
            accel.rawData.yL = buffer[0];
            accel.rawData.yH = buffer[1];
            accel.rawData.zL = buffer[2];
            accel.rawData.zH = buffer[4];

            accel.origVector.gY = calcAcc(buffer[1], buffer[0]);
            accel.origVector.gZ = calcAcc(buffer[3], buffer[2]);
            accel.origVector.gX = calcAcc(buffer[5], buffer[4]);
            accel.origVector.modVector = getMod(accel.origVector);
            if (isHasCalibrated()){
                // Сырые данные проекций пересчитанные в е.и. G
                accel.nonFiltVector.gX = convertInG( accel.origVector.gX);
                accel.nonFiltVector.gY = convertInG( accel.origVector.gY);
                accel.nonFiltVector.gZ = convertInG( accel.origVector.gZ);
                accel.nonFiltVector.modVector = getMod(accel.nonFiltVector);

                // Отфильтрованные данные проекций пересчитанные в е.и. G
                accel.filtVector.gX = convertInG( filterX( accel.origVector.gX));
                accel.filtVector.gY = convertInG( filterY( accel.origVector.gY));
                accel.filtVector.gZ = convertInG( filterZ( accel.origVector.gZ));
                accel.filtVector.modVector = getMod(accel.filtVector);

                accel.ultraFiltVector.gX = convertInG( ultraFilterX( accel.origVector.gX));
                accel.ultraFiltVector.gY = convertInG( ultraFilterY( accel.origVector.gY));
                accel.ultraFiltVector.gZ = convertInG( ultraFilterZ( accel.origVector.gZ));

//                LOG_DEBUG("TippingAngle %.3f %.10f %.10f %.10f\r\n%.3f %.3f %.3f\r\n", accel.currentTippingAngle, accel.ultraFiltVector.gX, accel.ultraFiltVector.gY, accel.ultraFiltVector.gZ, accel.g1.gX, accel.g1.gY, accel.g1.gZ);
            }

            if ((!needHorizon) & calibrated){
                if(FALSE) watchFilt();

                // Совершаем горизонтирование
                accel.g1 = rotateVect(accel.qHor, accel.filtVector);

                // Текущий вектор гравитации
                qH = getHorizonQuat(accel.filtVector);
                accel.g1Auto = rotateVect(qH, accel.filtVector);

                // Фиксация переворота
                coupTracking();

                // Фиксация удара и запись в лог (экспериментальные функции) по дефолту всегда выключены
                if(enableSpec) printMeasurements(accel);
                if(enableSpec | is_enable_accel_crush_detect()) hitDetection();
            }
            calibrated = isHasCalibrated();
        }
    }
    isStarted = FALSE;
    close(i2c_fd);
}

static void watchFilt(void){
    // Есть 2 основных режима фильтрации:
    // CUSTOM_FILTERING_MODE - малая чувствительность
    // VERY_HIGH_FILTERING_MODE - высокая чувствительность
    if(getCurrentFilterMode() == VERY_HIGH_FILTERING_MODE){
        timeF2 += ACCEL_TIMER_INTERVAL;
        if ((timeF2 >= 5000) | (accel.currentTippingAngle >= (FLOAT32) get_rollover_angle())){
            setFilteringMode(CUSTOM_FILTERING_MODE);
            timeF2 = 0;
        }
    } else{
       timeF2 = 0;
    }

    if((accel.currentTippingAngle >= (FLOAT32) get_rollover_angle() * 0.5f)){
        needVHF = TRUE;
    } else {
        if(getCurrentFilterMode() != CUSTOM_FILTERING_MODE){
            setFilteringMode(CUSTOM_FILTERING_MODE);
        }
        needVHF = FALSE;
        one = FALSE;
    }

    if ((one == FALSE) & needVHF){
        if(timeFilt >= 5000/ACCEL_TIMER_INTERVAL){
            if(getCurrentFilterMode() != VERY_HIGH_FILTERING_MODE) {
                setFilteringMode(VERY_HIGH_FILTERING_MODE);
                one = TRUE;
            }
            timeFilt = 0;
            needVHF = FALSE;
        } else {
            timeFilt += ACCEL_TIMER_INTERVAL;
        }
    } else {
        timeFilt = 0;
    }
}

// Ожидается, что машина стоит на ровной поверхности без уклонов и не движется
extern BOOLEAN getHorQuat(void){
    needHorizon = TRUE;
    if(!calibrated) {
        return FALSE;
    }

    accel.qHor = getHorizonQuat(accel.ultraFiltVector);

    char qu[100];
    quatToString(accel.qHor, qu);
    set_hor_quat(qu);
    needHorizon = FALSE;
    return TRUE;
}

/**
 * Фиксация переворота.
 */
static void coupTracking(void) {
    FLOAT32 gZGround = accel.g1.gZ;
    FLOAT32 gZAuto = accel.g1Auto.gZ;
//    LOG_INFO("ang=%.3f /gZGround x%.3f y%.3f z%.3f / gZAuto x%.3f y%.3f z%.3f", accel.currentTippingAngle, accel.g1.gX, accel.g1.gY, accel.g1.gZ, accel.g1Auto.gX, accel.g1Auto.gY, accel.g1Auto.gZ);
    FLOAT32 ang = getAxisDeviation(gZGround, gZAuto);
    if (ang < 0) ang = ang * (-1);

    accel.currentTippingAngle = ang;
    prevCoupe = currentCoupe;
    // Если угол завала превысил допустимый, и ранее переворот не фиксировался,
    // то начало переворота
    if ((accel.currentTippingAngle >= (FLOAT32) get_rollover_angle())) {
        UINT32 t = get_rollover_time() * 100;
        // Событие переворот
        if (currentRolloverTime > t) {
            currentCoupe = TRUE;
            if (prevCoupe == FALSE) {
                LOG_DEBUG("ACCEL: >>>>>>>>>>>>>>>>>>>>>>>>>>>> Coup detected");
            }
            if((get_no_auto_trig() == 1) & (prevCoupe == FALSE)){
                send_to_ecall_task(ECALL_ROLLOVER, ECALL_NO_REPEAT_REASON, 0);
                disableClearLog();
            }
        } else {
            currentRolloverTime += (ACCEL_TIMER_INTERVAL + 4);
        }
    } else {
        currentRolloverTime = 0;
    }

    // Окончание переворота
    if ((prevCoupe == TRUE) & (accel.currentTippingAngle < (FLOAT32) get_rollover_angle())){
        LOG_DEBUG("ACCEL: Coup released");
        currentCoupe = FALSE;
        currentRolloverTime = 0;
    }
}

extern UINT8 getCurrentRollAng(void){
    return (UINT8) accel.currentTippingAngle;
}

/**
   * Фиксация удара.
   */
static void hitDetection(void){
    prevStateHit = currentStateHit;

    accel.nonFiltVector = rotateVect(accel.qHor, accel.nonFiltVector);
    FLOAT32 gx = accel.nonFiltVector.gX / 9.81;
    FLOAT32 gy = accel.nonFiltVector.gY / 9.81;
    FLOAT32 gz = accel.nonFiltVector.gZ / 9.81;

    // Берем значения по модулю
    if (gx < 0) gx = gx * (-1);
    if (gy < 0) gy = gy * (-1);
    if (gz < 0) gz = gz * (-1);

    float zyx_t = get_zyx_threshold();
//    LOG_DEBUG("TRESH %.3f", zyx_t);

    BOOLEAN condition_one = (gx > zyx_t) & (gy > zyx_t) & (gz > zyx_t);
    BOOLEAN condition_two = (gx > 4.0f) | (gy > 4.0f) | (gz > 4.0f);  // (Не используется) Повышает вероятность зафиксировать удар, но блок будет звонить от любого чиха
    //LOG_DEBUG("ABS NON_FILT VECTOR x%f y%f z%f",gx, gy, gz);

    if (condition_one){
        if (prevStateHit == FALSE){
            LOG_DEBUG("ACCEL: >>>>>>>>>>>>>>>>>>>>>>>>>>>> (t%f) (%d/%d) hit detected", zyx_t, condition_one, condition_two);
            if(enableSpec){
                char x[100];
                memset(&x[0], 0x00, 100);
                sprintf(x,"ACCEL: >>>>>>>>>>>>>>>>>>>>>>>>>>>> (t%f) (%d/%d) hit marker x%f y%f z%f\r\n", zyx_t, condition_one, condition_two, accel.nonFiltVector.gX, accel.nonFiltVector.gY, accel.nonFiltVector.gZ);
                save_to_file(x, strlen(x));
            }
            currentStateHit = TRUE;
            // Событие удар
            if((get_no_auto_trig() == 1)){
                send_to_ecall_task(ECALL_FRONT, ECALL_NO_REPEAT_REASON, 0);
                disableClearLog();
            }
        }
    } else{
        if (prevStateHit == TRUE){
            LOG_DEBUG("ACCEL: hit released");
            currentStateHit = FALSE;
        }
    }
}

extern BOOLEAN isEnableSpecF(void){
    return enableSpec;
}

// Горизонтирование
static Quaternion getHorizonQuat(Vector vector){
    // Нормированный текущий вектор +
    accel.normCurrentVector = getNorm(vector);
    // Вектор вокруг которого все будем вращать +
    Vector rotVector = crossProdVector(accel.normCurrentVector, accel.normGravVector);
    // Угол на который будем вращать
    FLOAT32 angle = acosf(getAngleBetweenTwoVector(accel.normCurrentVector, accel.normGravVector)) * ONE_RADIAN;
    // Получаем кватернион поворота в горизонт
    return createQuat(rotVector, angle);
}

extern void stopAccelService(void){
    isStarted = FALSE;
    LOG_DEBUG("ACCEL: service was stopped");
}

extern void resetAccelPref(void){
    accel.qHor.w = 0.0f;
    accel.qHor.vector.gX = 0.0f;
    accel.qHor.vector.gY = 0.0f;
    accel.qHor.vector.gZ = 0.0f;

    char qu[100];
    quatToString(accel.qHor, qu);
    set_hor_quat(qu);
    needHorizon = TRUE;
    accel.currentTippingAngle = 0;
    prevCoupe = FALSE;
    currentCoupe = FALSE;
    prevStateHit = FALSE;
    currentStateHit = FALSE;

    accel.filtVector = getNewVector(0.0f,0.0f,0.0f);
    accel.nonFiltVector = getNewVector(0.0f,0.0f,0.0f);
    accel.g1 = getNewVector(0.0f,0.0f,0.0f);
    accel.normCurrentVector = getNewVector(0.0f,0.0f,0.0f);
    accel.normGravVector = getNewVector(0.0f,0.0f,-1.0f);
    accel.origVector = getNewVector(0.0f,0.0f,0.0f);
    accel.g1Auto = getNewVector(0.0f,0.0f,0.0f);
    accel.ultraFiltVector = getNewVector(0.0f,0.0f,0.0f);
    Vector vQ = getNewVector(0.0f,0.0f,0.0f);
    qH = getNewQuaternion(vQ, 0.0f);
}

