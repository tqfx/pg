#include "app.h"
#include <time.h>
#if defined(_WIN32)
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 5105)
#endif /* _MSC_VER */
#include <windows.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif /* _MSC_VER */
#endif /* _WIN32 */

// clang-format off
#define TEXT_RED       CONSOLE_TEXT_RED
#define TEXT_GREEN     CONSOLE_TEXT_GREEN
#define TEXT_TURQUOISE CONSOLE_TEXT_TURQUOISE
#define TEXT_WHITE     CONSOLE_TEXT_WHITE
#define TEXT_DEFAULT   CONSOLE_TEXT_DEFAULT
// clang-format on

static void app_log3(char const *obj, unsigned int color, char const *status, char const *text)
{
    app_log(3, TEXT_WHITE, obj, color, status, TEXT_TURQUOISE, text);
}

static void app_item(size_t index, pg_item const *item)
{
    pg_view view;
    pg_item_view(item, &view);
    char buf_i[1 << 4];
    snprintf(buf_i, 1 << 4, "%05zu", index);
    char buf_t[1 << 2];
    snprintf(buf_t, 1 << 2, "%u", view.type);
    char buf_z[1 << 2];
    snprintf(buf_z, 1 << 2, "%3u", view.size);
    char buf_h[1 << 3];
    snprintf(buf_h, 1 << 3, "%-7s", view.hash);
    char *text = a_str_ptr(item->text);
#if defined(_WIN32)
    code_utf8_to(&text, text);
#endif /* _WIN32 */
    app_log(7, TEXT_TURQUOISE, buf_i, TEXT_GREEN, buf_t, TEXT_GREEN, buf_z, TEXT_GREEN, buf_h,
            TEXT_DEFAULT, text, TEXT_GREEN, view.misc, TEXT_DEFAULT, view.hint);
#if defined(_WIN32)
    free(text);
#endif /* _WIN32 */
}

void app_log(unsigned int num, ...)
{
    va_list ap;
    va_start(ap, num);
    console_vprint(" ", num, ap);
    printf("\n");
    va_end(ap);
}

#define STATUS_ZERO 0
#define STATUS_INIT (1 << 0)
#define STATUS_DONE (1 << 1)
#define STATUS_DUMP (1 << 2)
#define STATUS_ISV2 (1 << 3)

#pragma pack(push, 4)
static struct
{
    sqlite3 *db;
    char const *code;
    char const *fname;
    pg_tree tree;
    int status;
} local = {
    .db = 0,
    .code = 0,
    .fname = 0,
    .status = STATUS_ZERO,
};
#pragma pack(pop)

#define STATUS_SET(mask) (local.status |= (mask))
#define STATUS_CLR(mask) (local.status &= ~(mask))
#define STATUS_IS1(mask) ((local.status & (mask)) == (mask))
#define STATUS_IS0(mask) ((local.status & (mask)) != (mask))

int app_gen(pg_item const *item, char const *code)
{
    pg_view view;
    pg_item_view(item, &view);

    if (code == 0 || strlen(code) == 0)
    {
        app_log(2, TEXT_RED, s_missing, TEXT_TURQUOISE, "-p");
        return A_FAILURE;
    }
    if (a_str_len(item->text) == 0)
    {
        app_log(2, TEXT_RED, s_missing, TEXT_TURQUOISE, "-g");
        return A_FAILURE;
    }
    if (a_str_len(item->misc) == 0 && item->type == PG_TYPE_OTHER)
    {
        app_log(2, TEXT_RED, s_missing, TEXT_TURQUOISE, "-m");
        return A_FAILURE;
    }

    char *out = 0;
    int (*gen)(pg_view const *, char const *, char **) = pg_v1;
    if (STATUS_IS1(STATUS_ISV2)) { gen = pg_v2; }
    if (gen(&view, code, &out))
    {
        app_log(2, TEXT_RED, s_failure, TEXT_TURQUOISE, view.text);
        return A_FAILURE;
    }

#if defined(_WIN32)
    char *text = 0;
    code_utf8_to(&text, view.text);
    app_log(2, TEXT_TURQUOISE, out, TEXT_DEFAULT, text);
    free(text);
    if (OpenClipboard(0))
    {
        a_size out_n = strlen(out);
        HGLOBAL hmem = GlobalAlloc(GMEM_MOVEABLE, out_n + 1);
        if (hmem)
        {
            LPTSTR lpdest = (LPTSTR)GlobalLock(hmem);
            memcpy(lpdest, out, out_n);
            GlobalUnlock(hmem);
            EmptyClipboard();
            SetClipboardData(sizeof(TCHAR) == sizeof(WCHAR) ? CF_UNICODETEXT : CF_TEXT, hmem);
        }
        CloseClipboard();
    }
#else /* !_WIN32 */
    app_log(2, TEXT_TURQUOISE, out, TEXT_DEFAULT, view.text);
#endif /* _WIN32 */

    free(out);

    return A_SUCCESS;
}

