#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>
#include <stdint.h>

#define _BOOL uint8_t
#define TRUE 1
#define FALSE 0

#define WAIT(ticks) for (size_t a = 0; a < ticks; ++a) {}
#define len(var, type) (sizeof(var)/sizeof(type))

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

inline uint16_t strcmp(const char* s1, const char* s2) {
  while(*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}


#endif // !KERNEL_H
