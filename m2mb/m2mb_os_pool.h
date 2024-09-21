/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_os_pool.h

  @brief
    OS pool prototypes

  @details
    basic functions for OS pool are provided

  @notes
    Dependencies:
             m2mb_types.h
             m2mb_os_types.h

  @author
    Goran Paic, Alessandro Papagno

  @date
    31/05/2018
*/

#ifndef M2MB_OS_POOL_H
#define M2MB_OS_POOL_H


/* Global declarations ==========================================================================*/

/* must remain 0 */
#define  M2MB_OS_POOL_INVALID       0
#define  M2MB_OS_POOL_INVALID_ATTR  0

#define M2MB_OS_POOL_BLOCK ( ( MEM_W ) 0x1 )
#define M2MB_OS_POOL_BYTE ( ( MEM_W ) 0x0 )

/* in bytes */
#define M2MB_OS_MIN_POOL_SIZE  ( 128 )


/*-----------------------------------------------------------------------------------------------

    can be called directly without specify CMDS_ARGS
    osRes = m2mb_os_pool_setAttrItem_( &poolAttrHandle,
                                      M2MB_OS_POOL_SEL_CMD_CREATE_ATTR, NULL,
                                      M2MB_OS_POOL_SEL_CMD_NAME, "myPool",
                                      M2MB_OS_POOL_SEL_CMD_USRNAME, "myUserPool",
                                      M2MB_OS_POOL_SEL_CMD_POOL_TYPE, M2MB_OS_POOL_BYTE,
                                      M2MB_OS_POOL_SEL_CMD_MEM_START, &poolArea,
                                      M2MB_OS_POOL_SEL_CMD_MEM_SIZE,  Size,
                                      M2MB_OS_POOL_SEL_CMD_VADDR, 0x48000000
                                    );
  -----------------------------------------------------------------------------------------------*/
#ifdef _WIN32
#define m2mb_os_pool_setAttrItem_( h, ... )        m2mb_os_pool_setAttrItem( h, CMDS_ARGS( __VA_ARGS__ ) )
#else
#define m2mb_os_pool_setAttrItem_( h, args... )        m2mb_os_pool_setAttrItem( h, CMDS_ARGS( args ) )
#endif

/* Global typedefs ==============================================================================*/

typedef struct M2MB_OS_POOL_HANDLE_TAG *M2MB_OS_POOL_HANDLE;
typedef struct M2MB_OS_POOL_ATTR_HANDLE_TAG *M2MB_OS_POOL_ATTR_HANDLE;

typedef enum
{
  MEM_PERMISSION_RONLY,
  MEM_PERMISSION_RW,
  MEM_PERMISSION_SHARE
}MEM_PERMISSION_E;

/* see m2mb_os_pool_setItem, m2mb_os_pool_getItem and m2mb_os_pool_setAttrItem
   for the allowd use of M2MB_OS_POOL_SEL_CMD_E values */
typedef enum
{
  M2MB_OS_POOL_SEL_CMD_CREATE_ATTR,
  M2MB_OS_POOL_SEL_CMD_DEL_ATTR,
  /* use as Os name */
  M2MB_OS_POOL_SEL_CMD_NAME,
  /* if set, usr name could be different from Os  */
  M2MB_OS_POOL_SEL_CMD_USRNAME,
  /* use only to get info on memory pool or create one: multiple of block size or multiple of byte */
  M2MB_OS_POOL_SEL_CMD_POOL_TYPE,
  /* select if block type is the size of block granularity for allocation */
  M2MB_OS_POOL_SEL_CMD_BLOCK_SIZE,
  /* use only to get info on pool size or to create a pool */
  M2MB_OS_POOL_SEL_CMD_MEM_SIZE,
  /* select memory where the pool start */
  M2MB_OS_POOL_SEL_CMD_MEM_START,
  /* select Virtual Address to be assigned to the pool: if available */
  M2MB_OS_POOL_SEL_CMD_VADDR,
  /* select the permission for the memory area if available */
  M2MB_OS_POOL_SEL_CMD_PERMISSION,

  /* ---- */
  M2MB_OS_POOL_SEL_CMD_END,
  ENUM_TO_INT( M2MB_OS_POOL_SEL_CMD_E )
} M2MB_OS_POOL_SEL_CMD_E;

