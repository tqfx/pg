#include "pg/pg.h"
#include <stdlib.h>
#include <ctype.h>
#include "hmac.h"

static hash_s const *tohash(char const *text)
{
    switch (text ? *text : 0)
    {
    case 'm':
        if (strcmp(text, "md5") == 0) { return &hash_md5; }
        break;
    case 'M':
        if (strcmp(text, "MD5") == 0) { return &hash_md5; }
        break;
    case 's':
        if (strcmp(text, "sha1") == 0) { return &hash_sha1; }
        if (strcmp(text, "sha256") == 0) { return &hash_sha256; }
        if (strcmp(text, "sha224") == 0) { return &hash_sha224; }
        if (strcmp(text, "sha512") == 0) { return &hash_sha512; }
        if (strcmp(text, "sha384") == 0) { return &hash_sha384; }
        if (strcmp(text, "sha3") == 0) { return &hash_sha3_512; }
        break;
    case 'S':
        if (strcmp(text, "SHA1") == 0) { return &hash_sha1; }
        if (strcmp(text, "SHA256") == 0) { return &hash_sha256; }
        if (strcmp(text, "SHA224") == 0) { return &hash_sha224; }
        if (strcmp(text, "SHA512") == 0) { return &hash_sha512; }
        if (strcmp(text, "SHA384") == 0) { return &hash_sha384; }
        if (strcmp(text, "SHA3") == 0) { return &hash_sha3_512; }
        break;
    case 'b':
        if (strcmp(text, "blake2s") == 0) { return &hash_blake2s_256; }
        if (strcmp(text, "blake2b") == 0) { return &hash_blake2b_512; }
        break;
    case 'B':
        if (strcmp(text, "BLAKE2S") == 0) { return &hash_blake2s_256; }
        if (strcmp(text, "BLAKE2B") == 0) { return &hash_blake2b_512; }
        break;
    default:
        break;
    }
    return &hash_md5;
}

static char *hmac(void const *key, size_t keysiz, void const *msg, size_t msgsiz, hash_s const *hash, void *out)
{
    hmac_s ctx;
    hmac_init(&ctx, hash, key, keysiz);
    hmac_proc(&ctx, msg, msgsiz);
    hmac_done(&ctx, ctx.buf);
    return (char *)pg_digest_lower(ctx.buf, ctx.outsiz, out);
}

#undef TO
#define TO(func, to)                                         \
    void *func(void const *pdata, size_t nbyte, void *out)   \
    {                                                        \
        char const *p = (char const *)pdata;                 \
        if (out || ((void)(out = malloc(nbyte + 1)), out))   \
        {                                                    \
            char *o = (char *)out;                           \
            pdata = (void const *)(p + nbyte);               \
            for (; p != pdata; ++p) { *o++ = (char)to(*p); } \
            *o = 0;                                          \
        }                                                    \
        return out;                                          \
    }
TO(pg_lower, tolower)
TO(pg_upper, toupper)
#undef TO

void *pg_digest(void const *pdata, size_t nbyte, unsigned int cases, void *out)
{
    static char const hexits[2][0x10] = {
        /* clang-format off */
        {
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f',
        },
        {
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
        },
        /* clang-format on */
    };

    char const *hexit = hexits[cases % 2];
    unsigned char const *p = (unsigned char const *)pdata;
    if (out || ((void)(out = malloc((nbyte << 1) + 1)), out))
    {
        char *o = (char *)out;
        pdata = (void const *)(p + nbyte);
        while (p != pdata)
        {
            *o++ = hexit[*p >> 0x4];
            *o++ = hexit[*p & 0x0F];
            ++p;
        }
        *o = 0;
    }

    return out;
}

void *pg_digest_lower(void const *pdata, size_t nbyte, void *out)
{
    return pg_digest(pdata, nbyte, 0, out);
}

