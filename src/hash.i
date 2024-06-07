#ifndef HASH_I
#define HASH_I

#if defined(_MSC_VER)
#include <stdlib.h> /* intrinsic */
#endif /* _MSC_VER */
#include <string.h>

/* fallback for __has_builtin */
#ifndef __has_builtin
#define __has_builtin(x) 0
#endif /* __has_builtin */

/* Endian Neutral macros that work on all platforms */

#undef BSWAP
#define BSWAP(x)                    \
    ((((x) >> 0x18) & 0x000000FF) | \
     (((x) << 0x18) & 0xFF000000) | \
     (((x) >> 0x08) & 0x0000FF00) | \
     (((x) << 0x08) & 0x00FF0000))

#undef LOAD32L
#undef LOAD64L
#undef STORE32L
#undef STORE64L
#ifndef ENDIAN_NEUTRAL
#define LOAD32L(x, y)       \
    do                      \
    {                       \
        memcpy(&(x), y, 4); \
        (x) &= 0xFFFFFFFF;  \
    } while (0)
#define STORE32L(x, y)      \
    do                      \
    {                       \
        uint32_t v__ = x;   \
        memcpy(y, &v__, 4); \
    } while (0)
#define LOAD64L(x, y)       \
    do                      \
    {                       \
        memcpy(&(x), y, 8); \
    } while (0)
#define STORE64L(x, y)      \
    do                      \
    {                       \
        uint64_t v__ = x;   \
        memcpy(y, &v__, 8); \
    } while (0)
#else /* !ENDIAN_NEUTRAL */
#define LOAD32L(x, y)                               \
    do                                              \
    {                                               \
        (x) = ((uint32_t)((y)[3] & 0xFF) << 0x18) | \
              ((uint32_t)((y)[2] & 0xFF) << 0x10) | \
              ((uint32_t)((y)[1] & 0xFF) << 0x08) | \
              ((uint32_t)((y)[0] & 0xFF) << 0x00);  \
    } while (0)
#define STORE32L(x, y)                            \
    do                                            \
    {                                             \
        (y)[3] = (uint8_t)(((x) >> 0x18) & 0xFF); \
        (y)[2] = (uint8_t)(((x) >> 0x10) & 0xFF); \
        (y)[1] = (uint8_t)(((x) >> 0x08) & 0xFF); \
        (y)[0] = (uint8_t)(((x) >> 0x00) & 0xFF); \
    } while (0)
#define LOAD64L(x, y)                                 \
    do                                                \
    {                                                 \
        (x) = (((uint64_t)((y)[7] & 0xFF)) << 0x38) | \
              (((uint64_t)((y)[6] & 0xFF)) << 0x30) | \
              (((uint64_t)((y)[5] & 0xFF)) << 0x28) | \
              (((uint64_t)((y)[4] & 0xFF)) << 0x20) | \
              (((uint64_t)((y)[3] & 0xFF)) << 0x18) | \
              (((uint64_t)((y)[2] & 0xFF)) << 0x10) | \
              (((uint64_t)((y)[1] & 0xFF)) << 0x08) | \
              (((uint64_t)((y)[0] & 0xFF)) << 0x00);  \
    } while (0)
#define STORE64L(x, y)                            \
    do                                            \
    {                                             \
        (y)[7] = (uint8_t)(((x) >> 0x38) & 0xFF); \
        (y)[6] = (uint8_t)(((x) >> 0x30) & 0xFF); \
        (y)[5] = (uint8_t)(((x) >> 0x28) & 0xFF); \
        (y)[4] = (uint8_t)(((x) >> 0x20) & 0xFF); \
        (y)[3] = (uint8_t)(((x) >> 0x18) & 0xFF); \
        (y)[2] = (uint8_t)(((x) >> 0x10) & 0xFF); \
        (y)[1] = (uint8_t)(((x) >> 0x08) & 0xFF); \
        (y)[0] = (uint8_t)(((x) >> 0x00) & 0xFF); \
    } while (0)
#endif /* ENDIAN_NEUTRAL */

#undef LOAD32H
#undef STORE32H
#if __has_builtin(__builtin_bswap32)
#define LOAD32H(x, y)               \
    do                              \
    {                               \
        memcpy(&(x), y, 4);         \
        (x) = __builtin_bswap32(x); \
    } while (0)
#define STORE32H(x, y)                       \
    do                                       \
    {                                        \
        uint32_t v__ = __builtin_bswap32(x); \
        memcpy(y, &v__, 4);                  \
    } while (0)
#else /* !__has_builtin(__builtin_bswap32) */
#define LOAD32H(x, y)                               \
    do                                              \
    {                                               \
        (x) = ((uint32_t)((y)[0] & 0xFF) << 0x18) | \
              ((uint32_t)((y)[1] & 0xFF) << 0x10) | \
              ((uint32_t)((y)[2] & 0xFF) << 0x08) | \
              ((uint32_t)((y)[3] & 0xFF) << 0x00);  \
    } while (0)
