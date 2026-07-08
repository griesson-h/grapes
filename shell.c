#include "shell.h"
#include "kernel.h"
#include "keys.h"
#include "vga_text_drivers.h"
#include "basic_keyboard_driver.h"
#include <stdint.h>


void run_shell() {
  while (1) {
    uint16_t cursor_pos = 0;
    uint16_t command_size = 0;
    char command[SHELL_MAX_COMMAND_SIZE];

    kprint("> ");
    while (1) {
      char in = get_input();
      if (in == '\n') {
        kprint_char('\n');
        break;
      }
      switch (in) {
        case '\0': continue;
        case '\b':
          if (cursor_pos != 0) {
            backspace_vga();
            cursor_pos--;
            command_size--;
          }
          break;
        case '\xE4': continue;
        case '\xE5': continue;
        case '\xF5': continue;
        case '\xD5': continue;
        default:
          if (command_size >= SHELL_MAX_COMMAND_SIZE - 1)
            continue;
          kprint_char(in);
          cursor_pos++;
          command_size++;
      }
    }
  }
}
//
//    case KEY_UP: return '\xE4';
//    case KEY_DOWN: return '\xE5';
//    case KEY_RIGHT: return '\xF5';
//    case KEY_LEFT: return '\xD5';
