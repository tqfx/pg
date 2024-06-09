#include "pg/pg.h"
#include <string.h>

void pg_items_ctor(pg_items *ctx)
{
    a_avl_root(&ctx->root);
    ctx->count = 0;
}

void pg_items_dtor(pg_items *ctx)
{
    a_avl_fortear(cur, next, &ctx->root)
    {
        pg_item *const it = pg_items_entry(cur);
        pg_item_dtor(it);
        a_die(it);
    }
    ctx->count = 0;
}

static int pg_item_cmp(void const *_lhs, void const *_rhs)
{
    pg_item const *lhs = pg_items_entry(_lhs);
    pg_item const *rhs = pg_items_entry(_rhs);
    return a_str_cmp(lhs->text, rhs->text);
}

void pg_items_insert(pg_items *ctx, pg_item *item)
{
    a_avl_insert(&ctx->root, &item->node, pg_item_cmp);
    ++ctx->count;
}

void pg_items_remove(pg_items *ctx, pg_item *item)
{
    a_avl_remove(&ctx->root, &item->node);
    --ctx->count;
}

pg_item *pg_items_add(pg_items *ctx, void const *text)
{
    for (a_avl_node *cur = ctx->root.node; cur;)
    {
        pg_item *const it = pg_items_entry(cur);
        int const res = a_str_cmps(it->text, text);
        if (res < 0) { cur = cur->left; }
        else if (res > 0) { cur = cur->right; }
        else { return it; }
    }
    pg_item *it = pg_item_new();
    if (!it) { return it; }
    pg_item_ctor(it);
    it->time = A_I32_MIN;
    pg_item_set_text(it, text);
    pg_items_insert(ctx, it);
    return it;
}

pg_item *pg_items_del(pg_items *ctx, void const *text)
{
    for (a_avl_node *cur = ctx->root.node; cur;)
    {
        pg_item *const it = pg_items_entry(cur);
        int const res = a_str_cmps(it->text, text);
        if (res < 0) { cur = cur->left; }
        else if (res > 0) { cur = cur->right; }
        else
        {
            pg_items_remove(ctx, it);
            return it;
        }
    }
    return A_NULL;
}
