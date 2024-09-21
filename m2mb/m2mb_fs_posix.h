/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_fs_posix.h

  @brief
    File system posix library implementation.

  @details
    posix library for files.
    Not all the standard posix functions are available but only the most commonly used.

  @notes
    Dependencies: m2m/m2m_common/m2mb_inc/m2mb_types.h

  @author
    Giorgio De Pauli

  @date
    20/02/2017
*/

#ifndef M2M_M2MB_FS_POSIX_H
#define M2M_M2MB_FS_POSIX_H



/* Global declarations ==========================================================================*/
/* Files open modes */
#define M2MB_O_RDONLY     00
#define M2MB_O_WRONLY     01
#define M2MB_O_RDWR       02
#define M2MB_O_LARGE     010 /* No support */
#define M2MB_O_CREAT    0100
#define M2MB_O_EXCL     0200
#define M2MB_O_NOCTTY   0400
#define M2MB_O_TRUNC   01000
#define M2MB_O_APPEND  02000

#ifndef M2M_M2MB_FS_STDIO_H
/* File seek flags */
#define M2MB_SEEK_SET  0  /* Seek from beginning of file */
#define M2MB_SEEK_CUR  1  /* Seek from current position */
#define M2MB_SEEK_END  2  /* Seek from end of file */
#endif  /* M2M_M2MB_FS_STDIO_H */

/* File and directory permissions */
/* "User" permissions */
#define M2MB_S_IRUSR  0400  /* User has Read permission */
#define M2MB_S_IWUSR  0200  /* User has Write permission */
#define M2MB_S_IXUSR  0100  /* User has eXecute permission*/
#define M2MB_S_IRWXU  0700
/* "Group" permissions */
#define M2MB_S_IRGRP  0040  /* Group has Read permission */
#define M2MB_S_IWGRP  0020  /* Group has Write permission */
#define M2MB_S_IXGRP  0010  /* Group has eXecute permission */
#define M2MB_S_IRWXG  0070
/* "Other" permissions */
#define M2MB_S_IROTH  0004  /* Others has Read permission */
#define M2MB_S_IWOTH  0002  /* Others has Write permission */
#define M2MB_S_IXOTH  0001  /* Others has eXecutepermission */
#define M2MB_S_IRWXO  0007
/* High bits */
#ifndef S_ISUID
#define S_ISUID      04000  /* Set UID on execution */
#endif
#ifndef S_ISGID
#define S_ISGID      02000  /* Set GID on execution */
#endif
#define S_ISVTX      01000  /* Sticky */
/* Combinations */
#define M2MB_ACCESSPERMS  0777
#define M2MB_ALLPERMS     0777

/* File system elements modes */
//#define M2MB_S_IFIFO   0010000  /* FIFO */
//#define M2MB_S_IFCHR   0020000  /* Character device */
#define M2MB_S_IFDIR   0040000  /* Directory */
//#define M2MB_S_IFBLK   0060000  /* Block device */
#define M2MB_S_IFREG   0100000  /* Regular file */
//#define M2MB_S_IFLNK   0120000  /* Symlink */
//#define M2MB_S_IFSOCK  0140000  /* Socket */

#define M2MB_S_ISDIR( tempStat )  ( ( tempStat.st_mode & M2MB_S_IFDIR ) == M2MB_S_IFDIR )
#define M2MB_S_ISREG( tempStat )  ( ( tempStat.st_mode & M2MB_S_IFDIR ) == M2MB_S_IFDIR )
/* Global typedefs ==============================================================================*/
struct M2MB_STAT
{
  DEV_T st_dev;       /* Device ID */
  INODE_T st_ino;     /* Inode number */
  MODE_T st_mode;     /* File mode */
  NLINK_T st_nlink;   /* Number of links */
  SIZE_T st_size;     /* File size in bytes */
  UINT16 st_uid;      /* Owner ID */
  UINT16 st_gid;      /* Group ID */
  UINT32 st_rdev;     /* Major & Minor device number */
  TIME_T st_atime;    /* Time of last access */
  TIME_T st_mtime;    /* Time of last modification */
  TIME_T st_ctime;    /* Time of last status change */
};

struct M2MB_STATVFS
{
  UINT32 f_bsize;     /* file system block size */
  UINT32 f_frsize;    /* fragment size */
  UINT32 f_blocks;    /* size of fs in f_frsize units */
  UINT32 f_bfree;     /* number of free blocks */
  UINT32 f_bavail;    /* number free blocks for unprivileged users */
};

