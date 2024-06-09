#include "pg/pg.h"
#include "pg/json.h"
#include "pg/sqlite.h"
#include <getopt.h>
#include <ncursesw/ncurses.h>

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

static struct
{
    char *self;
    char *file;
    a_str rule;
    a_str code;
} local = {
    .self = 0,
    .file = 0,
};

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

static void main_exit(void)
{
    a_str_dtor(&local.rule);
    a_str_dtor(&local.code);
    free(local.self);
    free(local.file);
    endwin();
}

static void main_init(void)
{
    atexit(main_exit);
    a_str_ctor(&local.rule);
    a_str_ctor(&local.code);
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

    initscr();
    cbreak();
    noecho();
    noraw();
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

    for (int ok; ((void)(ok = getopt_long(argc, argv, shortopts, longopts, &ok)), ok) != -1;)
    {
        switch (ok)
        {
        case 'v':
            printf("ncurses %s\n", NCURSES_VERSION);
            printf("sqlite %s\n", SQLITE_VERSION);
            printf("cjson %s\n", cJSON_Version());
            printf("pg 0.1.0\n");
            exit(EXIT_SUCCESS);
        case 'h':
        default:
            exit(main_help());
        }
    }

    main_init();

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
        if (strstr(a_str_ptr(&str), "-tui"))
        {
            a_str_getn_(&str, 0, 4);
        }
        a_str_puts(&str, ".db");
        local.file = a_str_exit(&str);
    }

    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_RED, COLOR_BLACK);
        init_pair(2, COLOR_BLUE, COLOR_WHITE);
    }

    WINDOW *win = newwin(LINES, COLS, 0, 0);
    box(win, ACS_VLINE, ACS_HLINE);
    scrollok(win, TRUE);
    keypad(win, TRUE);

    for (int x = 1; x < LINES - 1; x += 1)
    {
        WINDOW *w = subwin(win, 1, COLS - 4, x, 2);
        wprintw(w, "( ) %i", x);
    }

    wrefresh(win);
    wgetch(win);
    delwin(win);

    return EXIT_SUCCESS;
}