typedef struct  M2MB_OS_POOL_CMD_ARG_TAG
{
  MEM_W cmd;
  void *pArg;
} M2MB_OS_POOL_CMD_ARG_T;

typedef enum
{
  M2MB_OS_POOL_BYTE_MEMINFO_FRAGMENTS,       /* the total number of memory fragments in byte pool */
  M2MB_OS_POOL_MEMINFO_BYTES_AVAILABLE,      /* the total number of available bytes in the pool */
  M2MB_OS_POOL_MEMINFO_SIZE,                 /* total size in byte of the pool  */
  M2MB_OS_POOL_MEMINFO_BLOCKS_AVAILABLE,     /* the total number of available blocks in the block pool */
  M2MB_OS_POOL_MEMINFO_BLOCK_SIZE      ,     /* the block size in the block pool */
  M2MB_OS_POOL_MEMINFO_TOTAL_BLOCKS,         /* the total number of blocks in the block pool */
  M2MB_OS_POOL_MEMINFO_BYTES_MAX_OCCUPATION, /* the max occupation of bytes that have been allocated */

  /* ---- */
  M2MB_OS_POOL_MEMINFO_END,
  ENUM_TO_INT( M2MB_OS_POOL_MEMINFO_E )
} M2MB_OS_POOL_MEMINFO_E;

/* Global functions =============================================================================*/


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Pool memory area creation

  @details
    This service creates a pool to be used for allocation via
    m2mb_os_pool_alloc( ... ) function.
    It creates it in the area specified \ or allocate by system with properties
    defined by attributes: size, block or byte granularity, Virtual address, permission...
    In case area is provided, some space will be lost due to internal management structures,
    so the area available will be a bit less than the area provided

  @param [out] pPoolHandle
    pointer to pool handle
  @param [in] pPoolAttrHandle
    pointer to pool attribute handle

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    M2MB_OS_PTR_ERROR
     Invalid starting address
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_SIZE_ERROR
      Size of pool is invalid.
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

  @note
    After successful pool creation, pool attribute will be deleted, their resources free
    and pPoolAttrHandle set to M2MB_OS_POOL_INVALID_ATTR.
    In case of fail, up to the user to remove attribute, by M2MB_OS_POOL_SEL_CMD_DEL_ATTR
    Allowed From
      Initialization and tasks
    Preemption Possible
      No

  @b
    Example
  @code
    M2MB_OS_RESULT_E          osRes;
    M2MB_OS_POOL_ATTR_HANDLE  poolAttrHandle;
    M2MB_OS_POOL_HANDLE       poolHandle;
    MEM_W Size = ( 50 * kB );
    UINT8 poolArea[ 50 * kB ];
    void *ptrAlloc;

    if ( m2mb_os_pool_setAttrItem( M2MB_OS_POOL_SEL_CMD_CREATE_ATTR, &poolAttrHandle, NULL ) != M2MB_OS_SUCCESS )
      exit(error creating attribute );

    // or simply using CMDS_ARGS(...) macro
    osRes = m2mb_os_pool_setAttrItem( &poolAttrHandle,
                                      CMDS_ARGS
                                      (
                                        M2MB_OS_POOL_SEL_CMD_CREATE_ATTR, NULL,
                                        M2MB_OS_POOL_SEL_CMD_NAME, "MyPool",
                                        M2MB_OS_POOL_SEL_CMD_POOL_TYPE, M2MB_OS_POOL_BYTE,
                                        M2MB_OS_POOL_SEL_CMD_MEM_START, &poolArea,
                                        M2MB_OS_POOL_SEL_CMD_MEM_SIZE,  Size,
                                        M2MB_OS_POOL_SEL_CMD_VADDR, 0x48000000
                                      )
                                    );
    //  VAddress feature: no available for now
    if ( osRes != M2MB_OS_SUCCESS )
    {
      m2mb_os_pool_setAttrItem( M2MB_OS_POOL_SEL_CMD_DEL_ATTR, &poolAttrHandle, NULL );
      exit(error creating attribute );
    }


    osRes = m2mb_os_pool_init( &poolHandle, &poolAttrHandle );

    if ( osRes != M2MB_OS_SUCCESS )
      exit(error creating pool );

  @endcode
