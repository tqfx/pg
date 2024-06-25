#ifndef PG_SQLITE_H
#define PG_SQLITE_H

#include "pg.h"

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#if __has_warning("-Wlanguage-extension-token")
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif /* -Wlanguage-extension-token */
#if __has_warning("-Wreserved-identifier")
#pragma clang diagnostic ignored "-Wreserved-identifier"
#endif /* -Wreserved-identifier */
#pragma GCC diagnostic ignored "-Wpadded"
#endif /* __GNUC__ || __clang__ */
#include "sqlite3.h"
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */

#if !defined PG_SQLITE_TABLE
#define PG_SQLITE_TABLE "pg_history"
#endif /* PG_SQLITE_TABLE */

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

PG_PUBLIC int pg_sqlite_init(sqlite3 *db);
PG_PUBLIC int pg_sqlite_exit(sqlite3 *db);

PG_PUBLIC int pg_sqlite_begin(sqlite3 *db);
PG_PUBLIC int pg_sqlite_commit(sqlite3 *db);

PG_PUBLIC int pg_sqlite_create(sqlite3 *db);
PG_PUBLIC int pg_sqlite_delete(sqlite3 *db);

PG_PUBLIC int pg_sqlite_out(sqlite3 *db, pg_tree *tree);
PG_PUBLIC int pg_sqlite_add(sqlite3 *db, pg_tree const *tree);
PG_PUBLIC int pg_sqlite_del(sqlite3 *db, pg_tree const *tree);

#if defined(__cplusplus)
} /* extern "C" */
#endif /* __cplusplus */

#endif /* pg/sqlite.h */
