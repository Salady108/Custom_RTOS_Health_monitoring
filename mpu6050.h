#ifndef MPU6050_H
#define MPU6050_H
#include <stdint.h>
void mpu6050_init(void);
void mpu6050_read_accel(int16_t* x, int16_t* y, int16_t* z);
void mpu6050_read_gyro(int16_t* gx,int16_t* gy,int16_t* gz);
#endif