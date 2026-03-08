#include "i2c.h"
#include "uart.h"
#define MPU6050_ADDR 0x68
#define REG_PWR_MGMT_1 0x6B
#define REG_ACCEL_XOUT_H 0x3B
#define REG_GYRO_XOUT_H 0x43
void mpu6050_init() {
    uint8_t buf[2];

    buf[0]=REG_PWR_MGMT_1;
    buf[1]=0x00;

    i2c_send(MPU6050_ADDR,buf,2);
}
void mpu6050_read_accel(int16_t* ax, int16_t* ay, int16_t* az) {
    uint8_t reg = REG_ACCEL_XOUT_H;
    
    // Initialize array to 0 to prevent reading "ghost memory"
    uint8_t data[6] = {0, 0, 0, 0, 0, 0}; 

    // 1. Tell sensor which register we want
    i2c_send(MPU6050_ADDR, &reg, 1);

    // Tiny pause to give the sensor a microsecond to prepare
    for(volatile int i=0; i<1000; i++); 

    // 2. Read the 6 bytes
    i2c_recv(MPU6050_ADDR, data, 6);

    // 3. Stitch them together
    *ax = (int16_t)((data[0] << 8) | data[1]);
    *ay = (int16_t)((data[2] << 8) | data[3]);
    *az = (int16_t)((data[4] << 8) | data[5]);
}
void mpu6050_read_gyro(int16_t* gx, int16_t* gy, int16_t* gz) {
    uint8_t reg = REG_GYRO_XOUT_H;
    
    // Initialize array to 0 to prevent reading "ghost memory"
    uint8_t data[6] = {0, 0, 0, 0, 0, 0}; 

    // 1. Tell sensor we want the Gyroscope registers
    i2c_send(MPU6050_ADDR, &reg, 1);

    // Tiny pause
    for(volatile int i=0; i<1000; i++); 

    // 2. Read the 6 bytes (X_High, X_Low, Y_High, Y_Low, Z_High, Z_Low)
    i2c_recv(MPU6050_ADDR, data, 6);

    // 3. Stitch them together just like the accelerometer
    *gx = (int16_t)((data[0] << 8) | data[1]);
    *gy = (int16_t)((data[2] << 8) | data[3]);
    *gz = (int16_t)((data[4] << 8) | data[5]);
}