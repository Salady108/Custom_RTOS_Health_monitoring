#include "gpio.h"
#include "timer.h"
#include "irq.h"
#include "uart.h"
#include "i2c.h"

// Define BSC1_S register for checking errors
#define BSC1_S ((volatile uint32_t*)(MMIO_BASE + 0x00804004))
#define BSC_S_ERR (1 << 8)

// Use volatile to prevent optimization
void delay_cycles(int delay){
    for (volatile int i=0; i<delay; i++){
        // Do nothing
    }
}

void kernel_main(void) {
    uart_init();
    
    // We don't need these for a simple scanner, and they might cause noise.
    // irq_init();     // <--- Commented out
    // timer_init();   // <--- Commented out
    // enable_irq();   // <--- Commented out

    uart_send_string("Initializing I2C...\n");
    i2c_init(); // Set up GPIO 2 & 3
    
    uart_send_string("Scanning I2C Bus...\n");

    // Loop through all valid I2C addresses (0x03 to 0x77)
    for(uint8_t addr = 0x03; addr < 0x78; addr++) {
        uint8_t dummy_data = 0;
        
        // Try to read 1 byte.
        // If the device exists, it will ACK. If not, it will NACK (Error).
        i2c_recv(addr, &dummy_data, 1);
        
        // Check the Status Register for the ERR bit
        if(*BSC1_S & BSC_S_ERR){
            // Error bit is SET (1) -> No device answered (NACK).
            // Do nothing.
        } else {
            // Error bit is CLEAR (0) -> Device answered (ACK)!
            uart_send_string("Device found at: ");
            uart_send_hex(addr);
            uart_send_string("\n");
        }
        
        delay_cycles(10000); // Small delay to settle the bus
    } // <--- The Loop ends HERE

    uart_send_string("Scan complete.\n"); // Only print this ONCE
    
    while(1) {
        asm volatile("wfi");
    }
}