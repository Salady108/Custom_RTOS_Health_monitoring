#include "gpio.h"
#include "timer.h"
#include"uart.h"
volatile uint64_t ticks=0;

void irq_handler(void){
    timer_reset(); //generic timer is level triggered, need to reset it 
    
    ticks++; //tells us that 1 ms has passed
    if(ticks%1000==0){
        //uart_send_string("SysTick: ");
        //uart_send_hex(ticks);
       // uart_send_string(" ms\n");
    }
    
       
} 