*/
M2MB_OS_RESULT_E m2mb_os_pool_init( M2MB_OS_POOL_HANDLE *pPoolHandle,
                                    M2MB_OS_POOL_ATTR_HANDLE *pPoolAttrHandle
                                  );


/*-----------------------------------------------------------------------------------------------*/
 /*!

   @brief
    Pool memory area deinit or deletion

   @details
      This service deletes the specified os pool.
      It is the applications responsibility to prevent use of a deleted
      pool.

   @param [in] poolHandle
     handle of the pool

   @return
     M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
  @return
    M2MB_OS_PTR_ERROR
     Invalid starting address
    M2MB_OS_INVALID_ARG
      invalid arguments
    M2MB_OS_SIZE_ERROR
      Size of pool is invalid.
    M2MB_OS_CALLER_ERROR
      Invalid caller of this service.

   @note

   @b

   @code

     osRes = m2mb_os_pool_deinit( poolHandle );

     if ( osRes != M2MB_OS_SUCCESS )
      exit(error deleting pool );

   @endcode

 */
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_pool_deinit( M2MB_OS_POOL_HANDLE poolHandle );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Memory allocation from specified pool

  @details
    This service allocates memory from the memory area associated with the pool.
    In case that poolHandle is M2MB_OS_POOL_INVALID, the function mallocs from systemHEAP.
    The highest allocated amount of memory in block pool can be block size.

  @param [in] poolHandle
    pointer to pool handle
  @param [in] size
    size of the memory to be allocated

  @return
    pointer != NULL in case of success
  @return
    NULL in case of error

  @note

    Allowed From
      Initialization and tasks
      in case of block granularity, allowed also in ISR and timer context
    Preemption Possible
      No

  @b
    Example
  @code

    M2MB_OS_POOL_HANDLE poolHandle;
    void* ptr;

    //set attributes and init pool
    ptr = ( UINT32 * )m2mb_os_pool_malloc( poolHandle, 100);
    if ( ptr == NULL )
      exit(error memory allocation );

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
void *m2mb_os_pool_malloc( M2MB_OS_POOL_HANDLE poolHandle, UINT32 size );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Memory allocation from specified pool and set them to 0

  @details
    This service allocates memory from the memory area associated with the pool and set to 0.
    In case that poolHandle is M2MB_OS_POOL_INVALID, the function callocs from systemHEAP.
    The highest allocated amount of memory in block pool can be block size.

  @param [in] poolHandle
    pointer to pool handle
  @param [in] size
    size of the memory to be allocated

  @return
    pointer != NULL in case of success
  @return
    NULL in case of error

  @note

    Allowed From
      Initialization and tasks
      in case of block granularity, allowed also in ISR and timer context
    Preemption Possible
      No

  @b
    Example
  @code

    M2MB_OS_POOL_HANDLE poolHandle;
    void* ptr;
    //set attributes and init pool
    ptr = ( UINT32 * )m2mb_os_pool_calloc( poolHandle, 100);
    if ( ptr == NULL )
      exit(error memory allocation );

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
void *m2mb_os_pool_calloc( M2MB_OS_POOL_HANDLE poolHandle, UINT32 size );

