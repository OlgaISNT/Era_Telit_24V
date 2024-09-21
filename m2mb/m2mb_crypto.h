/*===============================================================================================*/
/*         >>> Copyright (C) Telit Communications S.p.A. Italy All Rights Reserved. <<<          */
/*!
  @file
    m2m/m2m_generic/common/m2mb_inc/m2mb_crypto.h

  @brief
    Crypto APIs implementation.

  @details
    Crypto apis including hash, hmac, asymmetric & symmetric key operations

  @notes
    Dependencies:
    m2mb_types.h


  @author
    Tiran Kaskas, Alessandro Papagno

  @date
    10/09/2017
*/

#ifndef M2M_M2MB_CRYPTO_H
#define M2M_M2MB_CRYPTO_H

/**
   @defgroup M2MB_CRYPTO
   @brief Cryptography and hashing functionalities.
*/

/* Global declarations ==========================================================================*/

#define TZ_LOGGING

/* free choice to use free or deinit in Telit style */
#define m2mb_crypto_md_free     m2mb_crypto_md_deinit
#define m2mb_crypto_hmac_free   m2mb_crypto_hmac_deinit
#define m2mb_crypto_cmac_free   m2mb_crypto_cmac_deinit
#define m2mb_crypto_aes_free    m2mb_crypto_aes_deinit
#define m2mb_crypto_des_free    m2mb_crypto_des_deinit
#define m2mb_crypto_rsa_free    m2mb_crypto_rsa_deinit
#define m2mb_crypto_ecc_free    m2mb_crypto_ecc_deinit




/* Global typedef ==========================================================================*/

/*!
  @brief
    Enumeration representing all supported Crypto algorithms.
*/
typedef enum
{
  M2MB_CRYPTO_CTX_ALG_NONE=0,
  M2MB_CRYPTO_CTX_ALG_MD2,
  M2MB_CRYPTO_CTX_ALG_MD4,
  M2MB_CRYPTO_CTX_ALG_MD5,
  M2MB_CRYPTO_CTX_ALG_SHA1,
  M2MB_CRYPTO_CTX_ALG_SHA224,
  M2MB_CRYPTO_CTX_ALG_SHA256,
  M2MB_CRYPTO_CTX_ALG_SHA384,
  M2MB_CRYPTO_CTX_ALG_SHA512,
  M2MB_CRYPTO_CTX_ALG_HMAC,
  M2MB_CRYPTO_CTX_ALG_CMAC,
  M2MB_CRYPTO_CTX_ALG_RSA,
  M2MB_CRYPTO_CTX_ALG_ECC,
  M2MB_CRYPTO_CTX_ALG_AES,
  M2MB_CRYPTO_CTX_ALG_DES,
  ENUM_TO_INT( M2MB_CRYPTO_CTX_ALG_E )
} M2MB_CRYPTO_CTX_ALG_E;

/*!
  @brief
    Opaque structure for context
*/
typedef struct M2MB_CRYPTO_GEN_CONTEXT_TAG *M2MB_CRYPTO_GEN_CONTEXT;


/*!
  @brief
    Opaque structure for context
*/
typedef struct M2MB_CRYPTO_MD_CONTEXT_TAG *M2MB_CRYPTO_MD_CONTEXT;

/*!
  @brief
    Opaque structure for context
*/
typedef struct M2MB_CRYPTO_HMAC_CONTEXT_TAG *M2MB_CRYPTO_HMAC_CONTEXT;

/*!
  @brief
    Opaque structure for context
*/
typedef struct M2MB_CRYPTO_CMAC_CONTEXT_TAG *M2MB_CRYPTO_CMAC_CONTEXT;

/*!
  @brief
    Opaque structure for context
*/
typedef struct M2MB_CRYPTO_AES_CONTEXT_TAG *M2MB_CRYPTO_AES_CONTEXT;

/*!
  @brief
    Opaque structure for context
*/
typedef struct M2MB_CRYPTO_DES_CONTEXT_TAG *M2MB_CRYPTO_DES_CONTEXT;

/*!
  @brief
    Opaque structure for context
*/
typedef struct M2MB_CRYPTO_RSA_CONTEXT_TAG *M2MB_CRYPTO_RSA_CONTEXT;

/*!
  @brief
    Opaque structure for context
*/
typedef struct M2MB_CRYPTO_ECC_CONTEXT_TAG *M2MB_CRYPTO_ECC_CONTEXT;

