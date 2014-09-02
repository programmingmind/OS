#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);

size_t strlen(const char* str);
uint8_t isAlpha(char c);
char toUpper(char c);
char toLower(char c);
char changeCase(char c);

#endif
