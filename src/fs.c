#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <zos_errors.h>
#include <zos_vfs.h>

#include "fs.h"
#include "shared.h"

zos_err_t list(const char* path, zc_entry_t* list, uint8_t* size) {
    zos_dev_t d = opendir(path);
    if(d < 0) {
        error(-d, "opendir err %s", path);
        return -d;
    }

    zos_err_t err;
    zos_dir_entry_t dir_entry;
    zos_stat_t entry_stat;

    strcpy(list[0].name, "..");
    list[0].flags = FileFlag_Directory;
    list[0].size = 0;

    uint8_t i;
    for(i = 1; i < MAX_FILE_ENTRIES; i++) {
        err = readdir(d, &dir_entry);
        if(err == ERR_NO_MORE_ENTRIES) break;
        if(err != ERR_SUCCESS) {
            error(err, "readdir %s", path);
            return err;
        }

        zc_entry_t *entry = &list[i];
        memcpy(entry->name, dir_entry.d_name, FILENAME_LEN_MAX);
        if(D_ISFILE(dir_entry.d_flags)) {
            char filename[PATH_MAX];
            sprintf(filename, "%s%s", path, dir_entry.d_name);
            err = stat(filename, &entry_stat);
            if(err != ERR_SUCCESS) {
                error(err, "stat %s", filename);
                return err;
            }

            entry->flags = FileFlag_File;
            entry->size = entry_stat.s_size;
            // memcpy(&entry->date, &entry_stat.s_date, sizeof(zos_date_t));

            if(str_ends_with(entry->name, ".bin") || str_pos(entry->name, '.') < 0) {
                entry->flags |= FileFlag_Executable;
            }
        } else {
            entry->size = 0;
            entry->flags = FileFlag_Directory;
            // memset(&entry->date, 0, sizeof(zos_date_t));
        }
    }
    *size = i;

    err = close(d);
    if(err != ERR_SUCCESS) {
        error(err, "close");
        return err;
    }
    return ERR_SUCCESS;
}

zos_err_t remove(const char* path)
{
    zos_err_t err = rm(path);
    return err;
}

zos_err_t move(const char* src, const char* dst)
{
    zos_err_t err;
    err = copy(src, dst);
    if(err != ERR_SUCCESS) return err;
    err = remove(src);
    return err;
}

zos_err_t copy(const char* src, const char* dst)
{
    zos_err_t err;
    zos_dev_t s, d;

    s = open(src, O_RDONLY);
    if(s < 0) return -s;

    d = open(dst, O_WRONLY | O_CREAT);
    if(d < 0) return -d;

    char buffer[1024];
    uint16_t size = 1024;;

    do {
        err = read(s, buffer, &size);
        err = write(d, buffer, &size);
    } while(err == ERR_SUCCESS && size > 0);

    err = close(s);
    if(err != ERR_SUCCESS) return err;
    err = close(d);
    return err;
}

zos_err_t rename(const char* src, const char* dst) {
    (void *)src;
    (void *)dst;
    return ERR_FAILURE;
}

zos_err_t exists(const char* src) {
    zos_err_t err;
    zos_dev_t dev;
    zos_stat_t zos_stat;

    dev = opendir(src);
    if(dev == -ERR_NOT_A_DIR) {
        // maybe a file?
        err = stat(src, &zos_stat);
        if(err == ERR_NOT_A_FILE || err == ERR_NO_SUCH_ENTRY) {
            // not a file, must be an invalid path
            return ERR_INVALID_PATH;
        }
        if(err != ERR_SUCCESS) return err;
    }

    // we had another error
    if (dev < 0) return -dev;

    // we opened a dir, close it
    return close(dev);
}

zos_err_t is_dir(const char* src) {
    zos_dev_t err;

    err = opendir(src);
    if(err < 0) return -err;
    return close(err);
}

zos_err_t is_disk(char letter) {
    char path[] = " :/";
    if(letter > 'a') letter -= ('a' - 'A'); // to upper
    path[0] = letter;
    return is_dir(path);
}