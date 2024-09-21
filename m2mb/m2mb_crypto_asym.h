#ifndef M2MB_ASYM_H_
#define M2MB_ASYM_H_
/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_crypto_asym.h

  @brief
    This file holds the asymmetric crypto definitions

  @details
    <Detailed description of the file>

  @notes
    <Notes>

  @author
    Mor A.

  @date
    03/21/2018
*/
/* Include files ================================================================================*/

/* Local defines ================================================================================*/
/*** RSA ***/

#define RSA1024_CYPHERTEXT_SIZE 128
#define M2MB_PKCS1_OAEP_PADDING_OVERHEAD 42
#define M2MB_PKCS1_V15_PADDING_OVERHEAD  11

/*** ECC ***/



#define M2MB_BIGLEN 9

#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;     /* Boolean value type. */
#define _BOOLEAN_DEFINED
#endif

#ifndef _UINT32_DEFINED
typedef  unsigned long int  uint32;      /* Unsigned 32 bit value */
#define _UINT32_DEFINED
#endif

/* Local typedefs ===============================================================================*/


/* Local statics ================================================================================*/
/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
/* Global functions =============================================================================*/

#endif
