#include "basic_keyboard_driver.h"
#include "keys.h"
#include "kernel.h"
#include "vga_text_drivers.h"
#include <stddef.h>
#include <stdint.h>

static ps2_keyboard_irq_state irq_state;

static ps2_keyboard_key_code key_code_buffer[PS2_KEYBOARD_KEY_CODE_BUFFER_MAX];
static uint16_t buffer_count = 0;

/* Modifiers ***********/
/*             s   s c */
/*             u a h t */
/*             p l f r */
/*             r t t l */
/*                     */
/*             ↓ ↓ ↓ ↓ */
/* 0 b 0 0 0 0 0 0 0 0 */

static uint8_t current_modifiers;

static const kernel_key scancode_lookup_table[] = {
  [0x1E] = KEY_A,
  [0x30] = KEY_B,
  [0x2E] = KEY_C,
  [0x20] = KEY_D,
  [0x12] = KEY_E,
  [0x21] = KEY_F,
  [0x22] = KEY_G,
  [0x23] = KEY_H,
  [0x17] = KEY_I,
  [0x24] = KEY_J,
  [0x25] = KEY_K,
  [0x26] = KEY_L,
  [0x32] = KEY_M,
  [0x31] = KEY_N,
  [0x18] = KEY_O,
  [0x19] = KEY_P,
  [0x10] = KEY_Q,
  [0x13] = KEY_R,
  [0x1F] = KEY_S,
  [0x14] = KEY_T,
  [0x16] = KEY_U,
  [0x2F] = KEY_V,
  [0x11] = KEY_W,
  [0x2D] = KEY_X,
  [0x15] = KEY_Y,
  [0x2C] = KEY_Z,
  [0x0B] = KEY_0,
  [0x02] = KEY_1,
  [0x03] = KEY_2,
  [0x04] = KEY_3,
  [0x05] = KEY_4,
  [0x06] = KEY_5,
  [0x07] = KEY_6,
  [0x08] = KEY_7,
  [0x09] = KEY_8,
  [0x0A] = KEY_9,
  [0x29] = KEY_GRAVE,
  [0x0C] = KEY_MINUS,
  [0x0D] = KEY_EQUAL,
  [0x2B] = KEY_SLASH,
  [0x1A] = KEY_L_SQRBR,
  [0x1B] = KEY_R_SQRBR,
  [0x27] = KEY_SEMICOLON,
  [0x28] = KEY_QUOTE,
  [0x33] = KEY_COMMA,
  [0x34] = KEY_DOT,
  [0x35] = KEY_SLASH,

  [0x0E] = KEY_BACKSPACE,
  [0x39] = KEY_SPACE,
  [0x0F] = KEY_TAB,
  [0x3A] = KEY_CAPS,
  [0x2A] = KEY_SHIFT,
  [0x1D] = KEY_CTRL,
  [0x38] = KEY_ALT,
  [0x36] = KEY_R_SHIFT,
  [0x1C] = KEY_ENTER,
  [0x01] = KEY_ESC,
  [0x3B] = KEY_F1,
  [0x3C] = KEY_F2,
  [0x3D] = KEY_F3,
  [0x3E] = KEY_F4,
  [0x3F] = KEY_F5,
  [0x40] = KEY_F6,
  [0x41] = KEY_F7,
  [0x42] = KEY_F8,
  [0x43] = KEY_F9,
  [0x44] = KEY_F10,
  [0x57] = KEY_F11,
  [0x58] = KEY_F12,
  // TODO: доделать

  // BREAK ----------

  [0x9E] = KEY_BREAK(KEY_A),
  [0xB0] = KEY_BREAK(KEY_B),
  [0xAE] = KEY_BREAK(KEY_C),
  [0xA0] = KEY_BREAK(KEY_D),
  [0x92] = KEY_BREAK(KEY_E),
  [0xA1] = KEY_BREAK(KEY_F),
  [0xA2] = KEY_BREAK(KEY_G),
  [0xA3] = KEY_BREAK(KEY_H),
  [0x97] = KEY_BREAK(KEY_I),
  [0xA4] = KEY_BREAK(KEY_J),
  [0xA5] = KEY_BREAK(KEY_K),
  [0xA6] = KEY_BREAK(KEY_L),
  [0xB2] = KEY_BREAK(KEY_M),
  [0xB1] = KEY_BREAK(KEY_N),
  [0x98] = KEY_BREAK(KEY_O),
  [0x99] = KEY_BREAK(KEY_P),
  [0x90] = KEY_BREAK(KEY_Q),
  [0x93] = KEY_BREAK(KEY_R),
  [0x9F] = KEY_BREAK(KEY_S),
  [0x94] = KEY_BREAK(KEY_T),
  [0x96] = KEY_BREAK(KEY_U),
  [0xAF] = KEY_BREAK(KEY_V),
  [0x91] = KEY_BREAK(KEY_W),
  [0xAD] = KEY_BREAK(KEY_X),
  [0x95] = KEY_BREAK(KEY_Y),
  [0xAC] = KEY_BREAK(KEY_Z),

  // TODO: доделать

  [0x9C] = KEY_BREAK(KEY_ENTER),
  [0xB9] = KEY_BREAK(KEY_SPACE),
  [0xAA] = KEY_BREAK(KEY_SHIFT),

  0
};

