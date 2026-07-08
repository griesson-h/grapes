#include "vga_text_drivers.h"
#include "kernel.h"
#include <stddef.h>
#include <stdint.h>

uint16_t cmd_xpos;
uint16_t cmd_ypos;

void move_cursor(int x, int y)
{
  uint16_t pos = y * VGA_WIDTH + x;

  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t) (pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

void vga_move_to(int x, int y) {
  cmd_ypos = y;
  cmd_xpos = x;
  move_cursor(cmd_xpos, cmd_ypos);
}

void write_char(char c, size_t row, size_t col, uint8_t color) {
  uint16_t* buffer = (uint16_t*)0xB8000;
  size_t index = (row * 80 + col);
  buffer[index] = (uint16_t)c | ((uint16_t)color << 8);
}

void clear_screen() {
  for (size_t row = 0; row < 25; row++) {
    for (size_t col = 0; col < 80; col++) {
        write_char(' ', row, col, 0x0F);
    }
  }
}

void backspace_vga() {
  if (cmd_xpos == 0) {
    //cmd_ypos = VGA_WIDTH;
    //cmd_xpos -= 1;
    return;
  } else {
    cmd_xpos -= 1;
  }
  write_char(' ', cmd_ypos, cmd_xpos, 0x0F);
  move_cursor(cmd_xpos, cmd_ypos);
}

void kprint_char_raw(char c) {
  if (c == '\n') {
    cmd_ypos++;
    cmd_xpos = 0;
    return;
  }
  write_char(c, cmd_ypos, cmd_xpos++, 0x07);
  if (cmd_xpos >= VGA_WIDTH) {
    cmd_ypos++;
    cmd_xpos = 0;
  }
}

void kprint_char(char c) {
  kprint_char_raw(c);
  move_cursor(cmd_xpos, cmd_ypos);
}

void kprint_size(const char* str, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    kprint_char_raw(str[i]);
  }
  move_cursor(cmd_xpos, cmd_ypos);
}

void kprint(const char *str) {
  kprint_size(str, strlen(str));
  move_cursor(cmd_xpos, cmd_ypos);
}

void init_cmd() {
  clear_screen();
  move_cursor(0, 0);
  cmd_xpos = 0;
  cmd_ypos = 0;
}


