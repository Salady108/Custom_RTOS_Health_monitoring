#ifndef GPIO_H  /*prevents duplication from occuring*/
#define GPIO_H /*GPIO pins are controlled by writing to specific memory address */

#include <stdint.h> /*it defines fixed width integer types*/

#define MMIO_BASE   0x3F000000UL   // Pi 2/3 memory mapped I/O peripherals live here
// #define MMIO_BASE 0xFE000000UL   // Pi 4, uncomment if needed
#define GPIO_BASE   (MMIO_BASE + 0x200000) // because GPIO block starts at offset from hardware manual

#define GPFSEL0     ((volatile uint32_t*)(GPIO_BASE + 0x00)) //GPIO function select , volatile so that the cpu doesn't cache them,hardware can change the values
#define GPFSEL1     ((volatile uint32_t*)(GPIO_BASE + 0x04)) //for pins 10 to 19
#define GPSET0      ((volatile uint32_t*)(GPIO_BASE + 0x1C)) // sets GPIO pin high if 1 
#define GPCLR0      ((volatile uint32_t*)(GPIO_BASE + 0x28)) // sets GPIO pin low if 1
#define GPLEV0      ((volatile uint32_t*)(GPIO_BASE + 0x34)) // reads current pin level

#define SYS_TIMER_BASE (MMIO_BASE + 0x3000)
#define SYS_TIMER_CS (*(volatile unsigned int*)(SYS_TIMER_BASE+0X00))
#define SYS_TIMER_CLO (*(volatile unsigned int*)(SYS_TIMER_BASE+0x04))
#define SYS_TIMER_C1 (*(volatile unsigned int*)(SYS_TIMER_BASE+0x10))

#define IRQ_BASE (MMIO_BASE +0xB200)
#define IRQ_ENABLE1 (*(volatile unsigned int*)(IRQ_BASE + 0x210))

void gpio_init_output(int pin); //configure a GPIO as o/p
void gpio_set(int pin); //writes 1 to GPIOSET
void gpio_clear(int pin);//writes 1 to GPIO CLEAR
int  gpio_read(int pin); //reads from gPIOLEV
void gpio_func_select(int pin,int func);
#endif
//Why this matters for GPIO

//GPIO registers work like this:

//Each GPIO pin corresponds to one bit

//Bit number = GPIO pin number
//Example: Setting GPIO17 HIGH
//*GPSET0 = (1 << 17);
//This does NOT set all bits.
//It sets only bit 17.