/* This struct must be used only as a pointer. The actual structure is opaque to users. */
typedef struct M2MB_DIR_TAG M2MB_DIR_T;

struct M2MB_DIRENT
{
  CHAR d_name[1024];  /* filename */
};


/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Open file.

  @details
    The function opens the file whose name is the string pointed to by path.

  @param[in] path
    Name of file to be open.
  @param[in] oflag
    Flags for opening file.
    One of the following three flags must be used:
    M2MB_O_RDONLY  Open for reading only.
    M2MB_O_WRONLY  Open for writing only.
    M2MB_O_RDWR    Open for reading and writing.
    In addition any of the following flags may be bitwise ORed:
    M2MB_O_APPEND  If set, the file offset shall be set to the end of the file prior to each write.
    M2MB_O_CREAT   If the file exists, this flag has no effect except as noted under M2MB_O_EXCL below.
                   Otherwise, the file shall be created.
    M2MB_O_EXCL    If M2MB_O_CREAT and M2MB_O_EXCL are set, open fails if the file exists.
    M2MB_O_TRUNC   If the file exists and is a regular file, and the file is successfully opened with M2MB_O_RDWR or M2MB_O_WRONLY,
                   its length is truncated to 0, and the mode is unchanged.
    M2MB_O_LARGE   it's not supported.
  @param[in] ...
    Mode for opening file.
    It must be used only if oflag includes M2MB_O_CREAT.
    It defines file permissions.
    M2MB_S_IRUSR         0400    User has Read permission
    M2MB_S_IWUSR         0200    User has Write permission
    M2MB_S_IXUSR         0100    User has eXecute permission
    M2MB_S_IRWU          0700
    M2MB_S_IRGRP         0040    Group has Read permission
    M2MB_S_IWGRP         0020    Group has Write permission
    M2MB_S_IXGRP         0010    Group has eXecute permission
    M2MB_S_IRWXG         0070
    M2MB_S_IROTH         0004    Others has Read permission
    M2MB_S_IWOTH         0002    Others has Write permission
    M2MB_S_IXOTH         0001    Others has eXecutepermission
    M2MB_S_IRWXO         0007
    M2MB_ACCESSPERMS     0777
    M2MB_ALLPERMS        0777

  @return
    Upon successful completion returns a non-negative integer representing the lowest numbered unused file descriptor.
    Otherwise, -1 is returned.

  @note
    The file and direcotry update(creating/deleting/writing) is available only on "/data" and its-subdirectories.
    Other directories are for reading only (M2MB_O_RDONLY).
    
    
  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_open( const CHAR *path, INT32 oflag, ... );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Read file.

  @details
    The function reads nbyte bytes from the file associated with the open file descriptor, fd, into the buffer pointed to by buf.

  @param[in] fd
    File descriptor.
  @param[out] buf
    Buffer to fill with read data.
  @param[in] nbyte
    Number of bytes to read.

  @return
    Upon successful completion returns a non-negative integer indicating the number of bytes actually read.
    Otherwise, -1 is returned

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
SSIZE_T m2mb_fs_read( INT32 fd, void *buf, SIZE_T nbyte );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Write file.

  @details
    The function writes nbyte bytes from the buffer pointed to by buf to the file associated with the open file descriptor, fd.

  @param[in] fd
    File descriptor.
  @param[in] buf
    Buffer containing data to be written.
  @param[in] nbyte
    Number of bytes to write.

  @return
    Upon successful completion returns the number of bytes actually written.
    Otherwise, -1 is returned

   @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
SSIZE_T m2mb_fs_write( INT32 fd, const void *buf, SIZE_T nbyte );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Close file.

  @details
    The function closes the file associated with the open file descriptor, fd.

  @param[in] fd
    File descriptor.

  @return
    Upon successful completion, 0 is returned. Otherwise, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_close( INT32 fd );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Move file offset.

  @details
    The function sets the file offset for the open file description associated with the file descriptor fd.
    The new offset, measured in bytes, is obtained by adding offset bytes to the position specified by whence.

  @param[in] fd
    File descriptor.
  @param[in] offset
    Number of bytes to move the file offset from the position specified by whence.
  @param[in] whence
    File position.
    M2MB_SEEK_SET  Beginning of the file.
    M2MB_SEEK_CUR  Current file position.
    M2MB_SEEK_END  End of file.

  @return
    Upon successful completion, the resulting offset, as measured in bytes from the beginning of the file, is returned.
    Otherwise, (OFF_T)-1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