/*-----------------------------------------------------------------------------------------------*/
 /*!

   @brief
    Reallocate memory from specified pool

   @details
    Changes the size of the memory pool pointed to by ptrOld.

    The function may move the memory pool to a new location (whose address is returned by the function).

    In the byte pool the content of the memory pool is preserved up to the lesser of the new and old sizes, even if the block is moved to a new location. If the new size is larger, the value of the newly allocated portion is indeterminate. Instead in the block pool the content of the memory pool is preserved up to the lesser of the new size and block pool size.

    In case that poolHOld and\or poolHNew are M2MB_OS_POOL_INVALID, the function realloc from systemHEAP if needed.

   @param [in] poolHOld
   @param [in] poolHNew
   @param [in] ptrOld
   @param [in] newsize

   @return

   @note

   @b

   @code

    M2MB_OS_POOL_HANDLE poolHandle;
    M2MB_OS_POOL_HANDLE poolHandle2;
    void* ptrOld, ptrNew;

    //set attributes and init pool
    ptrOld = (UINT32 *)m2mb_os_pool_malloc ( poolHandle1, 128 );
    if ( ptrOld == NULL )
      exit(error memory allocation );
    ptrNew = (UINT32 *)m2mb_os_pool_realloc ( poolHandle1, poolHandle2, ptrOld, 256 );
    if ( ptrNew == NULL )
      exit(error memory reallocation );

   @endcode

 */
/*-----------------------------------------------------------------------------------------------*/
void *m2mb_os_pool_realloc( M2MB_OS_POOL_HANDLE poolHOld, M2MB_OS_POOL_HANDLE poolHNew,
                            void *ptrOld, UINT32 newsize );
