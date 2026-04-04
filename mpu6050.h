#ifndef MPU6050_H
#define MPU6050_H
#include <stdint.h>
void mpu6050_init(void);
void mpu6050_read_accel(int32_t* x, int32_t* y, int32_t* z);
void mpu6050_read_gyro(int32_t* gx, int32_t* gy, int32_t* gz);
#endif