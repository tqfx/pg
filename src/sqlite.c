#include "pg/sqlite.h"

int pg_sqlite_begin(sqlite3 *db)
{
    sqlite3_stmt *stmt = 0;
    sqlite3_prepare(db, "BEGIN;", -1, &stmt, 0);
    sqlite3_step(stmt);
    return sqlite3_finalize(stmt);
}

int pg_sqlite_commit(sqlite3 *db)
{
    sqlite3_stmt *stmt = 0;
    sqlite3_prepare(db, "COMMIT;", -1, &stmt, 0);
    sqlite3_step(stmt);
    return sqlite3_finalize(stmt);
}

int pg_sqlite_create(sqlite3 *db)
{
    sqlite3_stmt *stmt = 0;

    sqlite3_str *str = sqlite3_str_new(db);
    sqlite3_str_appendf(str, "CREATE TABLE IF NOT EXISTS %s("
                             "%s TEXT PRIMARY KEY ASC,"
                             "%s TEXT DEFAULT MD5,"
                             "%s INTEGER DEFAULT 16,"
                             "%s INTEGER DEFAULT 0,"
                             "%s TEXT,%s TEXT,%s INTEGER DEFAULT -2147483648);",
                        PG_SQLITE_TABLE, "text", "hash", "size", "type", "misc", "hint", "time");
    char *sql = sqlite3_str_finish(str);
    sqlite3_prepare(db, sql, -1, &stmt, 0);
    sqlite3_free(sql);
    sqlite3_step(stmt);

    return sqlite3_finalize(stmt);
}

int pg_sqlite_delete(sqlite3 *db)
{
    sqlite3_stmt *stmt = 0;

    sqlite3_str *str = sqlite3_str_new(db);
    sqlite3_str_appendf(str, "DROP TABLE IF EXISTS %s;", PG_SQLITE_TABLE);
    char *sql = sqlite3_str_finish(str);
    sqlite3_prepare(db, sql, -1, &stmt, 0);
    sqlite3_free(sql);
    sqlite3_step(stmt);

    return sqlite3_finalize(stmt);
}

int pg_sqlite_init(sqlite3 *db)
{
    pg_sqlite_create(db);
    return pg_sqlite_begin(db);
}

int pg_sqlite_exit(sqlite3 *db)
{
    return pg_sqlite_commit(db);
}

int pg_sqlite_out(sqlite3 *db, pg_tree *tree)
{
    sqlite3_stmt *stmt = 0;

    sqlite3_str *str = sqlite3_str_new(db);
    sqlite3_str_appendf(str, "SELECT * FROM %s ORDER BY %s ASC;", PG_SQLITE_TABLE, "text");
    char *sql = sqlite3_str_finish(str);
    sqlite3_prepare(db, sql, -1, &stmt, 0);
    sqlite3_free(sql);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        unsigned char const *text = sqlite3_column_text(stmt, 0);
        if (text == 0) { continue; }
        pg_item *item = pg_tree_add(tree, text);
        if (((void)(text = sqlite3_column_text(stmt, 1)), text))
        {
            pg_item_set_hash(item, text);
        }
        else
        {
            pg_item_set_hash(item, "MD5");
        }
        pg_item_set_size(item, (unsigned int)sqlite3_column_int(stmt, 2));
        pg_item_set_type(item, (unsigned int)sqlite3_column_int(stmt, 3));
        if (((void)(text = sqlite3_column_text(stmt, 4)), text) && item->type == PG_TYPE_OTHER)
        {
            pg_item_set_misc(item, text);
        }
        if (((void)(text = sqlite3_column_text(stmt, 5)), text))
        {
            pg_item_set_hint(item, text);
        }
        item->time = sqlite3_column_int64(stmt, 6);
    }

    return sqlite3_finalize(stmt);
}

int pg_sqlite_add(sqlite3 *db, pg_tree const *tree)
{
    sqlite3_stmt *stmt = 0;

    sqlite3_str *str = sqlite3_str_new(db);
    sqlite3_str_appendf(str, "INSERT INTO %s VALUES(?,?,?,?,?,?,?);", PG_SQLITE_TABLE);
    char *sql = sqlite3_str_finish(str);
    sqlite3_prepare(db, sql, -1, &stmt, 0);
    sqlite3_free(sql);

    pg_tree_foreach(cur, tree)
    {
        pg_item *it = pg_tree_entry(cur);
        if (a_str_len(it->text))
        {
            sqlite3_reset(stmt);
            {
                int size = (int)a_str_len(it->text);
                sqlite3_bind_text(stmt, 1, a_str_ptr(it->text), size, SQLITE_STATIC);
            }
            {
                int size = (int)a_str_len(it->hash);
                sqlite3_bind_text(stmt, 2, a_str_ptr(it->hash), size, SQLITE_STATIC);
            }
            sqlite3_bind_int(stmt, 3, (int)it->size);
            sqlite3_bind_int(stmt, 4, (int)it->type);
            {
                int size = (int)a_str_len(it->misc);
                if (it->type != PG_TYPE_OTHER) { size = 0; }
                sqlite3_bind_text(stmt, 5, a_str_ptr(it->misc), size, SQLITE_STATIC);
            }
            {
                int size = (int)a_str_len(it->hint);
                sqlite3_bind_text(stmt, 6, a_str_ptr(it->hint), size, SQLITE_STATIC);
            }
            sqlite3_bind_int64(stmt, 7, it->time);
            sqlite3_step(stmt);
        }
    }

    return sqlite3_finalize(stmt);
}

int pg_sqlite_del(sqlite3 *db, pg_tree const *tree)
{
    sqlite3_stmt *stmt = 0;

    sqlite3_str *str = sqlite3_str_new(db);
    sqlite3_str_appendf(str, "DELETE FROM %s WHERE %s = ?;", PG_SQLITE_TABLE, "text");
    char *sql = sqlite3_str_finish(str);
    sqlite3_prepare(db, sql, -1, &stmt, 0);
    sqlite3_free(sql);

    pg_tree_foreach(cur, tree)
    {
        pg_item *it = pg_tree_entry(cur);
        if (a_str_len(it->text))
        {
            sqlite3_reset(stmt);
            {
                int size = (int)a_str_len(it->text);
                sqlite3_bind_text(stmt, 1, a_str_ptr(it->text), size, SQLITE_STATIC);
            }
            sqlite3_step(stmt);
        }
    }

    return sqlite3_finalize(stmt);
}
