#include <stdint.h>
#include <zos_errors.h>
#include <zos_video.h>
#include "windows.h"

#define MESSAGE_Y           (SCREEN_COL80_HEIGHT-2)

#define LIST_PAGE_SIZE      10
#define LIST_VIEW_SIZE      34
#define LIST_MAX_ENTRIES    256

extern const char *ERROR_STRINGS[ERR_DIR_NOT_EMPTY+1];
extern const uint8_t ERROR_STRINGS_LEN;

int __exit(zos_err_t err);
void handle_error(zos_err_t err, char *msg, uint8_t fatal);
void message(const char* str, ...);
uint16_t input(const char* prefix, char* buffer, uint16_t len);
void error(zos_err_t err, const char* str, ...);
int str_ends_with(const char *str, const char *suffix);
int16_t str_pos(const char* str, char c);
