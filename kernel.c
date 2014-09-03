#if !defined(__cplusplus)
#include <stdbool.h> /* C doesn't have booleans by default. */
#endif
#include <stddef.h>
#include <stdint.h>

#include "common.h"
#include "keyboard.h"
#include "pic.h"
#include "tables.h"
#include "terminal.h"
#include "timer.h"

/* Check if the compiler thinks if we are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif
 
/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

struct GDT gdt[] = {
   {0, 0, 0},                     // Selector 0x00 cannot be used
   {0, 0xffffffff, 0x9A},         // Selector 0x08 will be our code
   {0, 0xffffffff, 0x92},         // Selector 0x10 will be our data
   //{base=&myTss, limit=sizeof(myTss), type=0x89}; // You can use LTR(0x18)
};
 
#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main()
{
   terminal_initialize();

   asm volatile ("cli");
   PIC_remap(0x20, 0x28);
   for (unsigned char i = 0; i < 32; i++)
    IRQ_set_mask(i);
  IRQ_clear_mask(0x00);
  IRQ_clear_mask(0x01);

   uint8_t target[24];
   for (int i = 0; i < 3; i++) {
      encodeGdtEntry(target + (i * 8), gdt[i]);
   }
   setGdt(target, sizeof(target));

   init_idt();

   reloadSegments();

   init_timer(1000);

   asm volatile ("sti");

   terminal_writestring("Hello, kernel World!");
   
   while (1) {
    timer_wait(500);
    terminal_writestring("\n0.5 seconds\n");
   }
}

void isr_handler(registers_t regs) {
  switch (regs.int_no) {
  case 0x20:
    timer_handler(&regs);
    break;
  case 0x21:
    keyboard_handler(&regs);
    break;
  default:
    terminal_writestring("\nrecieved interrupt\taddress: ");
    terminal_writehex32((uint32_t)&regs);
    terminal_writestring("\tvalue: 0x");
    terminal_writehex32(regs.int_no);
    terminal_writestring("\terror: ");
    terminal_writehex32(regs.err_code);
    terminal_writestring("\n");
  }

  PIC_sendEOI(regs.int_no);
}
