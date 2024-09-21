/**
 * @file msd_storage.c
 * @version 1.0.0
 * @date 05.05.2022
 * @author: DKozenkov
 * @brief Хранилище МНД в RAM и FLASH
 */

/* Include files ================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zconf.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "m2mb_fs_posix.h"
#include "log.h"
#include "app_cfg.h"
#include "msd_storage.h"
#include "msd.h"
#include "tasks.h"
#include "auxiliary.h"
#include "files.h"
#include "msd.h"
#include "app_cfg.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
static UINT8 *buf;				 // буфер для хранения массива МНД в кол-ве до MAX_NUMBER_MSD
static volatile UINT8 n_msd = 0; // текущее кол-во МНД в хранилище
static UINT16 msd_size;			 // кол-во байт в одном МНД
static char file_name[] = LOCALPATH_S MSD_DAT_FILE_NAME;
static INT32 find_msd_numb(T_MSDFlag msd_flag);
static M2MB_OS_MTX_ATTR_HANDLE mtx_attr_handle;
static M2MB_OS_MTX_HANDLE      mtx_handle = NULL;
/* Local function prototypes ====================================================================*/
static BOOLEAN rewrite_file(void);
static M2MB_OS_RESULT_E mut_init(void);
static void get_mutex(void);
static void put_mutex(void);
/* Static functions =============================================================================*/
static void get_mutex(void) {
    if (mtx_handle == NULL) mut_init();
    M2MB_OS_RESULT_E res = m2mb_os_mtx_get(mtx_handle, M2MB_OS_WAIT_FOREVER);
    if (res != M2MB_OS_SUCCESS) LOG_ERROR("gmsdmtx");
}
static void put_mutex(void) {
    if (mtx_handle == NULL) {
        LOG_ERROR("mtP");
        return;
    }
    M2MB_OS_RESULT_E res = m2mb_os_mtx_put(mtx_handle);
    if (res != M2MB_OS_SUCCESS) LOG_ERROR("pmsdmtx");
}
static M2MB_OS_RESULT_E mut_init(void) {
    M2MB_OS_RESULT_E os_res = m2mb_os_mtx_setAttrItem( &mtx_attr_handle,
                                                       CMDS_ARGS(
                                                               M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
                                                               M2MB_OS_MTX_SEL_CMD_NAME, "msd1",
                                                               M2MB_OS_MTX_SEL_CMD_USRNAME, "msd1",
                                                               M2MB_OS_MTX_SEL_CMD_INHERIT, 3
                                                       )
    );
    if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("msdm1");
    os_res = m2mb_os_mtx_init( &mtx_handle, &mtx_attr_handle);
    if (os_res != M2MB_OS_SUCCESS) LOG_ERROR("msdm2");
    return os_res;
}

static BOOLEAN rewrite_file(void) {
    if (m2mb_fs_unlink(file_name) < 0) LOG_WARN("Can't delete %s", file_name);
    //INT32 fd = -1;
    //fd = m2mb_fs_open(file_name, M2MB_O_RDWR | M2MB_O_CREAT, M2MB_ALLPERMS);
    FILE *fileStream = fopen(file_name, "w+");
    if (fileStream == NULL) {
        LOG_ERROR("Can't open %s", file_name);
        return FALSE;
    }
    UINT32 size = n_msd * msd_size;
    //int r = m2mb_fs_write(fd, buf, size);
    UINT32 res = fwrite(buf, 1, size, fileStream);
    //m2mb_fs_close(fd);

    if (res != size) {
        LOG_ERROR("Write error in %s", file_name);
        fclose(fileStream);
        return FALSE;
    } else {
        fflush(fileStream);
        int fd = fileno(fileStream);
        fsync(fd);
        fclose(fileStream);
        return TRUE;
    }
}

