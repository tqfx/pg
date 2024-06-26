#include "a/buf.h"
#include <stdlib.h>

static A_INLINE void *a_buf_inc_(a_buf *ctx)
{
    return (a_byte *)ctx->ptr_ + ctx->siz_ * ctx->num_++;
}

static A_INLINE void *a_buf_dec_(a_buf *ctx)
{
    return (a_byte *)ctx->ptr_ + ctx->siz_ * --ctx->num_;
}

static void a_buf_drop_(a_buf *ctx, a_size bot, void (*dtor)(void *))
{
    if (dtor)
    {
        while (ctx->num_ > bot) { dtor(a_buf_dec_(ctx)); }
    }
    ctx->num_ = bot;
}

void a_buf_ctor(a_buf *ctx, void *ptr, a_size siz, a_size num)
{
    ctx->ptr_ = ptr;
    ctx->siz_ = siz;
    ctx->mem_ = num;
    ctx->num_ = 0;
}

void a_buf_dtor(a_buf *ctx, void (*dtor)(void *))
{
    if (ctx->ptr_)
    {
        a_buf_drop_(ctx, 0, dtor);
        ctx->ptr_ = A_NULL;
    }
    ctx->siz_ = 0;
    ctx->mem_ = 0;
}

void a_buf_move(a_buf *ctx, a_buf *obj)
{
    a_copy(ctx, obj, sizeof(*obj));
    a_zero(obj, sizeof(*obj));
}

void a_buf_drop(a_buf *ctx, void (*dtor)(void *))
{
    a_buf_drop_(ctx, 0, dtor);
}

void a_buf_swap(a_buf const *ctx, a_size lhs, a_size rhs)
{
    a_size const num = ctx->num_ - 1;
    lhs = lhs < ctx->num_ ? lhs : num;
    rhs = rhs < ctx->num_ ? rhs : num;
    if (lhs != rhs)
    {
        a_swap((a_byte *)ctx->ptr_ + lhs * ctx->siz_,
               (a_byte *)ctx->ptr_ + rhs * ctx->siz_,
               ctx->siz_);
    }
}

void a_buf_sort(a_buf const *ctx, int (*cmp)(void const *, void const *))
{
    qsort(ctx->ptr_, ctx->num_, ctx->siz_, cmp);
}

void a_buf_sort_fore(a_buf const *ctx, int (*cmp)(void const *, void const *))
{
    if (ctx->num_ > 1)
    {
        a_byte *const end = (a_byte *)ctx->ptr_ + ctx->siz_ * ctx->num_;
        a_byte *const top = end - ctx->siz_;
        a_byte *ptr = (a_byte *)ctx->ptr_;
        if (ctx->num_ < ctx->mem_)
        {
            a_byte *const pos = ptr;
            do {
                a_byte *const cur = ptr + ctx->siz_;
                if (cmp(pos, cur) <= 0) { break; }
                ptr = cur;
            } while (ptr != top);
            if (ptr > pos)
            {
                a_copy(end, pos, ctx->siz_);
                a_move(pos, pos + ctx->siz_, (a_size)(ptr - pos));
                a_copy(ptr, end, ctx->siz_);
            }
        }
        else
        {
            do {
                a_byte *const cur = ptr + ctx->siz_;
                if (cmp(ptr, cur) > 0)
                {
                    a_swap(cur, ptr, ctx->siz_);
                }
                else { break; }
                ptr = cur;
            } while (ptr != top);
        }
    }
}

void a_buf_sort_back(a_buf const *ctx, int (*cmp)(void const *, void const *))
{
    if (ctx->num_ > 1)
    {
        a_byte *const end = (a_byte *)ctx->ptr_ + ctx->siz_ * ctx->num_;
        a_byte *ptr = end - ctx->siz_;
        if (ctx->num_ < ctx->mem_)
        {
            a_byte *const pos = ptr;
            do {
                a_byte *const cur = ptr - ctx->siz_;
                if (cmp(cur, pos) <= 0) { break; }
                ptr = cur;
            } while (ptr != ctx->ptr_);
            if (ptr < pos)
            {
                a_copy(end, pos, ctx->siz_);
                a_move(ptr + ctx->siz_, ptr, (a_size)(pos - ptr));
                a_copy(ptr, end, ctx->siz_);
            }
        }
        else
        {
            do {
                a_byte *const cur = ptr - ctx->siz_;
                if (cmp(cur, ptr) > 0)
                {
                    a_swap(cur, ptr, ctx->siz_);
                }
                else { break; }
                ptr = cur;
            } while (ptr != ctx->ptr_);
        }
    }
}

void *a_buf_push_sort(a_buf *ctx, void const *key, int (*cmp)(void const *, void const *))
{
    if (ctx->num_ < ctx->mem_)
    {
        a_byte *ptr = (a_byte *)a_buf_inc_(ctx);
        if (ctx->num_ > 1)
        {
            a_byte *const pos = ptr;
            do {
                a_byte *const cur = ptr - ctx->siz_;
                if (cmp(cur, key) <= 0) { break; }
                ptr = cur;
            } while (ptr != ctx->ptr_);
            if (ptr < pos)
            {
                a_move(ptr + ctx->siz_, ptr, (a_size)(pos - ptr));
            }
        }
        return ptr;
    }
    return A_NULL;
}

void *a_buf_search(a_buf const *ctx, void const *obj, int (*cmp)(void const *, void const *))
{
    return bsearch(obj, ctx->ptr_, ctx->num_, ctx->siz_, cmp);
}

void *a_buf_insert(a_buf *ctx, a_size idx)
{
    if (ctx->num_ < ctx->mem_)
    {
        if (idx < ctx->num_)
        {
            a_byte *const src = (a_byte *)ctx->ptr_ + ctx->siz_ * (idx + 0);
            a_byte *const dst = (a_byte *)ctx->ptr_ + ctx->siz_ * (idx + 1);
            a_move(dst, src, (ctx->num_ - idx) * ctx->siz_);
            ++ctx->num_;
            return src;
        }
        return a_buf_inc_(ctx);
    }
    return A_NULL;
}

void *a_buf_push_fore(a_buf *ctx) { return a_buf_insert(ctx, 0); }

void *a_buf_push_back(a_buf *ctx)
{
    return ctx->num_ < ctx->mem_ ? a_buf_inc_(ctx) : A_NULL;
}

void *a_buf_remove(a_buf *ctx, a_size idx)
{
    a_size const num = ctx->num_ - 1;
    if (ctx->num_ && idx < num)
    {
        a_byte *const ptr = (a_byte *)ctx->ptr_ + ctx->siz_ * num;
        a_byte *const dst = (a_byte *)ctx->ptr_ + ctx->siz_ * (idx + 0);
        a_byte *const src = (a_byte *)ctx->ptr_ + ctx->siz_ * (idx + 1);
        a_swap(dst, src, (a_size)(ptr - dst));
        --ctx->num_;
        return ptr;
    }
    return ctx->num_ ? a_buf_dec_(ctx) : A_NULL;
}

void *a_buf_pull_fore(a_buf *ctx) { return a_buf_remove(ctx, 0); }

void *a_buf_pull_back(a_buf *ctx)
{
    return ctx->num_ ? a_buf_dec_(ctx) : A_NULL;
}
