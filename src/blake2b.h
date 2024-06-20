/*!
 @file blake2b.h
 @brief RFC 7693 compliant BLAKE implementation
 @details https://www.ietf.org/rfc/rfc7693.txt
*/

#ifndef BLAKE2B_H
#define BLAKE2B_H

#include <stddef.h>
#include <stdint.h>

#define BLAKE2B_BUFSIZ 0x80
#define BLAKE2B_OUTSIZ 0x40
#define BLAKE2B_160_OUTSIZ (160 >> 3)
#define BLAKE2B_256_OUTSIZ (256 >> 3)
#define BLAKE2B_384_OUTSIZ (384 >> 3)
#define BLAKE2B_512_OUTSIZ (512 >> 3)

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* __GNUC__ || __clang__ */

typedef struct blake2b_s
{
    uint64_t t_[2];
    uint64_t f_[2];
    uint32_t cursiz_;
    uint32_t outsiz;
    uint64_t state_[BLAKE2B_OUTSIZ >> 3];
    unsigned char out[BLAKE2B_OUTSIZ];
    unsigned char buf_[BLAKE2B_BUFSIZ];
    unsigned char lastnode_;
} blake2b_s;

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void blake2b_160_init(blake2b_s *ctx);
void blake2b_256_init(blake2b_s *ctx);
void blake2b_384_init(blake2b_s *ctx);
void blake2b_512_init(blake2b_s *ctx);
int blake2b_init(blake2b_s *ctx, size_t siz, void const *pdata, size_t nbyte);
int blake2b_proc(blake2b_s *ctx, void const *pdata, size_t nbyte);
unsigned char *blake2b_done(blake2b_s *ctx, void *out);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* blake2b.h */
