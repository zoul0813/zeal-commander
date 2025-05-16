#include <zos_video.h>

#ifndef THEME_H
#define THEM_H
/*

typedef enum {
    TEXT_COLOR_BLACK         = 0x0,
    TEXT_COLOR_DARK_BLUE     = 0x1,
    TEXT_COLOR_DARK_GREEN    = 0x2,
    TEXT_COLOR_DARK_CYAN     = 0x3,
    TEXT_COLOR_DARK_RED      = 0x4,
    TEXT_COLOR_DARK_MAGENTA  = 0x5,
    TEXT_COLOR_BROWN         = 0x6,
    TEXT_COLOR_LIGHT_GRAY    = 0x7,
    TEXT_COLOR_DARK_GRAY     = 0x8,
    TEXT_COLOR_BLUE          = 0x9,
    TEXT_COLOR_GREEN         = 0xa,
    TEXT_COLOR_CYAN          = 0xb,
    TEXT_COLOR_RED           = 0xc,
    TEXT_COLOR_MAGENTA       = 0xd,
    TEXT_COLOR_YELLOW        = 0xe,
    TEXT_COLOR_WHITE         = 0xf,
} zos_text_color_t;

*/

#define LIST_PAGE_SIZE   10

/** PRIMARY THEME */
#define FG_PRIMARY              TEXT_COLOR_LIGHT_GRAY
#define FG_PRIMARY_HIGHLIGHT    TEXT_COLOR_WHITE
#define BG_PRIMARY              TEXT_COLOR_DARK_BLUE
#define BORDER_PRIMARY          TEXT_COLOR_WHITE

#define FG_FOLDER   TEXT_COLOR_WHITE
#define FG_EXEC     TEXT_COLOR_YELLOW
#define FG_HEADING  TEXT_COLOR_DARK_BLUE
#define FG_ERROR    TEXT_COLOR_DARK_RED

#define FG_SECONDARY            TEXT_COLOR_LIGHT_GRAY
#define FG_SECONDARY_HIGHLIGHT  TEXT_COLOR_WHITE
#define BG_SECONDARY            TEXT_COLOR_BLUE
#define BORDER_SECONDARY        TEXT_COLOR_LIGHT_GRAY

#define FG_MENU     TEXT_COLOR_DARK_CYAN
#define BG_MENU     TEXT_COLOR_BLACK

#define FG_MESSAGE  TEXT_COLOR_WHITE
#define BG_MESSAGE  TEXT_COLOR_BLACK


/** DARK THEME */
// #define FG_PRIMARY              TEXT_COLOR_LIGHT_GRAY
// #define FG_PRIMARY_HIGHLIGHT    TEXT_COLOR_WHITE
// #define BG_PRIMARY              TEXT_COLOR_BLACK
// #define BORDER_PRIMARY          TEXT_COLOR_DARK_BLUE

// #define FG_FOLDER   TEXT_COLOR_WHITE
// #define FG_EXEC     TEXT_COLOR_YELLOW
// #define FG_HEADING  TEXT_COLOR_DARK_BLUE
// #define FG_ERROR    TEXT_COLOR_DARK_RED

// #define FG_SECONDARY            TEXT_COLOR_LIGHT_GRAY
// #define FG_SECONDARY_HIGHLIGHT  TEXT_COLOR_WHITE
// #define BG_SECONDARY            TEXT_COLOR_BLACK
// #define BORDER_SECONDARY        TEXT_COLOR_LIGHT_GRAY

// #define FG_MENU     TEXT_COLOR_DARK_CYAN
// #define BG_MENU     TEXT_COLOR_BLACK

// #define FG_MESSAGE  TEXT_COLOR_WHITE
// #define BG_MESSAGE  TEXT_COLOR_BLACK

/** BRIGHT THEME */
// #define FG_PRIMARY              TEXT_COLOR_LIGHT_GRAY
// #define FG_PRIMARY_HIGHLIGHT    TEXT_COLOR_WHITE
// #define BG_PRIMARY              TEXT_COLOR_BLACK
// #define BORDER_PRIMARY          TEXT_COLOR_DARK_BLUE

// #define FG_FOLDER   TEXT_COLOR_WHITE
// #define FG_EXEC     TEXT_COLOR_YELLOW
// #define FG_HEADING  TEXT_COLOR_DARK_BLUE
// #define FG_ERROR    TEXT_COLOR_DARK_RED

// #define FG_SECONDARY            TEXT_COLOR_BLUE
// #define FG_SECONDARY_HIGHLIGHT  TEXT_COLOR_BROWN
// #define BG_SECONDARY            TEXT_COLOR_CYAN
// #define BORDER_SECONDARY        TEXT_COLOR_LIGHT_GRAY

// #define FG_MENU     TEXT_COLOR_DARK_CYAN
// #define BG_MENU     TEXT_COLOR_BLACK

// #define FG_MESSAGE  TEXT_COLOR_WHITE
// #define BG_MESSAGE  TEXT_COLOR_BLACK

// #define CH_NEWLINE      0x0A // New line
// #define CH_SPACE        0x20 // Space
// #define CH_TAB          0x09   /* tabulator */
// #define CH_ULCORNER     0xDA // Top Left
// #define CH_URCORNER     0xBF // Top Right
// #define CH_LLCORNER     0xC0 // Bottom Left
// #define CH_LRCORNER     0xD9 // Bottom Right
// #define CH_HLINE        0xC4 // Horizonal line
// #define CH_VLINE        0xB3 // Vertical line
// #define CH_DOT          0xF9 // Vertically Centered "Dot"


#define CH_TREE_BRANCH  195
#define CH_TREE_LEAF    196
#define CH_TREE_LAST    192
#define CH_TREE_TRUNK   179

#endif // ifndef THEME_H