#include <stdio.h>
#include <string.h>
#include <zos_errors.h>

#ifndef FS_H
#define FS_H

#define KILOBYTE    (uint32_t)1024
#define MEGABYTE    (uint32_t)(1024 * 1024)

#define MAX_FILE_ENTRIES    33

typedef enum {
    FileFlag_Directory = 1,
    FileFlag_File = 2,
    FileFlag_Executable = 4,
    FileFlag_Disk = 8,
} zc_file_flags;

typedef struct {
    zc_file_flags flags;
    uint32_t size;
    zos_date_t date;
    char name[FILENAME_LEN_MAX];
} zc_entry_t;

zos_err_t list(const char* path, zc_entry_t* list, uint8_t* size);
zos_err_t remove(const char* path);
zos_err_t move(const char* src, const char* dst);
zos_err_t copy(const char* src, const char* dst);
zos_err_t rename(const char* src, const char* dst);
zos_err_t exists(const char* src);
zos_err_t is_dir(const char* src);
zos_err_t is_disk(const char letter);

#endif
