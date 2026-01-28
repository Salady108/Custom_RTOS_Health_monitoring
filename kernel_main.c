#include "/mnt/shared/C++programming/Rpi_practice/gpio.h"

#define LED_PIN 4   // GPIO4 = physical pin 7

static void delay(unsigned long count) {
    while (count--) {
        asm volatile("nop"); //no operation , volatile prevents it from getting removed from code by compiler 
    }
}

void kernel_main(void) {
    gpio_init_output(LED_PIN);

    while (1) {
        gpio_set(LED_PIN);
        delay(500000); // this basically runs the delay loop 500000 iterations, one iteration is in nano sesonds 

        gpio_clear(LED_PIN);
        delay(500000);
    }
}