static const kernel_key extended_scancode_lookup_table[] = {
  [0x48] = KEY_UP,
  [0x4B] = KEY_LEFT,
  [0x50] = KEY_DOWN,
  [0x4D] = KEY_RIGHT,

  // BREAK -------------

  [0xC8] = KEY_BREAK(KEY_UP),
  [0xCB] = KEY_BREAK(KEY_LEFT),
  [0xD0] = KEY_BREAK(KEY_DOWN),
  [0xCD] = KEY_BREAK(KEY_RIGHT),
};


static inline void set_mod_ctrl(uint8_t* mod) {
  *mod |= 0b1;
}
static inline void set_mod_shift(uint8_t* mod) {
  *mod |= 0b1 << 1;
}
static inline void set_mod_alt(uint8_t* mod) {
  *mod |= 0b1 << 2;
}
static inline void set_mod_super(uint8_t* mod) {
  *mod |= 0b1 << 3;
}

static inline void unset_mod_ctrl(uint8_t* mod) {
  *mod &= 0b1110;
}
static inline void unset_mod_shift(uint8_t* mod) {
  *mod &= 0b1101;
}
static inline void unset_mod_alt(uint8_t* mod) {
  *mod &= 0b1011;
}
static inline void unset_mod_super(uint8_t* mod) {
  *mod &= 0b0111;
}

