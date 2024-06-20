/*!
 @file hmac.h
 @brief RFC 2104 compliant HMAC implementation
 @details https://www.ietf.org/rfc/rfc2104.txt
*/

#ifndef HMAC_H
#define HMAC_H

#include "hash.h"

#define HMAC_BUFSIZ HASH_BUFSIZ

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* __GNUC__ || __clang__ */

typedef struct hmac_s
{
    hash_u state_;
    unsigned int outsiz;
    hash_s const *hash_;
    unsigned char buf[HMAC_BUFSIZ];
} hmac_s;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 @brief Initialize function for HMAC.
 @param[in,out] ctx points to an instance of HMAC.
 @param[in] hash points to an instance of hash descriptor.
 @param[in] pdata points to key.
 @param[in] nbyte length of key.
 @return the execution state of the function
  @retval 0 success
*/
int hmac_init(hmac_s *ctx, hash_s const *hash, void const *pdata, size_t nbyte);

/*!
 @brief Process function for HMAC.
 @param[in,out] ctx points to an instance of HMAC.
 @param[in] pdata points to text.
 @param[in] nbyte length of text.
 @return the execution state of the function
  @retval 0 success
*/
int hmac_proc(hmac_s *ctx, void const *pdata, size_t nbyte);

/*!
 @brief Terminate function for HMAC.
 @param[in,out] ctx points to an instance of HMAC.
 @param[in,out] out points to buffer that holds the digest.
 @return the digest internal buffer.
  @retval 0 generic invalid argument.
*/
unsigned char *hmac_done(hmac_s *ctx, void *out);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 @brief HMAC a block of memory to produce the authentication tag.
 @param[in] hash points to an instance of hash.
 @param[in] pkey points to secret key to HMAC.
 @param[in] nkey length of secret key to HMAC.
 @param[in] pmsg points to data to HMAC.
 @param[in] nmsg length of data to HMAC.
 @param[out] out where to store the authentication tag.
 @param[in,out] siz max size and resulting size of the authentication tag.
 @return the execution state of the function.
  @retval 0 success
*/
int hmac_memory(hash_s const *hash, void const *pkey, size_t nkey, void const *pmsg, size_t nmsg, void *out, size_t *siz);

/*!
 @brief HMAC multiple blocks of memory to produce the authentication tag.
 @param[in] hash points to an instance of hash.
 @param[out] out where to store the authentication tag.
 @param[in,out] siz max size and resulting size of the authentication tag.
 @param[in] pkey points to secret key to HMAC.
 @param[in] nkey length of secret key to HMAC.
 @param[in] pmsg points to data to HMAC.
 @param[in] nmsg length of data to HMAC.
 @param[in] ... tuples of (p,n) pairs to hmac, terminated with a (NULL,x) (x=don't care)
 @return the execution state of the function.
  @retval 0 success
*/
int hmac_mmulti(hash_s const *hash, void *out, size_t *siz, void const *pkey, size_t nkey, void const *pmsg, size_t nmsg, ...);

/*!
 @brief HMAC data from an file.
 @param[in] hash points to an instance of hash.
 @param[in] pkey points to secret key to HMAC.
 @param[in] nkey length of secret key to HMAC.
 @param[in] in points to FILE handle to HMAC.
 @param[out] out where to store the authentication tag.
 @param[in,out] siz max size and resulting size of the authentication tag.
 @return the execution state of the function.
  @retval 0 success
*/
int hmac_filehandle(hash_s const *hash, void const *pkey, size_t nkey, FILE *in, void *out, size_t *siz);

/*!
 @brief HMAC data from an open file handle.
 @param[in] hash points to an instance of hash.
 @param[in] pkey points to secret key to HMAC.
 @param[in] nkey length of secret key to HMAC.
 @param[in] fname name of file to HMAC.
 @param[out] out where to store the authentication tag.
 @param[in,out] siz max size and resulting size of the authentication tag.
 @return the execution state of the function.
  @retval 0 success
*/
int hmac_file(hash_s const *hash, void const *pkey, size_t nkey, char const *fname, void *out, size_t *siz);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* hmac.h */