void *pg_digest_upper(void const *pdata, size_t nbyte, void *out)
{
    return pg_digest(pdata, nbyte, 1, out);
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* __GNUC__ || __clang__ */

#undef N
#define N 61

static struct
{
    /* points to rule data */
    char const *r0;
    char const *r1;
    char const *r2;
    char const *r3;
    /* length for rule data */
    unsigned int l0;
    unsigned int l1;
    unsigned int l2;
    unsigned int l3;
    /* character table */
    const char ch[N];
} stat = {
    .r0 = 0,
    .r1 = 0,
    .r2 = 0,
    .r3 = 0,
    .l0 = 0,
    .l1 = 0,
    .l2 = 0,
    .l3 = 0,
    .ch = {
        /* clang-format off */
        'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', 'g', 'G', 'h',
        'H', 'i', 'j', 'J', 'k', 'K', 'l', 'L', 'm', 'M', '0', '1', '2', '3', '4', 'I',
        '5', '6', '7', '8', '9', 'n', 'N', 'o', 'p', 'P', 'q', 'Q', 'r', 'R', 's',
        'S', 't', 'T', 'u', 'U', 'v', 'V', 'w', 'W', 'x', 'X', 'y', 'Y', 'z', 'Z',
        /* clang-format on */
    },
};

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */

int pg_init(char *s, char const *sep)
{
    int n = 0;
    if (!s) { return n; }
    /* Set the rules */
    for (stat.r0 = s, stat.l0 = 0; *s; ++s, ++stat.l0)
    {
        if (strchr(sep, *s))
        {
            *s++ = 0;
            break;
        }
    }
    if (stat.l0) { ++n; }
    for (stat.r1 = s, stat.l1 = 0; *s; ++s, ++stat.l1)
    {
        if (strchr(sep, *s))
        {
            *s++ = 0;
            break;
        }
    }
    if (stat.l1) { ++n; }
    for (stat.r2 = s, stat.l2 = 0; *s; ++s, ++stat.l2)
    {
        if (strchr(sep, *s))
        {
            *s++ = 0;
            break;
        }
    }
    if (stat.l2) { ++n; }
    for (stat.r3 = s, stat.l3 = 0; *s; ++s, ++stat.l3)
    {
        if (strchr(sep, *s))
        {
            *s++ = 0;
            break;
        }
    }
    if (stat.l3) { ++n; }
    return n;
}

int pg_gen1(pg_view const *ctx, char const *code, char **out)
{
    hash_s const *hash = tohash(ctx->hash);
    if (ctx->text == 0 || code == 0) { return -3; }
    unsigned int lcode = (unsigned int)strlen(code);
    unsigned int ltext = (unsigned int)strlen(ctx->text);
    if (ctx->misc == 0 && ctx->type == PG_TYPE_OTHER) { return -2; }
    if ((ctx->size == 0) || (lcode == 0) || (ltext == 0)) { return -1; }

    unsigned char count = 0;
    unsigned char num[10] = {0};
    unsigned int outsiz = hash->outsiz << 1;
    unsigned int length = ctx->size < outsiz ? ctx->size : outsiz;
    unsigned char *msg = (unsigned char *)hmac(ctx->text, ltext, code, lcode, hash, 0);

    char const *kise = stat.l0 ? stat.r0 : "kise";
    unsigned int kise_n = stat.l0 ? stat.l0 : 4;
    char const *snow = stat.l1 ? stat.r1 : "snow";
    unsigned int snow_n = stat.l1 ? stat.l1 : 4;

    char *buf0 = hmac(kise, kise_n, msg, outsiz, hash, 0);
    char *buf1 = hmac(snow, snow_n, msg, outsiz, hash, 0);

    *out = (char *)calloc(length + 1, sizeof(char));
    for (unsigned int i = 0; i != length; ++i)
    {
        int x = pg_xdigit(buf0[i]) + pg_xdigit(buf1[i]);
        msg[i] = (unsigned char)x;

        switch (ctx->type)
        {
        case PG_TYPE_EMAIL:
        case PG_TYPE_OTHER:
        {
            (*out)[i] = buf1[i];
            if (!isdigit((int)(*out)[i]))
            {
                if (strchr("sunlovesnow1990090127xykab", buf0[i]))
                {
                    (*out)[i] = (char)toupper((*out)[i]);
                }
            }
            break;
        }

        case PG_TYPE_DIGIT:
        {
            x %= 10;

            int m = x;
            while (num[x] > count)
            {
                if (++x == 10)
                {
                    x = 0;
                }
                if (x == m)
                {
                    ++count;
                }
            }
            ++num[x];

            (*out)[i] = (char)('0' + x);
            break;
        }

        default:
            break;
        }
    }

    if (ctx->type != PG_TYPE_DIGIT)
    {
        if (isdigit((int)(*out)[0])) { (*out)[0] = 'K'; }
        if (outsiz && ctx->type == PG_TYPE_OTHER)
        {
            unsigned int lmisc = (unsigned int)strlen(ctx->misc);
            for (unsigned int i = 0; i != lmisc; ++i)
            {
                (*out)[msg[i % outsiz] % length] = ctx->misc[i];
            }
        }
    }

    free(buf1);
    free(buf0);
    free(msg);

    return 0;
}

int pg_gen2(pg_view const *ctx, char const *code, char **out)
{
    hash_s const *hash = tohash(ctx->hash);
    if (ctx->text == 0 || code == 0) { return -3; }
    unsigned int lword = (unsigned int)strlen(code);
    unsigned int ltext = (unsigned int)strlen(ctx->text);
    if (ctx->misc == 0 && ctx->type == PG_TYPE_OTHER) { return -2; }
    if ((ctx->size == 0) || (lword == 0) || (ltext == 0)) { return -1; }

    unsigned char count = 0;
    unsigned char num[N] = {0};
    unsigned int outsiz = hash->outsiz << 1;
    unsigned int length = ctx->size < outsiz ? ctx->size : outsiz;
    unsigned char *msg = (unsigned char *)hmac(ctx->text, ltext, code, lword, hash, 0);

    char *buf0 = hmac(stat.r0, stat.l0, msg, outsiz, hash, 0);
    char *buf1 = hmac(stat.r1, stat.l1, msg, outsiz, hash, 0);
    char *buf2 = hmac(stat.r2, stat.l2, msg, outsiz, hash, 0);
    char *buf3 = hmac(stat.r3, stat.l3, msg, outsiz, hash, 0);

    *out = (char *)calloc(length + 1, sizeof(char));
    for (unsigned int i = 0; i != length; ++i)
    {
        int x = pg_xdigit(buf0[i]) + pg_xdigit(buf1[i]) + pg_xdigit(buf2[i]) + pg_xdigit(buf3[i]);
        msg[i] = (unsigned char)x;

        switch (ctx->type)
        {
        case PG_TYPE_EMAIL:
        case PG_TYPE_OTHER:
        {
            int m = x;
            while (num[x] > count)
            {
                if ((m % 2 == 0) && (N == ++x)) { x = 0; }
                if ((m % 2 == 1) && (0 == x--)) { x = N - 1; }
                if (x == m) { ++count; }
            }
            ++num[x];

            (*out)[i] = stat.ch[x];
            break;
        }
#undef N
#define N 10
        case PG_TYPE_DIGIT:
        {
            x %= N;

            int m = x;
            while (num[x] > count)
            {
                if ((m % 2 == 0) && (N == ++x)) { x = 0; }
                if ((m % 2 == 1) && (0 == x--)) { x = N - 1; }
                if (x == m) { ++count; }
            }
            ++num[x];

            (*out)[i] = (char)('0' + x);
            break;
        }
#undef N
        default:
            break;
        }
    }

    if (outsiz && ctx->type == PG_TYPE_OTHER)
    {
        unsigned int lmisc = (unsigned int)strlen(ctx->misc);
        for (unsigned int i = 0; i != lmisc; ++i)
        {
            (*out)[msg[i % outsiz] % length] = ctx->misc[i];
        }
    }

    free(buf3);
    free(buf2);
    free(buf1);
    free(buf0);
    free(msg);

    return 0;
}

int pg_xdigit(int x)
{
    int ret = ~0;
    switch (x)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    {
        /* 0 ~ 9 */
        ret = x - '0';
    }
    break;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    {
        /* A ~ F */
        ret = x - 'A' + 10;
    }
    break;
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    {
        /* a ~ f */
        ret = x - 'a' + 10;
    }
    break;
    default:
        break;
    }
    return ret;
}

