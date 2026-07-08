#include <stddef.h>
#include <stdint.h>
#include "kernel.h"
#include "basic_keyboard_driver.h"
#include "shell.h"
#include "vga_text_drivers.h"

void kmain() {
  init_cmd();

  kprint("Welcome to Grapes\n\n");
  kprint("Type 'help' to see the list of available commands\n");

  run_shell();
}
