#include <stdio.h>
#include <string.h>

#include <zos_errors.h>
#include <zos_vfs.h>

#include "path.h"


/**
 * Return a resolved path, parsing `../` markers up a directory
 *
 * @note This will call `opendir()` at the end
 *
 * @param path The path to resolve
 * @param root The root path to resolve against, must contain an `X:/` drive root
 * @param resolved The string pointer to return the resolved path to
 *
 * @returns ERR_SUCCESS on success, error code else.
 */
zos_err_t path_resolve(const char* path, const char* root, char *resolved) {
    char a, b, c;
    uint8_t i = 0;
    uint8_t l = strlen(root);

    a = root[0];
    b = root[1];
    c = root[2];

    // root must contain an X:/ drive
    if(b != PATH_DRIVE && c != PATH_SEPARATOR) {
        return ERR_INVALID_PARAMETER;
    }

    // root is the current resolved path
    strcpy(resolved, root);

    l = strlen(resolved);

    // must have a drive root
    if(l < 3) return ERR_INVALID_PARAMETER;


    a = path[0];
    b = path[1];
    c = path[2];

    // path has a drive, use it as new root
    if(b == PATH_DRIVE && c == PATH_SEPARATOR) {
        resolved[0] = path[0];
        resolved[1] = path[1];
        resolved[2] = path[2];
        resolved[3] = NULL_TERM;
        i = 3;
        l = 3;
    }

    // if path starts with root, resolve to X:/
    if(a == PATH_SEPARATOR) {
        resolved[3] = NULL_TERM;
        i = 1; // skip the /
        l = 3; // start at X:/*
    }

    // resolve `..` parent markers
    for(;;) {
        a = path[i];
        if(a == NULL_TERM) break; // reached end of path
        b = path[i+1];
        c = path[i+2];

        // find `..`
        if(a == PATH_DOT && b == PATH_DOT && c == PATH_SEPARATOR) {
            // can't go above the drive root
            if(l < 4) return ERR_DISK_OFFSET;

            l--;
            if(resolved[l] == PATH_SEPARATOR) {
                if(l == 0) return ERR_INVALID_PATH;
                l--;
                resolved[l] = NULL_TERM;
            }

            for(;l > 0; l--) {
                if(resolved[l] == PATH_SEPARATOR) {
                    resolved[l] = NULL_TERM;
                    break;
                }
            }
            i += 2;
        } else if (a == PATH_DOT && (b == PATH_SEPARATOR || b == NULL_TERM)) {
            if(b == NULL_TERM) break;
            // resolve to current, so skip
            i += 2;
        } else {
            resolved[l] = a;
            l++;
            if(l >= PATH_MAX) {
                resolved[PATH_MAX - 1] = NULL_TERM; // make it printable
                return ERR_INVALID_PATH;
            }
            i++;
        }
    }

    resolved[l] = NULL_TERM;

    if(resolved[l-1] != PATH_SEPARATOR) {
        zos_dev_t err = opendir(resolved);
        if(err >= 0) close(err); // if we opened a dir, close it
        if(err == -ERR_NOT_A_DIR || err == -ERR_NO_SUCH_ENTRY) {
            return ERR_SUCCESS;
        }
        resolved[l++] = PATH_SEPARATOR;
        resolved[l] = NULL_TERM;
    }

    return ERR_SUCCESS;
}

zos_err_t path_concat(const char* path, const char* root, char *resolved) {
    strcpy(resolved, root);
    strcat(resolved, path);
    return ERR_SUCCESS;
}