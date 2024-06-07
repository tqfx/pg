#include "sha1.h"
#include "hash.i"

static void sha1_compress(sha1_s *ctx, unsigned char const *buf)
{
    /* copy state into s */
    uint32_t s[sizeof(ctx->_state) / sizeof(*ctx->_state)];
    for (unsigned int i = 0; i != sizeof(ctx->_state) / sizeof(*ctx->_state); ++i)
    {
        s[i] = ctx->_state[i];
    }

    /* copy the state into 512-bits into w[0..15] */
    uint32_t w[0x50];
    for (unsigned int i = 0x00; i != 0x10; ++i)
    {
        LOAD32H(w[i], buf + sizeof(*ctx->_state) * i);
    }

    /* expand it */
    for (unsigned int i = 0x10; i != 0x50; ++i)
    {
        w[i] = ROL(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    }

    /* compress */
    unsigned int i = 0;
#undef F0
#undef F1
#undef F2
#undef F3
#define F0(x, y, z) (z ^ (x & (y ^ z)))
#define F1(x, y, z) (x ^ y ^ z)
#define F2(x, y, z) ((x & y) | (z & (x | y)))
#define F3(x, y, z) (x ^ y ^ z)
#undef FF0
#undef FF1
#undef FF2
#undef FF3
#define FF0(a, b, c, d, e, i)                               \
    e = (ROLc(a, 5) + F0(b, c, d) + e + w[i] + 0x5A827999); \
    b = ROLc(b, 30)
#define FF1(a, b, c, d, e, i)                               \
    e = (ROLc(a, 5) + F1(b, c, d) + e + w[i] + 0x6ED9EBA1); \
    b = ROLc(b, 30)
#define FF2(a, b, c, d, e, i)                               \
    e = (ROLc(a, 5) + F2(b, c, d) + e + w[i] + 0x8F1BBCDC); \
    b = ROLc(b, 30)
#define FF3(a, b, c, d, e, i)                               \
    e = (ROLc(a, 5) + F3(b, c, d) + e + w[i] + 0xCA62C1D6); \
    b = ROLc(b, 30)
    /* round 1 */
    while (i != 0x14)
    {
        FF0(s[0], s[1], s[2], s[3], s[4], i++);
        FF0(s[4], s[0], s[1], s[2], s[3], i++);
        FF0(s[3], s[4], s[0], s[1], s[2], i++);
        FF0(s[2], s[3], s[4], s[0], s[1], i++);
        FF0(s[1], s[2], s[3], s[4], s[0], i++);
    }
    /* round 2 */
    while (i != 0x28)
    {
        FF1(s[0], s[1], s[2], s[3], s[4], i++);
        FF1(s[4], s[0], s[1], s[2], s[3], i++);
        FF1(s[3], s[4], s[0], s[1], s[2], i++);
        FF1(s[2], s[3], s[4], s[0], s[1], i++);
        FF1(s[1], s[2], s[3], s[4], s[0], i++);
    }
    /* round 3 */
    while (i != 0x3c)
    {
        FF2(s[0], s[1], s[2], s[3], s[4], i++);
        FF2(s[4], s[0], s[1], s[2], s[3], i++);
        FF2(s[3], s[4], s[0], s[1], s[2], i++);
        FF2(s[2], s[3], s[4], s[0], s[1], i++);
        FF2(s[1], s[2], s[3], s[4], s[0], i++);
    }
    /* round 4 */
    while (i != 0x50)
    {
        FF3(s[0], s[1], s[2], s[3], s[4], i++);
        FF3(s[4], s[0], s[1], s[2], s[3], i++);
        FF3(s[3], s[4], s[0], s[1], s[2], i++);
        FF3(s[2], s[3], s[4], s[0], s[1], i++);
        FF3(s[1], s[2], s[3], s[4], s[0], i++);
    }
#undef FF0
#undef FF1
#undef FF2
#undef FF3
#undef F0
#undef F1
#undef F2
#undef F3

    /* feedback */
    for (i = 0; i != sizeof(ctx->_state) / sizeof(*ctx->_state); ++i)
    {
        ctx->_state[i] += s[i];
    }
}

void sha1_init(sha1_s *ctx)
{
    ctx->_cursiz = 0;
    ctx->_length = 0;

    ctx->_state[0] = 0x67452301;
    ctx->_state[1] = 0xEFCDAB89; // 0x10325476 ^ 0xFFFFFFFF
    ctx->_state[2] = 0x98BADCFE; // 0x67452301 ^ 0xFFFFFFFF
    ctx->_state[3] = 0x10325476;
    ctx->_state[4] = 0xC3D2E1F0;
}

HASH_PROC(sha1_s, sha1_proc, sha1_compress)

HASH_DONE(sha1_s, sha1_done, sha1_compress, STORE64H, STORE32H, 0x80, 0x38, 0x38)
