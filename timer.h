#ifndef TIMER_H
#define TIMER_H

#include "tables.h"

void init_timer(uint32_t hz);

void timer_handler(registers_t *r);

void timer_wait(uint16_t milliseconds);

#endif