/* Global functions =============================================================================*/
BOOLEAN init_MSD(UINT16 msdSize) {
    msd_init_variables();
    msd_size = msdSize;
    int buf_size = msd_size * MAX_NUMBER_MSD;
    buf = m2mb_os_malloc(buf_size);
    INT32 fd = -1;
    fd = m2mb_fs_open(file_name, M2MB_O_RDWR | M2MB_O_CREAT, M2MB_ALLPERMS);
    if (fd == -1) {
        LOG_CRITICAL("Can't open %s", file_name);
        return FALSE;
    }
    int bytes = m2mb_fs_read(fd, buf, buf_size);
    if (bytes < 0) {
        LOG_CRITICAL("Can't read %s", file_name);
        return FALSE;
    }
    m2mb_fs_close(fd);
    if (bytes % msd_size != 0) {
        LOG_ERROR("Invalid MSD crc in %s, it will be delete", MSD_DAT_FILE_NAME);
        delete_all_MSDs();
    } else {
        n_msd = bytes / msd_size;
    }
    for (int i = 0; i < n_msd; i++) {
        T_msd *msd = (T_msd *) (buf + i * msd_size);
        if (!msd_check(msd)) {
            LOG_ERROR("Invalid MSD #%i in %s", i, file_name);
            print_hex("inv MSD", (char *)msd, sizeof(T_msd));
            delete_all_MSDs();
        } else {
            switch (msd->msdFlag) {
                default:
                case MSD_EMPTY:
                case MSD_UNDEFINED:
                    LOG_ERROR("Invalid msdFlag %i in MSD#%i", msd->msdFlag, i);
                    break;
                case MSD_INBAND:
                    msd_counter.inband++;
                    break;
                case MSD_SMS:
                    msd_counter.sms++;
                    break;
                case MSD_SAVED:
                    msd_counter.saved++;
                    break;
            }
        }
    }
    print_msd_storage_status();
    return TRUE;
}

void print_msd_storage_status(void) {
    to_log_info_uart("%i MSDs in storage: %i - INBAND, %i - SMS, %i - SAVED", n_msd, msd_counter.inband, msd_counter.sms, msd_counter.saved);
}

BOOLEAN save_MSD(T_msd *msd) {
    get_mutex();
    //print_hex("before", (char *)buf, n_msd *msd_size);
    UINT8 *p;
    if (n_msd >= MAX_NUMBER_MSD) {
        LOG_INFO("Overflow MSD storage!");
        memmove(buf, buf + msd_size, (MAX_NUMBER_MSD - 1) * msd_size);
        p = buf + (MAX_NUMBER_MSD - 1) * msd_size;
    } else {
        p = buf + n_msd * msd_size;
        n_msd++;
        switch(msd->msdFlag) {
            case MSD_INBAND: 	msd_counter.inband++; 	break;
            case MSD_SMS: 		msd_counter.sms++; 		break;
            case MSD_SAVED: 	msd_counter.saved++; 	break;
            default:	break;
        }
    }
    memcpy(p, msd, msd_size);
    //print_hex("after", (char *)buf, n_msd *msd_size);
    BOOLEAN ok = rewrite_file();
    put_mutex();
    return ok;
}

BOOLEAN read_MSD(UINT8 msd_number, T_msd *msd) {
    get_mutex();
    BOOLEAN ok = TRUE;
    if (msd_number >= n_msd || n_msd == 0) ok = FALSE;
    else memcpy(msd, buf + msd_number * msd_size, msd_size);
    put_mutex();
    return ok;

}

BOOLEAN update_MSD(UINT8 msd_number, T_msd *msd) {
    BOOLEAN ok;
    get_mutex();
    if (msd_number >= n_msd || n_msd == 0) {
        put_mutex();
        return FALSE;
    }
    T_msd *prev = (T_msd *) (buf + msd_number * msd_size);
    switch(prev->msdFlag) {
        case MSD_INBAND: 	msd_counter.inband--; 	break;
        case MSD_SMS: 		msd_counter.sms--; 		break;
        case MSD_SAVED: 	msd_counter.saved--; 	break;
        default:	break;
    }
    memcpy(buf + msd_number * msd_size, msd, msd_size);
    switch(msd->msdFlag) {
        case MSD_INBAND: 	msd_counter.inband++; 	break;
        case MSD_SMS: 		msd_counter.sms++; 		break;
        case MSD_SAVED: 	msd_counter.saved++; 	break;
        default:	break;
    }
    ok = rewrite_file();
    if (ok) LOG_DEBUG("MSD #%i was updated", msd_number);
    else LOG_ERROR("MSD #%i isn't updated", msd_number);
    put_mutex();
    return ok;
}

