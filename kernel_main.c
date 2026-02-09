#include "gpio.h"
#include "timer.h"
#include "irq.h"
#include"uart.h"



void kernel_main(void) {
    uart_init();
    uart_send_string("Hello from Bare Metal Pi\n");
    irq_init();
    timer_init();
    enable_irq();
    uart_send_string("TImer started. Waiting for interrupts...\n");
    while(1) {
        asm volatile("wfi");
    }
}
