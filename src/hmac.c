#include "hmac.h"
#include "hash.i"

#undef HMAC_IPAD
#undef HMAC_OPAD
#define HMAC_IPAD 0x36
#define HMAC_OPAD 0x5C

int hmac_init(hmac_s *ctx, hash_s const *hash, void const *pdata, size_t nbyte)
{
    unsigned char buf[sizeof(ctx->buf)];

    if (sizeof(ctx->buf) < hash->bufsiz) { return OVERFLOW; }

    ctx->_hash = hash;
    ctx->outsiz = ctx->_hash->outsiz;

    if (ctx->_hash->bufsiz < nbyte)
    {
        ctx->_hash->init(ctx->_state);
        if (ctx->_hash->proc(ctx->_state, pdata, nbyte) != SUCCESS) { return FAILURE; }
        if (ctx->_hash->done(ctx->_state, ctx->buf) == 0) { return FAILURE; }
        nbyte = ctx->_hash->outsiz;
    }
    else if (nbyte) { memcpy(ctx->buf, pdata, nbyte); }

    if (nbyte < ctx->_hash->bufsiz) { memset(ctx->buf + nbyte, 0, sizeof(ctx->buf) - nbyte); }

    for (unsigned int i = 0; i != ctx->_hash->bufsiz; ++i) { buf[i] = ctx->buf[i] ^ HMAC_IPAD; }

    ctx->_hash->init(ctx->_state);
    return ctx->_hash->proc(ctx->_state, buf, ctx->_hash->bufsiz);
}

int hmac_proc(hmac_s *ctx, void const *pdata, size_t nbyte)
{
    return ctx->_hash->proc(ctx->_state, pdata, nbyte);
}

unsigned char *hmac_done(hmac_s *ctx, void *out)
{
    unsigned char buf[sizeof(ctx->buf)];

    if (ctx->_hash->done(ctx->_state, buf) == 0) { return 0; }

    for (unsigned int i = 0; i != ctx->_hash->bufsiz; ++i) { ctx->buf[i] ^= HMAC_OPAD; }

    ctx->_hash->init(ctx->_state);
    if (ctx->_hash->proc(ctx->_state, ctx->buf, ctx->_hash->bufsiz) != SUCCESS) { return 0; }
    if (ctx->_hash->proc(ctx->_state, buf, ctx->_hash->outsiz) != SUCCESS) { return 0; }
    if (ctx->_hash->done(ctx->_state, ctx->buf) == 0) { return 0; }

    if (out && out != ctx->buf) { memcpy(out, ctx->buf, ctx->_hash->outsiz); }

    return ctx->buf;
}

#undef HMAC_IPAD
#undef HMAC_OPAD

#include <stdarg.h>

int hmac_memory(hash_s const *hash, void const *pkey, size_t nkey, void const *pmsg, size_t nmsg, void *out, size_t *siz)
{
    if (*siz < hash->outsiz)
    {
        *siz = hash->outsiz;
        return OVERFLOW;
    }

    hmac_s hmac;

    if (hmac_init(&hmac, hash, pkey, nkey) != SUCCESS) { return FAILURE; }
    if (hmac_proc(&hmac, pmsg, nmsg) != SUCCESS) { return FAILURE; }
    *siz = hmac_done(&hmac, out) ? hash->outsiz : 0;

    return SUCCESS;
}

int hmac_mmulti(hash_s const *hash, void *out, size_t *siz, void const *pkey, size_t nkey, void const *pmsg, size_t nmsg, ...)
{
    if (*siz < hash->outsiz)
    {
        *siz = hash->outsiz;
        return OVERFLOW;
    }

    hmac_s hmac;
    va_list arg;
    int ret = FAILURE;
    va_start(arg, nmsg);

    if (hmac_init(&hmac, hash, pkey, nkey) != SUCCESS) { goto done; }
    for (;;)
    {
        ret = hmac_proc(&hmac, pmsg, nmsg);
        if (ret != SUCCESS) { goto done; }
        pmsg = va_arg(arg, void const *);
        if (pmsg == 0) { break; }
        nmsg = va_arg(arg, size_t);
    }
    *siz = hmac_done(&hmac, out) ? hash->outsiz : 0;

done:
    va_end(arg);
    return ret;
}

int hmac_filehandle(hash_s const *hash, void const *pkey, size_t nkey, FILE *in, void *out, size_t *siz)
{
    if (*siz < hash->outsiz)
    {
        *siz = hash->outsiz;
        return OVERFLOW;
    }

    hmac_s hmac;
    char buf[BUFSIZ];

    if (hmac_init(&hmac, hash, pkey, nkey) != SUCCESS) { return FAILURE; }
    do
    {
        *siz = fread(buf, 1, BUFSIZ, in);
        hmac_proc(&hmac, buf, *siz);
    } while (*siz == BUFSIZ);
    *siz = hmac_done(&hmac, out) ? hash->outsiz : 0;

    return SUCCESS;
}

int hmac_file(hash_s const *hash, void const *pkey, size_t nkey, char const *fname, void *out, size_t *siz)
{
    if (*siz < hash->outsiz)
    {
        *siz = hash->outsiz;
        return OVERFLOW;
    }

    FILE *in = fopen(fname, "rb");
    if (in == 0) { return NOTFOUND; }

    int ret = hmac_filehandle(hash, pkey, nkey, in, out, siz);

    if (fclose(in)) { return FAILURE; }

    return ret;
}
