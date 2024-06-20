/*!
 @file sha3.h
 @brief SHA3 implementation
 @details https://en.wikipedia.org/wiki/SHA-3
*/

#ifndef SHA3_H
#define SHA3_H

#include <stddef.h>
#include <stdint.h>

#define SHA3_224_BUFSIZ (200 - (224 >> 2))
#define SHA3_256_BUFSIZ (200 - (256 >> 2))
#define SHA3_384_BUFSIZ (200 - (384 >> 2))
#define SHA3_512_BUFSIZ (200 - (512 >> 2))
#define SHA3_224_OUTSIZ (224 >> 3)
#define SHA3_256_OUTSIZ (256 >> 3)
#define SHA3_384_OUTSIZ (384 >> 3)
#define SHA3_512_OUTSIZ (512 >> 3)

typedef struct sha3_s
{
    uint64_t s_[25];
    uint64_t saved_; /* the portion of the input message that we didn't consume yet */
    unsigned char out[200]; /* used for storing `uint64_t s[25]` as little-endian bytes */
    unsigned short byte_index_; /* 0..7--the next byte after the set one (starts from 0; 0--none are buffered) */
    unsigned short word_index_; /* 0..24--the next word to integrate input (starts from 0) */
    unsigned short capacity_words_; /* the double length of the hash output in words (e.g. 16 for Keccak 512) */
    unsigned short xof_flag_;
} sha3_s;

#define KECCAK224_BUFSIZ (200 - (224 >> 2))
#define KECCAK256_BUFSIZ (200 - (256 >> 2))
#define KECCAK384_BUFSIZ (200 - (384 >> 2))
#define KECCAK512_BUFSIZ (200 - (512 >> 2))
#define KECCAK224_OUTSIZ (224 >> 3)
#define KECCAK256_OUTSIZ (256 >> 3)
#define KECCAK384_OUTSIZ (384 >> 3)
#define KECCAK512_OUTSIZ (512 >> 3)

typedef sha3_s keccak_s;

#define SHAKE128_BUFSIZ (200 - (128 >> 2))
#define SHAKE256_BUFSIZ (200 - (256 >> 2))
#define SHAKE128_OUTSIZ (128 >> 3)
#define SHAKE256_OUTSIZ (256 >> 3)

typedef sha3_s sha3_shake_s;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void sha3_224_init(sha3_s *ctx);
void sha3_256_init(sha3_s *ctx);
void sha3_384_init(sha3_s *ctx);
void sha3_512_init(sha3_s *ctx);

int sha3_proc(sha3_s *ctx, void const *pdata, size_t nbyte);
unsigned char *sha3_done(sha3_s *ctx, void *out);

#define keccak224_init(ctx) sha3_224_init(ctx)
#define keccak256_init(ctx) sha3_256_init(ctx)
#define keccak384_init(ctx) sha3_384_init(ctx)
#define keccak512_init(ctx) sha3_512_init(ctx)

#define keccak_proc(ctx, pdata, nbyte) sha3_proc(ctx, pdata, nbyte)
unsigned char *keccak_done(sha3_s *ctx, void *out);

void shake128_init(sha3_s *ctx);
void shake256_init(sha3_s *ctx);
unsigned char *shake128_done(sha3_s *ctx, void *out);
unsigned char *shake256_done(sha3_s *ctx, void *out);

int sha3shake_init(sha3_s *ctx, unsigned int num);
#define sha3shake_proc(ctx, pdata, nbyte) sha3_proc(ctx, pdata, nbyte)
void sha3shake_done(sha3_s *ctx, unsigned char *out, unsigned int siz);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* sha3.h */
