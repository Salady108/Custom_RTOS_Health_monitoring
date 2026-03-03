#ifndef I2C_H
#define I2C_H
#include <stdint.h>
void i2c_init(void);
void i2c_send(uint8_t address, uint8_t*buffer, uint32_t len);
void i2c_recv(uint8_t address,uint8_t *buffer, uint32_t len);
#endif