/*!
  @brief
    Enumeration respresenting all supported HASH algorithms.
*/
typedef enum
{
  M2MB_CRYPTO_MD_ALG_NONE=0,
  M2MB_CRYPTO_MD_ALG_MD2,
  M2MB_CRYPTO_MD_ALG_MD4,
  M2MB_CRYPTO_MD_ALG_MD5,
  M2MB_CRYPTO_MD_ALG_SHA1,
  M2MB_CRYPTO_MD_ALG_SHA224,
  M2MB_CRYPTO_MD_ALG_SHA256,
  M2MB_CRYPTO_MD_ALG_SHA384,
  M2MB_CRYPTO_MD_ALG_SHA512,
  ENUM_TO_INT( M2MB_CRYPTO_MD_ALG_E )
} M2MB_CRYPTO_MD_ALG_E;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Returns the Crypto algorithm.

  @details
    This function returns an enum that indicates what algorithm was used to initialize it.

  @param[in] ctx
    Pointer to a context structure.

  @return
    M2MB_CRYPTO_CTX_ALG_E

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_CRYPTO_CTX_ALG_E m2mb_crypto_get_algo( M2MB_CRYPTO_GEN_CONTEXT ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Returns the context ID for the given context structure

  @details
    This function returns the context ID for the given context structure, as the structure is opaque.

  @param[in] ctx
    Pointer to a context structure.
  @param[out] info
    Pointer to return the context id.
    Must be allocated by caller.
    If set to NULL, the function will return the size required for the info.

  @return
    The required size of the info buffer.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
UINT32 m2mb_crypto_ctx_info_for_tee( M2MB_CRYPTO_GEN_CONTEXT ctx, UINT8 *info );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates an M2MB_CRYPTO_MD_CONTEXT structure

  @details
    This function allocates an MD context to be later used by any MD function.

  @param[in] ctx
    Pointer to a context structure to be allocated by this function.
    Any consecutive call to MD function must use the context.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    Context must be free via a call to m2mb_crypto_md_deinit().

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_md_init( M2MB_CRYPTO_MD_CONTEXT *ctx );

/*!
  @brief
    Sets up a M2MB_CRYPTO_MD_CONTEXT structure

  @details
    This function performs setup of a MD context to be later used by any MD function.

  @param[in] ctx
    Pointer to a previously allocated context structure

  @param[in] md_info
    The message digest method to use.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    Has typicalli to be used after m2mb_crypto_md_init call, to set up the handler

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_md_setup( M2MB_CRYPTO_MD_CONTEXT              ctx,
                                    const M2MB_CRYPTO_MD_ALG_E          md_info );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Continuously hashing of data.
    This function should be called repeatedly with chunks of the message to be hashed (len bytes from data).

  @details
    This function is used to continuosly hash data (streaming data when not all buffer to be hashed is available in one shot).
    This function should be called repeatedly with chunks of the message to be hashed (len bytes from data).

  @param[in] ctx
    Pointer to a previously allocated context structure

  @param[in] data
    Pointer of the starting data to hash

  @param[in] len
    length of the data to hash

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_md_update( M2MB_CRYPTO_MD_CONTEXT         ctx,
                                     const UINT8                    *data,
                                     SIZE_T                         len );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This function computes the final digest for the hash process

  @details
    This function is the final step for digest calculation in a streaming mode.
    It will return the calculated digest based on all the previous messages (sent via m2mb_crypto_md_update)

  @param[in] ctx
    Pointer to a previously initialized context structure

  @param[in] md
    Message digest array to be returned from the hash function.
    Must be large enough to hold the result message digest based on the hash algorithm used.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_md_final( M2MB_CRYPTO_MD_CONTEXT         ctx,
                                    UINT8                         *md );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This function frees an MD context.

  @details
    This function free the provided context.
    No further use of the context shall be made.

  @param[in] ctx
    Pointer to a previously allocated context structure to be free.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_md_deinit( M2MB_CRYPTO_MD_CONTEXT ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This function will compute the message digest in one shot, in case of limited and available size of data

  @details
    This function is used to create a message digest for a given data.
    It will compute the required message digest of <len> bytes from <data> and places it in md.
    If md is NULL, the digest is placed in a static array which address is provided on return.
    This function is similar of running the following function flow in one shot:
    m2mb_crypto_md_init();
    m2mb_crypto_md_setup();
    m2mb_crypto_md_update();
    m2mb_crypto_md_final();
    m2mb_crypto_md_deinit();

  @param[in] md_info
    The message digest method to use.

  @param[in] data
    Pointer to the data to hash

  @param[in] len
    length of the data to hash

  @param[out] md
    Message digest array to be returned from the hash function.
    Must be large enough to hold the result message digest based on the hash algorithm used.

  @return
    if md is provided
    returned value equal to md in case of success
  @return
    If md is NULL, returns a pointer to a static array containing the message digest in case of success
  @return
    NULL in case of fail

  @note
    Setting md to NULL is not thread safe.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
UINT8 *m2mb_crypto_md( const M2MB_CRYPTO_MD_ALG_E   md_info,
                       const UINT8                 *data,
                       SIZE_T                       len,
                       UINT8                       *md );


/* Global declarations ==========================================================================*/

/*!
  @brief
    Opaque structure for context
*/


/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates a M2MB_CRYPTO_HMAC_CONTEXT structure

  @details
    This function allocates an HMAC context to be later used by any HMAC function.

  @param[in] ctx
    Pointer to a context structure to be allocated by this function.
    Any consecutive call to HMAC function must use the context.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_hmac_init( M2MB_CRYPTO_HMAC_CONTEXT *ctx );

/*!
  @brief
    Sets up an M2MB_CRYPTO_HMAC_CONTEXT structure

  @details
    This function performs setup of a HMAC context providing the hash algorithm to use and the symmetric key.
    Usually comes after m2mb_crypto_hmac_init() or m2mb_crypto_hmac_final().
    Might also be called after m2mb_crypto_hmac_update() in order to restart the digest process.

  @param[in] ctx
    Pointer to a previously allocated context structure

  @param[in] md_info
    The hash algorithm to use for the HMAC

  @param[in] key
    The symmetric key to use for the hmac

  @param[in] keylen
    The length of the provided key in bytes

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    It is under user responsibility the correct use of keylen, conformed with M2MB_CRYPTO_MD_ALG_E,
    for key pointer

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_hmac_setup( M2MB_CRYPTO_HMAC_CONTEXT         ctx,
                                      M2MB_CRYPTO_MD_ALG_E             md_info,
                                      const UINT8                     *key,
                                      SIZE_T                           keylen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Continuously HMAC of data.
    This function can be called repeatedly similar to streaming

  @details
    This function is used to continuosly hash data (streaming data when not all buffer to be hashed is available in one shot).
    This function can be called repeatedly.

  @param[in] ctx
    A previously allocated hmac context.
    The context setup must be also done before using this function (using m2mb_crypto_hmac_setup)

  @param[in] data
    Buffer pointer holding the data as start of hmacking

  @param[in] len
    length of the provided data

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_hmac_update( M2MB_CRYPTO_HMAC_CONTEXT         ctx,
                                       const UINT8                     *data,
                                       SIZE_T                           len );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This function computes the final HMAC for all previously provided messages for the given context.

  @details
    This function is the final step for HMAC calculation.
    It returns the calculated HMAC based on all the previous messages (sent via m2mb_crypto_hmac_update)

  @param[in] ctx
    A previously allocated hmac context.
    The context setup must be also done before using this function (using m2mb_crypto_hmac_setup)

  @param[out] hmac
    The calculated hmac.
    Must be large enough to hold the HMAC for the selected message digest algorithm used during setup.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_hmac_final( M2MB_CRYPTO_HMAC_CONTEXT         ctx,
                                      UINT8                           *hmac );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Prepare to run HMAC for a new message reusing the context

  @details
    Resets all previously provided messages.
    The same key & hash algorithm provided to m2mb_crypto_hmac_setup() remains and will be used for new HMAC.
    Usually used after m2mb_crypto_hmac_final() and before m2mb_crypto_hmac_update()

  @param[in] ctx
    Pointer to a previously allocated context structure to reset

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_hmac_reset( M2MB_CRYPTO_HMAC_CONTEXT  ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This function free an HMAC context releasing the associated resources

  @details
    This function release the provided context memory.

  @param[in] ctx
    Pointer to a previously initialized context structure to free

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_hmac_deinit( M2MB_CRYPTO_HMAC_CONTEXT ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This function will compute the HMAC in one shot without needing any context.

  @details
    This function is used to calculate an HMAC for a given data, key & hash algorithm.
    If hmac is NULL, the HMAC is placed in a static array.
    This function is similar to running the following function flow:
    m2mb_crypto_hmac_init();
    m2mb_crypto_hmac_setup();
    m2mb_crypto_hmac_update();
    m2mb_crypto_hmac_final();
    m2mb_crypto_hmac_deinit();

  @param[in] md_info
    The hash algorithm to use for the HMAC

  @param[in] key
    The symmetric key to use for the HMAC

  @param[in] keylen
    The length of the provided key in bytes

  @param[in] input
    Buffer holding the data to compute HMAC for

  @param[in] inlen
    length of the provided data

  @param[out] hmac
    The calculated HMAC

  @return
    if hmac is provided
    returned value equal to hmac in case of success
  @return
    If hmac is NULL, returns a pointer to a static array containing the HMAC in case of success
  @return
    NULL in case of fail

  @note
    Setting hmac to NULL is not thread safe.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
UINT8 *m2mb_crypto_hmac( M2MB_CRYPTO_MD_ALG_E   md_info,
                         const UINT8           *key,
                         const SIZE_T           keylen,
                         const UINT8           *input,
                         const SIZE_T           inlen,
                         UINT8                 *hmac );

/** @} */

/**
   @addtogroup CMAC M2MB_CMAC
   @ingroup M2MB_CRYPTO
   @brief Ciphered MAC implementation, per FIPS pub 198.
   @{
*/

/* Global declarations ==========================================================================*/


typedef enum
{
  M2MB_CRYPTO_CMAC_ALG_AES_128 = 1,
  M2MB_CRYPTO_CMAC_ALG_AES_192 = 2,
  M2MB_CRYPTO_CMAC_ALG_AES_256 = 3,
  ENUM_TO_INT( M2MB_CRYPTO_CMAC_ALG_E )
}  M2MB_CRYPTO_CMAC_ALG_E;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates a M2MB_CRYPTO_CMAC_CONTEXT structure

  @details
    This function allocates an CMAC context to be later used by any CMAC function.

  @param[in] ctx
    Pointer to a context structure to be allocated by this function.
    Any consecutive call to CMAC function must use the context.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_cmac_init( M2MB_CRYPTO_CMAC_CONTEXT *ctx );

/*!
  @brief
    Sets up an M2MB_CRYPTO_CMAC_CONTEXT structure

  @details
    This function performs setup of a CMAC context providing the hash algorithm to use and the symmetric key.
    Usually comes after m2mb_crypto_cmac_init() or m2mb_crypto_cmac_final().
    Might also be called after m2mb_crypto_cmac_update() in order to restart the digest process.

  @param[in] ctx
    Pointer to a previously allocated context structure

  @param[in] key
    The symmetric key to use for the cmac.

  @param[in] keylen
    The length in bytes of the provided key.
    Only 128 bits & 256 bits are supported (keylen=16,32 bytes)

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_cmac_setup( M2MB_CRYPTO_CMAC_CONTEXT         ctx,
                                      const UINT8                     *key,
                                      SIZE_T                           keylen );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    continuously CMAC of data.
    This function can be called repeatedly similar to streaming

  @details
    This function is used to continuously hash data (streaming data when not all buffer to be hashed is available in one shot).
    This function can be called repeatedly.

  @param[in] ctx
    A previously allocated cmac context.
    The context setup must be also done before using this function (using m2mb_crypto_cmac_setup)

  @param[in] data
    Buffer holding the data

  @param[in] len
    length of the provided data

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_cmac_update( M2MB_CRYPTO_CMAC_CONTEXT         ctx,
                                       const UINT8                     *data,
                                       SIZE_T                           len );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This function computes the final CMAC for all previously provided messages for the given context.

  @details
    This function is the final step for CMAC calculation.
    It will return the calculated CMAC based on all the previous messages (sent via m2mb_crypto_cmac_update)

  @param[in] ctx
    A previously allocated cmac context.
    The context setup must be also done before using this function (using m2mb_crypto_cmac_setup)

  @param[out] cmac
    The calculated cmac.
    Must be large enough to0 hold the CMAC per the selected message digest algorithm used for the setup.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_cmac_final( M2MB_CRYPTO_CMAC_CONTEXT         ctx,
                                      UINT8                           *cmac );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Prepare to run CMAC for a new message.

  @details
    Resets all previously provided messages.
    The same key & hash algorithm provided to m2mb_crypto_cmac_setup() remains and will be used for new CMAC.
    Usually used after m2mb_crypto_cmac_final() and before m2mb_crypto_cmac_update()

  @param[in] ctx
    Pointer to a previously allocated context structure to reset

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_cmac_reset( M2MB_CRYPTO_CMAC_CONTEXT  ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This function free an CMAC context

  @details
    This function free the provided context memory.

  @param[in] ctx
    Pointer to a previously initialized context structure to free

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_cmac_deinit( M2MB_CRYPTO_CMAC_CONTEXT  ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    This function will compute the CMAC in one shot without needing any context.

  @details
    This function is used to calculate an CMAC for a given data, key & hash algorithm.
    If cmac is NULL, the CMAC is placed in a static array.
    This function is similar to running the following function flow:
    m2mb_crypto_cmac_init();
    m2mb_crypto_cmac_setup();
    m2mb_crypto_cmac_update();
    m2mb_crypto_cmac_final();
    m2mb_crypto_cmac_deinit();

  @param[in] key
    The symmetric key to use for the CMAC

  @param[in] keylen
    The length of the provided key in bytes

  @param[in] input
    Buffer holding the data to compute CMAC for

  @param[in] inlen
    length of the provided data

  @param[out] cmac
    The calculated CMAC

  @return
    If cmac is NULL, Upon successful completion returns a pointer to a static array containing the CMAC, 0 otherwise.

  @note
    Setting cmac to NULL is not thread safe.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
UINT8 *m2mb_crypto_cmac( M2MB_CRYPTO_CMAC_ALG_E cmac_alg,
                         const UINT8           *key,
                         const SIZE_T           keylen,
                         const UINT8           *input,
                         const SIZE_T           inlen,
                         UINT8                 *cmac );


/**
   @addtogroup AES M2MB_AES
   @ingroup M2MB_CRYPTO
   @brief AES CBC & ECB algorithm implementation.
    https://tools.ietf.org/pdf/rfc3602.pdf
    http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
   @{
*/

/* Global declarations ==========================================================================*/



/*!
  @brief
    AES operation mode (encrypt decrypt)
*/
typedef enum
{
  M2MB_CRYPTO_AES_MODE_ENCRYPT,
  M2MB_CRYPTO_AES_MODE_DECRYPT,
  ENUM_TO_INT( M2MB_CRYPTO_AES_MODE_E )
} M2MB_CRYPTO_AES_MODE_E;

/*! AES ecb block size */
#define M2MB_CRYPTO_AES_ECB_BLOCK_SIZE 16

/*! AES ecb initialization vector size */
#define M2MB_CRYPTO_AES_IV_SIZE 16


/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates an M2MB_CRYPTO_AES_CONTEXT structure

  @details
    This function allocates an AES context to be later used by any AES ciphering function.

  @param[in] ctx
    Pointer to a context structure to be allocated by this function.
    Any consecutive call to AES function must use the context.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_aes_init( M2MB_CRYPTO_AES_CONTEXT *ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Free an M2MB_CRYPTO_AES_CONTEXT structure

  @details
    This function free a previously allocated AES context.

  @param[in] ctx
    Pointer to a context structure to be free by this function.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_aes_deinit( M2MB_CRYPTO_AES_CONTEXT  ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Generates an AES encryption key for a given context.

  @details
    This function generates a symmetric AES key with a given size and stores it into the context to be later used by any AES encryption function.
    AES is a symmetric encryption, i.e. same key is used for encryption and decryption.

  @param[in] ctx
    Pointer to a context structure previously initialized by m2mb_crypto_aes_init()

  @param[in] keybits
    The number of bits in the given key, must be 128, 192 or 256

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_aes_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_aes_generate_key( M2MB_CRYPTO_AES_CONTEXT         ctx,
                                            UINT32                          keybits );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Exports the AES symmetric key from the given context.

  @details
    This function exports the AES symmetric key from the given context.
    The key information will be exported from the given context (assuming context has keys associated, either imported or via m2mb_crypto_aes_generate_key()).
    The key blob is encrypted with a device specific key and can only be used on the same HW originating it.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init.

  @param[out] keyblob
    The buffer that will hold the result keyblob representing the exported AES symmetric key.
    If keyblob ptr is NULL, this function will return the number of bytes required for the keyblob via the keyblob_length pointer.
    The key blob is encrypted with a device specific key and can only be used on the same HW originating it.

  @param[out] keyblob_length
    The length of the AES key blob

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_aes_init() & m2mb_crypto_aes_generate_key() (or keys were set via m2mb_crypto_aes_set_encrypt_key(), m2mb_crypto_aes_set_decrypt_key()) must be called before using this function.

  @b
    Example
  @code
    int main()
    {
      M2MB_RESULT_E                  res ;

      M2MB_CRYPTO_AES_CONTEXT_T *    ctx = NULL;
      UINT8   *keyblob = NULL;
      UINT32  keyblob_length;
......
      res = m2mb_crypto_aes_init( &ctx );

      res = m2mb_crypto_aes_generate_key( ctx,256);

      res = m2mb_crypto_aes_keyblob_export( ctx,
                                            keyblob,
                                            &keyblob_length );

      keyblob = (UINT8*)m2mb_os_malloc( keyblob_length );
.....
      res = m2mb_crypto_aes_keyblob_export( ctx,
                                          keyblob,
                                          &keyblob_length );

    }
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_aes_keyblob_export( M2MB_CRYPTO_AES_CONTEXT         ctx,
                                              UINT8                          *keyblob,
                                              UINT32                         *keyblob_length );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Import a symmetric key, previously generated on the same device, into a given context.

  @details
    This function keyblob into a context to be later used by any AES encryption function.
    Assumption made is that this is not a plaintext key but a ciphered key that was previously generated via m2mb_crypto_aes_generate_key() and was exported via m2mb_crypto_aes_key_export().
    Another option might be that the key was generated via the ECDH key derivation (via m2mb_crypto_ecdh_shared_key_derive()).
    AES is a symmetric encryption, i.e. same key is used for encryption and decryption.

  @param[in] ctx
    Pointer to a context structure previously initialized by m2mb_crypto_aes_init().

  @param[in] keyblob
    Pointer to a keyblob to setup.
    The key blob is encrypted with a device specific key and can only be used on the same HW originating it.

  @param[in] keyblob_length
    The length of the keyblob buffer.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_aes_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_aes_keyblob_import( M2MB_CRYPTO_AES_CONTEXT         ctx,
                                              const UINT8                    *keyblob,
                                              UINT32                          keyblob_length );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Imports a symmetric key into the given context.
    This is a plaintext key that may have been generated externally.

  @details
    This function sets up a key with a given size to a context to be later used by any AES encryption function.
    Assumption made is that this is a plaintext key and this API is to serve a key that was generated outside of the device.
    AES is a symmetric encryption, i.e. same key is used for encryption and decryption.

  @param[in] ctx
    Pointer to a context structure previously initialized by m2mb_crypto_aes_init().

  @param[in] key
    Pointer to a key to setup. The key is in the size of keybits bits.
    This key is a plaintext key created externally.

  @param[in] keylength
    The number of bytes in the given key, must be 16, 26, 32 (128, 192, 256 bits key).

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_aes_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_aes_externalkey_import ( M2MB_CRYPTO_AES_CONTEXT         ctx,
                                                   const UINT8                    *key,
                                                   UINT32                          keylength );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Encrypts/decrypts a single 16 bytes block onto the output buffer using the AES ECB algorithm.

  @details
    This function will encrypt/decrypt (depending on the <mode> parameter) a given 16 bytes length input buffer and will store the result into the output 16 bytes buffer.

  @param[in] ctx
    Pointer to a context structure previously initialized by m2mb_crypto_aes_init()

  @param[in] mode
    The requested operation mode (encrypt/decrypt)

  @param[in] in
    The input buffer to encrypt/decrypt, 16 bytes length

  @param[out] out
    The output buffer to store the result encryption/decryption values, must have 16 bytes allocated space

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_aes_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_aes_ecb_encdec( M2MB_CRYPTO_AES_CONTEXT         ctx,
                                          M2MB_CRYPTO_AES_MODE_E          mode,
                                          const UINT8                     in[M2MB_CRYPTO_AES_ECB_BLOCK_SIZE],
                                          UINT8                           out[M2MB_CRYPTO_AES_ECB_BLOCK_SIZE] );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Encrypts/decrypts a given buffer with a given length using the AES CBC.
    The length must be a multiple of the block size (16 bytes).

  @details
    This function will encrypt/decrypt (depending on the mode parameter) <inlen> bytes from the <in> buffer and store the output into the <out> buffer.
    The ivec will be updated as part of this function call to allow consecuritve calls to encrypt a stream of data.
    If it is required to encrypt several independant buffers (which are not related to each other), the ivec should be restored by the caller (i.e. storing a local copy of the ivec).

  @param[in] ctx
    Pointer to a context structure previously initialized by m2mb_crypto_aes_init()

  @param[in] mode
    The requested operation mode (encrypt/decrypt)

  @param[in] in
    The input buffer to encrypt/decrypt

  @param[out] out
    The output buffer to store the result encryption/decryption values, must have 16 bytes allocated space

  @param[in] inlen
    The length of the input data (<in>), must be multiple of block size (16 bytes)

  @param[in] ivec
    Initialization vector. The ivec is updated after each use to allow working in "streaming" mode

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_aes_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_aes_cbc_encdec( M2MB_CRYPTO_AES_CONTEXT         ctx,
                                          M2MB_CRYPTO_AES_MODE_E          mode,
                                          const UINT8                    *in,
                                          UINT8                          *out,
                                          SIZE_T                          inlen,
                                          UINT8                           ivec[M2MB_CRYPTO_AES_IV_SIZE] );

/** @} */

/**
   @addtogroup DES M2MB_DES
   @ingroup M2MB_CRYPTO
   @brief DES & 3DES CBC & ECB algorithm implementation.
   @{
*/

/* Global declarations ==========================================================================*/



/*! Cipher algorithm to be used by the DES context */
typedef enum
{
  M2MB_CRYPTO_ALG_DES,
  M2MB_CRYPTO_ALG_3DES,
  ENUM_TO_INT( M2MB_CRYPTO_DES_ALG_TYPE_E )
} M2MB_CRYPTO_DES_ALG_TYPE_E;

/*! DES operation mode (encrypt/decrypt) */
typedef enum
{
  M2MB_CRYPTO_DES_MODE_ENCRYPT,
  M2MB_CRYPTO_DES_MODE_DECRYPT,
  ENUM_TO_INT( M2MB_CRYPTO_DES_MODE_E )
} M2MB_CRYPTO_DES_MODE_E;

/*! DES 56 bit key + parity, i.e. 64 bits */
#define M2MB_CRYPTO_DES_KEY_SIZE   8

/*! 3DES 112 bit key + parity, i.e. 128 bits */
#define M2MB_CRYPTO_3DES_KEY2_SIZE 16

/*! 3DES 168 bit key + parity, i.e. 192 bits */
#define M2MB_CRYPTO_3DES_KEY3_SIZE 24

/*! DES block bytes length */
#define M2MB_CRYPTO_DES_BLOCK_BYTE_LEN 8

/*! DES initialization vector bytes length */
#define M2MB_CRYPTO_DES_IV_BYTE_LEN    8

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates an M2MB_CRYPTO_DES_CONTEXT structure

  @details
    This function allocates a DES context to be later used by any DES ciphering function.

  @param[in] ctx
    Pointer to a context structure to be allocated by this function.
    Any consecutive call to DES function must use the context

  @param[in] des_alg
    The DES algorithm selected for the context.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_des_init( M2MB_CRYPTO_DES_CONTEXT           *ctx,
                                    M2MB_CRYPTO_DES_ALG_TYPE_E         des_alg );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Free an M2MB_CRYPTO_DES_CONTEXT structure

  @details
    This function free a previously allocated DES context.

  @param[in] ctx
    Pointer to a context structure to be free by this function.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_des_deinit( M2MB_CRYPTO_DES_CONTEXT  ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Generates a DES encryption key for a given context.

  @details
    This function generates a symmetric DES key with a given size and stores it into the context to be later used by any DES encryption function.
    DES is a symmetric encryption, i.e. same key is used for encryption and decryption.

  @param[in] ctx
    Pointer to a context structure previously initialized by m2mb_crypto_des_init.

  @param[in] keybits
    The key-bits size to generate, must be 64, 128 or 192 (length is including parity)

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_des_init() must be called before using this function.
  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_des_generate_key( M2MB_CRYPTO_DES_CONTEXT         ctx,
                                            UINT32                          keybits );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Sets up a DES encryption key (64 bits = 56 effective bits key + parity) for a given context.

  @details
    This function sets up a key, 64 bits length, to a context to be later used by the DES encryption functions.
    This key may have come from the outside and is provided as plaintext.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_des_init.

  @param[in] key
    The encryption key to setup.
    This key may have come from the outside and is provided as plaintext.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    The context must be set for DES algorithm using m2mb_crypto_des_init()

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_des_externalkey_import( M2MB_CRYPTO_DES_CONTEXT         ctx,
                                                  const UINT8                     key[M2MB_CRYPTO_DES_KEY_SIZE] );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Sets up a 3DES encryption key (128 bits = 112 effective bits key + parity) for a given context.

  @details
    This function sets up a key with a given size to a context to be later used by the 3DES encryption functions.
    This key may have come from the outside and is provided as plaintext.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_des_init.

  @param[in] key
    The encryption key to setup.
    This key may have come from the outside and is provided as plaintext.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_des_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_3des_2externalkey_import( M2MB_CRYPTO_DES_CONTEXT         ctx,
                                                    const UINT8                     key[M2MB_CRYPTO_3DES_KEY2_SIZE] );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Sets up a 3DES encryption key (192 bits = 168 effective bits key + parity) for a given context.

  @details
    This function sets up a key with a given size to a context to be later used by the 3DES encryption functions.
    This key may have come from the outside and is provided as plaintext.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_des_init.

  @param[in] key
    The encryption key to setup.
    The key is in plaintext.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_des_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_3des_3externalkey_import( M2MB_CRYPTO_DES_CONTEXT         ctx,
                                                    const UINT8                     key[M2MB_CRYPTO_3DES_KEY3_SIZE] );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Imports a DES/3DES symmetric key into the context.

  @details
    This function imports a given keyblob (which was previously created by m2mb_crypto_des_generate_key() and was exported by m2mb_crypto_des_keyblob_export()) to the provided context.
    This basically allows using of an old keys with an existing context.
    The blob is encrypted with a device specific key and can only be used on the same HW originating it.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_des_init.

  @param[in] keyblob
    The keyblob previously exported using m2mb_crypto_des_keyblob_export() that will be imported into the given context.
    These keys can then be used for DES operations like encrypt & decrypt.

  @param[in] keyblob_length
    The length of the DES/3DES key blob

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_des_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_des_keyblob_import( M2MB_CRYPTO_DES_CONTEXT         ctx,
                                              const UINT8                    *keyblob,
                                              UINT32                          keyblob_length );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Exports a DES key blob from the given context.
    The blob is encrypted with a device specific key and can only be used on the same HW originating it.

  @details
    This function exports a DES key blob for the given context.
    Assumption made is that the context already have keys, i.e. keys were created using the m2mb_crypto_des_generate_key()

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_des_init.

  @param[out] keyblob
    The buffer that will hold the result keyblob representing the exported DES key.
    If keyblob ptr is NULL, this function will return the number of bytes required for the keyblob via the keyblob_length pointer.

  @param[out] keyblob_length
    The length of the DES key blob

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_des_init() & m2mb_crypto_des_generate_key() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_des_keyblob_export( M2MB_CRYPTO_DES_CONTEXT         ctx,
                                              UINT8                          *keyblob,
                                              UINT32                         *keyblob_length );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Encrypts/decrypts a single 8 bytes block onto the output buffer using the DES ECB algorithm.

  @details
    This function will encrypt/decrypt (depending on the <mode> parameter) a given 8 bytes length input buffer and will store the result into the output 16 bytes buffer.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_des_init.

  @param[in] mode
    The requested operation mode (encrypt/decrypt)

  @param[in] in
    The input buffer to encrypt/decrypt, 8 bytes length.

  @param[out] out
    The output buffer to store the result encryption/decryption values.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_des_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_des_ecb_encdec( M2MB_CRYPTO_DES_CONTEXT         ctx,
                                          M2MB_CRYPTO_DES_MODE_E          mode,
                                          const UINT8                     in[M2MB_CRYPTO_DES_BLOCK_BYTE_LEN],
                                          UINT8                           out[M2MB_CRYPTO_DES_BLOCK_BYTE_LEN] );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Encrypts/decrypts a given buffer with a given length using the DES CBC.
    The length must be a multiple of the block size (8 bytes).

  @details
    This function will encrypt/decrypt (depending on the mode parameter) <inlen> bytes from the <in> buffer and store the output into the <out> buffer.
    The ivec will be updated as part of this function call to allow consecuritve calls to encrypt a stream of data.
    If it is required to encrypt several independant buffers (which are not related to each other), the ivec should be restored by the caller (i.e. storing a local copy of the ivec).

  @param[in] ctx
    Pointer to a context structure previously initialized by m2mb_crypto_des_init.

  @param[in] mode
    The requested operation mode (encrypt/decrypt)

  @param[in] in
    The input buffer to encrypt/decrypt.

  @param[out] out
    The output buffer to store the result encryption/decryption values, must have 8 bytes allocated space.

  @param[in] inlen
    The length of the input data (<in>), must be multiple of block size (8 bytes).

  @param[in] ivec
    Initialization vector. The ivec is updated after each use to allow working in "streaming" mode.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_des_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_des_cbc_encdec( M2MB_CRYPTO_DES_CONTEXT         ctx,
                                          M2MB_CRYPTO_DES_MODE_E          mode,
                                          const UINT8                    *in,
                                          UINT8                          *out,
                                          SIZE_T                          inlen,
                                          UINT8                           ivec[M2MB_CRYPTO_DES_IV_BYTE_LEN] );

/** @} */
/** @} */

/**
   @addtogroup ASYM M2MB_ASYM
   @ingroup M2MB_CRYPTO
   @brief RSA & ECC algorithms implementation.
   @{
*/

/**
   @addtogroup RSA M2MB_RSA
   @ingroup M2MB_CRYPTO
   @brief The RSA public-key cryptosystem.
   @{
*/

/* Global declarations ==========================================================================*/


/* Padding types for encryption. */
/*!
  @brief
    Padding types for RSA encryption.
*/
typedef enum
{
  /*! For PKCS version 1.5 */
  M2MB_CRYPTO_RSA_PADDING_PKCS1_V15,
  /*! For PKCS version 2.1 i.e. for OAEP ENCRYPT / DECRYPT function*/
  M2MB_CRYPTO_RSA_PADDING_PKCS1_OAEP,
  /*! For PKCS version 2.1 i.e. for PSS-VERIFY function */
  M2MB_CRYPTO_RSA_PADDING_PKCS1_PSS,
  /*! For PKCS version 2.1 PSS with unknown salt length */
  M2MB_CRYPTO_RSA_PADDING_PKCS1_PSS_AUTORECOVER_SALTLEN,
  /*! No Padding */
  M2MB_CRYPTO_RSA_PADDING_NONE,

  ENUM_TO_INT( M2MB_CRYPTO_RSA_PADDING_TYPE_E )
} M2MB_CRYPTO_RSA_PADDING_TYPE_E;

/*!
  @brief
    The operation mode (encrypt/decrypt).
*/
typedef enum
{
  M2MB_CRYPTO_RSA_PUBLIC,
  M2MB_CRYPTO_RSA_PRIVATE,
  ENUM_TO_INT( M2MB_CRYPTO_RSA_MODE_E )
} M2MB_CRYPTO_RSA_MODE_E;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates an M2MB_CRYPTO_RSA_CONTEXT structure

  @details
    This function allocates an RSA context to be later used by any RSA function.

  @param[in] ctx
    Pointer to a context structure to be allocated by this function.
    Any consecutive call to RSA function must use this context.

  @param[in] padding_type
    Padding type to use.

  @param[in] hash_id
    The index of hash algorithm used for generating signature.
    There are also two RSA encryption schemes with padding: RSA PKCS-v1.5 and RSA OAEP,
    the hash_id is needed only for RSA OAEP.

  @return
    M2MB_RESULT_SUCCESS on success

  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    For PKCS version 2.1:
    M2MB_CRYPTO_RSA_PADDING_PKCS1_OAEP is for  encryption scheme.
    M2MB_CRYPTO_RSA_PADDING_PKCS1_PSS and
    M2MB_CRYPTO_RSA_PADDING_PKCS1_PSS_AUTORECOVER_SALTLEN are used for signature
    (PSS probabilistic signature scheme)
    Setting any of the 3 padding type will set the PKCS version 2.1

    Setting M2MB_CRYPTO_RSA_PADDING_NONE falls in PKCS#1 v1.5 padding mode when using
    m2mb_crypto_rsa_encrypt and m2mb_crypto_rsa_decrypt.

    To encrypt / decript a buffer without padding, use m2mb_crypto_rsa_public
    and m2mb_crypto_rsa_private

  @b
    Example
  @code

    M2MB_CRYPTO_RSA_CONTEXT ctx;
    M2MB_RESULT_E           res;

    res = m2mb_crypto_rsa_init( &ctx, M2MB_CRYPTO_RSA_PADDING_PKCS1_V15, M2MB_CRYPTO_MD_ALG_MD5);

    if( M2MB_RESULT_SUCCESS != res )
    {
      ....
    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_init( M2MB_CRYPTO_RSA_CONTEXT                 *ctx,
                                    M2MB_CRYPTO_RSA_PADDING_TYPE_E          padding_type,
                                    M2MB_CRYPTO_MD_ALG_E                    hash_id );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Free an M2MB_CRYPTO_RSA_CONTEXT structure

  @details
    This function free a previously allocated RSA context.

  @param[in] ctx
    Pointer to a context structure to be free by this function.

  @return
    M2MB_RESULT_SUCCESS on success

  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_deinit( M2MB_CRYPTO_RSA_CONTEXT  ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    RSA public key operation

  @details
    This function is doing an RSA public key operation.
    This function does not handle message padding.
    Generally used to produce secret from original message.
    If instead it used to get original message from secret, the secret must have been produced with an
    RSA private key operation.
    See m2mb_crypto_rsa_private(...)

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[in] in
    The input buffer that must be large enough for the RSA operation key that is used
    (e.g. 128 bytes if RSA-1024 is used).

  @param[out] out
    The output buffer that must be large enough for the RSA operation key that is used
    (e.g. 128 bytes if RSA-1024 is used).

  @return
    M2MB_RESULT_SUCCESS on success

  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    Make sure to set in[0] = 0 or ensure that input is smaller than the public modulus .
    m2mb_crypto_rsa_init() must be called before using this function.
    Input and Output buffers must be large enough for the RSA operation key that is used
    (e.g. 128 bytes if RSA-1024 is used).

  @b
    Example
  @code
    #define RSA_1024_BLOCK_SIZE (SIZE_T)128
    #define PUBLIC_EXPONENT     (UINT32)65537
    void someFunction( )
    {
      INT32 res;
      M2MB_CRYPTO_RSA_CONTEXT ctx;
      uint8_t outplain[RSA_1024_BLOCK_SIZE]={0x00};
      uint8_t secret[RSA_1024_BLOCK_SIZE] ={0x00};
      uint8_t plain[RSA_1024_BLOCK_SIZE] = {0x00};

      plain[3]=0x6d;
      plain[2]=0x92;
      plain[1]=0x03;
      plain[0]=0;

      res = m2mb_crypto_rsa_init( &ctx, M2MB_CRYPTO_RSA_PADDING_PKCS1_V15, M2MB_CRYPTO_MD_ALG_MD5);
      if( M2MB_RESULT_SUCCESS != res )
      {
        return;
      }

      res = m2mb_crypto_rsa_generate_key(ctx, RSA_1024_MODE, PUBLIC_EXPONENT);
      if( M2MB_RESULT_SUCCESS != res )
      {
        m2mb_crypto_rsa_deinit(ctx);
        return;
      }

       res = m2mb_crypto_rsa_public(ctx, plain, secret);
      if( M2MB_RESULT_SUCCESS != res)
      {
        m2mb_crypto_rsa_deinit(ctx);
        return;
      }

      res = m2mb_crypto_rsa_private(ctx, secret, outplain);
      if( M2MB_RESULT_SUCCESS != res)
      {
        m2mb_crypto_rsa_deinit(ctx);
        return;
      }

      res = memcmp(plain,outplain,sizeof(plain));

      res = m2mb_crypto_rsa_deinit(ctx);
      ...
    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_public( M2MB_CRYPTO_RSA_CONTEXT         ctx,
                                      const UINT8                    *in,
                                      UINT8                          *out );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    RSA private key operation

  @details
    This function is doing an RSA private key operation.
    This function does not handle message padding.
    Generally used to get the original message from the secret produced with m2mb_crypto_rsa_public(...).
    If instead it used to generate the secret, to get back the original message,
    RSA public key operation need to be performed.
    See m2mb_crypto_rsa_public(...)

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[in] in
    The input buffer.
    Must be large enough for the RSA operation key that is used (e.g. 128 bytes if RSA-1024 is used)

  @param[out] out
    The output buffer.
    Must be large enough for the RSA operation key that is used (e.g. 128 bytes if RSA-1024 is used)

  @return
    M2MB_RESULT_SUCCESS on success

  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_rsa_init() must be called before using this function.

  @b
  Example

  @code
  //see m2mb_crypto_rsa_public example
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_private( M2MB_CRYPTO_RSA_CONTEXT         ctx,
                                       const UINT8                    *in,
                                       UINT8                          *out );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Generates an RSA key pair.

  @details
    This function generates an RSA key pair.
    The RSA key pair is stored in the context and can be exported via a call to m2mb_crypto_rsa_key_export().

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[in] key_size
    RSA key length (in bytes).

  @param[in] public_exponent
    The public key exponent
    This must be odd and greater than 1.

  @return
    M2MB_RESULT_SUCCESS on success

  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_rsa_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_generate_key( M2MB_CRYPTO_RSA_CONTEXT         ctx,
                                            UINT32                          key_size,
                                            UINT32                          public_exponent );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Exports an RSA key pair blob from the given context.
    The blob is encrypted with a device specific key and can only be used on the same HW originating it.

  @details
    This function exports an RSA key pair for the given context.
    Assumption made is that the context already have keys, i.e. keys were created using the m2mb_crypto_rsa_generate_key()

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[out] keyblob
    The buffer that will hold the result keyblob representing the exported RSA key pair.
    If keyblob ptr is NULL, this function will return the number of bytes required for the keyblob via the keyblob_length pointer.

  @param[out] keyblob_length
    The length of the RSA key blob

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    The exported keyblob contains the public and the private keys
    m2mb_crypto_rsa_init() & m2mb_crypto_rsa_generate_key() must be called before using this function.

  @b
    Example
  @code
    #define RSA_1024_BLOCK_SIZE (SIZE_T)128
    #define PUBLIC_EXPONENT     (UINT32)65537

    void rsa_main( void )
    {
      M2MB_CRYPTO_RSA_CONTEXT ctx1,ctx2;
      INT32 res;
      UINT32 keylen;
      UINT8 *pKey;

      res = m2mb_crypto_rsa_init( &ctx1, M2MB_CRYPTO_RSA_PADDING_PKCS1_V15, M2MB_CRYPTO_MD_ALG_MD5 );

      if( M2MB_RESULT_SUCCESS != res )
      {
        return;
      }

      res = m2mb_crypto_rsa_init( &ctx2, M2MB_CRYPTO_RSA_PADDING_PKCS1_V15, M2MB_CRYPTO_MD_ALG_MD5 );
      if( M2MB_RESULT_SUCCESS != res )
      {
        m2mb_crypto_rsa_deinit(ctx1);
        return;
      }

      res = m2mb_crypto_rsa_generate_key(ctx1, RSA_1024_MODE, PUBLIC_EXPONENT);
      if( M2MB_RESULT_SUCCESS != res)
      {
        m2mb_crypto_rsa_deinit(ctx1);
        m2mb_crypto_rsa_deinit(ctx2);
        return;
      }

      // get keyblob length
      pKey = NULL;
      keylen=0;
      res = m2mb_crypto_rsa_keyblob_export(ctx1, pKey, &keylen);
      if( M2MB_RESULT_SUCCESS != res)
      {
        m2mb_crypto_rsa_deinit(ctx1);
        m2mb_crypto_rsa_deinit(ctx2);
        return;
      }

      // allocate space for keyblob
      pKey = ( UINT8* ) m2mb_os_calloc( keylen * sizeof( UINT8 ) );

      if( pKey == NULL )
      {
        m2mb_crypto_rsa_deinit(ctx1);
        m2mb_crypto_rsa_deinit(ctx2);
        return;
      }

      // fill the keyblob
      res = m2mb_crypto_rsa_keyblob_export(ctx1, pKey, &keylen);
      if( M2MB_RESULT_SUCCESS != res)
      {
        m2mb_crypto_rsa_deinit(ctx1);
        m2mb_crypto_rsa_deinit(ctx2);
        m2mb_os_free(pKey);
        return;
      }

      // import keyblob into ctx2 context
      res = m2mb_crypto_rsa_keyblob_import(ctx2, pKey,keylen);
      if( M2MB_RESULT_SUCCESS != res)
      {
        m2mb_crypto_rsa_deinit(ctx1);
        m2mb_crypto_rsa_deinit(ctx2);
        m2mb_os_free(pKey);
        return;
      }

      ...
    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_keyblob_export( M2MB_CRYPTO_RSA_CONTEXT         ctx,
                                              UINT8                          *keyblob,
                                              UINT32                         *keyblob_length );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Imports an RSA key pair into the context.

  @details
    This function imports a given keyblob (which was previously created by m2mb_crypto_rsa_generate_key and was exported by m2mb_crypto_rsa_key_export) to the provided context.
    This basically allows using of an old keys with an existing context.
    The blob is encrypted with a device specific key and can only be used on the same HW originating it.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[in] keyblob
    The keyblob previously exported using m2mb_crypto_rsa_key_export() that will be imported into the given context.
    These keys can then be used for RSA operations like encrypt, decrypt, sign & verify.

  @param[in] keyblob_length
    The length of the RSA key blob

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_rsa_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_keyblob_import( M2MB_CRYPTO_RSA_CONTEXT         ctx,
                                              const UINT8                    *keyblob,
                                              UINT32                          keyblob_length );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Exports an RSA public key from the given context.

  @details
    This function exports an RSA public key exponent and modulus for the given context.
    The RSA public key information will be exported from the given context (assuming context has keys associated).

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[out] modulus
    RSA modulus.
    If modulus ptr is NULL, this function will return the number of bytes required for the modulus via the modulus_size pointer.

  @param[out] modulus_size
    RSA modulus size

  @param[out] public_exponent
    RSA public exponent.
    If public_exponent ptr is NULL, this function will return the number of bytes required for the public_exponent via the public_exponent_size pointer.

  @param[out] public_exponent_size
    RSA public exponent size.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_rsa_init() & m2mb_crypto_rsa_generate_key() must be called before using this function.

  @b
    Example
  @code
    #define RSA_1024_BLOCK_SIZE (SIZE_T)128
    #define PUBLIC_EXPONENT     (UINT32)65537

    void rsa_main( void )
    {
      M2MB_CRYPTO_RSA_CONTEXT ctx1,ctx2;
      INT32 res;
      UINT8* pModulus;
      UINT8* pExponent;
      UINT32 modulusSize;
      UINT32 exponentSize;

      res = m2mb_crypto_rsa_init(&ctx1, M2MB_CRYPTO_RSA_PADDING_PKCS1_V15, M2MB_CRYPTO_MD_ALG_MD5);
      if( M2MB_RESULT_SUCCESS != res)
      {
        return;
      }

      res = m2mb_crypto_rsa_init(&ctx2, M2MB_CRYPTO_RSA_PADDING_PKCS1_V15, M2MB_CRYPTO_MD_ALG_MD5);
      if( M2MB_RESULT_SUCCESS != res)
      {
        m2mb_crypto_rsa_deinit(ctx1);
        return;
      }

      res = m2mb_crypto_rsa_generate_key(ctx1, RSA_1024_MODE, PUBLIC_EXPONENT);
      if( M2MB_RESULT_SUCCESS != res)
      {
        goto EXPORT_FREE;
      }

      // with modulus and exponent NULL, m2mb_crypto_rsa_public_key_export
      // will return modulus and exponent size
      pModulus = NULL;
      pExponent = NULL;
      res = m2mb_crypto_rsa_public_key_export(ctx1, pModulus, &modulusSize, pExponent, &exponentSize);
      if( M2MB_RESULT_SUCCESS != res)
      {
        goto EXPORT_FREE;
      }

      pModulus = (UINT8*) m2mb_os_malloc( modulusSize * sizeof(UINT8));
      pExponent = (UINT8*) m2mb_os_malloc( exponentSize * sizeof(UINT8));

      if( ( pModulus == NULL ) || (pExponent == NULL) )
      {
        goto EXPORT_FREE;
      }

      res = m2mb_crypto_rsa_public_key_export(ctx1, pModulus, &modulusSize, pExponent, &exponentSize);
      if( M2MB_RESULT_SUCCESS != res)
      {
        goto EXPORT_FREE;
      }

      // import in the second context
      res = m2mb_crypto_rsa_public_key_import(ctx2, pModulus, modulusSize, pExponent, exponentSize);
      if( M2MB_RESULT_SUCCESS != res)
      {
        goto EXPORT_FREE;
      }

      EXPORT_FREE:
      if( pModulus != NULL)
      {
        m2mb_os_free(pModulus);
      }
      if( pExponent != NULL)
      {
        m2mb_os_free(pExponent);
      }
      m2mb_crypto_rsa_deinit(ctx1);
      m2mb_crypto_rsa_deinit(ctx2);

      ...
    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_public_key_export( M2MB_CRYPTO_RSA_CONTEXT         ctx,
                                                 UINT8                          *modulus,
                                                 UINT32                         *modulus_size,
                                                 UINT8                          *public_exponent,
                                                 UINT32                         *public_exponent_size );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Imports an RSA public key to the given context.

  @details
    This function imports an RSA public key exponent and modulus into the given context.
    The RSA public key information, i.e. exponent and mudulus will be stored into the provided context.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[in] modulus
    RSA modulus.
    Must not be NULL.

  @param[in] modulus_size
    RSA modulus size, in bytes.

  @param[in] public_exponent
    RSA public exponent.
    Must not be NULL.

  @param[in] public_exponent_size
    RSA public exponent size, in bytes.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_rsa_init() must be called before using this function.

  @b
    Example present in the above m2mb_crypto_rsa_public_key_export

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_public_key_import( M2MB_CRYPTO_RSA_CONTEXT               ctx,
                                                 const UINT8                          *modulus,
                                                 UINT32                                modulus_size,
                                                 const UINT8                          *public_exponent,
                                                 UINT32                                public_exponent_size );

/*-----------------------------------------------------------------------------------------------*/
/*!
   m2mb_crypto_rsa_externalkey_import

  @brief
    Imports an externally generated RSA key pair into the context.

  @details
    This function imports a given RSA key pair (which was created by an external implementation) to the provided context.
    This basically allows using of a keys not created by the m2mb_crypto component.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[in] modulus
    RSA modulus

  @param[in] modulus_size
    RSA modulus size in bytes

  @param[in] p_factor
    RSA p factor

  @param[in] p_factor_size
    RSA p factor size in bytes

  @param[in] q_factor
    RSA q factor

  @param[in] q_factor_size
    RSA q factor size in bytes

  @param[in] public_exponent
    RSA public exponent.

  @param[in] public_exponent_size
    RSA public exponent size in bytes

  @param[in] private_exponent
    RSA private exponent.

  @param[in] private_exponent_size
    RSA private exponent size in bytes

  @param[in] padding_type
    Padding type to use.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_rsa_init() must be called before using this function.
    A check is made after the import for the consistency of RSA public and  RSA private key,
    but is the responsibility of the user to ensure the trustworthiness of the source of his RSA
    parameters, which goes beyond what is effectively checkable.

  @b
    Example
  @code

    ...

    M2MB_CRYPTO_RSA_CONTEXT ctx;
    const uint8_t exponent[] = { 0x01, 0x00, 0x01 };
    const uint8_t modulus[] = {
       0xB5, 0x3F, 0xB8, 0x51 ,0x36, 0x8D, 0x7F, 0x4D, 0x3B, 0x71, 0x71, 0x15, 0x60, 0x62, 0x66, 0x61,
        0x4F, 0x1D, 0x7F, 0x2F, 0x78 ,0x23 ,0x99, 0x04,
        0xCE, 0x17, 0x23, 0xAB ,0x39,0x86, 0xBD, 0xA6, 0x4A, 0xC6 ,0x0E, 0xFF ,0x7A,
        0x1F, 0xA5, 0x53, 0xA7, 0x80, 0x86, 0xB6, 0x0B, 0x12, 0x21, 0x18, 0xDE, 0x01, 0xF1, 0xCD ,0xC6, 0x5F,
        0x7D, 0xB4, 0x70, 0xEA, 0x53, 0x0A, 0x79, 0xFA, 0xF5,
        0x23, 0xAB, 0x18, 0x5E, 0x04, 0xCB, 0x9A, 0xEE, 0x93, 0x61,
        0xCC, 0xA1, 0xD1, 0xD2, 0x9E, 0xFD, 0xCB, 0x23, 0x5E, 0x5D, 0x2B, 0x3E, 0x13, 0x14 ,0x1B, 0xA8, 0x0F,
        0x2A, 0xD7, 0xF9, 0x80, 0x12, 0xC6, 0x18, 0x66, 0xE3,
        0xD0, 0x91, 0x93, 0xE5, 0x67, 0xCA, 0xA0, 0x97, 0x84, 0x64 ,0xD7, 0xEA,
        0x0E, 0x54, 0x5F, 0x92, 0xB5, 0x44, 0x3C, 0xA6, 0x94, 0xCE, 0x2A, 0x02, 0x80, 0x64, 0xB7, 0xF7, 0xDD
    };

    const uint8_t p[] = {
        0xED, 0x9B, 0xA7, 0xC2, 0xD7, 0x09, 0x4B, 0x10, 0xFE, 0x63, 0x49, 0x8E, 0x6A,
        0xD0, 0xB6, 0xA6, 0x70, 0x2F, 0xC4, 0x9B,
        0x98, 0x29, 0xCE, 0xC2, 0x2A, 0xB5, 0x04, 0x01, 0x43, 0xC9,
        0x5A, 0x6C ,0xEC, 0x17, 0x2F, 0xF8, 0x18, 0xF3, 0x16, 0x50, 0xFA ,0x9B, 0x02, 0x33,
        0xA4 ,0xCD, 0x64 ,0xA3, 0xA2, 0x19 ,0xB8,
        0x5E, 0x47, 0xCA, 0x99,  0x18, 0xE3, 0x04, 0x7B, 0x81, 0x4E, 0x2B, 0x58, 0xB3
    };

    const uint8_t q[] = {
        0xC3, 0x47, 0x46, 0x6B, 0x6D, 0xAA ,0x58 ,0xE6, 0xC2, 0x07, 0x5A ,0xE5, 0x09, 0x53,
        0xAB ,0x9A, 0x4F, 0x16, 0x73, 0x74, 0x9C, 0x4A, 0xE3, 0x00, 0xC0,
        0xA4, 0xC5, 0x45, 0xC4, 0x62, 0x15, 0x32, 0x06, 0xA0, 0xB5, 0x99, 0x03, 0x4F, 0xF1,
        0x8F ,0x8F, 0x1C, 0x2C, 0x75, 0xCF, 0xC0, 0xB8, 0x20, 0x79, 0x10, 0x2D,
        0xA0, 0x5D, 0x09, 0x45, 0xDE, 0xE5, 0x4D, 0xE2, 0x35, 0x77, 0x94, 0x15 ,0x2F

    };

    const uint8_t d[] = {
        0x12, 0x8F, 0x56, 0xD9, 0x82, 0x23, 0x95, 0x30, 0x9C, 0x70, 0xAE,
        0x6A, 0xC8, 0xF6, 0xEA, 0x92, 0xF6, 0x5E, 0x07, 0xE3, 0x1D, 0x83, 0x4F,
        0xAD, 0x0D, 0x49, 0x93, 0x4E, 0x7A, 0x52, 0x90, 0xE3, 0x11, 0x6F, 0x1C, 0xA4, 0x0E, 0xA6,
        0x64, 0xF0, 0x9F, 0x0B, 0x6B, 0x40, 0x02, 0xFC, 0x35, 0x45, 0x6D, 0x9E, 0x77, 0x5B, 0x61,
        0xFF, 0xEF, 0x48, 0x65, 0xF3, 0xA8, 0xD3, 0x3E, 0xD0, 0x72, 0xFF,
        0xAA, 0xB6, 0x84, 0x20, 0x4C, 0xAA, 0x2C, 0x9A, 0xA9, 0x38, 0xF6,
        0x90, 0x0E, 0x4E, 0xB1, 0xD4, 0xC7, 0x11, 0x2D, 0xE3, 0x89, 0xE5, 0x9D, 0x1E, 0x87, 0x1C,
        0x6F, 0xC4, 0x3E, 0xE2, 0x32, 0xDE, 0xD4, 0x5E, 0x0A ,0x0D, 0x07,
        0x7F, 0x63, 0x5F, 0xF1, 0x46, 0xB5, 0x85, 0x0D, 0xEC, 0x10, 0xC8, 0x93,
        0xF9, 0x2A, 0x19, 0x7D, 0x53, 0x66, 0x75, 0x6E, 0xDE, 0xB9, 0x32, 0x28, 0x44, 0x3D, 0x25

    };


    res = m2mb_crypto_rsa_init( &ctx, M2MB_CRYPTO_RSA_PADDING_NONE, M2MB_CRYPTO_MD_ALG_MD5);
    if( M2MB_RESULT_SUCCESS != res )
    {
        return;
    }

    res = m2mb_crypto_rsa_externalkey_import( ctx, modulus, sizeof(modulus), p, sizeof(p),
                                              q, sizeof(q), exponent,  sizeof(exponent),
                                              d, sizeof(d), M2MB_CRYPTO_RSA_PADDING_NONE );
    if( M2MB_RESULT_SUCCESS != res )
    {
      return;
    }

    ...

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_externalkey_import( M2MB_CRYPTO_RSA_CONTEXT             ctx,
                                                  const UINT8                        *modulus,
                                                  UINT32                              modulus_size,
                                                  const UINT8                        *p_factor,
                                                  UINT32                              p_factor_size,
                                                  const UINT8                        *q_factor,
                                                  UINT32                              q_factor_size,
                                                  const UINT8                        *public_exponent,
                                                  UINT32                              public_exponent_size,
                                                  const UINT8                        *private_exponent,
                                                  UINT32                              private_exponent_size,
                                                  M2MB_CRYPTO_RSA_PADDING_TYPE_E      padding_type );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Generic RSA PKCS#1 encryption method.
    This function adds the message padding, then performs an RSA operation (encryption).

  @details
    This function encrypts <inlen> bytes from <in> buffer using the public/private key from <ctx>
    (depending on the <mode> provided>) and writes, at most, <modulus> bytes of cyphertext data to <out>.
    It is the generic wrapper for performing a PKCS#1 encryption (PKCS#1 v1.5 and PKCS#1 v2.1 padding)

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[in] mode
    M2MB_CRYPTO_RSA_PUBLIC or M2MB_CRYPTO_RSA_PRIVATE

  @param[in] inlen
    The length, in bytes, of the <in> plaintext buffer

  @param[in] in
    The plaintext buffer to encrypt.
    This must not be NULL
    In case of M2MB_CRYPTO_RSA_PADDING_NONE or M2MB_CRYPTO_RSA_PADDING_PKCS1_V15,
    the plaintext max length must be M-11 octets, where M is the length in bytes of the
    modulus n.
    References:https://tools.ietf.org/html/rfc2437#section-7.2.1
    In the others cases (PKCS version 2.1), the plaintext max length must be M-2-2hLen octets,
    where M is the length in bytes of the modulus n and hLen is the length  the hash function.
    References: https://tools.ietf.org/html/rfc2437#section-7.1.1

  @param[out] out
    The buffer that will hold the ciphertext. Must be in the size of RSA modulus size
    (eg. 128 bytes if RSA-1024 is used).

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_rsa_init() & m2mb_crypto_rsa_generate_key() must be called before using this function
    (or keys were imported into the context).
    There are two RSA encryption schemes: RSAES-PKCS-v1_5 and RSAES-OAEP (PKCS#1 v2.1).
    Padding M2MB_CRYPTO_RSA_PADDING_PKCS1_V15 and M2MB_CRYPTO_RSA_PADDING_NONE uses RSAES-PKCS-v1_5,
    all the others paddings use RSAES-OAEP

  @b
    Example
  @code

    #define RSA_1024_BLOCK_SIZE (SIZE_T)128
    #define PUBLIC_EXPONENT     (UINT32)65537

    void rsa_main( void )
    {
      INT32 res;
      M2MB_CRYPTO_RSA_CONTEXT ctx;
      SIZE_T outlen;

      CHAR mytext[] = "Hello world, hello sun";
      /// secret must have length of 128, because we are using RSA-1024
      UINT8 secret[RSA_1024_BLOCK_SIZE] ={0x00};
      /// will contain decyphered secret
      CHAR outplain[RSA_1024_BLOCK_SIZE]={0x00};

      res = m2mb_crypto_rsa_init( &ctx, M2MB_CRYPTO_RSA_PADDING_NONE, M2MB_CRYPTO_MD_ALG_MD5 );
      if( M2MB_RESULT_SUCCESS != res )
      {
        return;
      }

      res = m2mb_crypto_rsa_generate_key(ctx, RSA_1024_MODE, PUBLIC_EXPONENT);
      if( M2MB_RESULT_SUCCESS != res )
      {
        m2mb_crypto_rsa_deinit(ctx);
        return ;
      }

      res = m2mb_crypto_rsa_encrypt(ctx, M2MB_CRYPTO_RSA_PUBLIC,  strlen(mytext),(UINT8*) mytext, secret);
      if( M2MB_RESULT_SUCCESS != res )
      {
        m2mb_crypto_rsa_deinit(ctx);
        return ;
      }

      res = m2mb_crypto_rsa_decrypt( ctx, M2MB_CRYPTO_RSA_PRIVATE, &outlen, (UINT8*)outplain,
                                     RSA_1024_BLOCK_SIZE, secret);
      if( M2MB_RESULT_SUCCESS != res )
      {
        m2mb_crypto_rsa_deinit(ctx);
        return ;
      }

      ...

    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_encrypt( M2MB_CRYPTO_RSA_CONTEXT         ctx,
                                       M2MB_CRYPTO_RSA_MODE_E          mode,
                                       SIZE_T                          inlen,
                                       const UINT8                    *in,
                                       UINT8                          *out );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Generic RSA PKCS#1 decryption method.
    This function performs an RSA operation (decryption), then removes the message padding.

  @details
    This function decrypts <inlen> bytes from <in> buffer using the public/private key from <ctx>
    (depending on the <mode> provided) and writes, at most, out_max_len bytes of cyphertext data to <out>.
    The value inlen cannot be bigger than the modulus size.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[in] mode
    M2MB_CRYPTO_RSA_PUBLIC or M2MB_CRYPTO_RSA_PRIVATE

  @param[out] outlen
    The length, in bytes, of the plaintext data put into <out>

  @param[out] out
    The buffer that will hold the resulting plaintext. Must be as large as the RSA modulus size .

  @param[in] out_max_len
    The maximum length of the out buffer

  @param[in] in
    The ciphertext buffer to decrypt.
    Must be as large as the size of the modulus (eg. 128 bytes if RSA-1024 is used)

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_rsa_init() & m2mb_crypto_rsa_generate_key() must be called before using this function
    (or keys were imported into the context).
    There are two RSA encryption schemes: RSAES-PKCS-v1_5 and RSAES-OAEP (PKCS#1 v2.1).
    Padding M2MB_CRYPTO_RSA_PADDING_PKCS1_V15 and M2MB_CRYPTO_RSA_PADDING_NONE uses RSAES-PKCS-v1_5,
    all the others paddings use RSAES-OAEP

  @b
    Example
  @code
    //see m2mb_crypto_rsa_encrypt example
  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_decrypt( M2MB_CRYPTO_RSA_CONTEXT         ctx,
                                       M2MB_CRYPTO_RSA_MODE_E          mode,
                                       SIZE_T                         *outlen,
                                       UINT8                          *out,
                                       SIZE_T                          out_max_len,
                                       const UINT8                    *in );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Generic RSA PKCS#1 signature creation method.
    The signature is created using the context key (created via the m2mb_crypto_rsa_generate_key()).

  @details
    This function will use the padding setup (done by m2mb_crypto_rsa_init()) and performs a private
    RSA operation to sign the message digest.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_rsa_init().

  @param[in] mode
    M2MB_CRYPTO_RSA_PUBLIC or M2MB_CRYPTO_RSA_PRIVATE
    It is deprecated and discouraged to call this function using M2MB_CRYPTO_RSA_PUBLIC mode

  @param[out] md_alg
    The message digest algorithm or M2MB_CRYPTO_MD_NONE for raw data signing.

  @param[out] hashlen
    The length of the provided <hash>, i.e. the message digest length. It is only required when using
    <md_alg>=M2MB_CRYPTO_MD_NONE and retrieved automatically from the hash algorithm if md_alg is specified.

  @param[in] hash
    The message digest to sign.

  @param[out] sig
    The result ciphertext.
    The sig buffer must be as large as the size of the modulus.
    For example len of sig = 128 bytes if RSA-1024 is used.

  @return
    M2MB_RESULT_SUCCESS on success

  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_rsa_init() must be called before using this function.
    Keys were previously created via the m2mb_crypto_rsa_generate_key() (or imported).
    It is discouraged to call this function using M2MB_CRYPTO_RSA_PUBLIC mode.
    There are two RSA signature schemes specified in [PKCS1]: RSA-PKCS1-v1_5 (for PKCS#1 v1.5 padding) and
    RSASSA-PSS ( for and PKCS#1 v2.1 padding)

  @b
    Example

  @code
    //see m2mb_crypto_rsa_verify example
  @endcode

*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_sign( M2MB_CRYPTO_RSA_CONTEXT         ctx,
                                    M2MB_CRYPTO_RSA_MODE_E          mode,
                                    M2MB_CRYPTO_MD_ALG_E            md_alg,
                                    UINT32                          hashlen,
                                    const UINT8                    *hash,
                                    UINT8                          *sig );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Generic RSA PKCS#1 signature verification method.

  @details
    This function will use the padding setup (done by m2mb_crypto_rsa_init()) and will do a public
    RSA operation to verify the signature and performs a public RSA operation.

  @param[in] ctx
    Pointer to a context structure previously initialized by m2mb_crypto_rsa_init().

  @param[in] pubkey
    The public key to use for the verification (plaintext).
    Today not SUPPORTED: pubkey must be set to NULL and
    the public key from the current context is used.
    With pubkey different from NULL an error is provided.

  @param[in] pubkey_length
    The length of the provided public key.
    Still NOT SUPPORTED: 0 need to be provided.

  @param[out] md_alg
    The message digest algorithm or M2MB_CRYPTO_MD_NONE for raw data verification.

  @param[out] hashlen
    The length of the provided <hash>, i.e. the message digest length. It is only required when using
    <md_alg>=M2MB_CRYPTO_MD_NONE and retrieved automatically from the hash algorithm if md_alg is specified.

  @param[in] hash
    The message digest to verify it's signature.

  @param[in] sig
    The signature ciphertext to verify against the hash.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    If an external public key is needed m2mb_crypto_rsa_public_key_import should be used
    before the m2mb_crypto_rsa_verify.
    m2mb_crypto_rsa_init() must be called before using this function.
    Keys were previously created via the m2mb_crypto_rsa_generate_key() (or imported).

  @b
    Example
  @code
    #define MD5_IN_LEN             16
    #define RSA_1024_MODE  (UINT32)128

     void rsa_main( void )
    {
      INT32 res;
      M2MB_CRYPTO_RSA_CONTEXT ctx;
      UINT8 md_data[MD5_IN_LEN] ={0xFF,0xAA,0x12,0x34};
      UINT8 out_md[MD5_IN_LEN] ={0x00};
      UINT8 signature[RSA_1024_MODE] ={0x00};

      res = m2mb_crypto_rsa_init( &ctx, M2MB_CRYPTO_RSA_PADDING_PKCS1_V15, M2MB_CRYPTO_MD_ALG_MD5);
      if( M2MB_RESULT_SUCCESS != res )
      {
        return;
      }

      res = m2mb_crypto_rsa_generate_key(ctx, RSA_1024_MODE, PUBLIC_EXPONENT);
      if( M2MB_RESULT_SUCCESS != res )
      {
        m2mb_crypto_rsa_deinit(ctx);
        return;
      }

      /// generate a md5 digest
      if( m2mb_crypto_md(M2MB_CRYPTO_MD_ALG_MD5, md_data, MD5_IN_LEN, out_md) == NULL )
      {
        m2mb_crypto_rsa_deinit(ctx);
        return;
      }

      res = m2mb_crypto_rsa_sign( ctx, M2MB_CRYPTO_RSA_PRIVATE, M2MB_CRYPTO_MD_ALG_MD5,
                                 MD5_IN_LEN, out_md, signature);
      if( M2MB_RESULT_SUCCESS != res )
      {
        m2mb_crypto_rsa_deinit(ctx);
        return;
      }

      /// Use public key existing in the context
      res = m2mb_crypto_rsa_verify( ctx, NULL, 0, M2MB_CRYPTO_MD_ALG_MD5, MD5_IN_LEN, out_md, signature);

      if( M2MB_RESULT_SUCCESS != res )
      {
        m2mb_crypto_rsa_deinit(ctx);
        return;
      }

      ...

    }

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_rsa_verify( M2MB_CRYPTO_RSA_CONTEXT         ctx,
                                      UINT8                          *pubkey,
                                      UINT32                          pubkey_length,
                                      M2MB_CRYPTO_MD_ALG_E            md_alg,
                                      UINT32                          hashlen,
                                      const UINT8                    *hash,
                                      const UINT8                    *sig );

/** @} */

/**
   @addtogroup ECC M2MB_ECC
   @ingroup M2MB_CRYPTO
   @brief Elliptic curve ECDSA and ECDH.
   @{
*/

/* Global declarations ==========================================================================*/



/**
   Domain parameters (curve, subgroup and generator) identifiers.

   Only curves over prime fields are supported.

   \warning Only well known domain parameters from trusted sources are used.
*/
typedef enum
{
  M2MB_CRYPTO_ECC_DP_BP256R1,        /*!< 256-bits Brainpool curve */
  M2MB_CRYPTO_ECC_DP_SECP256R1,      /*!< 256-bits NIST curve  */
  M2MB_CRYPTO_ECC_DP_CUSTOM,         /*!< Custom domain parameter imported via m2mb_crypto_ecc_dp_init()  */

  ENUM_TO_INT( M2MB_CRYPTO_ECC_DOMAIN_GROUP_ID_E )
} M2MB_CRYPTO_ECC_DOMAIN_GROUP_ID_E;

/*!
  @brief
    Number of 32-bytes required for a big number.
*/
//#define M2MB_CRYPTO_ECC_BIGLEN 9
#define M2MB_CRYPTO_ECC_BIGLEN 18

/*!
  @brief
    Maximum size for key size (x or y coordinates in binary format, private key).
*/
#define M2MB_MAX_ECC_KEY_SIZE 32

/*!
  @brief
    Structure representing the X,Y coordinates of a point.
    The point is zero, or "at infinity", if infinity == 1.
    Otherwise, X and Y are its standard (affine) coordinates.
    key_size respresents the x or y coordinates content length (for 160 bit key, size should be 20).
*/
typedef struct
{
   UINT8         x[M2MB_MAX_ECC_KEY_SIZE+1];
   UINT8         y[M2MB_MAX_ECC_KEY_SIZE+1];
   INT32         flags;
   INT32         key_size_bytes;
} M2MB_CRYPTO_ECC_AFFINE_POINT_T;

/*!
  @brief
    Enumeration representing all the ECC Domain Parameters.
*/
typedef enum
{
  M2MB_CRYPTO_ECC_DP_P,
  M2MB_CRYPTO_ECC_DP_A,
  M2MB_CRYPTO_ECC_DP_B,
  M2MB_CRYPTO_ECC_DP_G_X,
  M2MB_CRYPTO_ECC_DP_G_Y,
  M2MB_CRYPTO_ECC_DP_N,
  M2MB_CRYPTO_ECC_DP_H,
  ENUM_TO_INT( M2MB_CRYPTO_ECC_DP_E )
} M2MB_CRYPTO_ECC_DP_E;

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates an M2MB_CRYPTO_ECC_CONTEXT structure

  @details
    This function allocates an ECC context to be later used by any ECC function.

  @param[out] ctx
    Pointer to a context structure to be allocated by this function.
    Any consecutive call to ECDSA and ECDH functions must use this context.

  @param[in] curve_domain_params
    This will indicate the curve, subgroup and generator, from a predefined list, or set for custom DP to be set later by m2mb_crypto_ecc_dp_init().

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_RESULT_E m2mb_crypto_ecc_init( M2MB_CRYPTO_ECC_CONTEXT                        *ctx,
                                    M2MB_CRYPTO_ECC_DOMAIN_GROUP_ID_E               curve_domain_params );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Allocates an M2MB_CRYPTO_ECC_CONTEXT structure

  @details
    This function allocates an ECC context to be later used by any ECC function, manually specifying the domain parameters.

  @param[out] ctx
    Pointer to a context structure to be allocated by this function.
    Any consecutive call to ECDSA and ECDH functions must use this context.

  @param[in] p
    Pointer to the prime number

  @param[in] a
    Pointer to the "a" constant in the ECC equation

  @param[in] b
    Pointer to the "b" constant in the ECC equation

  @param[in] g_x
    Pointer to the x coordinate of the base point

  @param[in] g_y
    Pointer to the y coordinate of the base point

  @param[in] n
    Pointer to the order of the base point

  @param[in] h
    Pointer to the cofactor, must be <= 4 and preferably equals 1

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/

M2MB_RESULT_E m2mb_crypto_ecc_dp_init( M2MB_CRYPTO_ECC_CONTEXT                      ctx,
                                       UINT8                                       *p,
                                       UINT8                                       *a,
                                       UINT8                                       *b,
                                       UINT8                                       *g_x,
                                       UINT8                                       *g_y,
                                       UINT8                                       *n,
                                       UINT32                                      *h );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Imports an ECC Domain Parameter to the given context.

  @details
    This function imports an ECC Domain Parameter for the given context.
    Except for the cofactor M2MB_CRYPTO_ECC_DP_H, all the values in param must be sent as
    a hexadecimals ASCII string
    i.e "188DA80EB03090F67CBF20EB43A18800F4FF0AFD82FF1012" ( example ).

    The value of M2MB_CRYPTO_ECC_DP_H must be a number ( <= 4 and preferably equals to 1 ),
    as in the example below.


  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_ecc_init().

  @param[in] domain
    The type of the Domain Parameter.

  @param[in] len
    The length of the Domain Parameter.

  @param[in] param
    The Domain Parameter.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_ecc_init() must be called before using this function.

  @b
    Example
  @code
    UINT32 h_value = 1;

    //Key length 160 bits
    CHAR p[] = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFF",     //p value
    CHAR a[] = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFC",     //a value
    CHAR b[] = "1C97BEFC54BD7A8B65ACF89F81D4D4ADC565FA45",     //b value
    ...
    len = strlen(p) + 1;
    res = m2mb_crypto_ecc_dp_import(ctx2, M2MB_CRYPTO_ECC_DP_P, len, (UINT8*)p );
    ...
    ...
    res = m2mb_crypto_ecc_dp_import(ctx2, M2MB_CRYPTO_ECC_DP_H, sizeof( h_value ), (UINT8*)&h_value);

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecc_dp_import( M2MB_CRYPTO_ECC_CONTEXT    ctx,
                                         M2MB_CRYPTO_ECC_DP_E       domain,
                                         UINT32                     len,
                                         const UINT8                *param);

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    ECDSA signature function.

  @details
    This function signs the provided msg.
    The private key used for signing the message is taken from the context (created via m2mb_crypto_ecc_keypair_generate()).
    The domain parameters must be set prior to using this function (m2mb_crypto_ecc_init()).

  @param[in] ctx
    Pointer to an allocated domain context structure.

  @param[in] msg
    The message to be signed.

  @param[in] msg_len
    The length of the message provided.

  @param[out] sig
    The result signature

  @param[in] sig_len
    The size, in bytes, of the signature buffer.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    The domain parameters must be set prior to using this function (m2mb_crypto_ecc_init()).
    The private key used for signing the message is taken from the context (created via m2mb_crypto_ecc_keypair_generate()).

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecdsa_sign( M2MB_CRYPTO_ECC_CONTEXT         ctx,
                                      UINT8                          *msg,
                                      UINT32                          msg_len,
                                      UINT8                          *sig,
                                      SIZE_T                          sig_len );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    ECDSA signature verification function.

  @details
    This function verifies the provided signature with the provided message.
    The domain parameters must be set prior to using this function (m2mb_crypto_ecc_init()).

  @param[in] ctx
    Pointer to an allocated domain context structure.

  @param[in] msg
    The message to be verified against the signature.

  @param[in] msg_len
    The length of the message provided.

  @param[in] pubkey
    The public key to use for the verification (plaintext).
    Respresented as the x,y coordinates of a point on the selected curve.
    If set to NULL, the key from the context will be used, if exist.

  @param[in] sig
    The signature to be verified.

  @param[in] sig_len
    The size, in bytes, of the signature.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    The domain parameters must be set prior to using this function (m2mb_crypto_ecc_init()).

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecdsa_verify( M2MB_CRYPTO_ECC_CONTEXT         ctx,
                                        UINT8                          *msg,
                                        UINT32                          msg_len,
                                        M2MB_CRYPTO_ECC_AFFINE_POINT_T *pubkey,
                                        UINT8                          *sig,
                                        SIZE_T                          sig_len );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Function to derive the shared key for the DH algorithm.

  @details
    This function calculates the shared key (using own private with recipients public) used for the diffie hellman key agreement algorithm.
    Key exchange for ECDH is based on the following math (D is the private key, H is the public key, G is the base point of the subgroup):
    DaHb = Da(DbG) = Db(DaG) = DbHa
    The domain parameters must be set prior to using this function (m2mb_crypto_ecc_init()).
    The private key used for the key derivation is taken from the context, assuming m2mb_crypto_ecc_keypair_generate() was previously called.

  @param[in] ctx
    Pointer to an already allocated domain context structure.

  @param[out] shared_keyblob
    The derived shared/session key.
    If shared_keyblob ptr is NULL, this function will return the number of bytes required for the shared_key via the shared_keyblob_len pointer.
    The key is encrypted with a device specific key and can only be used on the same HW originating it.

  @param[in] shared_keyblob_len
    The shared_keyblob size, in bytes.

  @param[in] pubkey
    The other ends public key, represented as the x,y coordinates of a point on the selected curve.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    The domain parameters must be set prior to using this function (m2mb_crypto_ecc_init()).
    The private key used for key derivation is taken from the context (created via m2mb_crypto_ecc_keypair_generate()).

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecdh_shared_key_derive( M2MB_CRYPTO_ECC_CONTEXT                ctx,
                                                  UINT8                                 *shared_keyblob,
                                                  UINT16                                *shared_keyblob_len,
                                                  const M2MB_CRYPTO_ECC_AFFINE_POINT_T  *pubkey );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Function to generate an ECC keypair.

  @details
    This function generates an ECC keypair.
    Same key can be used for both ECDH and ECDSA.
    The key is stored into the domain context and can be later exported via the m2mb_crypto_ecc_keyblob_export().
    The domain parameters must be set prior to using this function (m2mb_crypto_ecc_init()).

  @param[in] ctx
    Pointer to a previously allocated domain context structure.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    The domain parameters must be set prior to using this function (m2mb_crypto_ecc_init()).

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecc_keypair_generate( M2MB_CRYPTO_ECC_CONTEXT  ctx );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Exports an ECC key pair blob from the given context.
    The blob is encrypted with a device specific key and can only be used on the same HW originating it.

  @details
    This function exports an ECC key pair for the given context.
    Assumption made is that the context already have keys, i.e. keys were created using the m2mb_crypto_ecc_keypair_generate()

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_ecc_init().

  @param[out] keyblob
    The buffer that will hold the result keyblob representing the exported ECC key pair (encrypted with device unique key).
    If keyblob ptr is NULL, this function will return the number of bytes required for the keyblob via the keyblob_length pointer.

  @param[out] keyblob_length
    The length of the ECC key blob

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_ecc_init() & m2mb_crypto_ecc_keypair_generate() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecc_keyblob_export( M2MB_CRYPTO_ECC_CONTEXT            ctx,
                                              UINT8                             *keyblob,
                                              UINT16                            *keyblob_length );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Imports an ECC key pair keyblob into the context.

  @details
    This function imports a given keyblob (which was previously created by m2mb_crypto_ecc_keypair_generate and was exported by m2mb_crypto_ecc_keyblob_export()) to the provided context.
    This basically allows using of an old keys with an existing context.
    The blob is encrypted with a device specific key and can only be used on the same HW originating it.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_ecc_init().

  @param[in] keyblob
    The keyblob previously exported using m2mb_crypto_ecc_keyblob_export() that will be imported into the given context.
    These keys can then be used for ECC operations like sign & verify.

  @param[in] keyblob_length
    The length of the ECC key blob

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_ecc_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecc_keyblob_import( M2MB_CRYPTO_ECC_CONTEXT         ctx,
                                              const UINT8                    *keyblob,
                                              UINT32                          keyblob_length );


/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Exports an ECC public key from the given context.

  @details
    This function exports an ECC public key for the given context.
    The ECC public key information will be exported from the given context (assuming context has keys associated).

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_ecc_init().

  @param[out] pubkey
    The exported ECC public key (plain public key).
    The public key is represented as the x,y coordinates of a point on the given curve.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_ecc_init() & m2mb_crypto_ecc_keypair_generate() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecc_public_key_export( M2MB_CRYPTO_ECC_CONTEXT         ctx,
                                                 M2MB_CRYPTO_ECC_AFFINE_POINT_T *pubkey );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Imports an ECC public key to the given context.

  @details
    This function imports an ECC public key for the given context.
    The ECC public key information will be stored into the given context.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_ecc_init().

  @param[in] pubkey
    The imported ECC public key (plain public key).
    The public key is represented as the x,y coordinates of a point on the given curve.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_ecc_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecc_public_key_import( M2MB_CRYPTO_ECC_CONTEXT               ctx,
                                                 const M2MB_CRYPTO_ECC_AFFINE_POINT_T *pubkey );


#ifdef TZ_DEBUG
/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Exports an ECC private key from the given context.

  @details
    This function exports an ECC private key for the given context.
    The ECC private key information will be exported from the given context (assuming context has keys associated).

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_ecc_init().

  @param[out] private_key
    The exported ECC private key (plain private key) in binary format.

  @param[in|out] private_key_len
    The length of the private_key buffer.
    Will be updated with the actual length of the exported key.


  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_ecc_init() & m2mb_crypto_ecc_keypair_generate() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecc_private_key_export( M2MB_CRYPTO_ECC_CONTEXT           ctx,
                                                  UINT8                            *private_key,
                                                  UINT16                           *private_key_len);

#endif

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Imports an ECC private key to the given context.

  @details
    This function imports an ECC private key for the given context.
    The ECC private key information will be stored into the given context.

  @param[in] ctx
    Pointer to a context structure previously allocated by m2mb_crypto_ecc_init().

  @param[in] privkey
    The imported ECC private key (plain private key) in binary format.

  @param[in] privkey_len
    The length of the privkey data.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note
    m2mb_crypto_ecc_init() must be called before using this function.

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecc_private_key_import( M2MB_CRYPTO_ECC_CONTEXT               ctx,
                                                  const UINT8                          *privkey,
                                                  UINT16                                privkey_len );

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Free an M2MB_CRYPTO_ECC_CONTEXT structure

  @details
    This function free a previously allocated ECC context.

  @param[in] ctx
    Pointer to a context structure to be free by this function.

  @return
    M2MB_RESULT_SUCCESS on success
  @return
    see M2MB_RESULT_E for different values in case of fail

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
M2MB_RESULT_E m2mb_crypto_ecc_deinit( M2MB_CRYPTO_ECC_CONTEXT  ctx );

/** @} */
/** @} */

/**
   @addtogroup PRNG M2MB_PRNG
   @ingroup M2MB_CRYPTO
   @brief Pseudo random number generator.
   @{
*/

/* Global declarations ==========================================================================*/

/*! Maximum number of PRNG bytes read */
#define M2MB_CRYPTO_PRNG_MAX_BYTES    512

/* Global functions =============================================================================*/

/*-----------------------------------------------------------------------------------------------*/
/*!
  @brief
    Generates prng data.

  @details
    This function will generate prng data with the given size but at most M2MB_CRYPTO_PRNG_MAX_BYTES bytes.

  @param[out] prng_data
    Buffer that will hold the result prng data (buffer length is at least <prng_data_len>).

  @param[in] prng_data_len
    The size of the prng_data buffer but cannot exceed M2MB_CRYPTO_PRNG_MAX_BYTES.

  @return
    The number of generated prng bytes.

  @note

  @b

  @code

  @endcode
*/
/*-----------------------------------------------------------------------------------------------*/
UINT32 m2mb_crypto_prng_get_data( UINT8   *prng_data,
                                  UINT32   prng_data_len );
/** @} */


#endif /* M2M_M2MB_CRYPTO_H */