int app_init(char const *fname, a_str const *code, a_str const *rule, int flag)
{
    if (STATUS_IS1(STATUS_INIT))
    {
        return A_FAILURE;
    }

    sqlite3_initialize();
    STATUS_CLR(STATUS_DONE);

    int ok = sqlite3_open(fname, &local.db);
    if (ok != SQLITE_OK)
    {
        fprintf(stderr, "%s\n", sqlite3_errmsg(local.db));
        exit(EXIT_FAILURE);
    }

    local.fname = fname;
    pg_sqlite_init(local.db);
    STATUS_SET(STATUS_INIT);

    pg_tree_ctor(&local.tree);

    pg_sqlite_out(local.db, &local.tree);

    if (a_str_len(code))
    {
        local.code = a_str_ptr(code);
    }

    if (a_str_len(rule) && pg_init(a_str_ptr(rule), ",") > 2)
    {
        STATUS_SET(STATUS_ISV2);
    }
    if (flag & (1 << 8)) { STATUS_CLR(STATUS_ISV2); }
    if (flag & (1 << 9)) { STATUS_SET(STATUS_ISV2); }

    return ok;
}

int app_exit(void)
{
    if (STATUS_IS0(STATUS_INIT))
    {
        return A_FAILURE;
    }

    if (STATUS_IS1(STATUS_DUMP))
    {
        pg_sqlite_delete(local.db);
        pg_sqlite_create(local.db);
        pg_sqlite_add(local.db, &local.tree);
        STATUS_CLR(STATUS_DUMP);
    }

    pg_sqlite_exit(local.db);
    sqlite3_close(local.db);
    STATUS_CLR(STATUS_INIT);

    pg_tree_dtor(&local.tree);
    STATUS_SET(STATUS_DONE);

    return sqlite3_shutdown();
}

int app_create(a_vec const *item)
{
    int ok = A_FAILURE;
    a_vec_foreach(pg_item, *, it, item)
    {
        if (a_str_len(it->misc) == 0 && it->type == PG_TYPE_OTHER)
        {
            app_log3(local.fname, TEXT_RED, s_missing, "-m");
            break;
        }
        if (a_str_len(it->text) == 0)
        {
            app_log3(local.fname, TEXT_RED, s_missing, "-g");
            break;
        }
        char const *text = a_str_ptr(it->text);
        pg_item *ctx = pg_tree_add(&local.tree, text);
        if (ctx)
        {
            STATUS_SET(STATUS_DUMP);
            ctx->time = time(NULL) + A_I32_MIN;
            pg_item_set_hash2(ctx, it->hash);
            pg_item_set_hint2(ctx, it->hint);
            pg_item_set_misc2(ctx, it->misc);
            pg_item_set_type(ctx, it->type);
            pg_item_set_size(ctx, it->size);
            ok = app_gen(ctx, local.code);
        }
        else
        {
            app_log3(local.fname, TEXT_RED, s_failure, text);
        }
    }
    return ok;
}

