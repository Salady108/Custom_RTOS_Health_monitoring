#include "uart.h"
#include "i2c.h"
#include "mpu6050.h"

void delay_cycles(int delay){
    for (volatile int i=0; i<delay; i++){ }
}

void kernel_main(void) {
    uart_init();
    uart_send_string("\n\n========================================\n");
    uart_send_string("      6-AXIS IMU STREAMING ACTIVE       \n");
    uart_send_string("========================================\n");

    i2c_init();
    mpu6050_init();

    // Variables for all 6 axes
    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    while(1) {
        // 1. Fetch ALL the data
        mpu6050_read_accel(&ax, &ay, &az);
        
        // Give the I2C bus a tiny microsecond to breathe between requests
        for(volatile int i=0; i<1000; i++); 
        
        mpu6050_read_gyro(&gx, &gy, &gz);

        // 2. Print Accelerometer Data (Gravity & Tilt)
        uart_send_string("ACC[ ");
        uart_send_decimal(ax); uart_send_string(", ");
        uart_send_decimal(ay); uart_send_string(", ");
        uart_send_decimal(az); uart_send_string(" ]   |   ");

        // 3. Print Gyroscope Data (Rotation & Spin)
        uart_send_string("GYRO[ ");
        uart_send_decimal(gx); uart_send_string(", ");
        uart_send_decimal(gy); uart_send_string(", ");
        uart_send_decimal(gz); uart_send_string(" ]");

        // 4. Overwrite the line to keep the terminal clean
        // Lots of spaces at the end ensure shrinking numbers don't leave ghost digits
        uart_send_string("          \r"); 

        // 5. Delay for readability (~3 updates a second)
        delay_cycles(2000000); 
    }
}