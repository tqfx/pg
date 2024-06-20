#include "sha3.h"
#include "hash.i"

#undef SHA3_KECCAK_SPONGE_WORDS
#define SHA3_KECCAK_SPONGE_WORDS 25 /* 1600 bits -> 200 bytes -> (25 << 3) */

static unsigned int const keccakf_piln[24] = {
    /* clang-format off */
    0x0A, 0x07, 0x0B, 0x11, 0x12, 0x03, 0x05, 0x10,
    0x08, 0x15, 0x18, 0x04, 0x0F, 0x17, 0x13, 0x0D,
    0x0C, 0x02, 0x14, 0x0E, 0x16, 0x09, 0x06, 0x01,
    /* clang-format on */
};
static unsigned int const keccakf_rotc[24] = {
    /* clang-format off */
    0x01, 0x03, 0x06, 0x0A, 0x0F, 0x15, 0x1C, 0x24,
    0x2D, 0x37, 0x02, 0x0E, 0x1B, 0x29, 0x38, 0x08,
    0x19, 0x2B, 0x3E, 0x12, 0x27, 0x3D, 0x14, 0x2C,
    /* clang-format on */
};
static uint64_t const keccakf_rndc[24] = {
    /* clang-format off */
    0x0000000000000001, 0x0000000000008082, 0x800000000000808A, 0x8000000080008000,
    0x000000000000808B, 0x0000000080000001, 0x8000000080008081, 0x8000000000008009,
    0x000000000000008A, 0x0000000000000088, 0x0000000080008009, 0x000000008000000A,
    0x000000008000808B, 0x800000000000008B, 0x8000000000008089, 0x8000000000008003,
    0x8000000000008002, 0x8000000000000080, 0x000000000000800A, 0x800000008000000A,
    0x8000000080008081, 0x8000000000008080, 0x0000000080000001, 0x8000000080008008,
    /* clang-format on */
};