#define STORE32H(x, y)                            \
    do                                            \
    {                                             \
        (y)[0] = (uint8_t)(((x) >> 0x18) & 0xFF); \
        (y)[1] = (uint8_t)(((x) >> 0x10) & 0xFF); \
        (y)[2] = (uint8_t)(((x) >> 0x08) & 0xFF); \
        (y)[3] = (uint8_t)(((x) >> 0x00) & 0xFF); \
    } while (0)
#endif /* __has_builtin(__builtin_bswap32) */

#undef LOAD64H
#undef STORE64H
#if __has_builtin(__builtin_bswap64)
#define LOAD64H(x, y)               \
    do                              \
    {                               \
        memcpy(&(x), y, 8);         \
        (x) = __builtin_bswap64(x); \
    } while (0)
#define STORE64H(x, y)                       \
    do                                       \
    {                                        \
        uint64_t v__ = __builtin_bswap64(x); \
        memcpy(y, &v__, 8);                  \
    } while (0)
#else /* !__has_builtin(__builtin_bswap64) */
#define LOAD64H(x, y)                                 \
    do                                                \
    {                                                 \
        (x) = (((uint64_t)((y)[0] & 0xFF)) << 0x38) | \
              (((uint64_t)((y)[1] & 0xFF)) << 0x30) | \
              (((uint64_t)((y)[2] & 0xFF)) << 0x28) | \
              (((uint64_t)((y)[3] & 0xFF)) << 0x20) | \
              (((uint64_t)((y)[4] & 0xFF)) << 0x18) | \
              (((uint64_t)((y)[5] & 0xFF)) << 0x10) | \
              (((uint64_t)((y)[6] & 0xFF)) << 0x08) | \
              (((uint64_t)((y)[7] & 0xFF)) << 0x00);  \
    } while (0)
#define STORE64H(x, y)                            \
    do                                            \
    {                                             \
        (y)[0] = (uint8_t)(((x) >> 0x38) & 0xFF); \
        (y)[1] = (uint8_t)(((x) >> 0x30) & 0xFF); \
        (y)[2] = (uint8_t)(((x) >> 0x28) & 0xFF); \
        (y)[3] = (uint8_t)(((x) >> 0x20) & 0xFF); \
        (y)[4] = (uint8_t)(((x) >> 0x18) & 0xFF); \
        (y)[5] = (uint8_t)(((x) >> 0x10) & 0xFF); \
        (y)[6] = (uint8_t)(((x) >> 0x08) & 0xFF); \
        (y)[7] = (uint8_t)(((x) >> 0x00) & 0xFF); \
    } while (0)
#endif /* __has_builtin(__builtin_bswap64) */

/* 32-bit Rotates */
#undef ROL
#undef ROR
#undef ROLc
#undef RORc
#if defined(_MSC_VER)
/* instrinsic rotate */
#pragma intrinsic(_rotl, _rotr)
#define ROL(x, n) _rotl(x, n)
#define ROR(x, n) _rotr(x, n)
#elif __has_builtin(__builtin_rotateleft32) && __has_builtin(__builtin_rotateright32)
#define ROL(x, n) __builtin_rotateleft32(x, n)
#define ROR(x, n) __builtin_rotateright32(x, n)
#else /* rotates the hard way */
#define ROL(x, y) ((((uint32_t)(x) << (uint32_t)((y) & 31)) | (((uint32_t)(x) & 0xFFFFFFFF) >> (uint32_t)((32 - ((y) & 31)) & 31))) & 0xFFFFFFFF)
#define ROR(x, y) (((((uint32_t)(x) & 0xFFFFFFFF) >> (uint32_t)((y) & 31)) | ((uint32_t)(x) << (uint32_t)((32 - ((y) & 31)) & 31))) & 0xFFFFFFFF)
#endif /* 32-bit Rotates */
#define ROLc(x, n) ROL(x, n)
#define RORc(x, n) ROR(x, n)

/* 64-bit Rotates */
#undef ROR64
#undef ROL64
#undef ROL64c
#undef ROR64c
#if defined(_MSC_VER)
/* instrinsic rotate */
#pragma intrinsic(_rotl64, _rotr64)
#define ROL64(x, n) _rotl64(x, n)
#define ROR64(x, n) _rotr64(x, n)
#elif __has_builtin(__builtin_rotateleft64) && __has_builtin(__builtin_rotateright64)
#define ROL64(x, n) __builtin_rotateleft64(x, n)
#define ROR64(x, n) __builtin_rotateright64(x, n)
#else /* rotates the hard way */
#define ROL64(x, y) ((((uint64_t)(x) << ((uint64_t)(y) & 63)) | (((uint64_t)(x) & 0xFFFFFFFFFFFFFFFF) >> (uint64_t)((64 - ((y) & 63)) & 63))) & 0xFFFFFFFFFFFFFFFF)
#define ROR64(x, y) ((((uint64_t)((x) & 0xFFFFFFFFFFFFFFFF) >> ((uint64_t)(y) & 63)) | ((uint64_t)(x) << (uint64_t)((64 - ((y) & 63)) & 63))) & 0xFFFFFFFFFFFFFFFF)
#endif /* 64-bit Rotates */
#define ROL64c(x, n) ROL64(x, n)
#define ROR64c(x, n) ROR64(x, n)

