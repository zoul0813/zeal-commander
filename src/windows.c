/**
 * SPDX-FileCopyrightText: 2024 David Higgins <www.github.com/zoul0813>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <zvb_hardware.h>

#include "windows.h"

inline void text_map_vram(void) {
  __asm__ ("di");
  mmu_page_current = mmu_page0_ro;
  mmu_page0 = VID_MEM_PHYS_ADDR_START >> 14;
}

inline void text_demap_vram(void) {
  mmu_page0 = mmu_page_current;
  __asm__("ei");
}

void window(window_t* window) {
  uint8_t color = (window->bg << 4 & 0xF0) | (window->fg & 0x0F);

  window->_attrs.offset = (window->flags & WIN_BORDER) ? 1 : 0;
  window->_attrs.pos_x = window->x + window->_attrs.offset;
  window->_attrs.pos_y = window->y + window->_attrs.offset;

  text_map_vram();
  uint8_t y = window->y;
  uint8_t x = window->x;
  uint8_t min_x = x;
  uint8_t min_y = y;
  uint8_t max_x = (x + window->w - 1);
  uint8_t max_y = (y + window->h - 1);

  for(y = min_y; y <= max_y; y++) {
    for(x = min_x; x <= max_x; x++) {
      char c = CH_SPACE;
      SCR_TEXT[y][x] = c;
      SCR_COLOR[y][x] = color;
    }
  }

  // OPTIMIZE: move this into a separate block with loops like before...?
  if((window->flags & WIN_BORDER) > 0) {
    x = min_x;
    y = min_y;

    SCR_TEXT[y][x] = CH_ULCORNER;
    SCR_TEXT[y][max_x] = CH_URCORNER;
    SCR_TEXT[max_y][x] = CH_LLCORNER;
    SCR_TEXT[max_y][max_x] = CH_LRCORNER;

    for(x++; x <= max_x - 1; x++) {
      SCR_TEXT[y][x] = CH_HLINE;
      SCR_TEXT[max_y][x] = CH_HLINE;
    }

    x = window->x;
    for(y++; y <= max_y - 1; y++) {
      SCR_TEXT[y][x] = CH_VLINE;
      SCR_TEXT[y][max_x] = CH_VLINE;
    }
  }

  if(window->title != NULL) {
    /* Draw the window heading */
    uint8_t len = strlen(window->title) + 4;

    x = min_x + ((window->w - len) >> 1);
    y = min_y;
    SCR_TEXT[y][x] = '[';
    SCR_TEXT[y][++x] = ' ';

    for(int i = 0; i < SCREEN_COL80_WIDTH; i++) {
      unsigned char c = window->title[i];
      if(c == 0x00) break;
      SCR_TEXT[y][++x] = c;
    }
    SCR_TEXT[y][++x] = ' ';
    SCR_TEXT[y][++x] = ']';
  }

  if((window->flags & WIN_SHADOW) > 0) {
    // draw the shadow
    // let's assume all shadows are black for now?
    x = min_x + window->w;
    min_x++;
    min_y++;
    max_x++;
    for(y = min_y; y <= max_y; y++) {
      SCR_TEXT[y][x] = ' ';
      SCR_COLOR[y][x] = COLOR(window->fg, TEXT_COLOR_BLACK);
    }

    for(x = min_x; x <= max_x; x++) {
      SCR_TEXT[y][x] = ' ';
      SCR_COLOR[y][x] = COLOR(window->fg, TEXT_COLOR_BLACK);
    }
  }
  text_demap_vram();
}

void window_gotox(window_t* window, uint8_t x) {
  window->_attrs.pos_x = window->x + x + window->_attrs.offset;
}

void window_gotoy(window_t* window, uint8_t y) {
  window->_attrs.pos_y = window->y + y + window->_attrs.offset;
}

void window_gotoxy(window_t* window, uint8_t x, uint8_t y) {
  window->_attrs.pos_x = window->x + x + window->_attrs.offset;
  window->_attrs.pos_y = window->y + y + window->_attrs.offset;
}

void window_clrscr(window_t *window) {
  uint8_t color = COLOR(window->fg, window->bg);
  uint8_t x = window->x + window->_attrs.offset;
  uint8_t y = window->y + window->_attrs.offset;
  uint8_t min_x = x;
  uint8_t min_y = y;
  uint8_t max_x = (window->x + (window->w - 1)) - window->_attrs.offset;
  uint8_t max_y = (window->y + (window->h - 1)) - window->_attrs.offset;

  text_map_vram();
  for(y = min_y; y <= max_y; y++) {
    for(x = min_x; x <= max_x; x++) {
      SCR_TEXT[y][x] = ' ';
      SCR_COLOR[y][x] = color;
    }
  }
  text_demap_vram();
}

void window_clreol(window_t *window) {
  uint8_t color = COLOR(window->fg, window->bg);
  uint8_t x = window->_attrs.pos_x;
  uint8_t y = window->_attrs.pos_y;
  uint8_t min_x = x;
  uint8_t max_x = (window->x + (window->w - 1)) - window->_attrs.offset;

  text_map_vram();
  for(x = min_x; x <= max_x; x++) {
    SCR_TEXT[y][x] = ' ';
    SCR_COLOR[y][x] = color;
  }
  text_demap_vram();

  window->_attrs.pos_x = window->x + window->_attrs.offset;
  window->_attrs.pos_y++;
}

