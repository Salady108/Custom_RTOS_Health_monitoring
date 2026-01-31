#include "gpio.h"
#include "timer.h"
#include "irq.h"

#define LED_PIN 4   // GPIO4 = physical pin 7


void kernel_main(void) {
    gpio_init_output(LED_PIN);

    timer_init(500000);
    irq_init();
    enable_irq();
    while(1){
    asm volatile("wfi");
}}
