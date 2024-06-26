#ifndef PG_PG_H
#define PG_PG_H

#include "a/avl.h"
#include "a/str.h"

/* attribute visibility */
#if defined(_WIN32) || defined(__CYGWIN__)
#define PG_EXPORT __declspec(dllexport)
#define PG_IMPORT __declspec(dllimport)
#define PG_HIDDEN
#elif A_PREREQ_GNUC(4, 0) || __has_attribute(visibility)
#define PG_EXPORT __attribute__((__visibility__("default")))
#define PG_IMPORT __attribute__((__visibility__("default")))
#define PG_HIDDEN __attribute__((__visibility__("hidden")))
#else /* !visibility */
#define PG_EXPORT
#define PG_IMPORT
#define PG_HIDDEN
#endif /* visibility */
#if defined(PG_EXPORTS)
#define PG_PUBLIC PG_EXPORT
#elif defined(PG_IMPORTS)
#define PG_PUBLIC PG_IMPORT
#else /* !PG_PUBLIC */
#define PG_PUBLIC
#endif /* PG_PUBLIC */

typedef enum pg_type
{
    PG_TYPE_EMAIL,
    PG_TYPE_DIGIT,
    PG_TYPE_OTHER,
    PG_TYPE_TOTAL,
} pg_type;

typedef struct pg_view
{
    char const *text;
    char const *hash;
    char const *hint;
    char const *misc;
    unsigned int type;
    unsigned int size;
} pg_view;

typedef struct pg_item
{
    a_avl_node node;
    a_str *text;
    a_str *hash;
    a_str *hint;
    a_str *misc;
    a_uint type;
    a_uint size;
    a_i64 time;
} pg_item;

typedef struct pg_tree
{
    a_avl root;
    a_size count;
} pg_tree;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/*!
 @brief convert hexadecimal to decimal.
 @param[in] x '0'-'9', 'a'-'f', 'A'-'F'
 @return 0 ~ 15
  @retval -1 failure
*/
PG_PUBLIC int pg_xdigit(int x);

/*!
 @brief convert a string to a lower string.
 @param[in] pdata points to data to convert.
 @param[in] nbyte length of data to convert.
 @param[in,out] out points to buffer that holds the string.
 @return a pointer to the string.
 @note When out is 0, you need to use free to release the memory.
*/
PG_PUBLIC void *pg_lower(void const *pdata, size_t nbyte, void *out);

/*!
 @brief convert a string to a upper string.
 @param[in] pdata points to data to convert.
 @param[in] nbyte length of data to convert.
 @param[in,out] out points to buffer that holds the string.
 @return a pointer to the string.
 @note When out is 0, you need to use free to release the memory.
*/
PG_PUBLIC void *pg_upper(void const *pdata, size_t nbyte, void *out);

/*!
 @brief convert a digest to a string.
 @param[in] pdata points to data to convert.
 @param[in] nbyte length of data to convert.
 @param[in] cases select the converted case.
  @arg 0 lower
  @arg 1 upper
 @param[in,out] out points to buffer that holds the string.
 @return a pointer to the string.
 @note When out is 0, you need to use free to release the memory.
*/
PG_PUBLIC void *pg_digest(void const *pdata, size_t nbyte, unsigned int cases, void *out);
PG_PUBLIC void *pg_digest_lower(void const *pdata, size_t nbyte, void *out);
PG_PUBLIC void *pg_digest_upper(void const *pdata, size_t nbyte, void *out);

PG_PUBLIC int pg_init(char *s, char const *sep);
PG_PUBLIC int pg_v1(pg_view const *ctx, char const *code, char **out);
PG_PUBLIC int pg_v2(pg_view const *ctx, char const *code, char **out);

PG_PUBLIC pg_item *pg_item_new(void);
PG_PUBLIC void pg_item_die(pg_item *ctx);
PG_PUBLIC void pg_item_ctor(pg_item *ctx);
PG_PUBLIC void pg_item_dtor(pg_item *ctx);
PG_PUBLIC void pg_view_ctor(pg_view *ctx);
PG_PUBLIC void pg_item_view(pg_item const *ctx, pg_view *out);
PG_PUBLIC void pg_item_set_type(pg_item *ctx, unsigned int type);
PG_PUBLIC void pg_item_set_size(pg_item *ctx, unsigned int size);
PG_PUBLIC int pg_item_set_text(pg_item *ctx, void const *text);
PG_PUBLIC int pg_item_set_hash(pg_item *ctx, void const *hash);
PG_PUBLIC int pg_item_set_hint(pg_item *ctx, void const *hint);
PG_PUBLIC int pg_item_set_misc(pg_item *ctx, void const *misc);
PG_PUBLIC int pg_item_set_text2(pg_item *ctx, a_str const *text);
PG_PUBLIC int pg_item_set_hash2(pg_item *ctx, a_str const *hash);
PG_PUBLIC int pg_item_set_hint2(pg_item *ctx, a_str const *hint);
PG_PUBLIC int pg_item_set_misc2(pg_item *ctx, a_str const *misc);

PG_PUBLIC void pg_tree_ctor(pg_tree *ctx);
PG_PUBLIC void pg_tree_dtor(pg_tree *ctx);
PG_PUBLIC void pg_tree_insert(pg_tree *ctx, pg_item *item);
PG_PUBLIC void pg_tree_remove(pg_tree *ctx, pg_item *item);

PG_PUBLIC pg_item *pg_tree_add(pg_tree *ctx, void const *text);
PG_PUBLIC pg_item *pg_tree_del(pg_tree *ctx, void const *text);

#define pg_tree_foreach(cur, ctx) a_avl_foreach(cur, &(ctx)->root)
#define pg_tree_entry(cur) a_avl_entry(cur, pg_item, node)

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdio.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

PG_PUBLIC long pg_io_fsize(FILE *handle);
PG_PUBLIC long pg_io_size(char const *fname);

PG_PUBLIC int pg_io_fread(FILE *handle, char **pdata, size_t *nbyte);
PG_PUBLIC int pg_io_read(char const *fname, char **pdata, size_t *nbyte);

PG_PUBLIC int pg_io_fwrite(FILE *handle, void const *pdata, size_t nbyte);
PG_PUBLIC int pg_io_write(char const *fname, void const *pdata, size_t nbyte);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* pg/pg.h */
