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

#include "view_help.h"

#define entry_get_focus() &list_focus->files[list_focus->selected - 1]
#define entry_get_blur() &list_focus->files[list_focus->selected - 1]

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
    // const char root[PATH_MAX];
    const char path[PATH_MAX];
    uint8_t len;
    zc_entry_t files[MAX_FILE_ENTRIES];
    uint8_t selected;
    window_t *window;
} zc_list_t;

View active_view = VIEW_NONE;
View previous_view = VIEW_NONE;

window_t win_ListingLeft = {
    .x = 0,
    .y = 1,
    .w = SCREEN_COL80_WIDTH/2,
    .h = SCREEN_COL80_HEIGHT - 3,
    .flags = WIN_BORDER | WIN_TITLE_LEFT,
    .fg = FG_SECONDARY,
    .bg = BG_SECONDARY,
    .fg_highlight = FG_SECONDARY_HIGHLIGHT,
    .title = "/"
};

window_t win_ListingRight = {
    .x = SCREEN_COL80_WIDTH/2,
    .y = 1,
    .w = SCREEN_COL80_WIDTH/2,
    .h = SCREEN_COL80_HEIGHT - 3,
    .flags = WIN_BORDER | WIN_TITLE_LEFT,
    .fg = FG_SECONDARY,
    .fg_highlight = FG_SECONDARY_HIGHLIGHT,
    .bg = BG_SECONDARY,
    .title = "/"
};


char original_path[PATH_MAX];
char path_src[PATH_MAX];
char path_dst[PATH_MAX];

zc_entry_t disks[6];
uint8_t disks_len = 0;

zos_stat_t zos_stat;
zc_list_t list_left;
zc_list_t list_right;

zc_list_t *list_focus = &list_left;
zc_list_t *list_blur = &list_right;

void view_switch(View view);
void toggle_view(View view);
void draw_screen(void);
void file_list_show(zc_list_t *list);
void file_list_highlight(zc_list_t *list);
void file_list_select(zc_list_t *list, int8_t index);
void file_list_disks(zc_list_t *list);
void execute(const char* path);

