#include <stdint.h>
#include "i2c.h"
#include "uart.h"
#include "mpu6050.h"

#define MPU6050_ADDR 0x68
#define REG_PWR_MGMT_1 0x6B
#define REG_ACCEL_XOUT_H 0x3B
#define REG_GYRO_XOUT_H 0x43

void mpu6050_init() {
    uint8_t buf[2];
    buf[0] = REG_PWR_MGMT_1;
    buf[1] = 0x00; // wake up sensor
    i2c_send(MPU6050_ADDR, buf, 2);
}

void mpu6050_read_accel(int32_t* ax, int32_t* ay, int32_t* az) {
    uint8_t reg = REG_ACCEL_XOUT_H;
    uint8_t data[6] = {0};

    i2c_send(MPU6050_ADDR, &reg, 1);
    for (volatile int i = 0; i < 1000; i++);
    i2c_recv(MPU6050_ADDR, data, 6);

    int16_t raw_x = (int16_t)((data[0] << 8) | data[1]);
    int16_t raw_y = (int16_t)((data[2] << 8) | data[3]);
    int16_t raw_z = (int16_t)((data[4] << 8) | data[5]);

    *ax = ((int32_t)raw_x * 1000) >> 14; // mg
    *ay = ((int32_t)raw_y * 1000) >> 14; // mg
    *az = ((int32_t)raw_z * 1000) >> 14; // mg
}

void mpu6050_read_gyro(int32_t* gx, int32_t* gy, int32_t* gz) {
    uint8_t reg = REG_GYRO_XOUT_H;
    uint8_t data[6] = {0};

    i2c_send(MPU6050_ADDR, &reg, 1);
    for (volatile int i = 0; i < 1000; i++);
    i2c_recv(MPU6050_ADDR, data, 6);

    int16_t raw_x = (int16_t)((data[0] << 8) | data[1]);
    int16_t raw_y = (int16_t)((data[2] << 8) | data[3]);
    int16_t raw_z = (int16_t)((data[4] << 8) | data[5]);

    *gx = ((int32_t)raw_x * 1000) / 131; // mdps
    *gy = ((int32_t)raw_y * 1000) / 131; // mdps
    *gz = ((int32_t)raw_z * 1000) / 131; // mdps
}