#define HASH_PROC(hash, func, compress)                                      \
    int func(hash *ctx, void const *pdata, size_t nbyte)                     \
    {                                                                        \
        if (sizeof(ctx->_buf) < ctx->_cursiz) { return INVALID; }            \
        if (ctx->_length + (nbyte << 3) < ctx->_length) { return OVERFLOW; } \
        unsigned char const *p = (unsigned char const *)pdata;               \
        while (nbyte)                                                        \
        {                                                                    \
            if ((ctx->_cursiz == 0) && (sizeof(ctx->_buf) - 1 < nbyte))      \
            {                                                                \
                compress(ctx, p);                                            \
                ctx->_length += sizeof(ctx->_buf) << 3;                      \
                nbyte -= sizeof(ctx->_buf);                                  \
                p += sizeof(ctx->_buf);                                      \
            }                                                                \
            else                                                             \
            {                                                                \
                uint32_t n = (uint32_t)sizeof(ctx->_buf) - ctx->_cursiz;     \
                n = n < nbyte ? n : (uint32_t)nbyte;                         \
                memcpy(ctx->_buf + ctx->_cursiz, p, n);                      \
                ctx->_cursiz += n;                                           \
                nbyte -= n;                                                  \
                p += n;                                                      \
                if (sizeof(ctx->_buf) == ctx->_cursiz)                       \
                {                                                            \
                    compress(ctx, ctx->_buf);                                \
                    ctx->_length += sizeof(ctx->_buf) << 3;                  \
                    ctx->_cursiz = 0;                                        \
                }                                                            \
            }                                                                \
        }                                                                    \
        return SUCCESS;                                                      \
    }

#define HASH_DONE(hash, func, compress, storelen, storeout, append, above, zero)       \
    unsigned char *func(hash *ctx, void *out)                                          \
    {                                                                                  \
        if (sizeof(ctx->_buf) - 1 < ctx->_cursiz) { return 0; }                        \
        /* increase the length of the message */                                       \
        ctx->_length += sizeof(ctx->_length) * ctx->_cursiz;                           \
        /* append the '1' bit */                                                       \
        ctx->_buf[ctx->_cursiz++] = append;                                            \
        /* if the length is currently above $above bytes we append zeros    */         \
        /* then compress. Then we can fall back to padding zeros and length */         \
        /* encoding like normal.                                            */         \
        if ((above) < ctx->_cursiz)                                                    \
        {                                                                              \
            while (sizeof(ctx->_buf) != ctx->_cursiz)                                  \
            {                                                                          \
                ctx->_buf[ctx->_cursiz++] = 0;                                         \
            }                                                                          \
            compress(ctx, ctx->_buf);                                                  \
            ctx->_cursiz = 0;                                                          \
        }                                                                              \
        /* pad up to $zero bytes of zeroes */                                          \
        while (ctx->_cursiz < (zero))                                                  \
        {                                                                              \
            ctx->_buf[ctx->_cursiz++] = 0;                                             \
        }                                                                              \
        /* store length */                                                             \
        storelen(ctx->_length, ctx->_buf + (zero));                                    \
        compress(ctx, ctx->_buf);                                                      \
        /* copy output */                                                              \
        for (unsigned int i = 0; i != sizeof(ctx->_state) / sizeof(*ctx->_state); ++i) \
        {                                                                              \
            storeout(ctx->_state[i], ctx->out + sizeof(*ctx->_state) * i);             \
        }                                                                              \
        if (out && (out != ctx->out))                                                  \
        {                                                                              \
            memcpy(out, ctx->out, sizeof(ctx->_state));                                \
        }                                                                              \
        return ctx->out;                                                               \
    }

#define SHA2_DONE(def, done, func, size)          \
    unsigned char *func(def *ctx, void *out)      \
    {                                             \
        unsigned char *ret = done(ctx, ctx->out); \
                                                  \
        if (ret && out && (out != ctx->out))      \
        {                                         \
            memcpy(out, ctx->out, size);          \
        }                                         \
                                                  \
        return ret;                               \
    }

enum
{
#undef SUCCESS
    SUCCESS = 0,
#undef WARNING
    WARNING = -1,
#undef FAILURE
    FAILURE = -2,
#undef INVALID
    INVALID = -3,
#undef OVERFLOW
    OVERFLOW = -4,
#undef NOTFOUND
    NOTFOUND = -5,
};

#endif /* hash.i */
