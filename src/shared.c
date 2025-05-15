#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_video.h>

#include "shared.h"
#include "windows.h"

const char *ERROR_STRINGS[] = {
    "ERR_SUCCESS",
    "ERR_FAILURE",
    "ERR_NOT_IMPLEMENTED",
    "ERR_NOT_SUPPORTED",
    "ERR_NO_SUCH_ENTRY",
    "ERR_INVALID_SYSCALL",
    "ERR_INVALID_PARAMETER",
    "ERR_INVALID_VIRT_PAGE",
    "ERR_INVALID_PHYS_ADDRESS",
    "ERR_INVALID_OFFSET",
    "ERR_INVALID_NAME",
    "ERR_INVALID_PATH",
    "ERR_INVALID_FILESYSTEM",
    "ERR_INVALID_FILEDEV",
    "ERR_PATH_TOO_LONG",
    "ERR_ALREADY_EXIST",
    "ERR_ALREADY_OPENED",
    "ERR_ALREADY_MOUNTED",
    "ERR_READ_ONLY",
    "ERR_BAD_MODE",
    "ERR_CANNOT_REGISTER_MORE",
    "ERR_NO_MORE_ENTRIES",
    "ERR_NO_MORE_MEMORY",
    "ERR_NOT_A_DIR",
    "ERR_NOT_A_FILE",
    "ERR_ENTRY_CORRUPTED",
    "ERR_DIR_NOT_EMPTY",
};
const uint8_t ERROR_STRINGS_LEN = sizeof(ERROR_STRINGS) / sizeof(ERROR_STRINGS[0]);


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
    // TODO: clear previous line
    va_list args;
    cursor_xy(0, SCREEN_COL80_HEIGHT-2);
    setcolor(FG_MESSAGE, BG_MESSAGE);

    va_start(args, str);
    vprintf(str, args);
    va_end(args);
    printf("%24s\n", ""); // this is dumb
}

void error(zos_err_t err, const char* str, ...) {
    // TODO: clear previous line
    va_list args;
    cursor_xy(0, SCREEN_COL80_HEIGHT-2);
    setcolor(FG_ERROR, BG_MESSAGE);

    if(err < ERROR_STRINGS_LEN) {
        printf("ERROR: %s ", ERROR_STRINGS[err]);
    } else {
        printf("ERROR: %02x ", err);
    }

    va_start(args, str);
    vprintf(str, args);
    va_end(args);
    printf("%24s\n"); // this is dumb
}

int str_ends_with(const char *str, const char *suffix) {
    if(str == NULL || suffix == NULL) return 0;

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if(suffix_len > str_len) return 0;

    return strcmp(str + str_len - suffix_len, suffix) == 0;
}