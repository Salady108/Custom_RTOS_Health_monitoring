#include "gpio.h"
#include "irq.h"
#include "stdint.h"
#define CORE0_TIMER_IRQCNTL 0x40000040

void irq_init(void){
    volatile uint32_t *timer_irq_routing = (volatile uint32_t *)CORE0_TIMER_IRQCNTL;
    *timer_irq_routing = 0x02;
}
void enable_irq(void){
    asm volatile ("msr daifclr,#2");
}
