/*!
 @file sha256.h
 @brief RFC 5754 compliant SHA2 implementation
 @details https://www.ietf.org/rfc/rfc5754.txt
*/

#ifndef SHA256_H
#define SHA256_H

#include <stddef.h>
#include <stdint.h>

#define SHA256_BUFSIZ 0x40
#define SHA256_OUTSIZ (256 >> 3)
#define SHA224_OUTSIZ (224 >> 3)

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* __GNUC__ || __clang__ */

typedef struct sha256_s
{
    uint64_t _length;
    uint32_t _state[SHA256_OUTSIZ >> 2];
    unsigned char _buf[SHA256_BUFSIZ];
    unsigned char out[SHA256_OUTSIZ];
    uint32_t _cursiz;
} sha256_s;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */

typedef sha256_s sha224_s;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void sha256_init(sha256_s *ctx);
int sha256_proc(sha256_s *ctx, void const *pdata, size_t nbyte);
unsigned char *sha256_done(sha256_s *ctx, void *out);

void sha224_init(sha256_s *ctx);
#define sha224_proc(ctx, pdata, nbyte) sha256_proc(ctx, pdata, nbyte)
unsigned char *sha224_done(sha256_s *ctx, void *out);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* sha256.h */
