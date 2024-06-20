#include "sha256.h"
#include "hash.i"

#undef S
#undef R
#undef Ch
#undef Maj
#undef Sigma0
#undef Sigma1
#undef Gamma0
#undef Gamma1
/* Various logical functions */
#define S(x, n) RORc((x), (n))
#define R(x, n) (((x) & 0xFFFFFFFF) >> (n))
#define Ch(x, y, z) (z ^ (x & (y ^ z)))
#define Maj(x, y, z) (((x | y) & z) | (x & y))
#define Sigma0(x) (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x) (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x) (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x) (S(x, 17) ^ S(x, 19) ^ R(x, 10))

static void sha256_compress(sha256_s *ctx, unsigned char const *buf)
{
    static uint32_t const k[0x40] = {
        /* clang-format off */
        0x428A2F98, 0x71374491, 0xB5C0FBCF, 0xE9B5DBA5, 0x3956C25B, 0x59F111F1, 0x923F82A4, 0xAB1C5ED5,
        0xD807AA98, 0x12835B01, 0x243185BE, 0x550C7DC3, 0x72BE5D74, 0x80DEB1FE, 0x9BDC06A7, 0xC19BF174,
        0xE49B69C1, 0xEFBE4786, 0x0FC19DC6, 0x240CA1CC, 0x2DE92C6F, 0x4A7484AA, 0x5CB0A9DC, 0x76F988DA,
        0x983E5152, 0xA831C66D, 0xB00327C8, 0xBF597FC7, 0xC6E00BF3, 0xD5A79147, 0x06CA6351, 0x14292967,
        0x27B70A85, 0x2E1B2138, 0x4D2C6DFC, 0x53380D13, 0x650A7354, 0x766A0ABB, 0x81C2C92E, 0x92722C85,
        0xA2BFE8A1, 0xA81A664B, 0xC24B8B70, 0xC76C51A3, 0xD192E819, 0xD6990624, 0xF40E3585, 0x106AA070,
        0x19A4C116, 0x1E376C08, 0x2748774C, 0x34B0BCB5, 0x391C0CB3, 0x4ED8AA4A, 0x5B9CCA4F, 0x682E6FF3,
        0x748F82EE, 0x78A5636F, 0x84C87814, 0x8CC70208, 0x90BEFFFA, 0xA4506CEB, 0xBEF9A3F7, 0xC67178F2,
        /* clang-format on */
    };

    uint32_t w[0x40], t0, t1;
    uint32_t s[sizeof(ctx->state_) / sizeof(*ctx->state_)];

    /* copy state into s */
    for (unsigned int i = 0; i != sizeof(ctx->state_) / sizeof(*ctx->state_); ++i)
    {
        s[i] = ctx->state_[i];
    }

    /* copy the state into 512-bits into w[0..15] */
    for (unsigned int i = 0x00; i != 0x10; ++i)
    {
        LOAD32H(w[i], buf + sizeof(*ctx->state_) * i);
    }

    /* fill w[16..63] */
    for (unsigned int i = 0x10; i != 0x40; ++i)
    {
        w[i] = Gamma1(w[i - 2]) + w[i - 7] + Gamma0(w[i - 15]) + w[i - 16];
    }

    /* compress */
#undef RND
#define RND(a, b, c, d, e, f, g, h, i)              \
    t0 = h + Sigma1(e) + Ch(e, f, g) + k[i] + w[i]; \
    t1 = Sigma0(a) + Maj(a, b, c);                  \
    d += t0;                                        \
    h = t0 + t1
    for (unsigned int i = 0; i != 0x40; i += 8)
    {
        RND(s[0], s[1], s[2], s[3], s[4], s[5], s[6], s[7], i + 0);
        RND(s[7], s[0], s[1], s[2], s[3], s[4], s[5], s[6], i + 1);
        RND(s[6], s[7], s[0], s[1], s[2], s[3], s[4], s[5], i + 2);
        RND(s[5], s[6], s[7], s[0], s[1], s[2], s[3], s[4], i + 3);
        RND(s[4], s[5], s[6], s[7], s[0], s[1], s[2], s[3], i + 4);
        RND(s[3], s[4], s[5], s[6], s[7], s[0], s[1], s[2], i + 5);
        RND(s[2], s[3], s[4], s[5], s[6], s[7], s[0], s[1], i + 6);
        RND(s[1], s[2], s[3], s[4], s[5], s[6], s[7], s[0], i + 7);
    }
#undef RND

    /* feedback */
    for (unsigned int i = 0; i != sizeof(ctx->state_) / sizeof(*ctx->state_); ++i)
    {
        ctx->state_[i] += s[i];
    }
}

#undef Gamma1
#undef Gamma0
#undef Sigma1
#undef Sigma0
#undef Maj
#undef Ch
#undef R
#undef S

void sha256_init(sha256_s *ctx)
{
    ctx->cursiz_ = 0;
    ctx->length_ = 0;

    ctx->state_[0] = 0x6A09E667;
    ctx->state_[1] = 0xBB67AE85;
    ctx->state_[2] = 0x3C6EF372;
    ctx->state_[3] = 0xA54FF53A;
    ctx->state_[4] = 0x510E527F;
    ctx->state_[5] = 0x9B05688C;
    ctx->state_[6] = 0x1F83D9AB;
    ctx->state_[7] = 0x5BE0CD19;
}

void sha224_init(sha256_s *ctx)
{
    ctx->cursiz_ = 0;
    ctx->length_ = 0;

    ctx->state_[0] = 0xC1059ED8;
    ctx->state_[1] = 0x367CD507;
    ctx->state_[2] = 0x3070DD17;
    ctx->state_[3] = 0xF70E5939;
    ctx->state_[4] = 0xFFC00B31;
    ctx->state_[5] = 0x68581511;
    ctx->state_[6] = 0x64F98FA7;
    ctx->state_[7] = 0xBEFA4FA4;
}

HASH_PROC(sha256_s, sha256_proc, sha256_compress)

HASH_DONE(sha256_s, sha256_done, sha256_compress, STORE64H, STORE32H, 0x80, 0x38, 0x38)

SHA2_DONE(sha256_s, sha256_done, sha224_done, 224 >> 3)