pg_item *pg_item_new(void)
{
    pg_item *ctx = (pg_item *)malloc(sizeof(pg_item));
    if (ctx) { pg_item_ctor(ctx); }
    return ctx;
}

void pg_item_die(pg_item *ctx)
{
    if (ctx)
    {
        pg_item_dtor(ctx);
        free(ctx);
    }
}

void pg_item_ctor(pg_item *ctx)
{
    ctx->text = a_str_new();
    ctx->hash = a_str_new();
    ctx->hint = a_str_new();
    ctx->misc = a_str_new();
    ctx->type = PG_TYPE_EMAIL;
    ctx->size = 16;
}

void pg_item_dtor(pg_item *ctx)
{
    a_str_die(ctx->text);
    a_str_die(ctx->hash);
    a_str_die(ctx->hint);
    a_str_die(ctx->misc);
    ctx->text = 0;
    ctx->hash = 0;
    ctx->hint = 0;
    ctx->misc = 0;
    ctx->type = PG_TYPE_EMAIL;
    ctx->size = 16;
}

void pg_view_ctor(pg_view *ctx)
{
    ctx->text = 0;
    ctx->hash = "MD5";
    ctx->hint = 0;
    ctx->misc = 0;
    ctx->type = PG_TYPE_EMAIL;
    ctx->size = 16;
}

