#include "gpio.h"
#include "irq.h"
void irq_init(void){
    IRQ_ENABLE1=(1<<1);
}
void enable_irq(void){
    asm volatile ("msr daifclr,#2");
}
