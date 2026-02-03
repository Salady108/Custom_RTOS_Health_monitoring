#include "gpio.h"
#include "timer.h"
void timer_init(void)
{
    uint32_t freq; //asm is used to write to CPU register
    asm volatile("mrs %0, cntfrq_el0":"=r"(freq)); //asks how fast CPU ticks
    uint32_t interval=freq;
    
    asm volatile("msr cntp_tval_el0, %0" :: "r"(interval)); //CNTP_TVAL_el0 is a countdown reg, store and keep decrementing to 0 , when 0 fir interrupt
    uint32_t ctl=1; // it turns on the register CNTP
    asm volatile("msr cntp_ctl_el0, %0" :: "r"(ctl));

}
void timer_reset(void) {
    uint32_t freq; //mrs move reg to sys reg
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("msr cntp_tval_el0, %0" :: "r"(freq));
}
