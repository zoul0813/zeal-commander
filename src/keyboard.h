#include <stdint.h>
#include <zos_errors.h>
#include <zos_keyboard.h>

#ifndef KEYBOARD_H
#define KEYBOARD_H
zos_err_t kb_mode(void *arg);
unsigned char getkey(void);
#endif