char key_code_into_char(ps2_keyboard_key_code key) {
  _BOOL shifted = (key.mods & 0b0010) >> 1;

  switch (key.code) {
    case KEY_A: return shifted ? 'A' : 'a';
    case KEY_B: return shifted ? 'B' : 'b';
    case KEY_C: return shifted ? 'C' : 'c';
    case KEY_D: return shifted ? 'D' : 'd';
    case KEY_E: return shifted ? 'E' : 'e';
    case KEY_F: return shifted ? 'F' : 'f';
    case KEY_G: return shifted ? 'G' : 'g';
    case KEY_H: return shifted ? 'H' : 'h';
    case KEY_I: return shifted ? 'I' : 'i';
    case KEY_J: return shifted ? 'J' : 'j';
    case KEY_K: return shifted ? 'K' : 'k';
    case KEY_L: return shifted ? 'L' : 'l';
    case KEY_M: return shifted ? 'M' : 'm';
    case KEY_N: return shifted ? 'N' : 'n';
    case KEY_O: return shifted ? 'O' : 'o';
    case KEY_P: return shifted ? 'P' : 'p';
    case KEY_Q: return shifted ? 'Q' : 'q';
    case KEY_R: return shifted ? 'R' : 'r';
    case KEY_S: return shifted ? 'S' : 's';
    case KEY_T: return shifted ? 'T' : 't';
    case KEY_U: return shifted ? 'U' : 'u';
    case KEY_V: return shifted ? 'V' : 'v';
    case KEY_W: return shifted ? 'W' : 'w';
    case KEY_X: return shifted ? 'X' : 'x';
    case KEY_Y: return shifted ? 'Y' : 'y';
    case KEY_Z: return shifted ? 'Z' : 'z';
    case KEY_1: return shifted ? '!' : '1';
    case KEY_2: return shifted ? '@' : '2';
    case KEY_3: return shifted ? '#' : '3';
    case KEY_4: return shifted ? '$' : '4';
    case KEY_5: return shifted ? '%' : '5';
    case KEY_6: return shifted ? '^' : '6';
    case KEY_7: return shifted ? '&' : '7';
    case KEY_8: return shifted ? '*' : '8';
    case KEY_9: return shifted ? '(' : '9';
    case KEY_0: return shifted ? ')' : '0';

    case KEY_COMMA: return shifted ? '<' : ',';
    case KEY_DOT: return shifted ? '>' : '.';
    case KEY_SLASH: return shifted ? '?' : '/';
    case KEY_QUOTE: return shifted ? '\"' : '\'';
    case KEY_L_SQRBR: return shifted ? '{' : '[';
    case KEY_R_SQRBR: return shifted ? '}' : ']';
    case KEY_SEMICOLON: return shifted ? ':' : ';';
    case KEY_MINUS: return shifted ? '_' : '-';

    case KEY_ENTER: return '\n';
    case KEY_BACKSPACE: return '\b';
    case KEY_SPACE: return ' ';

    case KEY_UP: return '\xE4';
    case KEY_DOWN: return '\xE5';
    case KEY_RIGHT: return '\xF5';
    case KEY_LEFT: return '\xD5';

    default: return '\0';
  }
}

void handle_ps2_keyboard_irq() {
  uint8_t in = inb(PS2);
  
  if (in == 0xE0) {
    irq_state = PS2_KEYBOARD_IRQ_STATE_EXTENDED;
    handle_ps2_keyboard_irq();
  }

  if (in == 0x1D) {
    set_mod_ctrl(&current_modifiers);
  } else if (in == 0x9D) {
    set_mod_ctrl(&current_modifiers);
  }

  if (in == 0x2A || in == 0x36) {
    set_mod_shift(&current_modifiers);
  } else if (in == 0xAA || in == 0xB6) {
    unset_mod_shift(&current_modifiers);
  }

  if (in == 0x38 || (irq_state == PS2_KEYBOARD_IRQ_STATE_EXTENDED && in == 0x38)) {
    set_mod_alt(&current_modifiers);
  } else if (in == 0xB8 || (irq_state == PS2_KEYBOARD_IRQ_STATE_EXTENDED && in == 0xB8)) {
    unset_mod_alt(&current_modifiers);
  }

  if (irq_state == PS2_KEYBOARD_IRQ_STATE_EXTENDED && (in == 0x5B || in == 0x5C)) {
    set_mod_super(&current_modifiers);
  } else if (irq_state == PS2_KEYBOARD_IRQ_STATE_EXTENDED && (in == 0xDB || in == 0xDC)) {
    set_mod_super(&current_modifiers);
  }

  if (irq_state == PS2_KEYBOARD_IRQ_STATE_EXTENDED) {
    irq_state = PS2_KEYBOARD_IRQ_STATE_NORMAL;
    key_code_buffer[buffer_count].code = extended_scancode_lookup_table[in];
    key_code_buffer[buffer_count].mods = current_modifiers;
    buffer_count = (buffer_count + 1) % PS2_KEYBOARD_KEY_CODE_BUFFER_MAX;
    return;
  }

  key_code_buffer[buffer_count].code = scancode_lookup_table[in];
  key_code_buffer[buffer_count].mods = current_modifiers;
  buffer_count = (buffer_count + 1) % PS2_KEYBOARD_KEY_CODE_BUFFER_MAX;
}

ps2_keyboard_key_code null_key_code = {0,0};
static char last = '\0';

char get_input() {
  handle_ps2_keyboard_irq();
  char in = key_code_into_char(key_code_buffer[buffer_count]);
  if (in == last) {
    return '\0';
  }
  last = in;
  return in;
}
