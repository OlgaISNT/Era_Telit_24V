/*
 * files.c
 * Created on: 31.03.2022, Author: DKozenkov     
 */

/* Include files ================================================================================*/
#include "files.h"
#include <stdio.h>
#include <string.h>
#include "m2mb_types.h"
#include "m2mb_os_api.h"
#include "m2mb_fs_posix.h"
#include "app_cfg.h"
#include "log.h"
/* Local defines ================================================================================*/
/* Local typedefs ===============================================================================*/
/* Local statics ================================================================================*/
/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
/* Global functions =============================================================================*/



int get_file_size(char *file_path) { // не использовать azx.log библиотеку в этой функции!
	struct M2MB_STAT stat;
	int r = m2mb_fs_stat((void*) file_path, &stat);
	return r == -1 ? r : (signed int) stat.st_size;
}

BOOLEAN delete_file(char *file_path) {
	int r = m2mb_fs_unlink((void*) file_path);
	return r == 0 ? TRUE : FALSE;
}

BOOLEAN check_file_size(char *file_path, UINT32 expectSize){
    UINT32 currentSize = get_file_size(file_path);
    if (currentSize != expectSize){
        LOG_WARN("Error file = %s, size = %d", file_path, currentSize);
        return FALSE;
    }
    LOG_TRACE("File %s verified successfully", file_path);
    return TRUE;
}

