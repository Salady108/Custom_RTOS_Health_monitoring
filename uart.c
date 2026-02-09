#include "gpio.h"
// Aux Peripheral Registers (Mini UART)
#define AUX_ENABLE      ((volatile uint32_t*)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO       ((volatile uint32_t*)(MMIO_BASE + 0x00215040))
#define AUX_MU_IER      ((volatile uint32_t*)(MMIO_BASE + 0x00215044))
#define AUX_MU_IIR      ((volatile uint32_t*)(MMIO_BASE + 0x00215048))
#define AUX_MU_LCR      ((volatile uint32_t*)(MMIO_BASE + 0x0021504C))
#define AUX_MU_MCR      ((volatile uint32_t*)(MMIO_BASE + 0x00215050))
#define AUX_MU_LSR      ((volatile uint32_t*)(MMIO_BASE + 0x00215054))
#define AUX_MU_CNTL     ((volatile uint32_t*)(MMIO_BASE + 0x00215060))
#define AUX_MU_BAUD     ((volatile uint32_t*)(MMIO_BASE + 0x00215068))

void uart_init(void){
    gpio_funct_select(14,2);
    gpio_funct_select(15,2);
    *AUX_ENABLE |=1;
    *AUX_MU_CNTL=0;
    *AUX_MU_IER=0;
    *AUX_MU_LCR=3;
    *AUX_MU_MCR=9;
    *AUX_MU_BAUD=270;
    *AUX_MU_CNTL=3;
}

void uart_send(char c) {
    while(!(*AUX_MU_LSR & 0x20));
    *AUX_MU_IO=c;
}
 void uart_send_string(char* str){
    while(*str){
        if(*str=='\n') uart_send('\r');
        uart_send(*str++);
    }
 }
 void uart_send_hex(unsigned int n){
    char*digits="0123456789ABCDEF";
    uart_send('0'); uart_send('x');
    for(int i=28;i>=0;i-=4){
        uart_send(digits[(n>>i)&0xF]);
    }
 }