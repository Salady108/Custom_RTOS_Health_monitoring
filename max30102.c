#include "max30102.h"
#include "i2c.h"

// Standard I2C address for MAX30102 and MAX30105
#define MAX30102_ADDR 0x57 

// Helper function to write a single register
void max30102_write_reg(uint8_t reg, uint8_t value) {
    uint8_t buf[2] = {reg, value};
    i2c_send(MAX30102_ADDR, buf, 2);
}

void max30102_init(void) {
    // 1. HARD RESET
    max30102_write_reg(0x09, 0x40); 
    for(volatile int i=0; i<500000; i++); // Wait for reboot

    // 2. SPO2 CONFIG: 4096nA range, 100Hz, 411us pulse width
    max30102_write_reg(0x0A, 0x27); 

    // 3. LED POWER: Medium-Safe Limit (0x24 = ~7.6mA)
    max30102_write_reg(0x0C, 0x24); // Red LED
    max30102_write_reg(0x0D, 0x24); // IR LED
    
    // THE FIX: Explicitly turn OFF the 3rd (Green) LED
    max30102_write_reg(0x0E, 0x00); 

    // 4. MULTI-LED SLOTS
    max30102_write_reg(0x11, 0x21); // Slot 1 = Red, Slot 2 = IR
    
    // THE FIX: Explicitly disable Slots 3 and 4 
    max30102_write_reg(0x12, 0x00); 

    // 5. MODE CONFIG: MULTI-LED MODE (0x07)
    max30102_write_reg(0x09, 0x07); 

    // 6. FIFO FLUSH
    max30102_write_reg(0x04, 0x00);
    max30102_write_reg(0x05, 0x00);
    max30102_write_reg(0x06, 0x00);
}

void max30102_read_fifo(uint32_t *red, uint32_t *ir) {
    // 1. CLEAR INTERRUPT STATUS
    // Tells the sensor we are ready for the next batch of data
    uint8_t int_reg = 0x00;
    uint8_t dummy = 0;
    i2c_send(MAX30102_ADDR, &int_reg, 1);
    i2c_recv(MAX30102_ADDR, &dummy, 1);

    // 2. FETCH FIFO DATA
    uint8_t reg = 0x07;
    uint8_t data[6] = {0};
    
    // Reading these 6 bytes automatically advances the sensor's internal Read Pointer.
    // We DO NOT need to manually clear it anymore!
    i2c_send(MAX30102_ADDR, &reg, 1);
    i2c_recv(MAX30102_ADDR, data, 6);

    // 3. PARSE 18-BIT DATA
    *red = ((uint32_t)data[0] << 16 | (uint32_t)data[1] << 8 | data[2]) & 0x03FFFF;
    *ir  = ((uint32_t)data[3] << 16 | (uint32_t)data[4] << 8 | data[5]) & 0x03FFFF;
    
    // Notice: The "Aggressive FIFO Flush" lines that were here have been DELETED.
}