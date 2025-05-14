#include <zos_errors.h>

#ifndef PATH_H
#define PATH_H

#define NULL_TERM       0x00
#define PATH_SEPARATOR  '/'
#define PATH_DRIVE      ':'
#define PATH_DOT        '.'
#define PATH_CURRENT    PATH_DOT
#define PATH_PARENT     '..'


zos_err_t path_resolve(const char* path, const char* root, char *resolved);
zos_err_t path_concat(const char* path, const char* root, char *resolved);
#endif