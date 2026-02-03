#include "gpio.h"
#include "timer.h"
static int led_state=0;
void irq_handler(void){
    timer_reset(); //generic timer is level triggered, need to reset it 
   
    
        if(led_state)
        {
            gpio_clear(4);
            led_state=0;
        }
        else{
            gpio_set(4);
            led_state=1;
        
} }