void view_switch(View view) {
    previous_view = active_view;
    cursor(0);
    switch(view) {
        case VIEW_NONE: {
            // TODO: refresh the main display
            draw_screen();
        } break;
        case VIEW_HELP: {
            view_draw_help();
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

void execute(const char* path) {

    err = ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);
    handle_error(err, "reset_screen", 1);

    err = kb_mode_default();
    handle_error(err, "reset keyboard", 1);

    uint8_t retval;
    exec(EXEC_PRESERVE_PROGRAM, path, NULL, &retval);

    if(retval != 0) {
        printf("\n\nExited with error $%02x\n", retval, retval);
    }
    printf("\n\nPress Enter to return to Zeal Commander...\n");

    err = kb_mode_non_block_raw();
    handle_error(err, "re-init keyboard", 1);

    do {
        key = getkey();
    } while(key != KB_KEY_ENTER);

    draw_screen();
}

void handle_keypress(char key) {
    // " [1] Help [4] Rename [5] Copy [6] Move [7] Mkdir [8] Delete [9] Refresh [10] Quit";
    switch(key) {
        // help
        case KB_F1: {
            toggle_view(VIEW_HELP);
        } break;
        // rename
        case KB_F4: {
            zc_entry_t *entry = entry_get_focus();
            path_resolve(entry->name, list_focus->path, path_src);
            path_resolve(entry->name, list_focus->path, path_dst);
            uint16_t l = input("rename: ", path_dst, PATH_MAX);
            move(path_src, path_dst);
            file_list_show(list_focus);
            message("rename: %s ... DONE!", path_dst);
        } break;
        // copy
        case KB_F5: {
            message("Copying...");
            zc_entry_t *entry_s = entry_get_focus();
            err = path_concat(entry_s->name, list_focus->path, path_src);
            if(err != ERR_SUCCESS) {
                error(err, " path_concat focus %s", entry_s->name);
                break;
            }

            err = path_concat(entry_s->name, list_blur->path, path_dst);
            if(err != ERR_SUCCESS) {
                error(err, "path_concat blur %s", entry_s->name);
                break;
            }

            err = copy(path_src, path_dst);
            if(err != ERR_SUCCESS) {
                error(err, "copy %s", path_src);
                break;
            }

            err = list(list_blur->path, list_blur->files, &list_blur->len);
            if(err != ERR_SUCCESS) {
                error(err, "list %s", path_src);
                break;
            }

            window_clrscr(list_blur->window);
            file_list_show(list_blur);
            message("Copying... DONE");
        } break;
        // move
        case KB_F6: {
            message("Moving...");
            zc_entry_t *entry_s = entry_get_focus();
            err = path_concat(entry_s->name, list_focus->path, path_src);

            err = path_concat(entry_s->name, list_blur->path, path_dst);

            if(err == ERR_SUCCESS) {
                move(path_src, path_dst);
            } else {
                error(err, "move %s", path_src);
            }

            list(list_focus->path, list_focus->files, &list_focus->len);
            window_clrscr(list_focus->window);
            file_list_show(list_focus);

            list(list_blur->path, list_blur->files, &list_blur->len);
            window_clrscr(list_blur->window);
            file_list_show(list_blur);
            message("Moving... DONE");
        } break;
        // delete
        case KB_F8: {
            message("Deleting...");
            zc_entry_t *entry = entry_get_focus();
            err = path_concat(entry->name, list_focus->path, path_src);
            if(err == ERR_SUCCESS) {
                remove(path_src);
            } else {
                error(err, "remove %s",path_src);
            }
            list(list_focus->path, list_focus->files, &list_focus->len);
            window_clrscr(list_focus->window); // TODO: instead of clearing the whole thing, just clear whats need to be cleaned up... ?
            file_list_show(list_focus);
            message("Deleting... DONE");
        } break;
        // mkdir
        case KB_F7: {
            char buffer[FILENAME_LEN_MAX];
            buffer[0] = 0;
            uint16_t l = input("mkdir: ", buffer, FILENAME_LEN_MAX);
            path_resolve(buffer, list_focus->path, path_dst);
            mkdir(path_dst);
            file_list_show(list_focus);
            message("mkdir: %s ... DONE!", buffer);
        } break;
        // refresh
        case KB_F9: {
            message("Refreshing...");
            file_list_show(list_focus);
            message("Refreshing... DONE");
        } break;
        // quit
        case KB_F10: __exit(ERR_SUCCESS); break;

        // view
        case KB_F3: // fall-thru for now into "action"

        // action
        case KB_KEY_ENTER: // fall-thru
        case KB_KEY_SPACE: {
            if(list_focus->selected == 0) {
                err = path_resolve("../", list_focus->path, path_src);
                if(err == ERR_DISK_OFFSET) {
                    file_list_disks(list_focus);
                    file_list_show(list_focus);
                    break;
                }
                if(err != ERR_SUCCESS) {
                    error(err, "resolve UP_DIR %s", path_src);
                    break;
                }
                strcpy(list_focus->path, path_src);
                file_list_show(list_focus);
                break;
            }

            zc_entry_t *entry = entry_get_focus();
            if((entry->flags & FileFlag_Disk) != 0) {
                // it's a disk
                strcpy(list_focus->path, entry->name);
            } else if((entry->flags & FileFlag_Executable) != 0) {
                // possible executable, execute it?
                path_resolve(entry->name, list_focus->path, path_src);
                message("Execute %s\n", path_src);
                execute(path_src);
            } else if((entry->flags & FileFlag_Directory) == 0) {
                error(ERR_NOT_A_DIR, "entry %s", list_focus->path);
                break;
            } else {
                err = path_resolve(entry->name, list_focus->path, path_src);
                if(err != ERR_SUCCESS) {
                    error(err, "resolve %s", list_focus->path);
                    break;
                }
                strcpy(list_focus->path, path_src);
            }

            list_focus->selected = 1;
            file_list_show(list_focus);
        } break;

        case KB_LEFT_ARROW: // fall-thru
        case KB_UP_ARROW: {
            file_list_select(list_focus, list_focus->selected - 1);
        } break;

        case KB_RIGHT_ARROW: // fall-thru
        case KB_DOWN_ARROW: {
            file_list_select(list_focus, list_focus->selected + 1);
        } break;

        case KB_PG_UP: {
            uint8_t selected = list_focus->selected;
            if(selected > LIST_PAGE_SIZE) selected -= LIST_PAGE_SIZE;
            else selected = 0;
            file_list_select(list_focus, selected);
        } break;
        case KB_PG_DOWN: {
            uint8_t selected = list_focus->selected;
            if(selected + LIST_PAGE_SIZE <= list_focus->len) selected += LIST_PAGE_SIZE;
            else selected = list_focus->len;
            file_list_select(list_focus, selected);
        } break;

        case KB_HOME: {
            file_list_select(list_focus, 0);
        } break;
        case KB_END: {
            file_list_select(list_focus, list_focus->len);
        } break;

        case KB_KEY_TAB: {
            zc_list_t *t = list_focus;
            list_focus = list_blur;
            list_blur = t;
            window_active(list_focus->window, 1);
            window_active(list_blur->window, 0);
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

    if(list->selected == 0) {
        // up dir
        err = path_resolve("../", list->path, path_src);
        message("%s", path_src);
        return;
    }

    zc_entry_t *entry = entry_get_focus();
    err = path_concat(entry->name, list_focus->path, path_src);
    if(err != ERR_SUCCESS) {
        error(err, "concat %s", path_src);
    } else {
        message("%s", path_src);
    }
}

void file_list_select(zc_list_t *list, int8_t index) {
    // (uint8_t) covers <0 and >len because neg is > 127
    if((uint8_t)index > list->len) return; // invalid
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

void file_list_show(zc_list_t *the_list) {
    if(strlen(the_list->path) > 0) {
        err = list(the_list->path, the_list->files, &the_list->len);
        if(err != ERR_SUCCESS) {
            error(err, "list %s",path_src);
            return;
        }
    }

    window_t *w = the_list->window;
    uint8_t i;
    uint8_t color = COLOR(FG_HEADING, BG_SECONDARY);
    const char str[FILENAME_LEN_MAX + 2];

    w->title = the_list->path;
    window(w);
    if(w == list_focus->window) {
        window_active(w, 1);
    } else {
        window_active(w, 0);
    }

    window_gotoxy(w, 0, 0);
    strcpy(str, "Name");
    window_puts_color(w, str, color);

    strcpy(str, "Size\n");
    window_gotox(w, w->w - 2 - strlen(str));
    window_puts_color(w, str, color);

    color = COLOR(FG_FOLDER, BG_SECONDARY);
    strcpy(str, "..");
    window_puts_color(w, str, color);

    strcpy(str, "UP-DIR\n");
    window_gotox(w, w->w - 2 - strlen(str));
    window_puts_color(w, str, color);


    for(i = 0; i < the_list->len; i++) {
        zc_entry_t *entry = &the_list->files[i];
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
        window_puts_color(w, str, color);

        if(entry->flags & FileFlag_File) {
            sprintf(str, "%lu%c\n", size, size_suffix);
        } else {
            strcpy(str, "-\n");
        }
        window_gotox(w, w->w - 2 - strlen(str));
        window_puts_color(w, str, color);
    }

    setcolor(FG_PRIMARY, BG_PRIMARY);


    if(the_list->selected > the_list->len) the_list->selected = the_list->len;
    file_list_highlight(the_list);
}

void file_list_disks(zc_list_t *the_list) {
    // show the disk list
    the_list->len = disks_len;
    strcpy(the_list->path, "");
    for(uint8_t i = 0; i < disks_len; i++) {
        memcpy(&the_list->files[i], &disks[i], sizeof(zc_entry_t));
    }
}

void init(void) {
    memset(&list_left, 0, sizeof(zc_list_t));
    list_left.window = &win_ListingLeft;

    memset(&list_right, 0, sizeof(zc_list_t));
    list_right.window = &win_ListingRight;

    memset(disks, 0, sizeof(zc_entry_t) * 6);

    // automatically make the first "entry" selected
    list_left.selected = 1;
    list_right.selected = 1;

    char letters[] = {'A','B','C','H','T'};
    for(uint8_t i = 0; i < sizeof(letters); i++) {
        char letter = letters[i];
        zos_err_t err = is_disk(letter);
        if(err == ERR_SUCCESS) {
            zc_entry_t *drive = &disks[disks_len];
            strcpy(drive->name, "A:/");
            drive->name[0] = letter;
            drive->flags = FileFlag_Disk;
            disks_len++;
        }
    }

    // set curdir as path
    strcpy(list_left.path, ".");
    strcpy(list_right.path, "../");

    // path_left
    err = path_resolve(list_left.path, original_path, path_src);
    if(err == ERR_DISK_OFFSET) {
        file_list_disks(&list_left);
    } else if(err != ERR_SUCCESS) {
        exit(err);
    } else {
        strcpy(list_left.path, path_src);
        err = is_dir(list_left.path);
        handle_error(err, "not a dir", 1);
    }

    // path_right
    err = path_resolve(list_right.path, original_path, path_src);
    if(err == ERR_DISK_OFFSET) {
        file_list_disks(&list_right);
    } else if(err != ERR_SUCCESS) {
        exit(err);
    } else {
        strcpy(list_right.path, path_src);
        err = is_dir(list_left.path);
        handle_error(err, "not a dir", 1);
    }
}

void draw_screen(void) {
    err = kb_mode_non_block_raw();
    handle_error(err, "init keyboard", 1);


    err = ioctl(DEV_STDOUT, CMD_CLEAR_SCREEN, NULL);
    handle_error(err, "clear_screen", 1);

    SET_CURSOR_BLINK(0);

    const char *menu_main = " File Options";
    setcolor(FG_MENU, BG_MENU); // inverted
    text_menu(0, 0, menu_main);

    const char *menu_file = "[1] Help [4] Ren [5] Copy [6] Move [7] Mkdir [8] Del [9] Update [10] Quit";
    setcolor(TEXT_COLOR_DARK_GRAY, FG_MENU); // inverted
    text_menu(0, SCREEN_COL80_HEIGHT-1, menu_file);

    file_list_show(&list_left);
    file_list_show(&list_right);
}

int main(void) {
    curdir(original_path); // record the original system path

    init();

    draw_screen();

    while(1) {
        key = getkey();
        handle_keypress(key);

        // // TODO: constantly resetting text scroll
        // zvb_peri_text_scroll_y = 0;
        // zvb_peri_text_scroll_x = 0;
    }

    // return 0; // unreachable
}