#include "shell.h"
#include "kernel.h"
#include "keys.h"
#include "vga_text_drivers.h"
#include "basic_keyboard_driver.h"
#include <stdint.h>

typedef struct {
  const char* name;
  void (*fun)(char** args);
} shell_command;

void help_command(char** args);
void clear_command(char** args);

static const shell_command commands[] = {
  {"help", &help_command},
  {"clear", &clear_command}
};

void help_command(char** args) {
  kprint("Available commands: \n\n");
  kprint("help  - get a list of available commands\n");
  kprint("clear - clear the screen\n");
}

void clear_command(char** args) {
  clear_screen();
}


void lookup_command(char* command) {
  for (size_t i = 0; i < len(commands, shell_command); ++i) {
    if (strcmp(commands[i].name, command) == 0) {
      char** args; // TODO: args
      commands[i].fun(args);
      return;
    }
  }
  kprint("huh\n");
}

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
          command[cursor_pos++] = in;
          command_size++;
      }
    }
    command[command_size] = '\0';
    lookup_command(command);
  }
}
//
//    case KEY_UP: return '\xE4';
//    case KEY_DOWN: return '\xE5';
//    case KEY_RIGHT: return '\xF5';
//    case KEY_LEFT: return '\xD5';
