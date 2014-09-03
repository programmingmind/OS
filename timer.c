#include "common.h"
#include "terminal.h"
#include "timer.h"

#define SYSTEM_FREQ 1193180

static volatile uint32_t timer_ticks;
static volatile uint32_t timer_freq;
static volatile uint32_t timer_divisor;

void init_timer(uint32_t hz) {
   timer_ticks = 0;
   timer_freq = hz;
   timer_divisor = SYSTEM_FREQ / timer_freq;       /* Calculate our divisor */
   outb(0x43, 0x36);             /* Set our command byte 0x36 */
   outb(0x40, timer_divisor & 0xFF);   /* Set low byte of divisor */
   outb(0x40, timer_divisor >> 8);     /* Set high byte of divisor */
}

void timer_handler(registers_t *r) {
   ++timer_ticks;
}

void timer_wait(uint16_t milliseconds) {
   uint32_t limit = timer_ticks + ((SYSTEM_FREQ * milliseconds) / (timer_divisor * 1000));
   while (timer_ticks < limit) {
      // terminal_writeint32(timer_ticks);
      // terminal_writestring("\t");
      // terminal_writeint32(limit);
      // terminal_writestring("\n");
   }
}
