/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_types.h

  @brief
    M2MB base types (ver. 121626N)

  @details
    <Detailed description of the file>

  @notes
    Dependencies: none

  @author
    Alessandro Papagno

  @date
    20/02/2017
*/

#ifndef M2M_M2MB_TYPES_H
#define M2M_M2MB_TYPES_H

/* Include files ================================================================================*/
#if !defined(NOT_STD_TYPE)
/* here could be included */
#include <stdint.h>
#endif


/* Global declarations ==========================================================================*/


/* in case some parameter is not used , set it 0
   e.g UNUSED( temp, res, 0 )
*/
#define UNUSED_1( a ) ( void )a
#define UNUSED_2( a, b ) ( void )a; ( void )b
#define UNUSED_3( a, b, c ) ( void )a; ( void )b; ( void )c
#define UNUSED_4( a, b, c, d ) ( void )a; ( void )b; ( void )c; ( void )d




/* to align segment: size should be power of 2 */
#define M2MB_ALIGN( x , size )   ( ( ( MEM_W )(x) + size - 1 ) & ( ~( ( MEM_W )size - 1 ) ) )


#if defined(M2M_PFM_LINUX) && defined (NULL)
//to avoid compile error: return makes integer from pointer without a cast [-Werror]
#undef NULL
#define NULL  0  /**< NULL value. */
#endif

#if !defined(NULL)
#define NULL ((void*)0)
#endif


#ifndef M2MB_MIN
#define M2MB_MIN( a, b ) ( ( (a) < (b) ) ? (a) : (b) )
#endif

#ifndef M2MB_MAX
#define M2MB_MAX( a, b ) ( ( (a) > (b) ) ? (a) : (b) )
#endif


#ifndef _M2M_TYPE_H
#define MAX_CMD_ARG 8
#define ARG_LENGTH(...) ARG_LENGTH__(__VA_ARGS__)
#define ARG_LENGTH__(...) ARG_LENGTH_(,__VA_ARGS__,                         \
                                        63, 62, 61, 60,\
                                        59, 58, 57, 56, 55, 54, 53, 52, 51, 50,\
                                        49, 48, 47, 46, 45, 44, 43, 42, 41, 40,\
                                        39, 38, 37, 36, 35, 34, 33, 32, 31, 30,\
                                        29, 28, 27, 26, 25, 24, 23, 22, 21, 20,\
                                        19, 18, 17, 16, 15, 14, 13, 12, 11, 10,\
                                        9,  8,  7,  6,  5,  4,  3,  2,  1,  0)



#define ARG_LENGTH_(_, \
                      _63, _62, _61, _60,\
                      _59, _58, _57, _56, _55, _54, _53, _52, _51, _50,\
                      _49, _48, _47, _46, _45, _44, _43, _42, _41, _40,\
                      _39, _38, _37, _36, _35, _34, _33, _32, _31, _30,\
                      _29, _28, _27, _26, _25, _24, _23, _22, _21, _20,\
                      _19, _18, _17, _16, _15, _14, _13, _12, _11, _10,\
                      _9,   _8,  _7,  _6,  _5,  _4,  _3,  _2,  _1, Count, ...) Count

#endif /* _M2M_TYPE_H */

