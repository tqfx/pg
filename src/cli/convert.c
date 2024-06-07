#include "convert.h"

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
#include <locale.h>
#define CHARSET_UTF8 ".utf8"
#define CHARSET_GBK "zh_CN.gbk"
#endif /* _WIN32 */

#if defined(_WIN32)
int code_convert(char **out, unsigned int out_charset,
                 void const *in, unsigned int in_charset)
{
    int n = MultiByteToWideChar(in_charset, 0, (char const *)in, -1, 0, 0);
    if (n == 0)
    {
        return ~0;
    }
    wchar_t *str = (wchar_t *)malloc(sizeof(wchar_t) * (size_t)n);
    MultiByteToWideChar(in_charset, 0, (char const *)in, -1, str, n);
    do
    {
        n = WideCharToMultiByte(out_charset, 0, str, -1, 0, 0, 0, 0);
        if (n == 0)
        {
            n = ~0;
            break;
        }
        *out = (char *)malloc(sizeof(char) * (size_t)n);
        WideCharToMultiByte(out_charset, 0, str, -1, *out, n, 0, 0);
        n = 0;
    } while (0);
    free(str);
    return n;
}
#else /* !_WIN32 */
int code_convert(char **out, char const *out_charset,
                 void const *in, char const *in_charset)
{
    if (setlocale(LC_CTYPE, in_charset) == 0)
    {
        return ~0;
    }
    size_t n = mbstowcs(0, (char const *)in, 0);
    if (n == 0)
    {
        return ~0;
    }
    wchar_t *str = (wchar_t *)malloc(sizeof(wchar_t) * n);
    mbstowcs(str, (char const *)in, n);
    int ret = ~0;
    do
    {
        if (setlocale(LC_CTYPE, out_charset) == 0)
        {
            break;
        }
        n = wcstombs(0, str, 0);
        if (n == 0)
        {
            break;
        }
        *out = (char *)malloc(sizeof(char) * (n + 1));
        wcstombs(*out, str, n);
        (*out)[n] = 0;
        ret = 0;
    } while (0);
    free(str);
    return ret;
}
#endif /* _WIN32 */

int utf8_gbk(char **out, void const *in)
{
#if defined(_WIN32)
    return code_convert(out, 936, in, 65001);
#else /* !_WIN32 */
    return code_convert(out, CHARSET_GBK, in, CHARSET_UTF8);
#endif /* _WIN32 */
}

int gbk_utf8(char **out, void const *in)
{
#if defined(_WIN32)
    return code_convert(out, 65001, in, 936);
#else /* !_WIN32 */
    return code_convert(out, CHARSET_UTF8, in, CHARSET_GBK);
#endif /* _WIN32 */
}

int code_to_utf8(char **out, void const *in)
{
#if defined(_WIN32)
    return code_convert(out, 65001, in, GetOEMCP());
#else /* !_WIN32 */
    (void)out, (void)in;
    return ~0;
#endif /* _WIN32 */
}

int code_utf8_to(char **out, void const *in)
{
#if defined(_WIN32)
    return code_convert(out, GetOEMCP(), in, 65001);
#else /* !_WIN32 */
    (void)out, (void)in;
    return ~0;
#endif /* _WIN32 */
}
