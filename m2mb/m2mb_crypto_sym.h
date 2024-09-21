#ifndef M2MB_SYM_H_
#define M2MB_SYM_H_

/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_crypto_sym.h

  @brief
    Symmetric crypto definitions for apps

  @details
    <Detailed description of the file>

  @notes
    <Notes>

  @author
    A. Mor

  @date
    03/21/2018
*/
/* Include files ================================================================================*/

/* Local defines ================================================================================*/

#define M2MB_AES128_IV_SIZE         16
#define M2MB_AES128_KEY_SIZE        16
#define M2MB_AES192_IV_SIZE         16
#define M2MB_AES192_KEY_SIZE        24
#define M2MB_AES256_IV_SIZE         16
#define M2MB_AES256_KEY_SIZE        32

#define M2MB_TRIPLE_DES_KEY_SIZE        24
#define M2MB_TRIPLE_DES_IV_SIZE         8
#define  M2MB_CRYPTO_DES_IV_BYTE_LEN    8
#define  M2MB_CRYPTO_DES_BLOCK_BYTE_LEN 8
#define  M2MB_CRYPTO_DES_KEY_SIZE       8
#define  M2MB_CRYPTO_3DES_KEY3_SIZE     24
#define  M2MB_CRYPTO_3DES_KEY2_SIZE     16

#define  M2MB_CRYPTO_AES_ECB_BLOCK_SIZE 16

/* Local typedefs ===============================================================================*/

/*** AES ***/

typedef enum
{
  M2MB_CIPHER_ALGO_AES,
  M2MB_CIPHER_ALGO_DES,
  M2MB_CIPHER_ALGO_DES_3_2_,
  M2MB_CIPHER_ALGO_DES_3_3_
} M2MB_CIPHER_ALGO_E;

typedef enum
{
  M2MB_CIPHER_MODE_ECB        = 0,
  M2MB_CIPHER_MODE_CBC,
  M2MB_CIPHER_MODE_CTR,
  M2MB_CIPHER_MODE_XTS,
  M2MB_CIPHER_MODE_CCM,
  M2MB_CIPHER_MODE_CTS,
  M2MB_CIPHER_MODE_INVALID = 0x7FFFFFFF,
} M2MB_CIPHER_MODE_E;

/* Local statics ================================================================================*/
/* Local function prototypes ====================================================================*/
/* Static functions =============================================================================*/
/* Global functions =============================================================================*/

#endif
