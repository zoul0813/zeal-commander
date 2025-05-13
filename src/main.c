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
    uint8_t selected;
    window_t *window;
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
    .flags = WIN_BORDER | WIN_TITLE_LEFT,
    .fg = FG_SECONDARY,
    .bg = BG_SECONDARY,
    .title = "/"
};

window_t win_ListingRight = {
    .x = SCREEN_COL80_WIDTH/2,
    .y = 1,
    .w = SCREEN_COL80_WIDTH/2,
    .h = SCREEN_COL80_HEIGHT - 2,
    .flags = WIN_BORDER | WIN_TITLE_LEFT,
    .fg = FG_SECONDARY,
    .bg = BG_SECONDARY,
    .title = "/"
};


const char original_path[PATH_MAX];
zos_stat_t zos_stat;
zc_list_t list_left = {
    .root = "H:/",
    .path = "/test2/../test1/../test2",
    .selected = 2,
    .window = &win_ListingLeft,
};
zc_list_t list_right = {
    .root = "H:/",
    // .path = "test1/"
    .path = "H:/test1/",
    .selected = 1,
    .window = &win_ListingRight,
};

zc_list_t *list_current = &list_left;

void view_switch(View view);
void toggle_view(View view);
void show_file_list(zc_list_t *list);
void file_list_highlight(zc_list_t *list);
void file_list_select(zc_list_t *list, int8_t index);

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
        case KB_F10: __exit(ERR_SUCCESS); break;
        case KB_F1: {
            toggle_view(VIEW_HELP);
        } break;
        case KB_UP_ARROW: {
            file_list_select(list_current, list_current->selected - 1);
        } break;
        case KB_DOWN_ARROW: {
            file_list_select(list_current, list_current->selected + 1);
        } break;
        case KB_KEY_TAB: {
            if(list_current == &list_left) {
                list_current = &list_right;
            } else {
                list_current = &list_left;
            }
        } break;
        default: {

        }
    }
}

void file_list_highlight(zc_list_t *list) {

    uint8_t min_x = list->window->x + 1;
    uint8_t max_x = list->window->x + list->window->w - 2;
    uint8_t y = list->window->y + list->selected + 2;  // title + column heading
    uint8_t x;

    text_map_vram();
    for(x = min_x; x < max_x; x++) {
        SCR_COLOR[y][x] = COLOR(TEXT_COLOR_WHITE, TEXT_COLOR_DARK_BLUE);
    }
    text_demap_vram();
}

void file_list_select(zc_list_t *list, int8_t index) {
    if(index < 0) return; // invalid
    if(index > list->len) return; // invalid
    zc_entry_t *previous = &list->files[list->selected - 1]; // -1 because `..` isn't in the list
    zc_entry_t *next = &list->files[index];

    uint8_t color = COLOR(FG_SECONDARY, BG_SECONDARY);
    uint8_t min_x = list->window->x + 1;
    uint8_t max_x = list->window->x + list->window->w - 2;
    uint8_t y = list->window->y + list->selected + 2;  // title + column heading
    uint8_t x;

    if(list->selected == 0) {
        // UP-DIR
        color = COLOR(FG_FOLDER, BG_SECONDARY);
    } else if(previous->flags & FileFlag_Directory) {
        color = COLOR(FG_FOLDER, BG_SECONDARY);
    } else if(previous->flags & FileFlag_Executable) {
        color = COLOR(FG_EXEC, BG_SECONDARY);
    }
    text_map_vram();
    for(x = min_x; x < max_x; x++) {
        SCR_COLOR[y][x] = color;
    }
    text_demap_vram();

    list->selected = index;
    file_list_highlight(list);
}

void show_file_list(zc_list_t *list) {
    window_t *window = list->window;
    uint8_t i;
    uint8_t color = COLOR(FG_HEADING, BG_SECONDARY);
    const char str[FILENAME_LEN_MAX + 2];

    sprintf(str, "Name");
    window_puts_color(window, str, color);

    sprintf(str, "Size\n");
    window_gotox(window, window->w - 2 - strlen(str));
    window_puts_color(window, str, color);

    color = COLOR(FG_FOLDER, BG_SECONDARY);
    sprintf(str, "..");
    window_puts_color(window, str, color);

    sprintf(str, "UP-DIR\n");
    window_gotox(window, window->w - 2 - strlen(str));
    window_puts_color(window, str, color);


    for(i = 0; i < list->len; i++) {
        zc_entry_t *entry = &list->files[i];
        char prefix = ' ';
        char suffix = NULL_TERM;
        color = COLOR(FG_SECONDARY, BG_SECONDARY);
        if(entry->flags & FileFlag_Executable) {
            color = COLOR(FG_EXEC, BG_SECONDARY);
            suffix = '*';
        } else if(entry->flags & FileFlag_Directory) {
            color = COLOR(FG_FOLDER, BG_SECONDARY);
            prefix = '/';
        }

        uint32_t size = entry->size;
        char size_suffix = 'B';
        if(size > KILOBYTE) {
            size = size / KILOBYTE;
            size_suffix = 'K';
        }

        sprintf(str, "%c%s%c", prefix, entry->name, suffix);
        window_puts_color(window, str, color);

        if(entry->flags & FileFlag_File) {
            sprintf(str, "%lu%c\n", size, size_suffix);
        } else {
            sprintf(str, "-\n");
        }
        window_gotox(window, window->w - 2 - strlen(str));
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
    // printf("path_resolve:left\n");
    err = path_resolve(list_left.path, original_path, path);
    if(err != ERR_SUCCESS) {
        printf("path_resolve: %d\n", err);
        exit(err);
    }
    strcpy(list_left.path, path);
    // printf("list_left: %s\n", list_left.path);
    err = stat(list_left.path, &zos_stat);
    if(err != ERR_SUCCESS) {
        printf("stat:left: %s\n", list_left.path);
        exit(err);
    }
    // printf("path:left : %s\n", list_left.path);
    // exit(0);

    // path_right
    // printf("path_resolve:right\n");
    err = path_resolve(list_right.path, original_path, path);
    if(err != ERR_SUCCESS) {
        printf("path_resolve: %d\n", err);
        exit(err);
    }
    strcpy(list_right.path, path);
    err = stat(list_left.path, &zos_stat);
    if(err != ERR_SUCCESS) {
        printf("stat:right: %s\n", list_right.path);
        exit(err);
    }
    // printf("path:right: %s\n", list_right.path);
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
    setcolor(FG_MENU, BG_MENU); // inverted
    text_menu(0, 0, menu_main);

    const char *menu_file = "[F1] Help [F2] Copy [F3] Move [F4] Rename [F5] Delete [F10] Quit";
    setcolor(BG_MENU, FG_MENU); // inverted
    text_menu(0, SCREEN_COL80_HEIGHT-1, menu_file);

    uint8_t columns[] = { 24 };

    window_columns(&win_ListingLeft, columns, 1);
    window_columns(&win_ListingRight, columns, 1);

    show_file_list(&list_left);
    show_file_list(&list_right);

    file_list_highlight(&list_left);
    file_list_highlight(&list_left);

    while(1) {
        key = getkey();
        handle_keypress(key);
    }

    // return 0; // unreachable
}