/* to be used with CMDS_ARGS macro: get even arguments of __VA_ARGS__ */
#define ARG_EVEN(...)           EVEN_DROP_TYPE(ARG_LENGTH(__VA_ARGS__),__VA_ARGS__)
#define EVEN_DROP_TYPE(n,...)   EVEN_DROP_TYPE_(n,__VA_ARGS__)
#define EVEN_DROP_TYPE_(n,...)  EVEN_DROP_TYPE_##n(__VA_ARGS__)
/* catches only second arg => even */
#define EVEN_DROP_TYPE_2(arg1,arg2,...)  arg2
#define EVEN_DROP_TYPE_4(arg1,arg2,...)  arg2, EVEN_DROP_TYPE_2(__VA_ARGS__)
#define EVEN_DROP_TYPE_6(arg1,arg2,...)  arg2, EVEN_DROP_TYPE_4(__VA_ARGS__)
#define EVEN_DROP_TYPE_8(arg1,arg2,...)  arg2, EVEN_DROP_TYPE_6(__VA_ARGS__)
#define EVEN_DROP_TYPE_10(arg1,arg2,...) arg2, EVEN_DROP_TYPE_8(__VA_ARGS__)
#define EVEN_DROP_TYPE_12(arg1,arg2,...) arg2, EVEN_DROP_TYPE_10(__VA_ARGS__)
#define EVEN_DROP_TYPE_14(arg1,arg2,...) arg2, EVEN_DROP_TYPE_12(__VA_ARGS__)
#define EVEN_DROP_TYPE_16(arg1,arg2,...) arg2, EVEN_DROP_TYPE_14(__VA_ARGS__)
#define EVEN_DROP_TYPE_18(arg1,arg2,...) arg2, EVEN_DROP_TYPE_16(__VA_ARGS__)
#define EVEN_DROP_TYPE_20(arg1,arg2,...) arg2, EVEN_DROP_TYPE_18(__VA_ARGS__)
#define EVEN_DROP_TYPE_22(arg1,arg2,...) arg2, EVEN_DROP_TYPE_20(__VA_ARGS__)
#define EVEN_DROP_TYPE_24(arg1,arg2,...) arg2, EVEN_DROP_TYPE_22(__VA_ARGS__)
#define EVEN_DROP_TYPE_26(arg1,arg2,...) arg2, EVEN_DROP_TYPE_24(__VA_ARGS__)
#define EVEN_DROP_TYPE_28(arg1,arg2,...) arg2, EVEN_DROP_TYPE_26(__VA_ARGS__)
#define EVEN_DROP_TYPE_30(arg1,arg2,...) arg2, EVEN_DROP_TYPE_28(__VA_ARGS__)
#define EVEN_DROP_TYPE_32(arg1,arg2,...) arg2, EVEN_DROP_TYPE_30(__VA_ARGS__)
#define EVEN_DROP_TYPE_34(arg1,arg2,...) arg2, EVEN_DROP_TYPE_32(__VA_ARGS__)
#define EVEN_DROP_TYPE_36(arg1,arg2,...) arg2, EVEN_DROP_TYPE_34(__VA_ARGS__)
#define EVEN_DROP_TYPE_38(arg1,arg2,...) arg2, EVEN_DROP_TYPE_36(__VA_ARGS__)
#define EVEN_DROP_TYPE_40(arg1,arg2,...) arg2, EVEN_DROP_TYPE_38(__VA_ARGS__)

/* to be used with CMDS_ARGS macro: get odd arguments of __VA_ARGS__ */
#define ARG_ODD(...)           ODD_DROP_TYPE(ARG_LENGTH(__VA_ARGS__),__VA_ARGS__)
#define ODD_DROP_TYPE(n,...)   ODD_DROP_TYPE_(n,__VA_ARGS__)
#define ODD_DROP_TYPE_(n,...)  ODD_DROP_TYPE_##n(__VA_ARGS__)
/* catches only first arg => odd : since they are related to CMDS_ARGS see below
   number of args must be even: cmd and arg must be in pair */
#define ODD_DROP_TYPE_2(arg1,arg2,...)  arg1
#define ODD_DROP_TYPE_4(arg1,arg2,...)  arg1, ODD_DROP_TYPE_2(__VA_ARGS__)
#define ODD_DROP_TYPE_6(arg1,arg2,...)  arg1, ODD_DROP_TYPE_4(__VA_ARGS__)
#define ODD_DROP_TYPE_8(arg1,arg2,...)  arg1, ODD_DROP_TYPE_6(__VA_ARGS__)
#define ODD_DROP_TYPE_10(arg1,arg2,...) arg1, ODD_DROP_TYPE_8(__VA_ARGS__)
#define ODD_DROP_TYPE_12(arg1,arg2,...) arg1, ODD_DROP_TYPE_10(__VA_ARGS__)
#define ODD_DROP_TYPE_14(arg1,arg2,...) arg1, ODD_DROP_TYPE_12(__VA_ARGS__)
#define ODD_DROP_TYPE_16(arg1,arg2,...) arg1, ODD_DROP_TYPE_14(__VA_ARGS__)
#define ODD_DROP_TYPE_18(arg1,arg2,...) arg1, ODD_DROP_TYPE_16(__VA_ARGS__)
#define ODD_DROP_TYPE_20(arg1,arg2,...) arg1, ODD_DROP_TYPE_18(__VA_ARGS__)
#define ODD_DROP_TYPE_22(arg1,arg2,...) arg1, ODD_DROP_TYPE_20(__VA_ARGS__)
#define ODD_DROP_TYPE_24(arg1,arg2,...) arg1, ODD_DROP_TYPE_22(__VA_ARGS__)
#define ODD_DROP_TYPE_26(arg1,arg2,...) arg1, ODD_DROP_TYPE_24(__VA_ARGS__)
#define ODD_DROP_TYPE_28(arg1,arg2,...) arg1, ODD_DROP_TYPE_26(__VA_ARGS__)
#define ODD_DROP_TYPE_30(arg1,arg2,...) arg1, ODD_DROP_TYPE_28(__VA_ARGS__)
#define ODD_DROP_TYPE_32(arg1,arg2,...) arg1, ODD_DROP_TYPE_30(__VA_ARGS__)
#define ODD_DROP_TYPE_34(arg1,arg2,...) arg1, ODD_DROP_TYPE_32(__VA_ARGS__)
#define ODD_DROP_TYPE_36(arg1,arg2,...) arg1, ODD_DROP_TYPE_34(__VA_ARGS__)
#define ODD_DROP_TYPE_38(arg1,arg2,...) arg1, ODD_DROP_TYPE_36(__VA_ARGS__)
#define ODD_DROP_TYPE_40(arg1,arg2,...) arg1, ODD_DROP_TYPE_38(__VA_ARGS__)


