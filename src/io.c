#include "pg/pg.h"

long pg_io_fsize(FILE *handle)
{
    long ret;
    long seek = ftell(handle);
    if (seek < 0) { return seek; }
    if (((void)(ret = fseek(handle, seek, SEEK_END)), ret))
    {
        return ret;
    }
    long size = ftell(handle);
    if (size < 0) { return size; }
    if (((void)(ret = fseek(handle, seek, SEEK_SET)), ret))
    {
        return ret;
    }
    return size;
}

long pg_io_size(char const *fname)
{
    FILE *handle = fopen(fname, "rb");
    if (handle == 0) { return ~0; }
    long size = pg_io_fsize(handle);
    fclose(handle);
    return size;
}

int pg_io_fread(FILE *handle, char **pdata, size_t *nbyte)
{
    long size = pg_io_fsize(handle);
    if (size < 0) { return ~0; }
    *pdata = malloc((size_t)size);
    if (*pdata == 0) { return ~0; }
    *nbyte = fread(*pdata, 1, (size_t)size, handle);
    return 0;
}

int pg_io_read(char const *fname, char **pdata, size_t *nbyte)
{
    int ret = ~0;
    FILE *handle = fopen(fname, "rb");
    if (handle)
    {
        ret = pg_io_fread(handle, pdata, nbyte);
        fclose(handle);
    }
    return ret;
}

int pg_io_fwrite(FILE *handle, void const *pdata, size_t nbyte)
{
    if (fwrite(pdata, 1, nbyte, handle) == nbyte)
    {
        return 0;
    }
    return ~0;
}

int pg_io_write(char const *fname, void const *pdata, size_t nbyte)
{
    int ret = ~0;
    FILE *handle = fopen(fname, "wb");
    if (handle)
    {
        ret = pg_io_fwrite(handle, pdata, nbyte);
        fclose(handle);
    }
    return ret;
}
