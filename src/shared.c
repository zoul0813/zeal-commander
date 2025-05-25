#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <zos_errors.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_video.h>

#include "shared.h"
#include "keyboard.h"
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

char line[SCREEN_COL80_WIDTH];

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
    uint8_t color = COLOR(FG_MESSAGE, BG_MESSAGE);

    memset(line, 0, SCREEN_COL80_WIDTH);

    va_start(args, str);
    vsprintf(line, str, args);
    va_end(args);

    text_map_vram();
    for(uint8_t i = 0; i < SCREEN_COL80_WIDTH; i++) {
        TEXT_WRITE(win_NonePtr, i, MESSAGE_Y, line[i]);
        COLOR_WRITE(win_NonePtr, i, MESSAGE_Y, color);
    }
    text_demap_vram();
}

uint16_t input(const char* prefix, char* buffer, uint16_t len) {
    message(prefix);
    SET_XY(8, MESSAGE_Y);
    SET_COLORS(FG_MESSAGE, BG_MESSAGE);
    SET_CURSOR_BLINK(DEFAULT_CURSOR_BLINK);
    zos_err_t err = kb_mode_default();
    if(err != ERR_SUCCESS) {
        error(err, "kb_mode_default");
        return 0;
    }
    uint16_t size = strlen(buffer);
    err = write(DEV_STDIN, buffer, &size);
    if(err != ERR_SUCCESS) {
        error(err, "write stdin");
        return 0;
    }
    size = len;
    err = read(DEV_STDIN, buffer, &size);
    if(err != ERR_SUCCESS) {
        error(err, "read stdin");
        return 0;
    }

    char *p = buffer;
    while((*p != '\n') && (*p != 0)) p++;
    size = p - buffer;
    buffer[size] = 0;

    SET_CURSOR_BLINK(0);
    kb_mode_non_block_raw();

    return size;
}

void error(zos_err_t err, const char* str, ...) {
    // TODO: clear previous line
    va_list args;
    // cursor_xy(0, MESSAGE_Y);
    uint8_t color = COLOR(FG_ERROR, BG_MESSAGE);
    uint8_t i = 0;
    memset(line, 0, SCREEN_COL80_WIDTH);

    if(err < ERROR_STRINGS_LEN) {
        sprintf(line, "ERROR: %s ", ERROR_STRINGS[err]);
    } else {
        sprintf(line, "ERROR: %02x ", err);
    }
    uint8_t l = strlen(line);

    text_map_vram();
    for(i = 0; i < l; i++) {
        TEXT_WRITE(win_NonePtr, i, MESSAGE_Y, line[i]);
        COLOR_WRITE(win_NonePtr, i, MESSAGE_Y, color);
    }
    text_demap_vram();

    va_start(args, str);
    vsprintf(&line[l], str, args);
    va_end(args);

    color = COLOR(FG_MESSAGE, BG_MESSAGE);
    text_map_vram();
    for(; i < SCREEN_COL80_WIDTH; i++) {
        TEXT_WRITE(win_NonePtr, i, MESSAGE_Y, line[i]);
        COLOR_WRITE(win_NonePtr, i, MESSAGE_Y, color);
    }
    text_demap_vram();
}

int str_ends_with(const char *str, const char *suffix) {
    if(str == NULL || suffix == NULL) return 0;

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if(suffix_len > str_len) return 0;

    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

int16_t str_pos(const char* str, char c) {
    if (!str) return -1;
    const char* start = str;
    while (*str) {
        if (*str == c) {
            return (int16_t)(str - start);
        }
        ++str;
    }
    return -1;
}


