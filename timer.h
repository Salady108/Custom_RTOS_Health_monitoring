#ifndef TIMER_H
#define TIMER_H
#include<stdint.h>
void timer_init(uint32_t usec);
void timer_clear_interrupt(void);
void timer_schedule(uint32_t usec);
#endif