void app_search(a_vec const *item)
{
    size_t idx = 0;
    pg_tree_foreach(cur, &local.tree)
    {
        pg_item *it = pg_tree_entry(cur);
        int matched = (int)a_vec_num(item);
        char const *text = a_str_ptr(it->text);
        a_vec_foreach(pg_item, *, at, item)
        {
            if (a_str_len(at->text) && !strstr(text, a_str_ptr(at->text))) { matched = 0; }
        }
        if (matched) { app_item(idx, it); }
        ++idx;
    }
}

void app_search_n(a_vec const *item)
{
    a_vec number;
    a_vec_ctor(&number, sizeof(unsigned int));
    a_vec_foreach(pg_item, *, it, item)
    {
        if (a_str_len(it->text))
        {
            char *p = 0;
            char const *s = a_str_ptr(it->text);
            unsigned long x = strtoul(s, &p, 0);
            if (s == p)
            {
                app_log3(local.fname, TEXT_RED, s_invalid, s);
                continue;
            }
            *A_VEC_PUSH(unsigned int, &number) = (unsigned int)x;
        }
    }

    a_size index = 0;
    pg_tree_foreach(cur, &local.tree)
    {
        pg_item *it = pg_tree_entry(cur);
        a_vec_foreach(unsigned int, *, n, &number)
        {
            if (*n == index)
            {
                app_item(index, it);
                break;
            }
        }
        ++index;
    }

    a_vec_dtor(&number, 0);
}

int app_delete(a_vec const *item)
{
    int ok = A_FAILURE;
    a_vec_foreach(pg_item, *, it, item)
    {
        char const *text = "";
        if (a_str_len(it->text))
        {
            text = a_str_ptr(it->text);
        }
        pg_item *ctx = pg_tree_del(&local.tree, text);
        if (ctx)
        {
            STATUS_SET(STATUS_DUMP);
            app_log3(local.fname, TEXT_GREEN, s_success, text);
            pg_item_die(ctx);
            ok = A_SUCCESS;
        }
        else
        {
            app_log3(local.fname, TEXT_RED, s_failure, text);
            ok = A_FAILURE;
        }
    }
    return ok;
}

int app_delete_n(a_vec const *item)
{
    struct pg_deleted
    {
        a_size index;
        pg_item *item;
    };
    int ok = A_FAILURE;
    a_vec number, deleted;
    a_vec_ctor(&number, sizeof(unsigned int));
    a_vec_ctor(&deleted, sizeof(struct pg_deleted));

    a_vec_foreach(pg_item, *, it, item)
    {
        if (a_str_len(it->text))
        {
            char *p = 0;
            char const *s = a_str_ptr(it->text);
            unsigned long x = strtoul(s, &p, 0);
            if (s == p)
            {
                app_log3(local.fname, TEXT_RED, s_invalid, s);
                continue;
            }
            if (x >= local.tree.count)
            {
                app_log3(local.fname, TEXT_RED, s_overrun, s);
                continue;
            }
            *A_VEC_PUSH(unsigned int, &number) = (unsigned int)x;
        }
    }

    a_size index = 0;
    pg_tree_foreach(cur, &local.tree)
    {
        pg_item *it = pg_tree_entry(cur);
        a_vec_foreach(unsigned int, *, n, &number)
        {
            if (*n == index)
            {
                struct pg_deleted *p = A_VEC_PUSH(struct pg_deleted, &deleted);
                p->index = index;
                p->item = it;
                break;
            }
        }
        ++index;
    }

    a_vec_foreach(struct pg_deleted, *, it, &deleted)
    {
        STATUS_SET(STATUS_DUMP);
        pg_tree_remove(&local.tree, it->item);
        app_item(it->index, it->item);
        pg_item_die(it->item);
        ok = A_SUCCESS;
    }

    a_vec_dtor(&deleted, 0);
    a_vec_dtor(&number, 0);
    return ok;
}

int app_exec(a_vec const *item)
{
    int ok = A_FAILURE;
    if (local.code)
    {
        a_vec_foreach(pg_item, *, it, item)
        {
            ok = app_gen(it, local.code);
        }
    }
    else
    {
        app_log3(local.fname, TEXT_RED, s_missing, "-p");
    }
    return ok;
}

