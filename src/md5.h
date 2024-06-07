/*!
 @file md5.h
 @brief RFC 1321 compliant MD5 implementation
 @details https://www.ietf.org/rfc/rfc1321.txt
*/

#ifndef MD5_H
#define MD5_H

#include <stddef.h>
#include <stdint.h>

#define MD5_BUFSIZ 0x40
#define MD5_OUTSIZ 0x10

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* __GNUC__ || __clang__ */

typedef struct md5_s
{
    uint64_t _length;
    uint32_t _state[MD5_OUTSIZ >> 2];
    unsigned char _buf[MD5_BUFSIZ];
    unsigned char out[MD5_OUTSIZ];
    uint32_t _cursiz;
} md5_s;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void md5_init(md5_s *ctx);
int md5_proc(md5_s *ctx, void const *pdata, size_t nbyte);
unsigned char *md5_done(md5_s *ctx, void *out);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* md5.h */
