/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_common/m2mb_inc/m2mb_fs_errno.h

  @brief
    File system non standard errno implementation.

  @details
    errno for file system.

  @notes
    Dependencies: m2m/m2m_common/m2mb_inc/m2mb_types.h

  @author
    Giorgio De Pauli

  @date
    28/02/2017
*/

#ifndef M2M_M2MB_FS_ERRNO_H
#define M2M_M2MB_FS_ERRNO_H


/* Global declarations ==========================================================================*/
#define M2MB_FS_ENOERR           0      /* Success */
#define M2MB_FS_EPERM            1      /* Operation not permitted */
#define M2MB_FS_ENOENT           2      /* No such file or directory */
#define M2MB_FS_EIO              5      /* I/O error */
#define M2MB_FS_EBADF            9      /* Bad file descriptor */
#define M2MB_FS_ENOMEM          12      /* Out of memory */
#define M2MB_FS_EACCES          13      /* Permission denied */
#define M2MB_FS_EBUSY           16      /* Device or resource busy */
#define M2MB_FS_EEXIST          17      /* File exists */
#define M2MB_FS_EXDEV           18      /* Cross-device link */
#define M2MB_FS_ENODEV          19      /* No such device */
#define M2MB_FS_ENOTDIR         20      /* Not a directory */
#define M2MB_FS_EISDIR          21      /* Is a directory */
#define M2MB_FS_EINVAL          22      /* Invalid argument */
#define M2MB_FS_EMFILE          24      /* Too many open files */
#define M2MB_FS_ETXTBSY         26      /* File or directory busy (already open) */
#define M2MB_FS_ENOSPC          28      /* No space left on device */
#define M2MB_FS_ESPIPE          29      /* Illegal seek */
#define M2MB_FS_ENAMETOOLONG    36      /* File name too long */
#define M2MB_FS_ENOTEMPTY       39      /* Directory not empty */
#define M2MB_FS_ELOOP           40      /* Too many symbolic links encountered */
#define M2MB_FS_EILSEQ          84      /* Illegal byte sequence */
#define M2MB_FS_ETIMEDOUT      110      /* Operation/Connection timed out. */
#define M2MB_FS_ESTALE         116      /* Stale remote file handle. */
#define M2MB_FS_EDQUOT         122      /* Quota exceeded */

/* Global typedefs ==============================================================================*/
/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get file system errno value.

  @details
    The function returns the value of errno for file system.
    It is not standard.
    It maps internal file system errno values to m2mb interface errno values.

  @param[in] void
    void

  @return
    The integer value of errno for file system is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_get_errno_value( void );

#endif  /* M2M_M2MB_FS_ERRNO_H */

