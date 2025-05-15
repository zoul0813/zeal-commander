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
    if(strlen(path) < 1) return ERR_INVALID_PARAMETER;
    if(strlen(root) < 3) return ERR_INVALID_PARAMETER; // must be a drive root

    char a, b, c;

    // goto the root of root
    a = root[0];
    b = root[1];
    c = root[2];
    if(b != PATH_DRIVE && b != PATH_SEPARATOR) {
        // root must contain an X:/ drive
        return ERR_INVALID_PARAMETER;
    }

    // copy the root to the resolved
    strcpy(resolved, root);

    uint8_t i = 0;
    uint8_t l = strlen(resolved);
    if(l == 0) return ERR_INVALID_PARAMETER;

    // if path starts with root, resolve to X:/
    if(path[0] == PATH_SEPARATOR) {
        resolved[3] = NULL_TERM;
        i = 1;
        l = 3;
    }

    a = path[0];
    b = path[1];
    c = path[2];
    // path has a drive, use it as new root
    if(b == PATH_DRIVE && c == PATH_SEPARATOR) {
        resolved[0] = path[0];
        resolved[1] = path[1];
        resolved[2] = path[2];
        i = 3;
        l = 3;
    }

    // resolve `..` parent markers
    do {
        a = path[i];
        if(a == NULL_TERM) break; // reached end of path
        b = path[i+1];
        c = path[i+2];

        // if(b == PATH_DRIVE && c == PATH_SEPARATOR) {
        //     // new root is the drive
        //     sprintf(resolved, "%c%c%c", a, b, c);
        //     i += 3; // move the pointer past the X:/ drive
        //     continue;
        // }

        // find `..`
        if(a == PATH_DOT && b == PATH_DOT && c == PATH_SEPARATOR) {
            // can't go above the drive root
            if(l < 3) return ERR_INVALID_PATH;

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
        } else {
            resolved[l] = a;
            l++;
            if(l >= PATH_MAX) {
                resolved[PATH_MAX - 1] = NULL_TERM; // make it printable
                return ERR_INVALID_PATH;
            }
            i++;
        }
    } while(1);

    resolved[l] = NULL_TERM;

    if(resolved[l-1] != PATH_SEPARATOR) {
        zos_dev_t err = opendir(resolved);
        if(err >= 0) close(err); // if we opened a dir, close it
        if(err == -ERR_NOT_A_DIR) {
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