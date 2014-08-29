#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>
 
/* Check if the compiler thinks if we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

extern void setGdt(void*, size_t);
extern void reloadSegments();

struct GDT {
   uint32_t base;
   uint32_t limit;
   uint8_t type;
};

struct GDT gdt[] = {
   {0, 0, 0},                     // Selector 0x00 cannot be used
   {0, 0xffffffff, 0x9A},         // Selector 0x08 will be our code
   {0, 0xffffffff, 0x92},         // Selector 0x10 will be our data
   //{base=&myTss, limit=sizeof(myTss), type=0x89}; // You can use LTR(0x18)
};

/**
 * \param target A pointer to the 8-byte GDT entry
 * \param source An arbitrary structure describing the GDT entry
 */
void encodeGdtEntry(uint8_t *target, struct GDT source)
{
    // Check the limit to make sure that it can be encoded
    if ((source.limit > 65536) && (source.limit & 0xFFF) != 0xFFF) {
        // kerror("You can't do that!");
    }
    if (source.limit > 65536) {
        // Adjust granularity if required
        source.limit = source.limit >> 12;
        target[6] = 0xC0;
    } else {
        target[6] = 0x40;
    }
 
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] |= (source.limit >> 16) & 0xF;
 
    // Encode the base 
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
 
    // And... Type
    target[5] = source.type;
}
 
/* Hardware text mode color constants. */
enum vga_color
{
   COLOR_BLACK = 0,
   COLOR_BLUE = 1,
   COLOR_GREEN = 2,
   COLOR_CYAN = 3,
   COLOR_RED = 4,
   COLOR_MAGENTA = 5,
   COLOR_BROWN = 6,
   COLOR_LIGHT_GREY = 7,
   COLOR_DARK_GREY = 8,
   COLOR_LIGHT_BLUE = 9,
   COLOR_LIGHT_GREEN = 10,
   COLOR_LIGHT_CYAN = 11,
   COLOR_LIGHT_RED = 12,
   COLOR_LIGHT_MAGENTA = 13,
   COLOR_LIGHT_BROWN = 14,
   COLOR_WHITE = 15,
};
 
uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
   return fg | bg << 4;
}
 
uint16_t make_vgaentry(char c, uint8_t color)
{
   uint16_t c16 = c;
   uint16_t color16 = color;
   return c16 | color16 << 8;
}
 
size_t strlen(const char* str)
{
   size_t ret = 0;
   while ( str[ret] != 0 )
      ret++;
   return ret;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t terminal_row;
size_t terminal_column;
size_t default_terminal_color;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize()
{
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
 
void terminal_setcolor(uint8_t color)
{
   terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
   const size_t index = y * VGA_WIDTH + x;
   terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_scroll() {
   for (size_t i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++)
      terminal_buffer[i] = terminal_buffer[i + VGA_WIDTH];
   for (size_t i = VGA_WIDTH * (VGA_HEIGHT - 1);  i < VGA_HEIGHT * VGA_WIDTH; i++)
      terminal_buffer[i] = make_vgaentry(' ', default_terminal_color);
}

void terminal_next_row() {
   if (terminal_row == VGA_HEIGHT - 1)
      terminal_scroll();
   else
      ++terminal_row;
}
 
void terminal_putchar(char c)
{
   if (c == '\n') {
      terminal_next_row();
      terminal_column = 0;
      return;
   }

   if (c == '\t') {
      size_t count = 8 - (terminal_column % 8);

      while (count--)
         terminal_putchar(' ');

      return;
   }

   terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
   if ( ++terminal_column == VGA_WIDTH )
   {
      terminal_column = 0;
      terminal_next_row();
   }
}
 
void terminal_writestring(const char* data)
{
   size_t datalen = strlen(data);
   for ( size_t i = 0; i < datalen; i++ )
      terminal_putchar(data[i]);
}
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main()
{
   asm volatile ("cli");
   uint8_t target[24];
   for (int i = 0; i < 3; i++) {
      encodeGdtEntry(target + (i * 8), gdt[i]);
   }
   setGdt(target, sizeof(target));
   reloadSegments();
   asm volatile ("sti");

   terminal_initialize();
   
   for (size_t i = 0; i < 20; i++) {
      for (size_t j = 0; j < i; j++)
         for (uint8_t k = 0; k < 5; k++)
            terminal_writestring(" ");

      terminal_writestring("Hello, kernel World!\n");
   }
}