void pg_item_view(pg_item const *ctx, pg_view *out)
{
    out->text = a_str_ptr(ctx->text);
    out->hash = a_str_ptr(ctx->hash);
    out->hint = a_str_ptr(ctx->hint);
    out->misc = a_str_ptr(ctx->misc);
    out->type = ctx->type;
    out->size = ctx->size;
}

void pg_item_set_type(pg_item *ctx, unsigned int type)
{
    ctx->type = type % PG_TYPE_TOTAL;
}

void pg_item_set_size(pg_item *ctx, unsigned int size)
{
    hash_s const *hash = tohash(a_str_ptr(ctx->hash));
    unsigned int const outsiz = hash->outsiz << 1;
    ctx->size = size < outsiz ? size : outsiz;
}

int pg_item_set_text(pg_item *ctx, void const *text)
{
    return a_str_sets(ctx->text, text ? text : "");
}
int pg_item_set_hash(pg_item *ctx, void const *hash)
{
    return a_str_sets(ctx->hash, hash ? hash : "MD5");
}
int pg_item_set_hint(pg_item *ctx, void const *hint)
{
    return a_str_sets(ctx->hint, hint ? hint : "");
}
int pg_item_set_misc(pg_item *ctx, void const *misc)
{
    return a_str_sets(ctx->misc, misc ? misc : "");
}
int pg_item_set_text2(pg_item *ctx, a_str const *text)
{
    return a_str_set(ctx->text, text);
}
int pg_item_set_hash2(pg_item *ctx, a_str const *hash)
{
    return a_str_set(ctx->hash, hash);
}
int pg_item_set_hint2(pg_item *ctx, a_str const *hint)
{
    return a_str_set(ctx->hint, hint);
}
int pg_item_set_misc2(pg_item *ctx, a_str const *misc)
{
    return a_str_set(ctx->misc, misc);
}
