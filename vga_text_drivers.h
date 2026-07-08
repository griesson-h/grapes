#ifndef VGA_TEXT_DRIVERS_H
#define VGA_TEXT_DRIVERS_H

#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

extern uint16_t cmd_xpos;
extern uint16_t cmd_ypos;

void backspace_vga();
void move_cursor(int x, int y);
void write_char(char c, size_t row, size_t col, uint8_t color);
void clear_screen();
void kprint_char_raw(char c);
void kprint_char(char c);
void kprint(const char* str);
void init_cmd();

#endif
