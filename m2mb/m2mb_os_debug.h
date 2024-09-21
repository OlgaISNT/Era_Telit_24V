/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_os_debug.h


  @brief
    Basic functionality of Operating systems debug

  @details
    OS debug functions

  @notes
    Dependencies:
             m2mb_types.h
             m2mb_os_types.h
             m2mb_platform_conf.h
             m2mb_os.h

  @author
    Pierluigi Collu

  @date
    30/01/2020
*/

#ifndef M2MB_OS_DEBUG_H
#define M2MB_OS_DEBUG_H

/* Include ======================================================================================*/

/* Global declarations ==========================================================================*/


/* Global typedefs ==============================================================================*/


/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates bytes of memory (to be used for debug purposes)

  @details
    This function provides service to reserve memory space to the caller

  @param [in] size
    size in byte of memory to be allocated

  @param [in] place
    the code it is called from (e.g. __FUNCTION__ or __MODULE__)

  @param [in] line
    the line it is called from (e.g. __LINE__)

  @return
    valid pointer in case of success

  @return
    NULL in case of error


  @note
    The performance of this service is a function of the requested block size and the
    amount of fragmentation in the heap. Hence, this service should not be
    used during time-critical task of execution.
    Allowed From
     Initialization and tasks
    Preemption Possible
      Yes

  @b
    //pointer to 10 UINT32
    UINT32 *pUint;
    pUint = ( UINT32 * )m2mb_os_malloc( 10 * sizeof(UINT32), __FUNCTION__, __LINE__ );
    if ( pUint == NULL )
      exit(...)
  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
void *m2mb_os_malloc_debug( UINT32 size, const CHAR *place, INT32 line );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates bytes of memory and init space with 0 (to be used for debug purposes)

  @details
    This function provides service to reserve memory space to the caller and initialize it to 0

  @param [in] size
    size in byte of memory to be allocated and initialize

  @param [in] place
    the code it is called from (e.g. __FUNCTION__ or __MODULE__)

  @param [in] line
    the line it is called from (e.g. __LINE__)

  @return
    valid pointer in case of success

  @return
    NULL in case of error

  @note
    The performance of this service is a function of the requested block size and the
    amount of fragmentation in the heap. Hence, this service should not be
    used during time-critical task of execution.
    Allowed From
     Initialization and tasks
    Preemption Possible
      Yes

  @b
    Example
  @code
    //pointer to 10 UINT32
    typedf struct
    {
      INT32 a;
      INT8  b;
      void *ptr;
    }GEN_T;

    GEN_T *pStruct;
    pStruct = ( GEN_T * )m2mb_os_calloc( sizeof(GEN_T), __FUNCTION__, __LINE__ );
    if ( pStruct == NULL )
      exit(...)
    //all pStruct initialized to 0: pStruct->a = 0; pStruct->b = 0; pStruct->ptr = 0;
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
void *m2mb_os_calloc_debug( UINT32 size, const CHAR *place, INT32 line );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Free allocated memory (to be used for debug purposes)

  @details
    This function provides service to free already allocated memory space

  @param [in] pMem
    pointer to memory where to release previous allocation

  @param [in] place
    the code it is called from (e.g. __FUNCTION__ or __MODULE__)

  @param [in] line
    the line it is called from (e.g. __LINE__)

  @return
    M2MB_OS_SUCCESS in case of success

  @return
    M2MB_OS_PTR_ERROR
      Invalid memory area pointer
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service

  @note
    The application must prevent using the memory area after it is released.
    Allowed From
     Initialization and tasks
    Preemption Possible
      Yes

  @b
    Example
  @code
    M2MB_OS_RESULT_E osRes;
    osRes = m2mb_os_free( pStruct, __FUNCTION__, __LINE__ );
    if ( osRes != M2MB_OS_SUCCESS )
      //...

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_free_debug( void *pMem, const CHAR *place, INT32 line );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Dynamic memory reallocation (to be used for debug purposes)

  @details
    Changes the size of the memory block pointed to by ptr.

    The function may move the memory block to a new location (whose address is returned by the function).

    The content of the memory block is preserved up to the lesser of the new and old sizes, even if the block is moved to a new location. If the new size is larger, the value of the newly allocated portion is indeterminate.

    In case that ptr is a null pointer, the function behaves like malloc, assigning a new block of size bytes and returning a pointer to its beginning.

  @param [in]: ptr
    Pointer to a memory block previously allocated with malloc, calloc or realloc.

  @param [in]: size
    New size for the memory block, in bytes.

  @param [in]: place
    the code it is called from (e.g. __FUNCTION__ or __MODULE__)

  @param [in]: line
    the line it is called from (e.g. __LINE__)

  @param [out]
    A pointer to the reallocated memory block, which may be either the same as ptr or a new location.

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    others in case of error : see m2mb_os_types.h

  @note

  @b
    Example
  @code

    UINT32 *ptr;
    UINT32 *new_ptr;
    UINT32 size;
    UINT32 new_size;

    ptr = ( UINT32* ) m2mb_os_calloc ( size * sizef( UINT32 ));

    new_ptr = ( UINT32* ) m2mb_os_realloc (( void *) ptr, new_size * sizeof ( UINT32 ), __FUNCTION__, __LINE__)
    if ( new_ptr == NULL )
    {
      PrintToUart("\n Dynamic memory reallocation error \n");
      return;
    }
    PrintToUart("\n [%d UINT32] dynamic memories were reallocated with the new pointer %d",new_size, new_ptr);

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
void *m2mb_os_realloc_debug( void *ptr, UINT32 size, const CHAR *place, INT32 line );

/* #define M2MB_MEM_DBG */ /* enable here the debug define */
#ifdef M2MB_MEM_DBG
#define m2mb_os_malloc(size)      m2mb_os_malloc_debug(size,__FUNCTION__,__LINE__)
#define m2mb_os_calloc(size)      m2mb_os_calloc_debug(size,__FUNCTION__,__LINE__)
#define m2mb_os_realloc(ptr,size) m2mb_os_realloc_debug(ptr,size,__FUNCTION__,__LINE__)
#define m2mb_os_free(pMem)        m2mb_os_free_debug(pMem,__FUNCTION__,__LINE__)
#endif

#endif /* M2MB_OS_DEBUG_H */
