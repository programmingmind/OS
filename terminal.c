#include "terminal.h"

#define LEN_16 6
#define LEN_32 11
#define LEN_64 21

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
static size_t terminal_row;
static size_t terminal_column;
static size_t default_terminal_color;
static uint8_t terminal_color;
static uint16_t* terminal_buffer;

uint8_t make_color(enum vga_color fg, enum vga_color bg) {
   return fg | bg << 4;
}
 
static uint16_t make_vgaentry(char c, uint8_t color) {
   uint16_t c16 = c;
   uint16_t color16 = color;
   return c16 | color16 << 8;
}
 
void terminal_initialize() {
   terminal_row = 0;
   terminal_column = 0;
   default_terminal_color = terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
   terminal_buffer = (uint16_t*) 0xB8000;
   for ( size_t y = 0; y < VGA_HEIGHT; y++ )
   {
      for ( size_t x = 0; x < VGA_WIDTH; x++ )
      {
         const size_t index = y * VGA_WIDTH + x;
         terminal_buffer[index] = make_vgaentry(' ', terminal_color);
      }
   }
}
 
void terminal_setcolor(uint8_t color) {
   terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
   const size_t index = y * VGA_WIDTH + x;
   terminal_buffer[index] = make_vgaentry(c, color);
}

static void terminal_scroll() {
   for (size_t i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++)
      terminal_buffer[i] = terminal_buffer[i + VGA_WIDTH];
   for (size_t i = VGA_WIDTH * (VGA_HEIGHT - 1);  i < VGA_HEIGHT * VGA_WIDTH; i++)
      terminal_buffer[i] = make_vgaentry(' ', default_terminal_color);
}

static void terminal_next_row() {
   if (terminal_row == VGA_HEIGHT - 1)
      terminal_scroll();
   else
      ++terminal_row;
}
 
void terminal_putchar(char c) {
   if (c == '\n') {
      terminal_next_row();
      terminal_column = 0;
   } else if (c == '\r') {
      terminal_column = 0;
   } else if (c == '\t') {
      size_t count = 8 - (terminal_column % 8);

      while (count--)
         terminal_putchar(' ');
   } else {
      terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
      if ( ++terminal_column == VGA_WIDTH ) {
         terminal_column = 0;
         terminal_next_row();
      }
   }

   // move flashing cursor
   uint16_t cursor = terminal_row * VGA_WIDTH + terminal_column;
   outb(0x3D4, 14);
   outb(0x3D5, cursor >> 8);
   outb(0x3D4, 15);
   outb(0x3D5, cursor);
}
 
void terminal_writestring(char *s) {
   while (*s)
      terminal_putchar(*s++);
}

void terminal_writeint(uint16_t i) {
   char data[LEN_16];
   uint8_t pos = LEN_16 - 1;

   if (i == 0) {
      terminal_putchar('0');
      return;
   }
   
   data[pos] = 0;
   while (i) {
      data[--pos] = '0' + (i % 10);
      i /= 10;
   }
   terminal_writestring(data + pos);
}

void terminal_writeint32(uint32_t i) {
   char data[LEN_32];
   uint8_t pos = LEN_32 - 1;
  
   if (i == 0) {
      terminal_putchar('0');
      return;
   }

   data[pos] = 0;
   while (i) {
      data[--pos] = '0' + (i % 10);
      i /= 10;
   }
   terminal_writestring(data + pos);
}

void terminal_writehex(uint16_t i) {
   char data[LEN_16];
   uint8_t pos = LEN_16 - 1;
   uint8_t tmp;

   if (i == 0) {
      terminal_putchar('0');
      return;
   }

   data[pos] = 0;
   while (i) {
      tmp = i % 16;
      data[--pos] = (tmp < 10) ? ('0' + tmp) : ('A' + tmp - 10);
      i /= 16;
   }
   terminal_writestring(data + pos);
}

void terminal_writehex32(uint32_t i) {
   char data[LEN_32];
   uint8_t pos = LEN_32 - 1;
   uint8_t tmp;

   if (i == 0) {
      terminal_putchar('0');
      return;
   }

   data[pos] = 0;
   while (i) {
      tmp = i % 16;
      data[--pos] = (tmp < 10) ? ('0' + tmp) : ('A' + tmp - 10);
      i /= 16;
   }
   terminal_writestring(data + pos);
}

void terminal_writehex64(uint64_t i) {
   char data[LEN_64];
   uint8_t pos = LEN_64 - 1;
   uint8_t tmp;

   if (i == 0) {
      terminal_putchar('0');
      return;
   }

   data[pos] = 0;
   while (i) {
      tmp = i % 16;
      data[--pos] = (tmp < 10) ? ('0' + tmp) : ('A' + tmp - 10);
      i /= 16;
   }
   terminal_writestring(data + pos);
}
