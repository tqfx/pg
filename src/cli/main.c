#include "app.h"
#include <getopt.h>
#if defined(_WIN32)
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 5105)
#endif /* _MSC_VER */
#include <windows.h>
#if defined(_MSC_VER)
#pragma warning(pop)
#endif /* _MSC_VER */
#else /* !_WIN32 */
#include <unistd.h>
#endif /* _WIN32 */

static char *path_self(void)
{
    char *self = 0;
    size_t length = 0;
    size_t size = 0x10;
    do
    {
        size <<= 1;
        char *ptr = (char *)realloc(self, size);
        if (ptr == 0) { break; }
        self = ptr;
#if defined(_WIN32)
        length = GetModuleFileName(NULL, self, (DWORD)(size - 1));
#else /* !_WIN32 */
        length = (size_t)readlink("/proc/self/exe", self, size - 1);
#endif /* _WIN32 */
    } while (length >= size - 1);
    if (self) { self[length] = 0; }
    return self;
}

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic ignored "-Wc++-compat"
#endif /* __GNUC__ || __clang__ */

#pragma pack(push, 4)
static struct
{
    char *self;
    char *file;
    char *import;
    char *export;
    pg_view view;
    a_str rule;
    a_str code;
    a_vec item;
    int option;
} local = {
    .self = 0,
    .file = 0,
    .import = 0,
    .export = 0,
    .option = 0,
};
#pragma pack(pop)

#define OPTION_NUMBER (1 << 0)
#define OPTION_SEARCH (1 << 1)
#define OPTION_CREATE (1 << 2)
#define OPTION_DELETE (1 << 3)

#define OPTION_GET(mask) (local.option & (mask))
#define OPTION_SET(mask) (local.option |= (mask))
#define OPTION_CLR(mask) (local.option &= ~(mask))
#define OPTION_IS1(mask) ((local.option & (mask)) == (mask))

static A_INLINE void item_push(pg_view const *view)
{
    pg_item *item = A_VEC_PUSH(pg_item, &local.item);
    pg_item_ctor(item);
    pg_item_set_text(item, view->text);
    pg_item_set_hash(item, view->hash);
    pg_item_set_hint(item, view->hint);
    if (view->type == PG_TYPE_OTHER)
    {
        pg_item_set_misc(item, view->misc);
    }
    pg_item_set_type(item, view->type);
    pg_item_set_size(item, view->size);
}

static int io_getline(char const *fname, char **pdata, size_t *nbyte)
{
    long seek = 0;
    FILE *handle = fopen(fname, "r");
    if (handle == 0) { return ~0; }
    *nbyte = 0;
    for (int c = fgetc(handle); c > ~0; c = fgetc(handle))
    {
        if (c == '\n' || c == '\r')
        {
            if (*nbyte) { break; }
            else { ++seek; }
            continue;
        }
        ++*nbyte;
    }
    *pdata = malloc(*nbyte);
    fseek(handle, seek, SEEK_SET);
    *nbyte = fread(*pdata, 1, *nbyte, handle);
    return fclose(handle);
}

static A_INLINE void item_dtor(void *ctx)
{
    pg_item_dtor((pg_item *)ctx);
}

static void main_exit(void)
{
    free(local.self);
    free(local.file);
    free(local.import);
    free(local.export);
    a_str_dtor(&local.rule);
    a_str_dtor(&local.code);
    a_vec_dtor(&local.item, item_dtor);
}

static void main_init(void)
{
    atexit(main_exit);
    a_str_ctor(&local.rule);
    a_str_ctor(&local.code);
    a_vec_ctor(&local.item, sizeof(pg_item));
    pg_view_ctor(&local.view);
    local.self = path_self();

    char *env = getenv("PG_FILE");
    if (env) { local.file = strdup(env); }

    env = getenv("PG_RULE");
    if (env)
    {
        char *p;
        a_size n;
        if (io_getline(env, &p, &n) > ~0)
        {
            a_str_setn(&local.rule, p, n);
            free(p);
        }
        else
        {
            a_str_sets(&local.rule, env);
        }
    }

    env = getenv("PG_CODE");
    if (env)
    {
        char *p;
        a_size n;
        if (io_getline(env, &p, &n) > ~0)
        {
            a_str_setn(&local.code, p, n);
            free(p);
        }
        else
        {
            a_str_sets(&local.code, env);
        }
    }
}

static int main_help(void)
{
    static const char help[] = " [option]... [text]...\n\
option:\n\
  -n --number    use numeric index\n\
  -s --search    search something\n\
  -c --create    create something\n\
  -d --delete    delete something\n\
  -r --rule      string\n\
  -p --code      string\n\
  -g --text      string\n\
  -a --hash      hash algorithm\n\
  -t --type      number(0:email 1:digit 2:other)\n\
  -m --misc      string\n\
  -h --hint      string\n\
  -l --length    number(0~128)\n\
  -i --import    filename\n\
  -o --export    filename\n\
  -f --filename  filename\n\
hash: MD5(default)\n\
     SHA1  SHA256  SHA224  BLAKE2S\n\
     SHA3  SHA512  SHA384  BLAKE2B\n\
Copyright (C) 2020-2024 tqfx, All rights reserved.";
    printf("%s%s\n", local.self, help);
    return EXIT_SUCCESS;
}

