/*!
 @file sha1.h
 @brief RFC 3174 compliant SHA1 implementation
 @details https://www.ietf.org/rfc/rfc3174.txt
*/

#ifndef SHA1_H
#define SHA1_H

#include <stddef.h>
#include <stdint.h>

#define SHA1_BUFSIZ 0x40
#define SHA1_OUTSIZ 0x14

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* __GNUC__ || __clang__ */

typedef struct sha1_s
{
    uint64_t length_;
    unsigned char buf_[SHA1_BUFSIZ];
    uint32_t state_[SHA1_OUTSIZ >> 2];
    unsigned char out[SHA1_OUTSIZ];
    uint32_t cursiz_;
} sha1_s;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void sha1_init(sha1_s *ctx);
int sha1_proc(sha1_s *ctx, void const *pdata, size_t nbyte);
unsigned char *sha1_done(sha1_s *ctx, void *out);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* sha1.h */
