/*!
 @file hash.h
 @brief hash library
*/

#ifndef HASH_H
#define HASH_H

#include "md5.h"

#include "sha1.h"

#include "sha256.h"
#include "sha512.h"

#include "sha3.h"

#include "blake2s.h"
#include "blake2b.h"

/*!
    shake128    0xA8    168
    sha3-224    0x90    144
*/
#define HASH_BUFSIZ 0xA8

typedef union hash_u
{
#if defined(MD5_H)
    md5_s md5;
#endif /* md5.h */
#if defined(SHA1_H)
    sha1_s sha1;
#endif /* sha1.h */
#if defined(SHA256_H)
    sha256_s sha256;
#endif /* sha256.h */
#if defined(SHA512_H)
    sha512_s sha512;
#endif /* sha512.h */
#if defined(SHA3_H)
    sha3_s sha3;
#endif /* sha3.h */
#if defined(BLAKE2S_H)
    blake2s_s blake2s;
#endif /* blake2s.h */
#if defined(BLAKE2B_H)
    blake2b_s blake2b;
#endif /* blake2b.h */
} hash_u;

typedef struct hash_s
{
    unsigned int bufsiz; /*!< size of block */
    unsigned int outsiz; /*!< size of digest */
    /*!
     @brief Initialize function for hash.
     @param[in,out] ctx points to an instance of hash state.
    */
    void (*init)(hash_u *ctx);
    /*!
     @brief Process function for hash.
     @param[in,out] ctx points to an instance of hash state.
     @param[in] pdata points to data to hash.
     @param[in] nbyte length of data to hash.
     @return the execution state of the function.
      @retval 0 success
    */
    int (*proc)(hash_u *ctx, void const *pdata, size_t nbyte);
    /*!
     @brief Terminate function for hash.
     @param[in,out] ctx points to an instance of hash state.
     @param[in,out] out points to buffer that holds the digest.
     @return the digest internal buffer.
      @retval 0 generic invalid argument.
    */
    unsigned char *(*done)(hash_u *ctx, void *out);
} hash_s;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#if defined(MD5_H)
extern const hash_s hash_md5;
#endif /* md5.h */
#if defined(SHA1_H)
extern const hash_s hash_sha1;
#endif /* sha1.h */
#if defined(SHA256_H)
extern const hash_s hash_sha224;
extern const hash_s hash_sha256;
#endif /* sha256.h */
#if defined(SHA512_H)
extern const hash_s hash_sha384;
extern const hash_s hash_sha512;
extern const hash_s hash_sha512_224;
extern const hash_s hash_sha512_256;
#endif /* sha512.h */
#if defined(SHA3_H)
extern const hash_s hash_sha3_224;
extern const hash_s hash_sha3_256;
extern const hash_s hash_sha3_384;
extern const hash_s hash_sha3_512;
extern const hash_s hash_shake128;
extern const hash_s hash_shake256;
extern const hash_s hash_keccak224;
extern const hash_s hash_keccak256;
extern const hash_s hash_keccak384;
extern const hash_s hash_keccak512;
#endif /* sha3.h */
#if defined(BLAKE2S_H)
extern const hash_s hash_blake2s_128;
extern const hash_s hash_blake2s_160;
extern const hash_s hash_blake2s_224;
extern const hash_s hash_blake2s_256;
#endif /* blake2s.h */
#if defined(BLAKE2B_H)
extern const hash_s hash_blake2b_160;
extern const hash_s hash_blake2b_256;
extern const hash_s hash_blake2b_384;
extern const hash_s hash_blake2b_512;
#endif /* blake2b.h */

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 @brief Hash a block of memory and store the digest.
 @param[in] ctx points to an instance of hash.
 @param[in] pdata points to data to hash.
 @param[in] nbyte length of data to hash.
 @param[out] out where to store the digest.
 @param[in,out] siz max size and resulting size of the digest.
 @return the execution state of the function.
  @retval 0 success
*/
int hash_memory(hash_s const *ctx, void const *pdata, size_t nbyte, void *out, size_t *siz);

/*!
 @brief Hash multiple (non-adjacent) blocks of memory at once.
 @param[in] ctx points to an instance of hash.
 @param[out] out where to store the digest.
 @param[in,out] siz max size and resulting size of the digest.
 @param[in] pdata points to data to hash.
 @param[in] nbyte length of data to hash.
 @param[in] ... tuples of (p,n) pairs to hash, terminated with a (NULL,x) (x=don't care)
 @return the execution state of the function.
  @retval 0 success
*/
int hash_mmulti(hash_s const *ctx, void *out, size_t *siz, void const *pdata, size_t nbyte, ...);

/*!
 @brief Hash data from an open file handle.
 @param[in] ctx points to an instance of hash.
 @param[in] in points to FILE handle to hash.
 @param[out] out where to store the digest.
 @param[in,out] siz max size and resulting size of the digest.
 @return the execution state of the function.
  @retval 0 success
*/
int hash_filehandle(hash_s const *ctx, FILE *in, void *out, size_t *siz);

/*!
 @brief Hash data from an file.
 @param[in] ctx points to an instance of hash.
 @param[in] fname name of file to hash.
 @param[out] out where to store the digest.
 @param[in,out] siz max size and resulting size of the digest.
 @return the execution state of the function.
  @retval 0 success
*/
int hash_file(hash_s const *ctx, char const *fname, void *out, size_t *siz);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* hash.h */