/*-----------------------------------------------------------------------------------------------*/
 /*!

   @brief
    Free allocated memory from specified pool
   @details
    This function provides service to free already allocated memory space.
    In case that poolHandle is M2MB_OS_POOL_INVALID, the function tries to free from systemHEAP.

   @param [in]
     poolHandle
   @param [in]
     ptrToFree

   @return
    M2MB_OS_SUCCESS in case of success (others in case of error : see m2mb_os_types.h)
   @note

   @b

   @code

    M2MB_OS_POOL_HANDLE poolHandle;
    void* ptr;
    M2MB_OS_RESULT_E osRes;

    osRes = m2mb_os_pool_free( poolHandle, ptr );
    if ( osRes != M2MB_OS_SUCCESS )
      exit(error memory free );

   @endcode

 */
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_pool_free( M2MB_OS_POOL_HANDLE poolHandle, void *ptrToFree );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific pool attributes for pool creation

  @details
    This function set special information of the specified pool.
    Selecting cmd, a parameter can be set.
    To be used when pool is already created: its right usage is under application responsibility.
    In the case of block pool the block size must be defined.
    In case area is provided as attibute ( M2MB_OS_POOL_SEL_CMD_MEM_START ),
    some space will be lost due to internal management structures,
    so the area available will be a bit less than the area provided

  @param [in\out] pPoolAttrHandle
    pointer to M2MB_OS_POOL_ATTR_HANDLE handle to be set

  @param [in] ...
    variable inputs, N cmds (see M2MB_OS_POOL_SEL_CMD_E) followed by N arguments
    for their respective issued commands. CMDS(...) and CMDS_ARGS(...) macro can be
    used to better insert cmd and parameters and to better control right insertion

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    others in case of error : see m2mb_os_types.h

  @note
    Allowed From
     Initialization, tasks, timers, and ISRs
    Preemption Possible
      No
    Please note
    M2MB_OS_POOL_SEL_CMD_VADDR,
    M2MB_OS_POOL_SEL_CMD_PERMISSION
    not still available

  @b
    Example
  @code

    //see Example init

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_pool_setAttrItem( M2MB_OS_POOL_ATTR_HANDLE *pPoolAttrHandle,
                                           UINT8 nCmds,
                                           ...
                                          );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get specific pool properties

  @details
    This function retrieve special information of the specified pool
    Selecting cmd, a parameter to be retrieved can be set
    To be used when pool is already created: its right usage is under application responsibility
    In case poolHandle is M2MB_OS_POOL_INVALID, retrieve info on systemHeap

  @param [in] poolHandle
    pool handle, identify the pool
  @param [in] selCmd
    Select parameter to be retrieved (see M2MB_OS_POOL_SEL_CMD_E)
  param [out] pOut
    pointer Output parameter, its meaning depend on selCmd used See examples
  @param [in] pIn
    pointer to Parameter to be specified, if any, to get right output See examples
    If parameter is not needed, use NULL

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    others in case of error : see m2mb_os_types.h

  @note
    Allowed From
     Initialization, tasks, timers, and ISRs
    Preemption Possible
      No

    Please note
    M2MB_OS_POOL_SEL_CMD_VADDR,
    M2MB_OS_POOL_SEL_CMD_PERMISSION
    not still available

  @b
    Example
  @code

    M2MB_OS_POOL_HANDLE poolHandle;
    MEM_W pOut;

    res = m2mb_os_pool_getItem ( poolHandle, M2MB_OS_POOL_SEL_CMD_USRNAME, &pOut, NULL );
    if ( res != M2MB_OS_SUCCESS )
      exit(error get attributes );
    PrintToUart("\n M2MB_OS_POOL_SEL_CMD_USRNAME: %s", pOut);
    res = m2mb_os_pool_getItem ( poolHandle, M2MB_OS_POOL_SEL_CMD_NAME, &pOut, NULL );
    PrintToUart("\n M2MB_OS_POOL_SEL_CMD_NAME: %s", pOut);
    if ( res != M2MB_OS_SUCCESS )
      exit(error get attributes );
    res = m2mb_os_pool_getItem ( poolHandle, M2MB_OS_POOL_SEL_CMD_POOL_TYPE, &pOut, NULL );
    PrintToUart("\n M2MB_OS_POOL_SEL_CMD_POOL_TYPE: %d", pOut);
    res = m2mb_os_pool_getItem ( poolHandle, M2MB_OS_POOL_SEL_CMD_BLOCK_SIZE, &pOut, NULL );
    if ( res != M2MB_OS_SUCCESS )
      exit(error get attributes );
    PrintToUart("\n M2MB_OS_POOL_SEL_CMD_BLOCK_SIZE: %d", pOut);
    res = m2mb_os_pool_getItem ( poolHandle, M2MB_OS_POOL_SEL_CMD_MEM_SIZE, &pOut,NULL );
    if ( res != M2MB_OS_SUCCESS )
      exit(error get attributes );
    PrintToUart("\n M2MB_OS_POOL_SEL_CMD_MEM_SIZE: %d", pOut);
    res = m2mb_os_pool_getItem ( poolHandle, M2MB_OS_POOL_SEL_CMD_MEM_START, &pOut, NULL );
    if ( res != M2MB_OS_SUCCESS )
      exit(error get attributes );
    PrintToUart("\n M2MB_OS_POOL_SEL_CMD_MEM_START: 0x%x", pOut);

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_pool_getItem( M2MB_OS_POOL_HANDLE poolHandle,
                                       M2MB_OS_POOL_SEL_CMD_E selCmd,
                                       MEM_W *pOut,
                                       void  *pIn
                                      );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Set specific pool properties

  @details
    This function set value for specified parameter of the specified pool
    Selecting cmd, a parameter to be set can be selected
    To be used when pool is already created: it is under application responsibility

  @param [in] poolHandle
    pool handle identify the pool to set parameter to
  @param [in] selCmd
    Select parameter to be set (see M2MB_OS_POOL_SEL_CMD_E)
  @param [in] pIn
    pointer to Parameter to be specified, if any, to get right output See examples
    If parameter is not needed, use NULL

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    others in case of error : see m2mb_os_types.h

  @note
    Allowed From
     Initialization, tasks, timers, and ISRs
    Preemption Possible
      No
    Only certain parameters can be set once the pool is created. Available values are:
    M2MB_OS_POOL_SEL_CMD_NAME
    M2MB_OS_POOL_SEL_CMD_USRNAME

  @b
    Example
  @code

  M2MB_OS_POOL_HANDLE poolHandle;
  CHAR MyPoolName[] = "myPoolName";


  res = m2mb_os_pool_setItem ( poolHandle, M2MB_OS_POOL_SEL_CMD_NAME, (void*)MyPoolName );
  if ( res != M2MB_OS_SUCCESS )
    exit(error get attributes );



  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_pool_setItem( M2MB_OS_POOL_HANDLE poolHandle,
                                       M2MB_OS_POOL_SEL_CMD_E selCmd,
                                       void  *pIn
                                      );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Get memory information from specific pool

  @details
    This function returns pool information:
      -the total number of memory fragments in byte pool
      -the total number of available bytes in the byte pool
      -the total number of bytes of the pool
      -the total number of available blocks in the block pool
      -the total number of blocks in the block pool

      in case poolHandle is NULL, it is considered as system heap pool

  @param [in] poolHandle
  @param [in] type of information required: M2MB_OS_POOL_MEMINFO_E
  @param [out] value of information required: *pOut

  @return
    M2MB_OS_SUCCESS in case of success
  @return
    others in case of error : see m2mb_os_types.h

  @note
    Allowed From
     Initialization, tasks, timers, and ISRs
    Preemption Possible
      No

  @b
    Example
  @code

  UINT32 res;
  MEM_W available_block_pool, total_block_pool, fragments_byte_pool, available_byte_pool;
  M2MB_OS_POOL_HANDLE poolHandle1, poolHandle2;

  //see pool init for creating two pools ( one block and one byte )

  res = m2mb_os_pool_memInfo( poolHandle1, M2MB_OS_POOL_MEMINFO_BLOCKS_AVAILABLE, &available_block_pool );
  if ( res != M2MB_OS_SUCCESS )
  {
    PrintToUart("\n Memory Information size error %d", res);
    return;
  }
  res = m2mb_os_pool_memInfo( poolHandle1, M2MB_OS_POOL_MEMINFO_TOTAL_BLOCKS, &total_block_pool );
  if ( res != M2MB_OS_SUCCESS )
  {
    PrintToUart("\n Memory Information size error %d", res);
    return;
  }
  res = m2mb_os_pool_memInfo( poolHandle2, M2MB_OS_POOL_BYTE_MEMINFO_FRAGMENTS, &fragments_byte_pool );
  if ( res != M2MB_OS_SUCCESS )
  {
    PrintToUart("\n Memory Information size error %d", res);
    return;
  }
  res = m2mb_os_pool_memInfo( poolHandle2, M2MB_OS_POOL_MEMINFO_BYTES_AVAILABLE, &available_byte_pool );
  if ( res != M2MB_OS_SUCCESS )
  {
    PrintToUart("\n Memory Information size error %d", res);
    return;
  }

  PrintToUart("\n Memory Informations: \n MEMINFO_BLOCKS_AVAILABLE: %d \n TOTAL_BLOCKS: %d \n BYTE_FRAGMENTS: %d \n BYTES_AVAILABLE: %d \n", available_block_pool, total_block_pool, fragments_byte_pool, available_byte_pool );


  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_OS_RESULT_E m2mb_os_pool_memInfo( M2MB_OS_POOL_HANDLE poolHandle,
                                       M2MB_OS_POOL_MEMINFO_E memInfoSelect,
                                       MEM_W  *pOut );


#endif /* M2MB_OS_POOL_H */

