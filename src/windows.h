/**
 * SPDX-FileCopyrightText: 2024 David Higgins <www.github.com/zoul0813>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*************************/
/*       windows.h       */
/*************************/


/**
 * Window functions for Zeal 8-bit Computer text mode.
 *
 * This API writes directly to the Zeal Video Boards registers,
 * implementing fast text mode "graphics".
 *
 * Screen coordinates are 0,0 based.  Most of the functions
 * do not perform any type of error checking, refer to ZOS/ZVB
 * [documentation](https://zeal8bit.com/) for valid parameters.
 *
 * This API pairs well with conio.h, but does not rely on it.
 */

#include <stdint.h>
#include <zos_video.h>
#include <zvb_hardware.h>
#include "theme.h"


#ifndef WINDOWS_H
#define WINDOWS_H

#define SCREEN_COL80_WIDTH  80
#define SCREEN_COL80_HEIGHT 40
#define SCREEN_COL40_WIDTH  40
#define SCREEN_COL40_HEIGHT 20

#define SCREEN_FONT_SIZE    3072

#define VIDEO_MODE_HIGH ZVB_CTRL_VID_MODE_TEXT_640
#define VIDEO_MODE_LOW  ZVB_CTRL_VID_MODE_TEXT_320
#ifndef VIDEO_MODE
#define VIDEO_MODE VIDEO_MODE_HIGH
#endif
 // video.asm: DEFC DEFAULT_CURSOR_BLINK = 30
#define DEFAULT_CURSOR_BLINK 30

#ifndef CH_NEWLINE
#define CH_NEWLINE      0x0A // New line
#endif

#ifndef CH_SPACE
#define CH_SPACE        0x20 // Space
#endif

#ifndef CH_TAB
#define CH_TAB          0x09   /* tabulator */
#endif

#ifndef CH_ULCORNER
#define CH_ULCORNER     0xDA // Top Left
#endif

#ifndef CH_URCORNER
#define CH_URCORNER     0xBF // Top Right
#endif

#ifndef CH_LLCORNER
#define CH_LLCORNER     0xC0 // Bottom Left
#endif

#ifndef CH_LRCORNER
#define CH_LRCORNER     0xD9 // Bottom Right
#endif

#ifndef CH_HLINE
#define CH_HLINE        0xC4 // Horizonal line
#endif

#ifndef CH_TLINE
#define CH_TLINE        0xC2
#endif

#ifndef CH_TLINEI
#define CH_TLINEI       0xC1
#endif

#ifndef CH_VLINE
#define CH_VLINE        0xB3 // Vertical line
#endif

#ifndef CH_DOT
#define CH_DOT          0xF9 // Vertically Centered "Dot"
#endif

#define COLOR(fg, bg)           ((bg << 4 & 0xF0) | (fg & 0xF))
#define GET_COLOR()             zvb_peri_text_color
#define GET_COLOR_BG()          (zvb_peri_text_color >> 4 & 0x0F)
#define GET_COLOR_FG()          (zvb_peri_text_color & 0x0F)
#define SET_COLOR(c)            zvb_peri_text_color = c
#define SET_COLORS(fg,bg)       zvb_peri_text_color = ((bg  << 4) | (fg & 0x0F))
#define GET_CURSOR_BLINK()      zvb_peri_text_curs_time
#define SET_CURSOR_BLINK(cur)   zvb_peri_text_curs_time = cur
#define GET_X()                 zvb_peri_text_curs_x
#define GET_Y()                 zvb_peri_text_curs_y
#define SET_X(x)                zvb_peri_text_curs_x = x
#define SET_Y(y)                zvb_peri_text_curs_y = y
#define SET_XY(x,y)             SET_X(x); SET_Y(y);

extern uint8_t mmu_page_current;
const __sfr __banked __at(0xF0) mmu_page0_ro;
__sfr __at(0xF0) mmu_page0;
uint8_t __at(0x0000) SCR_TEXT[SCREEN_COL80_HEIGHT][SCREEN_COL80_WIDTH];
uint8_t __at(0x1000) SCR_COLOR[SCREEN_COL80_HEIGHT][SCREEN_COL80_WIDTH];
uint8_t __at(0x3000) SCR_FONT[SCREEN_FONT_SIZE];

extern uint8_t MIRROR_TEXT[SCREEN_COL80_HEIGHT][SCREEN_COL80_WIDTH];
extern uint8_t MIRROR_COLOR[SCREEN_COL80_HEIGHT][SCREEN_COL80_WIDTH];

#define TEXT_WRITE(w,x,y,c) SCR_TEXT[y][x] = c; if((w->flags & WIN_DIALOG) == 0) { MIRROR_TEXT[y][x] = c; }
#define COLOR_WRITE(w,x,y,c) SCR_COLOR[y][x] = c; if((w->flags & WIN_DIALOG) == 0) { MIRROR_COLOR[y][x] = c; }

inline void text_map_vram(void);
inline void text_demap_vram(void);

typedef enum {
  WIN_NONE          = 0,
  WIN_BORDER        = 1 << 0,
  WIN_SHADOW        = 1 << 1,
  WIN_TITLE_LEFT    = 1 << 2,
  WIN_TITLE_RIGHT   = 1 << 3,
  WIN_DIALOG        = 1 << 4, // do not write to screen mirror
} WindowFlags;

typedef struct {
  uint8_t pos_x;
  uint8_t pos_y;
  uint8_t offset;
} _window_attrs_t;

typedef struct {
  uint8_t x;
  uint8_t y;
  uint8_t w;
  uint8_t h;
  uint8_t fg;
  uint8_t bg;
  uint8_t fg_highlight;
  uint8_t flags;
  const char* title;
  /* private */
  _window_attrs_t _attrs;
} window_t;

extern window_t win_None;
extern window_t* win_NonePtr;

void window(window_t* w);
void window_columns(window_t* w, uint8_t *columns, uint8_t count);
void window_active(window_t* w, uint8_t active);
void window_gotox(window_t* w, uint8_t x);
void window_gotoy(window_t* w, uint8_t y);
void window_gotoxy(window_t* w, uint8_t x, uint8_t y);
void window_clrscr(window_t* w);
void window_clreol(window_t* w);
void window_refresh(void);

uint8_t window_putc(window_t* w, char c);
uint8_t window_putc_color(window_t* w, char c, uint8_t color);
uint8_t window_puts(window_t* w, const char* s);
uint8_t window_puts_color(window_t* w, const char* s, uint8_t color);
uint8_t window_wherex(window_t* w);
uint8_t window_wherey(window_t* w);

/* Where does this belong? */
void text_banner(uint8_t x, uint8_t y, uint8_t c, uint8_t centered, const char* s);
void text_header(uint8_t x, uint8_t y, uint8_t c, const char* s);
void text_menu(uint8_t x, uint8_t y, uint8_t c, const char* items);


void window_banner(window_t* w, uint8_t x, uint8_t y, uint8_t centered, const char* s);

#endif