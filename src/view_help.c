#include "windows.h"

#define WIN_HELP_WIDTH  ((SCREEN_COL80_WIDTH / 4) * 3)
#define WIN_HELP_HEIGHT (SCREEN_COL80_HEIGHT / 4) * 3

window_t win_Help = {
  .x = (SCREEN_COL80_WIDTH - WIN_HELP_WIDTH) / 2,
  .y = (SCREEN_COL80_HEIGHT - WIN_HELP_HEIGHT) / 2,
  .w = WIN_HELP_WIDTH,
  .h = WIN_HELP_HEIGHT,
  .flags = WIN_BORDER | WIN_SHADOW,
  .fg = FG_SECONDARY,
  .bg = BG_SECONDARY,
  .title = "Help"
};

void view_draw_help(void) {
    // move this into a view!
    window(&win_Help);
    window_clrscr(&win_Help);
    window_puts(&win_Help, "\n Zeal Commander v1.0.0\n");
    window_puts(&win_Help, " by github.com/zoul0813\n\n");

    window_puts(&win_Help, " The menu items at the bottom of the screen are\n");
    window_puts(&win_Help, " accessed via the Fn keys.\n\n");

    window_puts(&win_Help, "   F4 - Rename selected file\n");
    window_puts(&win_Help, "   F5 - Copy file to other side\n");
    window_puts(&win_Help, "   F6 - Move file to other side\n");
    window_puts(&win_Help, "   F7 - Make directory\n");
    window_puts(&win_Help, "   F8 - Delete selected file\n");
    window_puts(&win_Help, "   F9 - Refresh panel\n");
    window_puts(&win_Help, "  F10 - Quit program\n\n");

    window_puts(&win_Help, "  Files ending in '.bin' will be detected as programs\n");
    window_puts(&win_Help, "  and marked with a highlight color and '*'.\n\n");

    window_puts(&win_Help, "  Dirs are highlighted in another color and prefixed\n");
    window_puts(&win_Help, "  with a `/`.\n");


    // window_gotoxy(&win_Help, 0, 3);
    // window_clreol(&win_Help);
}