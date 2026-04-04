#ifndef MAX30102_H
#define MAX30102_H

#include <stdint.h>

void max30102_write_reg(uint8_t reg, uint8_t value);
void max30102_init(void);
void max30102_read_fifo(uint32_t *red, uint32_t *ir);

#endif // MAX30102_H