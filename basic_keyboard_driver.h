#ifndef BASIC_KEYBOARD_DRIVER_H
#define BASIC_KEYBOARD_DRIVER_H

#include <stdint.h>
#include "keys.h"
#define PS2_KEYBOARD_KEY_CODE_BUFFER_MAX 256

#define PS2              0x60
#define PS2_STATUS_REG   0x64
#define PS2_COMMAND_REG  0x64
#define PS2_ACK          0xFA
#define PS2_RESEND       0xFE

#define PS2_SET_SCAN     0xF0
  #define PS2_SCAN_SET_1   0x01
  #define PS2_SCAN_SET_2   0x02
  #define PS2_SCAN_SET_3   0x03
#define PS2_ENABLE_SCAN  0xF4
#define PS2_DISABLE_SCAN 0xF5

typedef enum {
  PS2_KEYBOARD_IRQ_STATE_NORMAL,
  PS2_KEYBOARD_IRQ_STATE_EXTENDED
} ps2_keyboard_irq_state;

typedef struct {
  kernel_key code;
  uint8_t mods;
} ps2_keyboard_key_code;

extern ps2_keyboard_key_code null_key_code;

char get_input();

#endif // !BASIC_KEYBOARD_DRIVER_H
