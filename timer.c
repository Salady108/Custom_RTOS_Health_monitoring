#include "gpio.h"
#include "timer.h"
void timer_init(void)
{
    uint32_t freq;
    asm volatile("mrs %0, cntfrq_el0":"=r"(freq));
    uint32_t interval=freq;
    
    asm volatile("msr cntp_tval_el0, %0" :: "r"(interval));
    uint32_t ctl=1;
    asm volatile("msr cntp_ctl_el0, %0" :: "r"(ctl));

}
void timer_reset(void) {
    uint32_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    asm volatile("msr cntp_tval_el0, %0" :: "r"(freq));
}
