#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_video.h>

#include "shared.h"
#include "windows.h"

char textbuff[SCREEN_COL80_WIDTH];

int __exit(zos_err_t err) {
  if(err == ERR_SUCCESS) err = ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);
  exit(err);
  return err;
}

void handle_error(zos_err_t err, char *msg, uint8_t fatal) {
  if(err != ERR_SUCCESS) {
    // cursor_xy(2,20); // why does ZMT move the cursor???
    printf("failed to %s, %d (%02x)\n", msg, err, err);
    if(fatal) __exit(err);
  }
}

void message(const char* str, ...) {
    va_list args;
    cursor_xy(0, SCREEN_COL80_HEIGHT-2);
    setcolor(FG_MESSAGE, BG_MESSAGE);

    va_start(args, str);
    vprintf(str, args);
    va_end(args);
    printf("%24s\n", "");
}

int str_ends_with(const char *str, const char *suffix) {
    if(str == NULL || suffix == NULL) return 0;

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if(suffix_len > str_len) return 0;

    return strcmp(str + str_len - suffix_len, suffix) == 0;
}