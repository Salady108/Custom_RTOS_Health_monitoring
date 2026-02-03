#include "gpio.h"

void gpio_init_output(int pin) {
    volatile uint32_t* reg;
    uint32_t shift = (pin % 10) * 3; //configures GPIO pin as o/p, each pin uses 3 bits 

    // Select correct GPFSEL register
    reg = (volatile uint32_t*)(GPIO_BASE + (pin / 10) * 4); // each register is 4 bytes wide 

    uint32_t val = *reg;
    val &= ~(7 << shift);    // clear
    val |=  (1 << shift);    // set to "001" = output
    *reg = val;
}

void gpio_set(int pin) {
    *GPSET0 = (1u << pin);
}

void gpio_clear(int pin) {
    *GPCLR0 = (1 << pin);
}

int gpio_read(int pin) {
    return (*GPLEV0 & (1 << pin)) ? 1 : 0;
}