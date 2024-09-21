/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_common/m2mb_inc/m2mb_fs_stdio.h

  @brief
    File system stdio library implementation.

  @details
    stdio library for files.
    The following functions are implemented:
    m2mb_fs_fopen
    m2mb_fs_fread
    m2mb_fs_fwrite
    m2mb_fs_fclose
    m2mb_fs_fseek
    m2mb_fs_ftell
    m2mb_fs_fflush
    m2mb_fs_fileno
    m2mb_fs_fgetc
    m2mb_fs_fgets
    m2mb_fs_fputc
    m2mb_fs_fputs
    m2mb_fs_remove
    m2mb_fs_rename

  @notes
    Dependencies: m2m/m2m_common/m2mb_inc/m2mb_types.h

  @author
    Giorgio De Pauli

  @date
    20/02/2017
*/

#ifndef M2M_M2MB_FS_STDIO_H
#define M2M_M2MB_FS_STDIO_H


/* Global declarations ==========================================================================*/
#ifndef NULL
#define NULL  0
#endif

#ifndef M2M_M2MB_FS_POSIX_H
/* Files seek flags */
#define M2MB_SEEK_SET  0   /* Seek from beginning of file.  */
#define M2MB_SEEK_CUR  1   /* Seek from current position.  */
#define M2MB_SEEK_END  2   /* Seek from end of file.  */
#endif  /* M2M_M2MB_FS_POSIX_H */

/* Global typedefs ==============================================================================*/
/* This struct must be used only as a pointer. The actual structure is opaque to users. */
typedef struct M2MB_FILE_TAG M2MB_FILE_T;

