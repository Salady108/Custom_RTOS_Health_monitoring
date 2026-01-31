#include "gpio.h"
#include "timer.h"
void timer_init(uint32_t usec)
{
    uint32_t now=SYS_TIMER_CLO;
    SYS_TIMER_C1=now+usec;
    SYS_TIMER_CS+(1<<1);
}
void timer_clear_interrupt(void){
    SYS_TIMER_CS=(1<<1);

}
void timer_schedule(uint32_t usec){
    SYS_TIMER_C1=SYS_TIMER_CLO +usec;

}
