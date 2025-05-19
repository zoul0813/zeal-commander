#include <stdint.h>
#include <zos_errors.h>
#include <zos_keyboard.h>

#ifndef KEYBOARD_H
#define KEYBOARD_H


#define kb_mode_non_block_raw() kb_mode((void *)(KB_READ_NON_BLOCK | KB_MODE_RAW))
#define kb_mode_default() kb_mode((void *)(KB_READ_BLOCK | KB_MODE_COOKED))

zos_err_t kb_mode(void *arg);
unsigned char getkey(void);
#endif