static void keccakf(uint64_t s[SHA3_KECCAK_SPONGE_WORDS])
{
    uint64_t bc[5], t;
    unsigned int i, j;
#undef KECCAK_ROUNDS
#define KECCAK_ROUNDS 24
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#endif /* __GNUC__ || __clang__ */
    for (unsigned int round = 0; round != KECCAK_ROUNDS; ++round)
    {
        /* Theta */
        for (i = 0; i != 5; ++i)
        {
            bc[i] = s[i] ^ s[i + 5] ^ s[i + 10] ^ s[i + 15] ^ s[i + 20];
        }
        for (i = 0; i != 5; ++i)
        {
            t = bc[(i + 4) % 5] ^ ROL64(bc[(i + 1) % 5], 1);
            for (j = 0; j != SHA3_KECCAK_SPONGE_WORDS; j += 5)
            {
                s[j + i] ^= t;
            }
        }
        /* Rho Pi */
        t = s[1];
        for (i = 0; i != KECCAK_ROUNDS; ++i)
        {
            j = keccakf_piln[i];
            bc[0] = s[j];
            s[j] = ROL64(t, keccakf_rotc[i]);
            t = bc[0];
        }
        /* Chi */
        for (j = 0; j != SHA3_KECCAK_SPONGE_WORDS; j += 5)
        {
            for (i = 0; i != 5; ++i)
            {
                bc[i] = s[j + i];
            }
            for (i = 0; i != 5; ++i)
            {
                s[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
            }
        }
        /* Iota */
        s[0] ^= keccakf_rndc[round];
    }
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */
#undef KECCAK_ROUNDS
}

static unsigned char *done(sha3_s *ctx, void *out, uint64_t pad)
{
    ctx->s_[ctx->word_index_] ^= (ctx->saved_ ^ (pad << (ctx->byte_index_ << 3)));
    ctx->s_[SHA3_KECCAK_SPONGE_WORDS - 1 - ctx->capacity_words_] ^= 0x8000000000000000;
    keccakf(ctx->s_);

    /* store ctx->s_[] as little-endian bytes into ctx->out */
    for (unsigned int i = 0; i != SHA3_KECCAK_SPONGE_WORDS; ++i)
    {
        STORE64L(ctx->s_[i], ctx->out + sizeof(*ctx->s_) * i);
    }

    if (out && (out != ctx->out))
    {
        memcpy(out, ctx->out, (unsigned int)ctx->capacity_words_ << 2);
    }

    return ctx->out;
}

/* ((2 * x) / (8 * 8)) -> ((x << 1) / (8 << 3)) -> (x >> 5) */

void sha3_224_init(sha3_s *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->capacity_words_ = 224 >> 5;
}

void sha3_256_init(sha3_s *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->capacity_words_ = 256 >> 5;
}

void sha3_384_init(sha3_s *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->capacity_words_ = 384 >> 5;
}

void sha3_512_init(sha3_s *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->capacity_words_ = 512 >> 5;
}

int sha3_proc(sha3_s *ctx, void const *pdata, size_t nbyte)
{
    if (nbyte == 0) { return SUCCESS; } /* nothing to do */

    /* 0...7 -- how much is needed to have a word */
    unsigned int old_tail = (8 - ctx->byte_index_) & 7;
    unsigned char const *p = (unsigned char const *)pdata;

    if (nbyte < old_tail)
    {
        /* have no complete word or haven't started the word yet */
        while (nbyte--)
        {
            ctx->saved_ |= (uint64_t)(*(p++)) << ((ctx->byte_index_++) << 3);
        }
        return SUCCESS;
    }

    if (old_tail)
    {
        /* will have one word to process */
        nbyte -= old_tail;
        while (old_tail--)
        {
            ctx->saved_ |= (uint64_t)(*(p++)) << ((ctx->byte_index_++) << 3);
        }
        /* now ready to add saved to the sponge */
        ctx->s_[ctx->word_index_] ^= ctx->saved_;
        ctx->byte_index_ = 0;
        ctx->saved_ = 0;
        if (++ctx->word_index_ == (SHA3_KECCAK_SPONGE_WORDS - ctx->capacity_words_))
        {
            keccakf(ctx->s_);
            ctx->word_index_ = 0;
        }
    }

    /* now work in full words directly from input */
    size_t words = nbyte >> 3;
    for (size_t i = 0; i != words; ++i, p += 8)
    {
        uint64_t t;
        LOAD64L(t, p);
        ctx->s_[ctx->word_index_] ^= t;
        if (++ctx->word_index_ == (SHA3_KECCAK_SPONGE_WORDS - ctx->capacity_words_))
        {
            keccakf(ctx->s_);
            ctx->word_index_ = 0;
        }
    }

    /* finally, save the partial word */
    unsigned int tail = (unsigned int)(nbyte % 8);
    while (tail--)
    {
        ctx->saved_ |= (uint64_t)(*(p++)) << ((ctx->byte_index_++) << 3);
    }

    return SUCCESS;
}

unsigned char *sha3_done(sha3_s *ctx, void *out)
{
    return done(ctx, out, 0x06);
}

unsigned char *keccak_done(sha3_s *ctx, void *out)
{
    return done(ctx, out, 0x01);
}

void shake128_init(sha3_s *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->capacity_words_ = 128 >> 5;
}

void shake256_init(sha3_s *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->capacity_words_ = 256 >> 5;
}

int sha3shake_init(sha3_s *ctx, unsigned int num)
{
    if (num != 0x80 && num != 0x100) { return INVALID; }

    memset(ctx, 0, sizeof(*ctx));
    ctx->capacity_words_ = (unsigned short)(num >> 5);

    return SUCCESS;
}

unsigned char *shake128_done(sha3_s *ctx, void *out)
{
    sha3shake_done(ctx, ctx->out, 128 >> 3);
    if (out && (out != ctx->out))
    {
        memcpy(out, ctx->out, 128 >> 3);
    }
    return ctx->out;
}

unsigned char *shake256_done(sha3_s *ctx, void *out)
{
    sha3shake_done(ctx, ctx->out, 256 >> 3);
    if (out && (out != ctx->out))
    {
        memcpy(out, ctx->out, 256 >> 3);
    }
    return ctx->out;
}

void sha3shake_done(sha3_s *ctx, unsigned char *out, unsigned int siz)
{
    /* IMPORTANT NOTE: sha3shake_done can be called many times */

    if (siz == 0) { return; } /* nothing to do */

    if (!ctx->xof_flag_)
    {
        /* shake_xof operation must be done only once */
        ctx->s_[ctx->word_index_] ^= (ctx->saved_ ^ (0x1FU << (ctx->byte_index_ << 3)));
        ctx->s_[SHA3_KECCAK_SPONGE_WORDS - ctx->capacity_words_ - 1] ^= 0x8000000000000000;
        keccakf(ctx->s_);
        /* store ctx->s_[] as little-endian bytes into ctx->out */
        for (unsigned int i = 0; i != SHA3_KECCAK_SPONGE_WORDS; ++i)
        {
            STORE64L(ctx->s_[i], ctx->out + sizeof(*ctx->s_) * i);
        }
        ctx->byte_index_ = 0;
        ctx->xof_flag_ = 1;
    }

    for (unsigned int idx = 0; idx != siz; ++idx)
    {
        if (ctx->byte_index_ >= (SHA3_KECCAK_SPONGE_WORDS - ctx->capacity_words_) << 3)
        {
            keccakf(ctx->s_);
            /* store ctx->s_[] as little-endian bytes into ctx->out */
            for (unsigned int i = 0; i != SHA3_KECCAK_SPONGE_WORDS; ++i)
            {
                STORE64L(ctx->s_[i], ctx->out + sizeof(*ctx->s_) * i);
            }
            ctx->byte_index_ = 0;
        }
        out[idx] = ctx->out[ctx->byte_index_++];
    }
}
