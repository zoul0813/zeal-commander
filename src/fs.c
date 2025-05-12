#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <zos_errors.h>
#include <zos_vfs.h>

#include "fs.h"
#include "shared.h"


// zc_entry_t files[MAX_FILE_ENTRIES] = {{0}};
// uint8_t files_len = 0;

zos_err_t tree(const char* path, int depth) {
    zos_dev_t d = opendir(path);
    if(d < 0) return -d;

    zos_err_t err;
    zos_dir_entry_t entry;
    zos_stat_t entry_stat;
    while((err = readdir(d, &entry)) == ERR_SUCCESS) {
        uint32_t filesize = 0;
        uint32_t filesize_format = 0;
        char filesize_suffix = 'B';

        // assume dir, read, write, exec privs
        char c_dir = 'd';
        char c_read = 'r';
        char c_write = 'w';
        char c_exec = 'x';

        if(D_ISFILE(entry.d_flags)) {
            c_dir = '-'; // it's not a dir
            err = stat(entry.d_name, &entry_stat);
            filesize = entry_stat.s_size;
            filesize_format = filesize;

            if(filesize >= KILOBYTE) {
                filesize_suffix = 'K';
                filesize_format = filesize / KILOBYTE;
            }

            if(!str_ends_with(entry.d_name, ".bin")) {
                // assume it's not executable because it isn't a "bin"
                c_exec = '-';
            }
        }

        setcolor(FG_PRIMARY, BG_PRIMARY);

        if(depth != 0) {
            printf("%c", CH_TREE_TRUNK);
            for(uint8_t i = depth; i > 0; i--) {
                if(i < depth) {
                    printf("%c", CH_TREE_TRUNK);
                }
                printf("   ");
            }
        }
        printf("%c%c ", CH_TREE_BRANCH, CH_TREE_LEAF);
        if(D_ISFILE(entry.d_flags)) {
            printf("[%" PRIu32 "%c] ", filesize_format, filesize_suffix);
        }
        if(c_dir == 'd') {
            setcolor(FG_FOLDER, BG_PRIMARY);
        } else if(c_exec == 'x') {
            setcolor(FG_EXEC, BG_PRIMARY);
        } else {
            setcolor(FG_PRIMARY, BG_PRIMARY);
        }
        printf("%s ", entry.d_name);
        // printf(" %c%c%c%c", c_dir, c_read, c_write, c_exec);
        printf("\n");

        if(D_ISDIR(entry.d_flags)) {
            chdir(entry.d_name);
            tree(".", depth+1);
            chdir(path);
        }
    }
    if(err != ERR_NO_MORE_ENTRIES) {
        printf("ERROR[%d 0x%02x]: error occured");
        handle_error(err, "readdir", 1);
    }

    return close(d);
}

zos_err_t list(const char* path, zc_entry_t* list, uint8_t* size) {
    zos_dev_t d = opendir(path);
    if(d < 0) {
        printf("opendir err\n");
        return -d;
    }

    zos_err_t err;
    zos_dir_entry_t dir_entry;
    zos_stat_t entry_stat;

    uint8_t i;
    for(i = 0; i < MAX_FILE_ENTRIES; i++) {
        err = readdir(d, &dir_entry);
        if(err == ERR_NO_MORE_ENTRIES) break;
        if(err != ERR_SUCCESS) {
            printf("readdir err\n");
            return err;
        }

        zc_entry_t *entry = &list[i];
        memcpy(entry->name, dir_entry.d_name, FILENAME_LEN_MAX);
        if(D_ISFILE(dir_entry.d_flags)) {
            char filename[PATH_MAX] = {0};
            sprintf(filename, "%s%s", path, dir_entry.d_name);
            err = stat(filename, &entry_stat);
            if(err != ERR_SUCCESS) {
                printf("stat err: %s\n", filename);
                return err;
            }

            entry->flags = FileFlag_File;
            entry->size = entry_stat.s_size;
            memcpy(&entry->date, &entry_stat.s_date, sizeof(zos_date_t));

            if(str_ends_with(entry->name, ".bin")) {
                entry->flags |= FileFlag_Executable;
            }
        } else {
            entry->size = 0;
            entry->flags = FileFlag_Directory;
            memset(&entry->date, 0, sizeof(zos_date_t));
        }
    }
    *size = i;

    err = close(d);
    if(err != ERR_SUCCESS) {
        printf("close err\n");
        return err;
    }
    return ERR_SUCCESS;
}

zos_err_t remove(const char* path)
{
    (void *)path;
    return ERR_FAILURE;
}

zos_err_t move(const char* src, const char* dst)
{
    (void *)src;
    (void *)dst;
    return ERR_FAILURE;
}

zos_err_t copy(const char* src, const char* dst)
{
    (void *)src;
    (void *)dst;
    return ERR_FAILURE;
}

zos_err_t rename(const char* src, const char* dst) {
    (void *)src;
    (void *)dst;
    return ERR_FAILURE;
}