static int main_app(void);
int main(int argc, char *argv[])
{
    char const *shortopts = "?12nscdvr:p:g::a:h:m:t:l:i:o:f:";
    static struct option const longopts[] = {
        {"help", no_argument, 0, '?'},
        {"number", no_argument, 0, 'n'},
        {"search", no_argument, 0, 's'},
        {"create", no_argument, 0, 'c'},
        {"delete", no_argument, 0, 'd'},
        {"version", no_argument, 0, 'v'},
        {"rule", required_argument, 0, 'r'},
        {"code", required_argument, 0, 'p'},
        {"text", required_argument, 0, 'g'},
        {"hash", required_argument, 0, 'a'},
        {"hint", required_argument, 0, 'h'},
        {"misc", required_argument, 0, 'm'},
        {"type", required_argument, 0, 't'},
        {"length", required_argument, 0, 'l'},
        {"import", required_argument, 0, 'i'},
        {"export", required_argument, 0, 'o'},
        {"filename", required_argument, 0, 'f'},
        {0, 0, 0, 0},
    };

    main_init();

    for (int ok; ((void)(ok = getopt_long(argc, argv, shortopts, longopts, &ok)), ok) != -1;)
    {
        switch (ok)
        {
        case '1':
            OPTION_SET(1 << 8);
            OPTION_CLR(1 << 9);
            break;
        case '2':
            OPTION_CLR(1 << 8);
            OPTION_SET(1 << 9);
            break;
        case 'n':
            OPTION_SET(OPTION_NUMBER);
            break;
        case 's':
            OPTION_SET(OPTION_SEARCH);
            break;
        case 'c':
            OPTION_SET(OPTION_CREATE);
            break;
        case 'd':
            OPTION_SET(OPTION_DELETE);
            break;
        case 'r':
            a_str_sets(&local.rule, optarg);
            break;
        case 'p':
            a_str_sets(&local.code, optarg);
            break;
        case 'g':
            local.view.text = optarg;
            item_push(&local.view);
            break;
        case 'a':
            local.view.hash = optarg;
            break;
        case 'h':
            local.view.hint = optarg;
            break;
        case 'm':
            local.view.misc = optarg;
            break;
        case 't':
            local.view.type = (unsigned int)strtoul(optarg, 0, 0);
            break;
        case 'l':
            local.view.size = (unsigned int)strtoul(optarg, 0, 0);
            break;
        case 'i':
            if (local.import) { free(local.import); }
            local.import = strdup(optarg);
            break;
        case 'o':
            if (local.export) { free(local.export); }
            local.export = strdup(optarg);
            break;
        case 'f':
            if (local.file) { free(local.file); }
            local.file = strdup(optarg);
            break;
        case 'v':
            printf("sqlite %s\n", SQLITE_VERSION);
            printf("cjson %s\n", cJSON_Version());
            printf("pg 0.1.0\n");
            exit(EXIT_SUCCESS);
        case '?':
        default:
            exit(main_help());
        }
    }

    a_vec_forenum_reverse(i, &local.item)
    {
        pg_item *it = A_VEC_AT_(pg_item, &local.item, i);
        if (!OPTION_GET(OPTION_SEARCH) && a_str_len(it->text) == 0)
        {
            a_vec_remove(&local.item, i);
        }
    }

    if (a_vec_num(&local.item))
    {
        pg_item *ctx = A_VEC_TOP_(pg_item, &local.item);
        if (local.view.type == PG_TYPE_OTHER)
        {
            pg_item_set_misc(ctx, local.view.misc);
        }
        if (local.view.hash)
        {
            pg_item_set_hash(ctx, local.view.hash);
        }
        if (local.view.hint)
        {
            pg_item_set_hint(ctx, local.view.hint);
        }
        pg_item_set_type(ctx, local.view.type);
        pg_item_set_size(ctx, local.view.size);
    }

    for (int i = optind; i < argc; ++i)
    {
        local.view.text = argv[i];
        item_push(&local.view);
    }

#if defined(_WIN32)
    a_vec_foreach(pg_item, *, it, &local.item)
    {
        if (a_str_len(it->text))
        {
            char *text;
            code_to_utf8(&text, a_str_ptr(it->text));
            pg_item_set_text(it, text);
            free(text);
        }
    }
#endif /* _WIN32 */

    if (local.file == 0)
    {
        a_str str = A_STR_INIT;
        a_str_sets(&str, local.self);
#if defined(_WIN32)
        if (strstr(a_str_ptr(&str), ".exe"))
        {
            a_str_getn_(&str, 0, 4);
        }
#endif /* _WIN32 */
        a_str_puts(&str, ".db");
        local.file = a_str_exit(&str);
    }

    return main_app();
}

static int main_app(void)
{
    app_init(local.file, &local.code, &local.rule, local.option >> 8);

    if (local.import && local.export)
    {
        app_convert(local.import, local.export);
    }
    else if (local.import)
    {
        app_import(local.import);
    }
    else if (local.export)
    {
        app_export(local.export);
    }
    else if (OPTION_IS1(OPTION_DELETE))
    {
        OPTION_CLR(OPTION_DELETE);
        if (OPTION_IS1(OPTION_NUMBER))
        {
            OPTION_CLR(OPTION_NUMBER);
            app_delete_n(&local.item);
        }
        else
        {
            app_delete(&local.item);
        }
    }
    else if (OPTION_IS1(OPTION_CREATE))
    {
        OPTION_CLR(OPTION_CREATE);
        app_create(&local.item);
    }
    else if (OPTION_IS1(OPTION_SEARCH | OPTION_NUMBER))
    {
        OPTION_CLR(OPTION_SEARCH | OPTION_NUMBER);
        app_search_n(&local.item);
    }
    else if (OPTION_IS1(OPTION_SEARCH))
    {
        OPTION_CLR(OPTION_SEARCH);
        app_search(&local.item);
    }
    else if (a_vec_num(&local.item) && OPTION_IS1(OPTION_NUMBER))
    {
        OPTION_CLR(OPTION_NUMBER);
        app_exec_n(&local.item);
    }
    else if (a_vec_num(&local.item))
    {
        app_exec(&local.item);
    }

    return app_exit();
}
