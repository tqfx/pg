#include "pg/pg.h"
#include <ctype.h>
#include "pg/json.h"
#include "pg/sqlite.h"
#include <getopt.h>
#include <ncursesw/ncurses.h>

static struct
{
    char *file;
    a_str rule;
    a_str code;
} local = {
    .file = 0,
};

static void main_exit(void)
{
    a_str_dtor(&local.rule);
    a_str_dtor(&local.code);
    free(local.file);
    endwin();
}

static void main_init(void)
{
    atexit(main_exit);
    a_str_ctor(&local.rule);
    a_str_ctor(&local.code);

    char *env = getenv("PG_FILE");
    if (env) { local.file = strdup(env); }

    env = getenv("PG_RULE");
    if (env)
    {
        char *p;
        a_size n;
        if (pg_io_read(env, (void **)&p, &n) > ~0)
        {
            while (n && isspace(p[n - 1])) { --n; }
            a_str_putn(&local.rule, p, n);
            free(p);
        }
        else
        {
            a_str_puts(&local.rule, env);
        }
    }

    env = getenv("PG_CODE");
    if (env)
    {
        char *p;
        a_size n;
        if (pg_io_read(env, (void **)&p, &n) > ~0)
        {
            while (n && isspace(p[n - 1])) { --n; }
            a_str_putn(&local.code, p, n);
            free(p);
        }
        else
        {
            a_str_puts(&local.code, env);
        }
    }

    initscr();
    cbreak();
}

static int main_help(void)
{
    static const char help[] = "Copyright (C) 2020-2024 tqfx, All rights reserved.";
    printf("%s\n", help);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    char const *shortopts = "vh";
    static struct option const longopts[] = {
        {"version", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0},
    };

    for (int ok; ((void)(ok = getopt_long(argc, argv, shortopts, longopts, &optind)), ok) != -1;)
    {
        switch (ok)
        {
        case 'v':
            printf("sqlite %s\n", SQLITE_VERSION);
            printf("cjson %s\n", cJSON_Version());
            printf("pg 0.1.0\n");
            break;
        case 'h':
        default:
            exit(main_help());
        }
    }

    main_init();

    printw("ok");
    refresh();
    getch();

    return EXIT_SUCCESS;
}
