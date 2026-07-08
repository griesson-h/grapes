#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>
#include <stdint.h>

#define _BOOL uint8_t
#define TRUE 1
#define FALSE 0

#define WAIT(ticks) for (size_t a = 0; a < ticks; ++a) {}

inline void outb(uint16_t port, uint16_t val) {
  __asm__ volatile ( "outb %b0, %w1" : : "a"(val), "Nd"(port) : "memory");
}

inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile ( "inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory");
  return ret;
}

inline size_t strlen(const char* str) {
  size_t len = 0;
  while (str[len]) {
    len++;
  }
  return len;
}

#endif // !KERNEL_H
