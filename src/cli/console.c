#include "console.h"

#include <stdarg.h>
#include <stdlib.h>

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

static struct
{
    unsigned int ft;
    unsigned int bg;
} console_text = {
    .ft = 7,
    .bg = 0,
};

void console_text_reset(void)
{
    console_text.ft = 7;
    console_text.bg = 0;
#if defined(_WIN32)
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, CONSOLE_TEXT_DEFAULT);
#else /* !_WIN32 */
    printf("\033[0m");
#endif /* _WIN32 */
}

unsigned int console_text_set(unsigned int ft, unsigned int bg)
{
    static unsigned int map[] = {
    /* clang-format off */
#if defined(_WIN32)
        0, 4, 6, 2, 3, 1, 5, 7,
#else /* !_WIN32 */
        0, 1, 3, 2, 6, 4, 5, 7,
#endif /* _WIN32 */
    };
    /* clang-format on */
    unsigned int ret = (bg << 4) + ft;
    ft = map[ft % (sizeof(map) / sizeof(*map))];
    bg = map[bg % (sizeof(map) / sizeof(*map))];
#if defined(_WIN32)
    if (console_text.ft != ft || console_text.bg != bg)
    {
        WORD x = (WORD)((bg << 4) + ft);
        HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(handle, x);
        console_text.ft = ft;
        console_text.bg = bg;
    }
#else /* !_WIN32 */
    if (console_text.ft != ft)
    {
        printf("\033[3%um", ft);
        console_text.ft = ft;
    }
    if (console_text.bg != bg)
    {
        printf("\033[4%um", bg);
        console_text.bg = bg;
    }
#endif /* _WIN32 */
    return ret;
}

int console_printf(unsigned int num, ...)
{
    int ret = 0;
    va_list ap;
    va_start(ap, num);
    while (num--)
    {
        unsigned int x = va_arg(ap, unsigned int);
        console_text_set(x, x >> 4);
        char const *str = va_arg(ap, char const *);
        if (str)
        {
            ret += printf("%s", str);
        }
    }
    console_text_reset();
    va_end(ap);
    return ret;
}

int console_vprint(char const *sep, unsigned int num, va_list ap)
{
    int ret = 0;
    while (num--)
    {
        unsigned int x = va_arg(ap, unsigned int);
        console_text_set(x, x >> 4);
        char const *str = va_arg(ap, char const *);
        if (str)
        {
            ret += printf("%s", str);
            if (sep && num)
            {
                ret += printf("%s", sep);
            }
        }
    }
    console_text_reset();
    return ret;
}

int console_print(char const *sep, unsigned int num, ...)
{
    va_list ap;
    va_start(ap, num);
    int ret = console_vprint(sep, num, ap);
    va_end(ap);
    return ret;
}

int console_println(char const *sep, unsigned int num, ...)
{
    va_list ap;
    va_start(ap, num);
    int ret = console_vprint(sep, num, ap) + 1;
    putchar('\n');
    va_end(ap);
    return ret;
}

int console_aprint(char const *sep, unsigned int num, unsigned int *x, char const **s)
{
    int ret = 0;
    while (num--)
    {
        console_text_set(*x, *x >> 4);
        if (*s)
        {
            ret += printf("%s", *s);
            if (sep && num)
            {
                ret += printf("%s", sep);
            }
        }
        ++x;
        ++s;
    }
    console_text_reset();
    return ret;
}

int console_aprintln(char const *sep, unsigned int num, unsigned int *x, char const **s)
{
    int ret = console_aprint(sep, num, x, s) + 1;
    putchar('\n');
    return ret;
}
