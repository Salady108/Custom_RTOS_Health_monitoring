#include "gpio.h"
#include "irq.h"
#include "stdint.h"
#define CORE0_TIMER_IRQCNTL 0x40000040 // address to core 0 interrrupt source reg

void irq_init(void){
    volatile uint32_t *timer_irq_routing = (volatile uint32_t *)CORE0_TIMER_IRQCNTL;
    *timer_irq_routing = 0x02; // by default CPU ignores interrupt, by writing 0x02, it tells not to ignore
}
void enable_irq(void){
    asm volatile ("msr daifclr,#2");
    //daif bits stores do not disturb sign, daifclr clears those bits so that cpu can handle interrupts
}

void disable_irq(void){
    asm volatile ("msr daifset,#2");
}
