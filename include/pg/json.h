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

PG_PUBLIC int pg_json_load(cJSON **out, char const *in);
PG_PUBLIC int pg_json_export(cJSON const *in, pg_items *out);
PG_PUBLIC int pg_json_import(cJSON **out, pg_items const *in);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* pg/json.h */
