#include <stdint.h>
#include <zos_errors.h>
#include <zos_video.h>
#include "windows.h"

 // video.asm: DEFC DEFAULT_CURSOR_BLINK = 30
#define DEFAULT_CURSOR_BLINK 30

extern char textbuff[SCREEN_COL80_WIDTH];

int __exit(zos_err_t err);
void handle_error(zos_err_t err, char *msg, uint8_t fatal);
int str_ends_with(const char *str, const char *suffix);

static inline void cursor_x(unsigned char x) {
  zvb_peri_text_curs_x = x;
}

static inline void cursor_y(unsigned char y) {
  zvb_peri_text_curs_y = y;
}

static inline void cursor_xy(unsigned char x, unsigned char y) {
  cursor_x(x);
  cursor_y(y);
}

static unsigned char cursor (unsigned char onoff) {
  static unsigned char _previous = 1;
  unsigned char old_state = _previous;
  _previous = onoff;
  zvb_peri_text_curs_time = onoff ? DEFAULT_CURSOR_BLINK : 0;
  return old_state;
}

static inline void setcolor(uint8_t fg, uint8_t bg) {
  zvb_peri_text_color = (bg << 4 & 0xF0) | (fg & 0x0F);
}