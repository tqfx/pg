#include "sha512.h"
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
#define S(x, n) ROR64c(x, n)
#define R(x, n) (((x) & 0xFFFFFFFFFFFFFFFF) >> (n))
#define Ch(x, y, z) (z ^ (x & (y ^ z)))
#define Maj(x, y, z) (((x | y) & z) | (x & y))
#define Sigma0(x) (S(x, 28) ^ S(x, 34) ^ S(x, 39))
#define Sigma1(x) (S(x, 14) ^ S(x, 18) ^ S(x, 41))
#define Gamma0(x) (S(x, 1) ^ S(x, 8) ^ R(x, 7))
#define Gamma1(x) (S(x, 19) ^ S(x, 61) ^ R(x, 6))

static void sha512_compress(sha512_s *ctx, unsigned char const *buf)
{
    static uint64_t const k[0x50] = {
        /* clang-format off */
        0x428A2F98D728AE22, 0x7137449123EF65CD, 0xB5C0FBCFEC4D3B2F, 0xE9B5DBA58189DBBC,
        0x3956C25BF348B538, 0x59F111F1B605D019, 0x923F82A4AF194F9B, 0xAB1C5ED5DA6D8118,
        0xD807AA98A3030242, 0x12835B0145706FBE, 0x243185BE4EE4B28C, 0x550C7DC3D5FFB4E2,
        0x72BE5D74F27B896F, 0x80DEB1FE3B1696B1, 0x9BDC06A725C71235, 0xC19BF174CF692694,
        0xE49B69C19EF14AD2, 0xEFBE4786384F25E3, 0x0FC19DC68B8CD5B5, 0x240CA1CC77AC9C65,
        0x2DE92C6F592B0275, 0x4A7484AA6EA6E483, 0x5CB0A9DCBD41FBD4, 0x76F988DA831153B5,
        0x983E5152EE66DFAB, 0xA831C66D2DB43210, 0xB00327C898FB213F, 0xBF597FC7BEEF0EE4,
        0xC6E00BF33DA88FC2, 0xD5A79147930AA725, 0x06CA6351E003826F, 0x142929670A0E6E70,
        0x27B70A8546D22FFC, 0x2E1B21385C26C926, 0x4D2C6DFC5AC42AED, 0x53380D139D95B3DF,
        0x650A73548BAF63DE, 0x766A0ABB3C77B2A8, 0x81C2C92E47EDAEE6, 0x92722C851482353B,
        0xA2BFE8A14CF10364, 0xA81A664BBC423001, 0xC24B8B70D0F89791, 0xC76C51A30654BE30,
        0xD192E819D6EF5218, 0xD69906245565A910, 0xF40E35855771202A, 0x106AA07032BBD1B8,
        0x19A4C116B8D2D0C8, 0x1E376C085141AB53, 0x2748774CDF8EEB99, 0x34B0BCB5E19B48A8,
        0x391C0CB3C5C95A63, 0x4ED8AA4AE3418ACB, 0x5B9CCA4F7763E373, 0x682E6FF3D6B2B8A3,
        0x748F82EE5DEFB2FC, 0x78A5636F43172F60, 0x84C87814A1F0AB72, 0x8CC702081A6439EC,
        0x90BEFFFA23631E28, 0xA4506CEBDE82BDE9, 0xBEF9A3F7B2C67915, 0xC67178F2E372532B,
        0xCA273ECEEA26619C, 0xD186B8C721C0C207, 0xEADA7DD6CDE0EB1E, 0xF57D4F7FEE6ED178,
        0x06F067AA72176FBA, 0x0A637DC5A2C898A6, 0x113F9804BEF90DAE, 0x1B710B35131C471B,
        0x28DB77F523047D84, 0x32CAAB7B40C72493, 0x3C9EBE0A15C9BEBC, 0x431D67C49C100D4C,
        0x4CC5D4BECB3E42B6, 0x597F299CFC657E2A, 0x5FCB6FAB3AD6FAEC, 0x6C44198C4A475817,
        /* clang-format on */
    };

    uint64_t w[0x50], t0, t1;
    uint64_t s[sizeof(ctx->state_) / sizeof(*ctx->state_)];

    /* copy state into s */
    for (unsigned int i = 0; i != sizeof(ctx->state_) / sizeof(*ctx->state_); ++i)
    {
        s[i] = ctx->state_[i];
    }

    /* copy the state into 1024-bits into w[0..15] */
    for (unsigned int i = 0x00; i != 0x10; ++i)
    {
        LOAD64H(w[i], buf + sizeof(*ctx->state_) * i);
    }

    /* fill w[16..79] */
    for (unsigned int i = 0x10; i != 0x50; ++i)
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
    for (unsigned int i = 0; i != 0x50; i += 8)
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

void sha512_init(sha512_s *ctx)
{
    ctx->cursiz_ = 0;
    ctx->length_ = 0;

    ctx->state_[0] = 0x6A09E667F3BCC908;
    ctx->state_[1] = 0xBB67AE8584CAA73B;
    ctx->state_[2] = 0x3C6EF372FE94F82B;
    ctx->state_[3] = 0xA54FF53A5F1D36F1;
    ctx->state_[4] = 0x510E527FADE682D1;
    ctx->state_[5] = 0x9B05688C2B3E6C1F;
    ctx->state_[6] = 0x1F83D9ABFB41BD6B;
    ctx->state_[7] = 0x5BE0CD19137E2179;
}

void sha384_init(sha512_s *ctx)
{
    ctx->cursiz_ = 0;
    ctx->length_ = 0;

    ctx->state_[0] = 0xCBBB9D5DC1059ED8;
    ctx->state_[1] = 0x629A292A367CD507;
    ctx->state_[2] = 0x9159015A3070DD17;
    ctx->state_[3] = 0x152FECD8F70E5939;
    ctx->state_[4] = 0x67332667FFC00B31;
    ctx->state_[5] = 0x8EB44A8768581511;
    ctx->state_[6] = 0xDB0C2E0D64F98FA7;
    ctx->state_[7] = 0x47B5481DBEFA4FA4;
}

void sha512_224_init(sha512_s *ctx)
{
    ctx->cursiz_ = 0;
    ctx->length_ = 0;

    ctx->state_[0] = 0x8C3D37C819544DA2;
    ctx->state_[1] = 0x73E1996689DCD4D6;
    ctx->state_[2] = 0x1DFAB7AE32FF9C82;
    ctx->state_[3] = 0x679DD514582F9FCF;
    ctx->state_[4] = 0x0F6D2B697BD44DA8;
    ctx->state_[5] = 0x77E36F7304C48942;
    ctx->state_[6] = 0x3F9D85A86A1D36C8;
    ctx->state_[7] = 0x1112E6AD91D692A1;
}

void sha512_256_init(sha512_s *ctx)
{
    ctx->cursiz_ = 0;
    ctx->length_ = 0;

    ctx->state_[0] = 0x22312194FC2BF72C;
    ctx->state_[1] = 0x9F555FA3C84C64C2;
    ctx->state_[2] = 0x2393B86B6F53B151;
    ctx->state_[3] = 0x963877195940EABD;
    ctx->state_[4] = 0x96283EE2A88EFFE3;
    ctx->state_[5] = 0xBE5E1E2553863992;
    ctx->state_[6] = 0x2B0199FC2C85B8AA;
    ctx->state_[7] = 0x0EB72DDC81C52CA2;
}

HASH_PROC(sha512_s, sha512_proc, sha512_compress)

HASH_DONE(sha512_s, sha512_done, sha512_compress, STORE64H, STORE64H, 0x80, 0x70, 0x78)

SHA2_DONE(sha512_s, sha512_done, sha384_done, 382 >> 3)
SHA2_DONE(sha512_s, sha512_done, sha512_224_done, 224 >> 3)
SHA2_DONE(sha512_s, sha512_done, sha512_256_done, 256 >> 3)
