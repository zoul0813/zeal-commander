#include <stdio.h>
#include <string.h>
#include <zos_sys.h>
#include <zos_vfs.h>
#include <zos_errors.h>

#include "theme.h"

#include "keyboard.h"
#include "windows.h"
#include "shared.h"
#include "path.h"
#include "fs.h"

static zos_err_t err = ERR_SUCCESS;
unsigned char key = 0;
uint8_t mmu_page_current;

typedef enum {
    VIEW_NONE,
    VIEW_HELP,
    // VIEW_FILE_SAVE,
    // VIEW_FILE_LOAD,
} View;

typedef struct {
    const char root[PATH_MAX];
    const char path[PATH_MAX];
    uint8_t len;
    zc_entry_t files[MAX_FILE_ENTRIES];
} zc_list_t;

View active_view = VIEW_NONE;
View previous_view = VIEW_NONE;

window_t win_Main = {
  .x = 0,
  .y = 0,
  .w = SCREEN_COL80_WIDTH,
  .h = SCREEN_COL80_HEIGHT,
  .flags = WIN_NONE,
  .fg = FG_PRIMARY,
  .bg = BG_PRIMARY,
};

window_t win_Help = {
  .x = SCREEN_COL80_WIDTH / 4,
  .y = SCREEN_COL80_HEIGHT / 4,
  .w = SCREEN_COL80_WIDTH / 2,
  .h = SCREEN_COL80_HEIGHT / 2,
  .flags = WIN_BORDER | WIN_SHADOW,
  .fg = FG_SECONDARY,
  .bg = BG_SECONDARY,
  .title = "Help"
};

window_t win_ListingLeft = {
    .x = 0,
    .y = 1,
    .w = SCREEN_COL80_WIDTH/2,
    .h = SCREEN_COL80_HEIGHT - 2,
    .flags = WIN_BORDER,
    .fg = FG_SECONDARY,
    .bg = BG_SECONDARY,
    .title = "/"
};

window_t win_ListingRight = {
    .x = SCREEN_COL80_WIDTH/2,
    .y = 1,
    .w = SCREEN_COL80_WIDTH/2,
    .h = SCREEN_COL80_HEIGHT - 2,
    .flags = WIN_BORDER,
    .fg = FG_SECONDARY,
    .bg = BG_SECONDARY,
    .title = "/"
};


const char original_path[PATH_MAX];
zos_stat_t zos_stat;
zc_list_t list_left = {
    .root = "H:/",
    // .path = "test2/../test1/"
    .path = "H:/test2/../test1/../test2"
};
zc_list_t list_right = {
    .root = "H:/",
    // .path = "test1/"
    .path = "H:/test1/"
};

void view_switch(View view) {
    previous_view = active_view;
    cursor(0);
    switch(view) {
        case VIEW_NONE: {
            window_clrscr(&win_Main);
        } break;
        case VIEW_HELP: {
            // move this into a view!
            window(&win_Help);
            window_clrscr(&win_Help);
            window_puts(&win_Help, "Help Text\n");
            window_puts(&win_Help, "Line 2\n");
            window_puts(&win_Help, "Line 3\n");
            window_puts(&win_Help, "Line 4\n");
            window_puts(&win_Help, "Line 5\n");
            window_puts(&win_Help, "Line 6\twith\ttabs\n");
            window_puts(&win_Help, "Line 7a\twi\ttabs\n");
            // window_gotoxy(&win_Help, 0, 3);
            // window_clreol(&win_Help);
        } break;
    }
    active_view = view;
 }

 void toggle_view(View view) {
    if(active_view == view) {
        return view_switch(VIEW_NONE);
    }
    return view_switch(view);
 }

void handle_keypress(char key) {
    switch(key) {
        case KB_ESC: __exit(ERR_SUCCESS); break;
        case KB_F1: {
            toggle_view(VIEW_HELP);
        } break;
        default: {

        }
    }
}

void show_file_list(window_t *window, zc_list_t *list) {
    uint8_t i;
    for(i = 0; i < list->len; i++) {
        zc_entry_t *entry = &list->files[i];
        char prefix = ' ';
        uint8_t color = COLOR(FG_SECONDARY, BG_SECONDARY);
        if(entry->flags & FileFlag_Executable) {
            color = COLOR(FG_EXEC, BG_SECONDARY);
        } else if(entry->flags & FileFlag_Directory) {
            color = COLOR(FG_FOLDER, BG_SECONDARY);
            prefix = '/';
        }
        const char str[FILENAME_LEN_MAX + 2];
        sprintf(str, "%c%s\n", prefix, entry->name);
        // printf("%c%s\n", prefix, entry->name);
        window_puts_color(window, str, color);
    }
    setcolor(FG_PRIMARY, BG_PRIMARY);
}

int main(void) {
    printf("Zeal Commander\n");
    curdir(original_path); // record the original system path
    printf("current_path: %s\n", original_path);
    char path[PATH_MAX];

    // path_left
    path_resolve(list_left.path, original_path, path);
    strcpy(list_left.path, path);
    printf("list_left: %s\n", list_left.path);
    err = stat(list_left.path, &zos_stat);
    if(err != ERR_SUCCESS) {
        printf("stat:left: %s\n", list_left.path);
        exit(err);
    }
    printf("path:left : %s\n", list_left.path);
    // exit(0);

    // path_right
    path_resolve(list_right.path, original_path, path);
    strcpy(list_right.path, path);
    err = stat(list_left.path, &zos_stat);
    if(err != ERR_SUCCESS) {
        printf("stat:right: %s\n", list_right.path);
        exit(err);
    }
    printf("path:right: %s\n", list_right.path);
    // exit(0);

    win_ListingLeft.title = list_left.path;
    err = list(list_left.path, list_left.files, &list_left.len);
    if(err != ERR_SUCCESS) {
        handle_error(err, "opendir left", 1);
    }

    win_ListingRight.title = list_right.path;
    err = list(list_right.path, list_right.files, &list_right.len);
    if(err != ERR_SUCCESS) {
        handle_error(err, "opendir right", 1);
    }

    err = kb_mode((void *)(KB_READ_NON_BLOCK | KB_MODE_RAW));
    handle_error(err, "init keyboard", 1);


    err = ioctl(DEV_STDOUT, CMD_CLEAR_SCREEN, NULL);
    if(err != ERR_SUCCESS) {
        handle_error(err, "clear_screen", 1);
    }

    SET_CURSOR_BLINK(0);

    window(&win_Main);

    const char *menu_main = "File Options";
    setcolor(BG_MENU, FG_MENU); // inverted
    text_menu(0, 0, menu_main);

    const char *menu_file = "[F1] Help [F2] Copy [F3] Move [F4] Rename [F5] Delete";
    setcolor(FG_MENU, BG_MENU); // inverted
    text_menu(0, SCREEN_COL80_HEIGHT-1, menu_file);

    window(&win_ListingLeft);
    window(&win_ListingRight);

    show_file_list(&win_ListingLeft, &list_left);
    show_file_list(&win_ListingRight, &list_right);

    while(1) {
        key = getkey();
        handle_keypress(key);
    }

    // return 0; // unreachable
}