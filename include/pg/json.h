#ifndef PG_JSON_H
#define PG_JSON_H

#include "pg.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* __GNUC__ || __clang__ */
#include "cjson/cJSON.h"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

PG_PUBLIC cJSON *pg_json_new(void);
PG_PUBLIC void pg_json_die(cJSON *json);

PG_PUBLIC cJSON *pg_json_load(char const *fname);
PG_PUBLIC int pg_json_dump(cJSON *json, char const *fname);

PG_PUBLIC int pg_json_export(cJSON const *json, pg_tree *tree);
PG_PUBLIC int pg_json_import(cJSON *json, pg_tree const *tree);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* pg/json.h */
