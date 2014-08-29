#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>

#include "isr.h"
#include "pic.h"

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

extern void idt_flush(uint32_t);

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

idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
   idt_entries[num].base_lo = base & 0xFFFF;
   idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

   idt_entries[num].sel     = sel;
   idt_entries[num].always0 = 0;
   // We must uncomment the OR below when we get to using user-mode.
   // It sets the interrupt gate's privilege level to 3.
   idt_entries[num].flags   = flags /* | 0x60 */;
}

static void init_idt()
{
   idt_ptr.limit = sizeof(idt_entry_t) * 256 -1;
   idt_ptr.base  = (uint32_t)&idt_entries;

   // memset(&idt_entries, 0, sizeof(idt_entry_t)*256);
   for (size_t i = 0; i < sizeof(idt_entry_t)*256; i++)
      *(((uint8_t *)&idt_entries) + i) = 0;

   idt_set_gate( 0, (uint32_t)isr0 , 0x08, 0x8E);
   idt_set_gate( 1, (uint32_t)isr1 , 0x08, 0x8E);
   idt_set_gate( 2, (uint32_t)isr2 , 0x08, 0x8E);
   idt_set_gate( 3, (uint32_t)isr3 , 0x08, 0x8E);
   idt_set_gate( 4, (uint32_t)isr4 , 0x08, 0x8E);
   idt_set_gate( 5, (uint32_t)isr5 , 0x08, 0x8E);
   idt_set_gate( 6, (uint32_t)isr6 , 0x08, 0x8E);
   idt_set_gate( 7, (uint32_t)isr7 , 0x08, 0x8E);
   idt_set_gate( 8, (uint32_t)isr8 , 0x08, 0x8E);
   idt_set_gate( 9, (uint32_t)isr9 , 0x08, 0x8E);
   idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
   idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
   idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
   idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
   idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
   idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
   idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
   idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
   idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
   idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
   idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
   idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
   idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
   idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
   idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
   idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
   idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
   idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
   idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
   idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
   idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
   idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

   idt_flush((uint32_t)&idt_ptr);
}

#define LEN_16 6
#define LEN_32 11
#define LEN_64 21

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
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main()
{
   terminal_initialize();

   asm volatile ("cli");
   uint8_t target[24];
   for (int i = 0; i < 3; i++) {
      encodeGdtEntry(target + (i * 8), gdt[i]);
   }
   setGdt(target, sizeof(target));
   
   PIC_remap(0x20, 0x28);

   init_idt();

   reloadSegments();

   asm volatile ("sti");

   terminal_writestring("Hello, kernel World!");
   
   while (1)
    ; // idle it up
}

void isr_handler(registers_t regs)
{
   terminal_writestring("\nrecieved interrupt -- address: ");
   terminal_writehex32((uint32_t)&regs);
   terminal_writestring("  -- value:");
   terminal_writeint32(regs.int_no);
   terminal_writestring("\n");
}