/* Global functions =============================================================================*/
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Open file as stream.

  @details
    The function opens the file whose name is the string pointed to by path and associates a stream with it.

  @param[in] path
    Name of file to be open.
  @param[in] mode
    Mode for opening file.
    r   Open text file for reading.
        The stream is positioned at the beginning of the file.
    r+  Open for reading and writing.
         The stream is positioned at the beginning of the file.
    w   Truncate file to zero length or create text file for writing.
         The stream is positioned at the beginning of the file.
    w+  Open for reading and writing. The file is created if it does not exist, otherwise it is truncated.
          The stream is positioned at the beginning of the file.
    a   Open for appending (writing at end of file). The file is created if it does not exist.
         The stream is positioned at the end of the file.
    a+  Open for reading and appending (writing at end of file). The file is created if it does not exist.
          The initial file position for reading is at the beginning of the file, but output is always appended to the end of the file.
    The mode string can also include the letter 'b' either as a last character or as a character between the characters in any of the two-character strings described above.
    The 'b' is ignored.

  @return
    Upon successful completion returns a pointer to stream file object. Otherwise, NULL is returned.

  @note
    The file and direcotry update(creating/deleting/writing) is available only on "/data" and its-subdirectories.
    Other directories are for reading only (r).

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_FILE_T *m2mb_fs_fopen( const CHAR *path, const CHAR *mode );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Read file as stream.

  @details
    The function reads nitems elements of data, each size bytes long, from the stream pointed to by stream,
    storing them at the location given by ptr.

  @param[out] ptr
    Buffer to fill with read data.
  @param[in] size
    Size in bytes of each item.
  @param[in] nitems
    Number of items to read.
  @param[in] stream
    Pointer to stream file object.

  @return
    On success returns the number of items read.
    If an error occurs, or the end of the file is reached, the return value is a short item count (or zero).

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
SIZE_T m2mb_fs_fread( void *ptr, SIZE_T size, SIZE_T nitems, M2MB_FILE_T *stream );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Write file as stream.

  @details
    The function writes nitems elements of data, each size bytes long, to the stream pointed to by stream,
    obtaining them from the location given by ptr.

  @param[in] ptr
    Buffer containing data to be written.
  @param[in] size
    Size in bytes of each item.
  @param[in] nitems
    Number of items to write.
  @param[in] stream
    Pointer to stream file object.

  @return
    On success returns the number of items written.
    If an error occurs, or the end of the file is reached, the return value is a short item count (or zero).

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
SIZE_T m2mb_fs_fwrite( void *ptr, SIZE_T size, SIZE_T nitems, M2MB_FILE_T *stream );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Close file as stream.

  @details
    The function flushes the stream pointed to by stream (writing any buffered output data)
    and closes the underlying file descriptor.

  @param[in] stream
    Pointer to stream file object.

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
INT32 m2mb_fs_fclose( M2MB_FILE_T *stream );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Move file position indicator.

  @details
    The function sets the file position indicator for the stream pointed to by stream.
    The new position, measured in bytes, is obtained by adding offset bytes to the position specified by whence.

  @param[in] stream
    Pointer to stream file object.
  @param[in] offset
    Number of bytes to move the file position indicator from the position specified by whence.
  @param[in] whence
    File position.
    M2MB_SEEK_SET  Beginning of the file.
    M2MB_SEEK_CUR  Current file position.
    M2MB_SEEK_END  End of file.

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
INT32 m2mb_fs_fseek( M2MB_FILE_T *stream, INT32 offset, INT32 whence );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get file position indicator.

  @details
    The function obtains the current value of the file position indicator for the stream pointed to by stream.

  @param[in] stream
    Pointer to stream file object.

  @return
    Upon successful completion returns the current offset. Otherwise, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_ftell( M2MB_FILE_T *stream );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Flush file as stream.

  @details
    The function forces a write of all user-space buffered data for the given output.

  @param[in] stream
    Pointer to stream file object.

  @return
    Upon successful completion 0 is returned. Otherwise, EOF is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_fflush( M2MB_FILE_T *stream );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get file integer descriptor.

  @details
    The function examines the argument stream and returns its integer descriptor.

  @param[in] stream
    Pointer to stream file object.

  @return
    Upon successful completion the file integer descriptor is returned. Otherwise, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_fileno( M2MB_FILE_T *stream );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Read char from file as stream.

  @details
    The function reads the next character from the stream pointed to by stream.

  @param[in] stream
    Pointer to stream file object.

  @return
    On success returns the character read as an unsigned char cast to an int.
    If an error occurs, or the end of the file is reached, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_fgetc( M2MB_FILE_T *stream );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Read line string from file as stream.

  @details
    The function reads in at most one less than size characters from stream and stores them into the buffer pointed to by s.
    Reading stops after an end of file or a newline.
    If a newline is read, it is stored into the buffer.
    A terminating null byte ('\0') is stored after the last character in the buffer.

  @param[out] s
    Buffer to fill with read data.
  @param[in] size
    Size of buffer s. One more than the max number of characters to be read.
  @param[in] stream
    Pointer to stream file object.

  @return
    On success returns s (the characters read as terminated string).
    If an error occurs, or the end of the file is reached and no characters have been read, NULL is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
CHAR *m2mb_fs_fgets( CHAR *s, INT32 size, M2MB_FILE_T *stream );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Write char to file as stream.

  @details
    The function writes the character c, cast to an unsigned char, to the stream pointed to by stream.

  @param[in] c
    Character to be written.
  @param[in] stream
    Pointer to stream file object.

  @return
    On success returns the character written as an unsigned char cast to an int.
    If an error occurs, -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_fputc( INT32 c, M2MB_FILE_T *stream );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Write string to file as stream.

  @details
    The function writes the string s to stream, without its terminating null byte ('\0').

  @param[in] s
    Null byte terminated string to be written.
  @param[in] stream
    Pointer to stream file object.

  @return
    On success returns  a nonnegative number.
    If an error occurs,  -1 is returned.

  @note
    <Notes>

  @b
    Example
  @code
    <C code example>
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
INT32 m2mb_fs_fputs( const CHAR *s, M2MB_FILE_T *stream );

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
INT32 m2mb_fs_remove( const CHAR *path );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Rename file or directory.

  @details
    The function changes the name of a file or a directory.
    The old argument points to the pathname of the file to be renamed. The new argument points to the new pathname of the file.

  @param[in] oldpath
    Name of file to be renamed.
  @param[in] newpath
    Name of new file.

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
INT32 m2mb_fs_rename( const CHAR *oldpath, const CHAR *newpath );


#endif  /* M2M_M2MB_FS_STDIO_H */

