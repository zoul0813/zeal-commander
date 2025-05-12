#include <stdio.h>
#include <string.h>
#include <zos_errors.h>
#include <zos_vfs.h>
#include "path.h"


zos_err_t path_resolve(const char* path, const char* root, char *resolved) {
    printf("path:a: %s\n", path);
    // copy the root to the resolved
    strcpy(resolved, root);

    uint8_t l = strlen(resolved);
    if(l == 0) return ERR_INVALID_PARAMETER;
    printf("resolved:a: %s\n", resolved);

    // resolve `..` parent markers
    uint8_t i = 0;
    do {
        char a = path[i];
        if(a == NULL_TERM) break; // reached end of path
        char b = path[i+1];
        char c = path[i+2];

        if(b == PATH_DRIVE && c == PATH_SEPARATOR) {
            // new root is the drive
            sprintf(resolved, "%c%c%c", a, b, c);
            i += 3; // move the pointer past the X:/ drive
            printf("path:abs: %s\n", resolved);
            continue;
        }

        printf("abc: %c%c%c %d %d\n", a, b, c, i, l);

        // find `..`
        if(a == PATH_DOT && b == PATH_DOT && c == PATH_SEPARATOR) {
            // skip the trailing /
            printf("parent:z: %s %d\n", resolved, l);
            printf("parent:char: %d\n", resolved[l]);
            l--;
            if(resolved[l] == PATH_SEPARATOR) {
                l--;
                resolved[l] = NULL_TERM;
            }
            printf("parent:a: %s %d\n", resolved, l);
            for(;l > 0; l--) {
                if(resolved[l] == PATH_SEPARATOR) {
                    resolved[l] = NULL_TERM;
                    break;
                }
            }
            printf("parent:b: %s %d\n", resolved, l);
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
    printf("resolved:b: %s\n", resolved);

    if(resolved[l-1] != PATH_SEPARATOR) {
        zos_err_t err = opendir(resolved);
        if(err == ERR_NOT_A_DIR) {
            return resolved;
        }
        resolved[l++] = PATH_SEPARATOR;
        resolved[l] = NULL_TERM;
    }

    return ERR_SUCCESS;
}