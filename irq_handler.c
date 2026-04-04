#include <stdint.h>

#include "timer.h"
#include "scheduler.h"

volatile uint64_t ticks = 0;

uintptr_t irq_handler(uintptr_t current_sp) {
    timer_reset();
    ticks++;
    return scheduler_tick(current_sp);
}