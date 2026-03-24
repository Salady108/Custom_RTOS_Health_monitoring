#include "i2c.h"
#include "uart.h"

// The physical I2C area code for the MAX30102
#define MAX30102_ADDR 0x57 

// Internal Mailboxes (Registers)
#define REG_MODE_CONFIG 0x09
#define REG_SPO2_CONFIG 0x0A
#define REG_LED1_PA     0x0C // Red LED Power
#define REG_LED2_PA     0x0D // IR LED Power
#define REG_FIFO_WR_PTR 0x04
#define REG_FIFO_RD_PTR 0x06
#define REG_FIFO_DATA   0x07

// Helper function to keep code clean
void max30102_write_reg(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    i2c_send(MAX30102_ADDR, buf, 2);
}

void max30102_init(void) {
    // 1. Set to SpO2 Mode (Turns on BOTH Red and IR LEDs)
    max30102_write_reg(REG_MODE_CONFIG, 0x03); 
    
    // 2. Configure the ADC (Range = 2048nA, Sample Rate = 100Hz, Pulse Width = 411us)
    max30102_write_reg(REG_SPO2_CONFIG, 0x27); 
    
    // 3. Set LED Brightness (0x24 is ~7mA power. Increase if the signal is weak)
    max30102_write_reg(REG_LED1_PA, 0x3F); 
    max30102_write_reg(REG_LED2_PA, 0x3F); 
    
    // 4. Reset the FIFO (First-In-First-Out) memory pointers to zero
    max30102_write_reg(REG_FIFO_WR_PTR, 0x00);
    max30102_write_reg(REG_FIFO_RD_PTR, 0x00);
}

void max30102_read_fifo(uint32_t *red, uint32_t *ir) {
    uint8_t reg = REG_FIFO_DATA;
    uint8_t data[6] = {0}; // 3 bytes for Red, 3 bytes for IR

    // 1. Point the sensor's reading glasses at the data mailbox
    i2c_send(MAX30102_ADDR, &reg, 1);
    
    // Tiny pause
    for(volatile int i=0; i<1000; i++); 

    // 2. Suck out 6 bytes in one burst
    i2c_recv(MAX30102_ADDR, data, 6);

    // 3. The 18-bit Magic Stitching
    // We shift the first byte 16 spaces, the second 8 spaces, and drop the third in.
    // We then use '& 0x03FFFF' to mask out any garbage in the top bits.
    *red = ((data[0] << 16) | (data[1] << 8) | data[2]) & 0x03FFFF;
    *ir  = ((data[3] << 16) | (data[4] << 8) | data[5]) & 0x03FFFF;
}