uint8_t window_wherex(window_t* window) {
  return window->_attrs.pos_x - window->_attrs.offset;
}

uint8_t window_wherey(window_t* window) {
  return window->_attrs.pos_y - window->_attrs.offset;
}

uint8_t window_putc(window_t* window, char c) {
  return window_putc_color(window, c, COLOR(window->fg, window->bg));
}

uint8_t window_putc_color(window_t* window, char c, uint8_t color) {
  uint8_t x = window->_attrs.pos_x;
  uint8_t min_x = x;
  uint8_t y = window->_attrs.pos_y;
  uint8_t lines = 0;

  uint8_t tab_width = ((window->_attrs.pos_x - window->_attrs.offset) - window->x) % 4;

  text_map_vram();
  switch(c) {
    case CH_NEWLINE:
      window->_attrs.pos_y = ++y;
      window->_attrs.pos_x = x = window->x + window->_attrs.offset;
      lines++;
      break;
    case CH_TAB:
      if(tab_width == 0) tab_width = 4;
      for(x = min_x; x < tab_width; x++) {
        SCR_TEXT[y][x] = ' ';
        SCR_COLOR[y][x] = color;
      }
      window->_attrs.pos_x += tab_width;
      break;
    default:
      SCR_TEXT[y][x] = c;
      SCR_COLOR[y][x] = color;
      window->_attrs.pos_x++;
  }
  text_demap_vram();

  if(window->_attrs.pos_x > ((window->x + window->w - 1) - window->_attrs.offset)) {
    window->_attrs.pos_x = window->x + window->_attrs.offset;
    window->_attrs.pos_y++;
    lines++;
  }
  // we can't do anything about vertical overflow, so just let it happen
  return lines;
}

uint8_t window_puts(window_t* window, const char* s) {
  return window_puts_color(window, s, COLOR(window->fg, window->bg));
}

uint8_t window_puts_color(window_t* window, const char* s, uint8_t color) {
  // TODO: arbitrary 256 byte max length?
  // uint8_t current_x = GET_X();
  // uint8_t current_y = GET_Y();
  uint8_t lines = 0;
  for(int i = 0; i < 256; i++) {
    if(s[i] == 0x00) break;
    lines += window_putc_color(window, s[i], color);
  }

  // SET_XY(current_x, current_y);

  return lines;
}



// TODO: refaactor the banner code to use SCR_TEXT/SCR_COLOR
void _text_banner(uint8_t x, uint8_t y, uint8_t centered, window_t* window, const char* s) {
  uint8_t bg = GET_COLOR_BG();
  uint8_t fg = GET_COLOR_FG();
  uint8_t blink = GET_CURSOR_BLINK();
  SET_CURSOR_BLINK(0);

  uint8_t width = 0;
  switch(zvb_ctrl_video_mode) {
    case ZVB_CTRL_VID_MODE_TEXT_320:
      width = 40;
      break;
    case ZVB_CTRL_VID_MODE_TEXT_640:
      width = 80;
      break;
  }

  if(window != NULL) {
    x = window->x + x;
    y = window->y + y;
    if(x > 0) {
      width = window->w - x - window->_attrs.offset;
    } else {
      width = window->w - x;
    }
  } else {
    width -= x;
  }

  // invert the colors
  SET_COLORS(bg, fg);
  // SET_COLOR(fg | bg);

  uint8_t len = width;
  for(uint8_t i = 0; i < width; i++) {
    if(s[i] == 0x00) {
      len = i;
      break;
    }
  }

  char pad = 0;
  if(len > 0) {
    SET_XY(x, y);
    if(centered) {
      pad = (width - len);
      if((pad % 2) == 0) {
        pad = pad >> 1;
      } else {
        pad = (pad >> 1) - 1;
      }
    }

    for(uint8_t i = 0; i < pad; i++) {
      PRINT_CHAR(' ');
    }

    for(uint8_t i = 0; i < len; i++) {
      PRINT_CHAR(s[i]);
    }

    for(uint8_t i = 0; i < (width - len - pad); i++) {
      PRINT_CHAR(' ');
    }
  }

  // invert the colors again
  SET_COLORS(fg, bg);
  SET_CURSOR_BLINK(blink);
}

void text_banner(uint8_t x, uint8_t y, uint8_t centered, const char* s) {
  _text_banner(x, y, centered, 0, s);
}

void text_header(uint8_t x, uint8_t y, const char* s) {
  _text_banner(x, y, 1, 0, s);
}

void text_menu(uint8_t x, uint8_t y, const char* items) {
  _text_banner(x, y, 0, 0, items);
}

void window_banner(window_t* window, uint8_t x, uint8_t y, uint8_t centered, const char* s) {
  SET_COLORS(window->fg, window->bg); // ???
  _text_banner(x, y, centered, window, s);
  window->_attrs.pos_y++;
  if(window->_attrs.pos_y < window->y) window->_attrs.pos_y = window->y;
}