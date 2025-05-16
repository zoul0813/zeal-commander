#include <zos_vfs.h>
#include <zos_sys.h>
#include "keyboard.h"
#include "shared.h"

char kb_buffer[32];

zos_err_t kb_mode(void *arg) {
  return ioctl(DEV_STDIN, KB_CMD_SET_MODE, arg);
}

unsigned char getkey(void) {
  uint8_t i;
  uint16_t size;
  zos_err_t err;
  unsigned char c;

  size = sizeof(kb_buffer);
  err = read(DEV_STDIN, kb_buffer, &size);
  handle_error(err, "read DEV_STDIN", 1);

  for(i = 0; i < size; i++) {
    c = kb_buffer[i];

    if(c == 0) break;
    if(c == KB_RELEASED) {
      // ignore released keys
      i++;
      continue;
    }
    return c;
  }

  return 0;
}