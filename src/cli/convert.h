#ifndef CONVERT_H
#define CONVERT_H

#include <stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 @brief source charset transcoding destination charset
 @note Note the support for C language localization
 @param[out] out pointer of buffer variable
 @param[in] out_charset charset of destination string
 @param[in] in source string
 @param[in] in_charset charset of source string
 @return the execution state of the function
  @retval -1 failure
  @retval 0 success
*/
#if defined(_WIN32)
int code_convert(char **out, unsigned int out_charset,
                 void const *in, unsigned int in_charset);
#else /* !_WIN32 */
int code_convert(char **out, char const *out_charset,
                 void const *in, char const *in_charset);
#endif /* _WIN32 */

/*!
 @brief code transcoding utf-8
 @note Note the support for C language localization
 @param[out] out pointer of buffer variable
 @param[in] in source string
 @return the execution state of the function
  @retval -1 failure
  @retval 0 success
*/
int code_to_utf8(char **out, void const *in);

/*!
 @brief utf-8 transcoding code
 @note Note the support for C language localization
 @param[out] out pointer of buffer variable
 @param[in] in source string
 @return the execution state of the function
  @retval -1 failure
  @retval 0 success
*/
int code_utf8_to(char **out, void const *in);

/*!
 @brief utf-8 transcoding gbk
 @note Note the support for C language localization
 @param[out] out pointer of buffer variable
 @param[in] in source string
 @return the execution state of the function
  @retval -1 failure
  @retval 0 success
*/
int utf8_gbk(char **out, void const *in);

/*!
 @brief gbk transcoding utf-8
 @note Note the support for C language localization
 @param[out] out pointer of buffer variable
 @param[in] in source string
 @return the execution state of the function
  @retval -1 failure
  @retval 0 success
*/
int gbk_utf8(char **out, void const *in);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* CONVERT_H */
