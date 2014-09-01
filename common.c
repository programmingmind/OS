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
