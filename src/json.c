#include "pg/json.h"
#include <time.h>

int pg_json_load(cJSON **out, char const *in)
{
    int ret;
    void *pdata = 0;
    size_t nbyte = 0;
    ret = pg_io_read(in, &pdata, &nbyte);
    if (ret == 0)
    {
        *out = cJSON_ParseWithLength((char *)pdata, nbyte);
        free(pdata);
    }
    return ret;
}

int pg_json_export(cJSON const *in, pg_items *out)
{
    pg_view view;
    cJSON *object;
    int n = cJSON_GetArraySize(in);
    for (int i = 0; i != n; ++i)
    {
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wbad-function-cast"
#endif /* __GNUC__ || __clang__ */
        cJSON *item = cJSON_GetArrayItem(in, i);

        object = cJSON_GetObjectItem(item, "text");
        if (object == 0) { continue; }
        view.text = cJSON_GetStringValue(object);

        object = cJSON_GetObjectItem(item, "hash");
        view.hash = object ? cJSON_GetStringValue(object) : "MD5";

        object = cJSON_GetObjectItem(item, "size");
        if (object == 0) { continue; }
        view.size = (unsigned int)cJSON_GetNumberValue(object);

        object = cJSON_GetObjectItem(item, "type");
        if (object == 0) { continue; }
        view.type = (unsigned int)cJSON_GetNumberValue(object);

        if (view.type == PG_TYPE_OTHER)
        {
            object = cJSON_GetObjectItem(item, "misc");
            if (object == 0) { continue; }
            view.misc = cJSON_GetStringValue(object);
        }
        else { view.misc = 0; }

        object = cJSON_GetObjectItem(item, "hint");
        view.hint = cJSON_GetStringValue(object);

        pg_item *ctx = pg_items_add(out, view.text);

        object = cJSON_GetObjectItem(item, "time");
        ctx->time = object ? (a_i64)cJSON_GetNumberValue(object) : time(NULL) + A_I32_MIN;

        pg_item_set_hash(ctx, view.hash);
        pg_item_set_hint(ctx, view.hint);
        pg_item_set_misc(ctx, view.misc);
        pg_item_set_size(ctx, view.size);
        pg_item_set_type(ctx, view.type);
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#endif /* __GNUC__ || __clang__ */
    }
    return 0;
}

int pg_json_import(cJSON **out, pg_items const *in)
{
    *out = cJSON_CreateArray();
    pg_items_foreach(cur, in)
    {
        pg_item *it = pg_items_entry(cur);
        if (a_str_len(it->text) == 0) { continue; }
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "text", a_str_ptr(it->text));
        cJSON_AddStringToObject(item, "hash", a_str_ptr(it->hash));
        cJSON_AddNumberToObject(item, "size", it->size);
        cJSON_AddNumberToObject(item, "type", it->type);
        if (a_str_len(it->misc) && it->type == PG_TYPE_OTHER)
        {
            cJSON_AddStringToObject(item, "misc", a_str_ptr(it->misc));
        }
        if (a_str_len(it->hint))
        {
            cJSON_AddStringToObject(item, "hint", a_str_ptr(it->hint));
        }
        cJSON_AddNumberToObject(item, "time", (double)it->time);
        cJSON_AddItemToArray(*out, item);
    }
    return 0;
}
