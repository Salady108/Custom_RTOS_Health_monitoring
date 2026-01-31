#include "gpio.h"
#include "timer.h"
static int led_state=0;
void irq_handler(void){
    if(SYS_TIMER_CS&(1<<1))
    {
        timer_clear_interrupt();
        if(led_state)
        {
            gpio_clear(21);
            led_state=0;
        }
        else{
            gpio_set(21);
            led_state=1;
        }
        timer_schedule(500000);
    }
}