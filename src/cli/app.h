#ifndef APP_H
#define APP_H

#include "a/vec.h"
#include "pg/json.h"
#include "pg/sqlite.h"

#include "console.h"
#include "convert.h"

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

void app_log(unsigned int n, ...);
int app_gen(pg_item const *view, char const *code);

int app_init(char const *fname, a_str const *code, a_str const *rule, int ver);
int app_exit(void);

int app_create(a_vec const *item);

void app_search(a_vec const *item);
void app_search_n(a_vec const *item);

int app_delete(a_vec const *item);
int app_delete_n(a_vec const *item);

int app_exec(a_vec const *item);
int app_exec_n(a_vec const *item);

int app_import(char const *fname);
int app_export(char const *fname);
int app_convert(char const *in, char const *out);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

static const char s_success[] = "success";
static const char s_failure[] = "failure";
static const char s_missing[] = "missing";
static const char s_invalid[] = "invalid";
static const char s_overrun[] = "overrun";

#endif /* APP_H */