OFF_T m2mb_fs_lseek( INT32 fd, OFF_T offset, INT32 whence );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    File status.

  @details
    The function gets status of the file whose name is the string pointed to by path, and writes it to the area pointed to by buf. .

  @param[in] path
    Name of file.
  @param[in] buf
    Pointer to a stat structure (see definition) to be filled with the stutus of the file.

  @return
    Upon successful completion, 0 is returned. Otherwise, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_stat( const CHAR *path, struct M2MB_STAT *buf );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    File status.

  @details
    The function gets status of the open file associated with the file descriptor fd, and writes it to the area pointed to by buf.

  @param[in] fd
    File descriptor.
  @param[in] buf
    Pointer to a stat structure (see definition) to be filled with the stutus of the file.

  @return
    Upon successful completion 0 is returned. Otherwise, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_fstat( INT32 fd, struct M2MB_STAT *buf );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Delete file.

  @details
    The function deletes the file whose name is the string pointed to by path.

  @param[in] path
    Name of file to be deleted.

  @return
    Upon successful completion returns 0. Otherwise, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_unlink( const CHAR *path );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Truncate file.

  @details
    The function effect is that the regular file named by path will have a size which will be equal to length bytes.

  @param[in] path
    Name of file to be truncated.
  @param[in] length
    New size of the file.

  @return
    Upon successful completion returns 0. Otherwise, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_truncate( const CHAR *path, OFF_T length );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Create directory.

  @details
    The function creates a new directory with name path.

  @param[in] path
    Name of directory to be created.
  @param[in] ...
    Mode for creating directory.
    It defines file permissions. See m2mb_fs_open for details.

  @return
    Upon successful completion returns 0. Otherwise, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_mkdir( const CHAR *path, MODE_T mode );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Remove directory.

  @details
    The function removes a directory whose name is given by path.
    The directory shall be removed only if it is an empty directory.

  @param[in] path
    Name of directory to be removed.

  @return
    Upon successful completion returns 0. Otherwise, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_rmdir( const CHAR *path );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Open directory as stream.

  @details
    The function opens the directory whose name is the string pointed to by name, and and associates a stream with it.
    The stream is positioned at the first entry in the directory.

  @param[in] name
    Name of directory to be open.

  @return
    Upon successful completion returns a pointer to stream directory object. Otherwise, NULL is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_DIR_T *m2mb_fs_opendir( const CHAR *name );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Read directory as stream.

  @details
    The function reads the next directory entry in the directory stream pointed to by dirp and returns corresponding information
    in a pointer to a dirent structure.
    It returns NULL on reaching the end of the directory stream

  @param[in] dirp
    Pointer to stream directory object.

  @return
    On success returns a pointer to dirent structure.
    If an error occurs, or the end of the directory stream is reached, NULL is returned.

  @note
    Posix requires that dirent structure contain at least 'd_name', different platform may contain more elements.
    Do not attempt to free pointer to returned dirent structure.
    The data returned by m2mb_fs_readdir() may be overwritten by subsequent calls to m2mb_fs_readdir() for the same directory stream.

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
struct M2MB_DIRENT *m2mb_fs_readdir( M2MB_DIR_T *dirp );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Close directory as stream.

  @details
    The function closes the directory stream pointed to by dirp (writing any buffered output data)
    and closes the underlying file descriptor.

  @param[in] stream
    Pointer to stream directory object.

  @return
    Upon successful completion 0 is returned. Otherwise, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_closedir( M2MB_DIR_T *dirp );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    File system statistics.

  @details
    The function gets information about file system specified by the string pointed to by path, and writes it to the area pointed to by buf.

  @param[in] path
    Pathname of any file within the mounted file system.
  @param[in] buf
    Pointer to a statvfs structure (see definition) to be filled with the information about file system.

  @return
    Upon successful completion 0 is returned. Otherwise, -1 is returned.

  @note
    In parameter path, root or any mounted path can be given to get information about file system.

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_statvfs( const CHAR *path, struct M2MB_STATVFS *buf );



#endif  /* M2M_M2MB_FS_POSIX_H */