/* to easily insert more than one command at once, in function like m2mb_xxx_conf
   or typically m2mb_os_xx_setAttrItem(...) */
#define CMDS( ... )   ARG_LENGTH(__VA_ARGS__), __VA_ARGS__

/*
   To insert CMD and ARG of command as a pair to better control the process:
   e.g.
   CMDS_ARGS( CMD1, ARGofCMD1, CMD2, ARGofCMD2, ..., CMDn, ARGofCMDn )
   cmds - arg pairs    =>   (NCMDS+NARGS)/2          , CMDS ,  ARGS
   since NCMDS = NARGS =>      ( NCMDS   , CMDS ,  ARGS )  as usual
   It checks automatically if num of args are even: if not a compilation error will arise
*/
#define CMDS_ARGS( ... )  (ARG_LENGTH(__VA_ARGS__)/2), ARG_ODD(__VA_ARGS__), ARG_EVEN(__VA_ARGS__)



#define ENUM_TO_INT(Prefix) \
  MIN_ENUM_##Prefix=(int)0x80000000,\
  MAX_ENUM_##Prefix=(int)0x7FFFFFFF

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define kB  (1024)
#define MB  (kB * kB)

/* Global typedefs ==============================================================================*/


#if ( !defined(_M2M_TYPE_H) || defined(TEST_IN_AZ) )
/* typedefs that indicates size and signedness */
typedef char                CHAR;     /*  8-bit plain char used for character data */
typedef float               FLOAT32;  /* 32-bit floating point */
typedef double              FLOAT64;  /* 64-bit extended floating point */
#if ( defined(NOT_STD_TYPE) && !defined(_M2M_TYPE_H) )
typedef signed char         INT8;     /*  8-bit signed integer, used for numeric data */
typedef signed short        INT16;    /* 16-bit signed integer */
typedef signed int          INT32;    /* 32-bit signed integer */
typedef signed long long    INT64;    /* 64-bit signed integer */

typedef unsigned char       UINT8;    /*  8-bit unsigned integer, used for numeric data */
typedef unsigned short      UINT16;   /* 16-bit unsigned integer */
typedef unsigned int        UINT32;   /* 32-bit unsigned integer */
typedef unsigned long long  UINT64;   /* 64-bit unsigned integer */

#elif ( !defined(_M2M_TYPE_H) )
typedef int8_t              INT8;     /*  8-bit signed integer, used for numeric data */
typedef int16_t             INT16;    /* 16-bit signed integer */
typedef int32_t             INT32;    /* 32-bit signed integer */
typedef int64_t             INT64;    /* 64-bit signed integer */

typedef uint8_t             UINT8;    /*  8-bit unsigned integer, used for numeric data */
typedef uint16_t            UINT16;   /* 16-bit unsigned integer */
typedef uint32_t            UINT32;   /* 32-bit unsigned integer */
typedef uint64_t            UINT64;   /* 64-bit unsigned integer */

//typedef uintmax_t           MEM_W;    /* memory width of bus memory : e.g 32 bits */

#endif

#endif

typedef UINT32              MEM_W;   /* memory width of bus memory : e.g 32 bits */
typedef UINT8               BOOLEAN;
typedef INT32               iHANDLE;
typedef UINT32              uiHANDLE;

/* typedefs used by standard */
typedef UINT32 SIZE_T;
typedef INT32  SSIZE_T;
typedef UINT16 DEV_T;
typedef UINT16 MODE_T;
typedef INT32  OFF_T;
typedef UINT32 TIME_T;
typedef UINT32 INODE_T;
typedef UINT8  NLINK_T;

typedef enum
{
  M2MB_RESULT_NOT_SUPPORTED   = -3,
  M2MB_RESULT_SM_UNAVAILABLE  = -2,
  M2MB_RESULT_INVALID_ARG     = -1,
  M2MB_RESULT_FAIL            =  0,
  M2MB_RESULT_SUCCESS         =  1
} M2MB_RESULT_E;

typedef struct
{
  void *pHandler;  /*internal usage*/
} M2MB_HANDLER_T;

typedef void                *HANDLE;

#ifdef _WIN32
#undef errno
#endif // _WIN32

/* Global functions =============================================================================*/

#endif  /* M2M_M2MB_TYPES_H */

