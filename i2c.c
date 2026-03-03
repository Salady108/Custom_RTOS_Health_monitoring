#include"gpio.h"
#include"uart.h"

#define BSC1_C      ((volatile uint32_t*)(MMIO_BASE + 0x00804000))
#define BSC1_S      ((volatile uint32_t*)(MMIO_BASE + 0x00804004))
#define BSC1_DLEN   ((volatile uint32_t*)(MMIO_BASE + 0x00804008))
#define BSC1_A      ((volatile uint32_t*)(MMIO_BASE + 0x0080400C))
#define BSC1_FIFO   ((volatile uint32_t*)(MMIO_BASE + 0x00804010))
#define BSC1_DIV    ((volatile uint32_t*)(MMIO_BASE + 0x00804014))
#define BSC1_DEL    ((volatile uint32_t*)(MMIO_BASE + 0x00804018))
#define BSC1_CLKT   ((volatile uint32_t*)(MMIO_BASE + 0x0080401C))

#define BSC_C_I2CEN     (1 << 15)
#define BSC_C_INTR      (1 << 10)
#define BSC_C_INTT      (1 << 9)
#define BSC_C_INTD      (1 << 8)
#define BSC_C_ST        (1 << 7)
#define BSC_C_CLEAR     (1 << 4)
#define BSC_C_READ      (1 << 0)

#define BSC_S_CLKT      (1 << 9)
#define BSC_S_ERR       (1 << 8)
#define BSC_S_RXF       (1 << 7)
#define BSC_S_TXE       (1 << 6)
#define BSC_S_RXD       (1 << 5)
#define BSC_S_TXD       (1 << 4)
#define BSC_S_RXR       (1 << 3)
#define BSC_S_TXW       (1 << 2)
#define BSC_S_DONE      (1 << 1)
#define BSC_S_TA        (1 << 0)

#define CLEAR_STATUS (BSC_S_CLKT |BSC_S_ERR | BSC_S_DONE)
void i2c_init(void){
    gpio_funct_select(2,4);
    gpio_funct_select(3,4);
    *BSC1_C=BSC_C_I2CEN|BSC_C_CLEAR;
    *BSC1_S=CLEAR_STATUS;

}
void i2c_send(uint8_t address, uint8_t *buffer,uint32_t len){
    *BSC1_A=address;
    *BSC1_DLEN=len;
    for (int i=0 ;i<len;i++){
        *BSC1_FIFO=buffer[i];
    }
    *BSC1_S=CLEAR_STATUS;
    *BSC1_C=BSC_C_I2CEN |BSC_C_ST;
    while (!(*BSC1_S & BSC_S_DONE)){

    }
}
void i2c_recv(uint8_t address, uint8_t *buffer, uint32_t len){
    *BSC1_A=address;
    *BSC1_DLEN=len;
    *BSC1_C=BSC_C_I2CEN | BSC_C_ST|BSC_C_READ;
    *BSC1_S=CLEAR_STATUS;
    while(!(*BSC1_S&BSC_S_DONE)){

    }
    for(int i=0; i<len; i++){
        buffer[i]=(uint8_t)(*BSC1_FIFO);
    }
}