BOOLEAN delete_MSD(UINT16 msd_number) {
    get_mutex();
    if (msd_number >= n_msd || n_msd == 0) {
        LOG_WARN("n_msd:%i, msd_n:%i", n_msd, msd_number);
        put_mutex();
        return FALSE;
    }
    T_msd *prev = (T_msd *) (buf + msd_number * msd_size);
    switch(prev->msdFlag) {
        case MSD_INBAND: 	msd_counter.inband--; 	break;
        case MSD_SMS: 		msd_counter.sms--; 		break;
        case MSD_SAVED: 	msd_counter.saved--; 	break;
        default:	break;
    }
    if (msd_number + 1 < n_msd) {
        memmove(buf + msd_number * msd_size, buf + (msd_number + 1) * msd_size, (n_msd - msd_number - 1) * msd_size);
    }
    n_msd--;
    BOOLEAN ok = rewrite_file();
    if (ok) LOG_DEBUG("MSD #%i was deleted, remain: %i", msd_number, n_msd);
    else LOG_ERROR("MSD #%i isn't deleted", msd_number);
    put_mutex();
    return ok;
}

BOOLEAN delete_all_MSDs(void) {
    get_mutex();
    memset(buf, 0, MAX_NUMBER_MSD * msd_size);
    msd_counter.inband = 0;
    msd_counter.sms = 0;
    msd_counter.saved = 0;
    n_msd = 0;
    if (m2mb_fs_unlink(file_name) < 0) {
        LOG_ERROR("Can't delete %s", file_name);
        put_mutex();
        return FALSE;
    } else {
        LOG_WARN("All MSDs were deleted");
        put_mutex();
        return TRUE;
    }
}

UINT16 get_number_MSDs(void) {
    return n_msd;
}

BOOLEAN find_msd_inband(T_msd *msd) {
    return find_msd_n(msd, MSD_INBAND) == -1 ? FALSE : TRUE;
}

BOOLEAN find_msd_sms(T_msd *msd) {
    return find_msd_n(msd, MSD_SMS) == -1 ? FALSE : TRUE;
}

BOOLEAN find_msd_saved(T_msd *msd) {
    return find_msd_n(msd, MSD_SAVED) == -1 ? FALSE : TRUE;
}

INT32 find_msd_n(T_msd *msd, T_MSDFlag msd_flag) {
    get_mutex();
    for (int i = 0; i < n_msd; i++) {
        T_msd *p = (T_msd*) (buf + i * msd_size);
        if (p->msdFlag == msd_flag) {
            memcpy(msd, p, msd_size);
            put_mutex();
            return i;
        }
    }
    put_mutex();
    return -1;
}

static INT32 find_msd_numb(T_MSDFlag msd_flag) {
    for (int i = 0; i < n_msd; i++) {
        T_msd *p = (T_msd*) (buf + i * msd_size);
        if (p->msdFlag == msd_flag) {
            return i;
        }
    }
    return -1;
}

BOOLEAN delete_inband_msd(void) {
    get_mutex();
    int n = find_msd_numb(MSD_INBAND);
    if (n > -1) {
        put_mutex();
        BOOLEAN ok = delete_MSD(n);
        return ok;
    }
    put_mutex();
    return FALSE;
}

BOOLEAN move_inband_to_sms(void) {
    get_mutex();
    BOOLEAN ok;
    int n = find_msd_numb(MSD_INBAND);
    if (n > -1) {
        msd_counter.inband--;
        T_msd *p = (T_msd*) (buf + n * msd_size);
        p->msdFlag = MSD_SMS;
        p->checkSum = 0;
        p->checkSum = auxiliary_crc8((UINT8 *)p, sizeof(T_msd));
        msd_counter.sms++;
        ok = rewrite_file();
        print_hex("MSD after moved to SMS", (char *)p, sizeof(T_msd));
    } else ok = FALSE;
    put_mutex();
    return ok;
}

BOOLEAN move_sms_to_saved(void) {
    get_mutex();
    BOOLEAN ok;
    int n = find_msd_numb(MSD_SMS);
    if (n > -1) {
        msd_counter.inband--;
        T_msd *p = (T_msd*) (buf + n * msd_size);
        p->msdFlag = MSD_SAVED;
        p->checkSum = 0;
        p->checkSum = auxiliary_crc8((UINT8 *)p, sizeof(T_msd));
        msd_counter.sms++;
        ok = rewrite_file();
    } else ok = FALSE;
    put_mutex();
    return ok;
}