int app_exec_n(a_vec const *item)
{
    int ok = A_FAILURE;

    a_vec number;
    a_vec_ctor(&number, sizeof(unsigned int));

    if (local.code == 0)
    {
        app_log3(local.fname, TEXT_RED, s_missing, "-p");
        goto exit;
    }

    if (local.tree.count == 0)
    {
        app_log3(local.fname, TEXT_RED, s_missing, "-g");
        goto exit;
    }

    a_vec_foreach(pg_item, *, it, item)
    {
        if (a_str_len(it->text))
        {
            char *p = 0;
            char const *s = a_str_ptr(it->text);
            unsigned long x = strtoul(s, &p, 0);
            if (s == p)
            {
                app_log3(local.fname, TEXT_RED, s_invalid, s);
                continue;
            }
            if (x >= local.tree.count)
            {
                app_log3(local.fname, TEXT_RED, s_overrun, s);
                continue;
            }
            *A_VEC_PUSH(unsigned int, &number) = (unsigned int)x;
        }
    }

    a_size index = 0;
    pg_tree_foreach(cur, &local.tree)
    {
        pg_item *it = pg_tree_entry(cur);
        a_vec_foreach(unsigned int, *, n, &number)
        {
            if (*n == index)
            {
                ok = app_gen(it, local.code);
                break;
            }
        }
        ++index;
    }

exit:
    a_vec_dtor(&number, 0);
    return ok;
}

static int app_import_(pg_tree *tree, char const *fname)
{
    int ok;

    cJSON *json = pg_json_load(fname);
    if (json)
    {
        ok = pg_json_export(json, tree);
        pg_json_die(json);
        return ok;
    }

    sqlite3 *db = 0;
    ok = sqlite3_open(fname, &db);
    if (ok == SQLITE_OK)
    {
        return pg_sqlite_out(db, tree);
    }
    fprintf(stderr, "%s\n", sqlite3_errmsg(db));
    sqlite3_close(db);

    return ok;
}

static int app_export_(pg_tree *tree, char const *fname)
{
    int ok;

    if (strstr(fname, ".db"))
    {
        sqlite3 *db = 0;
        ok = sqlite3_open(fname, &db);
        if (ok == SQLITE_OK)
        {
            pg_sqlite_create(db);
            pg_sqlite_begin(db);
            pg_sqlite_add(db, tree);
            pg_sqlite_commit(db);
        }
        sqlite3_close(db);
        return ok;
    }

    cJSON *json = pg_json_new();
    pg_json_import(json, tree);
    ok = pg_json_dump(json, fname);

    return ok;
}

int app_convert(char const *in, char const *out)
{
    int ok;
    pg_tree tree;
    pg_tree_ctor(&tree);
    ok = app_import_(&tree, in);
    if (ok) { goto exit; }
    ok = app_export_(&tree, out);
    if (ok) { goto exit; }
exit:
    pg_tree_dtor(&tree);
    return ok;
}

int app_import(char const *fname)
{
    pg_tree tree;
    pg_tree_ctor(&tree);
    int ok = app_import_(&tree, fname);
    if (tree.count && ok == A_SUCCESS)
    {
        STATUS_SET(STATUS_DUMP);
        pg_tree_foreach(cur, &tree)
        {
            pg_item *it = pg_tree_entry(cur);
            pg_item *item = pg_tree_add(&local.tree, a_str_ptr(it->text));
            if (it->time >= item->time)
            {
                pg_item_set_hash2(item, it->hash);
                pg_item_set_hint2(item, it->hint);
                pg_item_set_misc2(item, it->misc);
                pg_item_set_type(item, it->type);
                pg_item_set_size(item, it->size);
                item->time = it->time;
            }
        }
        app_log3(local.fname, TEXT_GREEN, s_success, fname);
    }
    else
    {
        app_log3(local.fname, TEXT_RED, s_failure, fname);
    }
    pg_tree_dtor(&tree);
    return ok;
}

int app_export(char const *fname)
{
    return app_export_(&local.tree, fname);
}
