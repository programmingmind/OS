#include "common.h"

void outb(uint16_t port, uint8_t val) {
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(port) );
    return ret;
}

size_t strlen(const char* str) {
    size_t ret = 0;
    while (str[ret])
        ret++;
    return ret;
}

uint8_t isAlpha(char c) {
   return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

char toUpper(char c) {
   if (c >= 'a' && c <= 'z')
      c += 'A' - 'a';
   return c;
}

char toLower(char c) {
   if (c >= 'A' && c <= 'Z')
      c -= 'A' - 'a';
   return c;
}

char changeCase(char c) {
   if (c >= 'a' && c <= 'z')
      return c + 'A' - 'a';
   if (c >= 'A' && c <= 'Z')
      return c - ('A' - 'a');
   return c;
}
