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

#define entry_get_focus() &list_focus->files[list_focus->selected]
#define entry_get_blur() &list_focus->files[list_focus->selected]

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
    uint8_t offset; // offset into files[]
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
void file_list_offset(zc_list_t* list);
void file_list_render(zc_list_t* list);
void file_list_highlight(zc_list_t *list, uint8_t color);
void file_list_message(zc_list_t *list);
void file_list_select(zc_list_t *list, uint8_t index);
void file_list_disks(zc_list_t *list);
zos_err_t execute(const char* path);

void view_switch(View view) {
    previous_view = active_view;
    SET_CURSOR_BLINK(0);
    switch(view) {
        case VIEW_NONE: {
            window_refresh();
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

zos_err_t execute(const char* path) {

    err = ioctl(DEV_STDOUT, CMD_RESET_SCREEN, NULL);
    handle_error(err, "reset_screen", 1);

    err = kb_mode_default();
    handle_error(err, "reset keyboard", 1);

    uint8_t retval;
    err = exec(EXEC_PRESERVE_PROGRAM, path, NULL, &retval);
    if(err != ERR_SUCCESS) return err;

    if(retval != 0) {
        printf("\n\nExited with error $%02x\n", retval, retval);
    }
    printf("\n\nPress Enter to return to Zeal Commander...\n");

    SET_CURSOR_BLINK(0);
    err = kb_mode_non_block_raw();
    handle_error(err, "re-init keyboard", 1);

    do {
        key = getkey();
    } while(key != KB_KEY_ENTER);

    window_refresh();
    return ERR_SUCCESS;
}

zos_err_t setfont(const char* path) {
    zos_dev_t dev = open(path, O_RDONLY);
    if(dev < 0) {
        // put_s(path); put_s(" not found\n");
        error(err, "not found %s", path);
        return -dev;
    }

    uint8_t buffer[SCREEN_FONT_SIZE];

    uint16_t size = SCREEN_FONT_SIZE;
    err = read(dev, buffer, &size);
    if(err != ERR_SUCCESS) {
        // put_s(path); put_s(" failed to read\n");
        error(err, "failed to read %s", path);
        return err;
    }
    err = close(dev);
    if(err != ERR_SUCCESS) {
        // put_s(path); put_s(" failed to close\n");
        error(err, "failed to close %s", path);
        return err;
    }

    text_map_vram();
    memcpy(SCR_FONT, buffer, size);
    text_demap_vram();
    return ERR_SUCCESS;
}

void handle_keypress(char key) {
    // " [1] Help [4] Rename [5] Copy [6] Move [7] Mkdir [8] Delete [9] Refresh [10] Quit";

    switch(key) {
        // help
        case KB_F1: {
            toggle_view(VIEW_HELP);
        } break;
        // case KB_F11: {
        //     window_refresh();
        // } break;
    }

    if(active_view != VIEW_NONE) return;

    switch(key) {
        // rename
        case KB_F4: {
            zc_entry_t *entry = entry_get_focus();
            err = path_resolve(entry->name, list_focus->path, path_src);
            if(err != ERR_SUCCESS) {
                error(err, "path_resolve path_src %s", path_src);
                break;
            }
            err = path_resolve(entry->name, list_focus->path, path_dst);
            if(err != ERR_SUCCESS) {
                error(err, "path_resolve path_dst %s", path_src);
                break;
            }
            uint16_t l = input("rename: ", path_dst, PATH_MAX);
            err = move(path_src, path_dst);
            if(err != ERR_SUCCESS) {
                error(err, "copy %s", path_src);
                break;
            }
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
            if(err != ERR_SUCCESS) {
                error(err, "path_concat path_src %s", path_src);
                break;
            }
            err = path_concat(entry_s->name, list_blur->path, path_dst);
            if(err != ERR_SUCCESS) {
                error(err, "path_concat path_src %s", path_src);
                break;
            }

            err = move(path_src, path_dst);
            if(err != ERR_SUCCESS) {
                error(err, "move %s", path_src);
                break;
            }

            err = list(list_focus->path, list_focus->files, &list_focus->len);
            if(err != ERR_SUCCESS) {
                error(err, "list %s", path_src);
                break;
            }
            window_clrscr(list_focus->window);
            file_list_show(list_focus);

            err = list(list_blur->path, list_blur->files, &list_blur->len);
            if(err != ERR_SUCCESS) {
                error(err, "list %s", path_src);
                break;
            }
            window_clrscr(list_blur->window);
            file_list_show(list_blur);
            message("Moving... DONE");
        } break;
        // delete
        case KB_F8: {
            message("Deleting...");
            zc_entry_t *entry = entry_get_focus();
            err = path_concat(entry->name, list_focus->path, path_src);
            if(err != ERR_SUCCESS) {
                error(err, "path_concat %s", path_src);
                break;
            }

            err = remove(path_src);
            if(err != ERR_SUCCESS) {
                error(err, "remove %s", path_src);
                break;
            }

            err = list(list_focus->path, list_focus->files, &list_focus->len);
            if(err != ERR_SUCCESS) {
                error(err, "list %s", path_src);
                break;
            }

            window_clrscr(list_focus->window); // TODO: instead of clearing the whole thing, just clear whats need to be cleaned up... ?
            file_list_show(list_focus);
            message("Deleting... DONE");
        } break;
        // mkdir
        case KB_F7: {
            char buffer[FILENAME_LEN_MAX];
            buffer[0] = 0;
            uint16_t l = input("mkdir: ", buffer, FILENAME_LEN_MAX);
            err = path_resolve(buffer, list_focus->path, path_dst);
            if(err != ERR_SUCCESS) {
                error(err, "path_resolve %s", path_src);
                break;
            }

            err = mkdir(path_dst);
            if(err != ERR_SUCCESS) {
                error(err, "copy %s", path_src);
                break;
            }
            file_list_show(list_focus);
            message("mkdir: %s ... DONE! %d", buffer, l);
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
            // TODO: re-evaluate this, now that `..` is the first entry in dir lists
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
                err = path_resolve(entry->name, list_focus->path, path_src);
                if(err != ERR_SUCCESS) {
                    error(err, "exec resolve");
                }
                message("Execute %s", path_src);
                err = execute(path_src);
                if(err != ERR_SUCCESS) {
                    error(err, "Failed to execute");
                }
                break;
            } else if((entry->flags & FileFlag_File)) {
                // it's a file, ... can we do something with it?
                uint8_t l = strlen(list_focus->path);
                if(str_ends_with(entry->name, ".f12")) {
                    path_resolve(entry->name, list_focus->path, path_src);
                    // it's a font file, switch fonts?
                    err = setfont(path_src);
                    if(err != ERR_SUCCESS) {
                        error(err, "setfont %s", list_focus->path);
                    } else {
                        message("Font: %s", list_focus->path);
                    }
                    break;
                }
                // we don't know what to do with it ...
                error(ERR_NOT_A_DIR, "entry %s", &list_focus->path[l-4]);
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
            if(list_focus->selected > 0) {
                file_list_select(list_focus, list_focus->selected - 1);
            }
        } break;

        case KB_RIGHT_ARROW: // fall-thru
        case KB_DOWN_ARROW: {
            if(list_focus->selected < list_focus->len) {
                file_list_select(list_focus, list_focus->selected + 1);
            }
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
            else selected = list_focus->len - 1;
            file_list_select(list_focus, selected);
        } break;

        case KB_HOME: {
            file_list_select(list_focus, 0);
        } break;
        case KB_END: {
            file_list_select(list_focus, list_focus->len - 1);
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

void file_list_highlight(zc_list_t *list, uint8_t color) {

    uint8_t min_x = list->window->x + 1;
    uint8_t max_x = list->window->x + list->window->w - 1;
    uint8_t y = list->window->y + list->selected + 2;  // title + column heading
    y -= list->offset;
    uint8_t x;

    text_map_vram();
    for(x = min_x; x < max_x; x++) {
        COLOR_WRITE(list->window, x, y, color);
    }
    text_demap_vram();
}

void file_list_message(zc_list_t *list) {
    (void)list;
    zc_entry_t *entry = entry_get_focus();

    strcpy(path_dst, entry->name);
    if(entry->flags & FileFlag_Directory) strcat(path_dst, "/");
    err = path_resolve(path_dst, list_focus->path, path_src);
    if(err != ERR_SUCCESS) {
        error(err, "concat %s", path_src);
    }

    message("%s", path_src);
}

void file_list_select(zc_list_t *list, uint8_t index) {
    // (uint8_t) covers <0 and >len because neg is > 127
    if((uint8_t)index >= list->len) return; // invalid
    zc_entry_t *previous = &list->files[list->selected]; // -1 because `..` isn't in the list
    zc_entry_t *next = &list->files[index];

    uint8_t color = COLOR(FG_SECONDARY, BG_SECONDARY);
    if(previous->flags & FileFlag_Directory) {
        color = COLOR(FG_FOLDER, BG_SECONDARY);
    } else if(previous->flags & FileFlag_Executable) {
        color = COLOR(FG_EXEC, BG_SECONDARY);
    }
    file_list_highlight(list, color);

    list->selected = index;
    file_list_offset(list);

    file_list_highlight(list, COLOR(FG_PRIMARY_HIGHLIGHT, BG_PRIMARY));
    file_list_message(list);
}

void file_list_offset(zc_list_t* list) {
    uint8_t current_page = list->offset / LIST_VIEW_SIZE;
    uint8_t target_page = list->selected / LIST_VIEW_SIZE;
    if(current_page != target_page) {
        list->offset = target_page * LIST_VIEW_SIZE;
        file_list_render(list);
    }
}

void file_list_render(zc_list_t* list) {
    uint8_t color, i;
    // const char str[FILENAME_LEN_MAX + 2];
    zc_entry_t *entry;

    window_t *w = list->window;
    uint8_t len = list->len - list->offset;

    color = COLOR(FG_HEADING, BG_SECONDARY);
    window_gotoxy(w, 0, 0);
    strcpy(path_src, "Name");
    window_puts_color(w, path_src, color);

    strcpy(path_src, "Size\n");
    window_gotox(w, w->w - 2 - strlen(path_src));
    window_puts_color(w, path_src, color);

    if(len > LIST_VIEW_SIZE) len = list->offset + LIST_VIEW_SIZE;
    else len += list->offset;
    for(i = list->offset; i < len; i++) {
        entry = &list->files[i];

        // filetype
        char prefix = ' ';
        color = COLOR(FG_SECONDARY, BG_SECONDARY);
        if(entry->flags & FileFlag_Executable) {
            color = COLOR(FG_EXEC, BG_SECONDARY);
            prefix = CH_DOT;
        } else if(entry->flags & FileFlag_Directory) {
            color = COLOR(FG_FOLDER, BG_SECONDARY);
            prefix = '/';
        }

        // filename
        sprintf(path_src, "%c%-16s", prefix, entry->name);
        window_puts_color(w, path_src, color);

        // filesize
        uint32_t size = entry->size;
        char size_suffix = 'B';
        if(size > KILOBYTE) {
            size = size / KILOBYTE;
            size_suffix = 'K';
        }
        if(entry->flags & FileFlag_File) {
            sprintf(path_src, "%lu%c\n", size, size_suffix);
        } else {
            if(strcmp(entry->name, "..") == 0) {
                strcpy(path_src, "UP-DIR\n");
            } else {
                strcpy(path_src, "-\n");
            }
        }
        sprintf(path_dst, "%8s", path_src);
        // right align the filesize
        window_gotox(w, w->w - 2 - strlen(path_dst));
        window_puts_color(w, path_dst, color);
    }

    // clear remaining rows
    color = COLOR(FG_SECONDARY, BG_SECONDARY);
    i -= list->offset;
    for(; i < LIST_VIEW_SIZE; i++) {
        sprintf(path_src, "%c%-16s%c", ' ', " ", ' ');
        window_puts_color(w, path_src, color);
        window_gotox(w, w->w - 2 - strlen(path_src));
        window_puts_color(w, path_src, color);
    }
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

    w->title = the_list->path;
    window(w);
    if(w == list_focus->window) {
        window_active(w, 1);
    } else {
        window_active(w, 0);
    }

    file_list_render(the_list);

    if(the_list->selected > (the_list->len - 1)) the_list->selected = the_list->len - 1;
    file_list_highlight(the_list, COLOR(FG_PRIMARY_HIGHLIGHT, BG_PRIMARY));
    file_list_message(the_list);
}

void file_list_disks(zc_list_t *the_list) {
    uint8_t i;
    // show the disk list
    the_list->len = disks_len;
    strcpy(the_list->path, "");
    for(i = 0; i < disks_len; i++) {
        memcpy(&the_list->files[i], &disks[i], sizeof(zc_entry_t));
    }
}

void init(void) {
    uint8_t i;

    memset(&list_left, 0, sizeof(zc_list_t));
    list_left.window = &win_ListingLeft;

    memset(&list_right, 0, sizeof(zc_list_t));
    list_right.window = &win_ListingRight;

    memset(disks, 0, sizeof(zc_entry_t) * 6);

    // automatically make the first "entry" selected
    list_left.selected = 1;
    list_right.selected = 1;

    char letters[] = {'A','B','C','H','T'};
    for(i = 0; i < sizeof(letters); i++) {
        char letter = letters[i];
        err = is_disk(letter);
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

    // display the program banner, for now...
    // const char *menu_main = "Zeal Commander";
    text_banner(0, 0, COLOR(BG_MENU, FG_MENU), 1, "Zeal Commander");
    // TODO: implement a top menu eventually???
    // const char *menu_main = " File Options";
    // text_menu(0, 0, COLOR(BG_MENU, FG_MENU), menu_main);

    const char *menu_file = "[1] Help [4] Ren [5] Copy [6] Move [7] Mkdir [8] Del [9] Update [10] Quit";
    text_menu(0, SCREEN_COL80_HEIGHT-1, COLOR(FG_MENU, TEXT_COLOR_DARK_GRAY), menu_file);

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
    }
}