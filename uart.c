#include "gpio.h"
// Aux Peripheral Registers (Mini UART)
#define AUX_ENABLE      ((volatile uint32_t*)(MMIO_BASE + 0x00215004))
#define AUX_MU_IO       ((volatile uint32_t*)(MMIO_BASE + 0x00215040)) //read write from UART FIFOS
#define AUX_MU_IER      ((volatile uint32_t*)(MMIO_BASE + 0x00215044))//to enable interrupts
#define AUX_MU_IIR      ((volatile uint32_t*)(MMIO_BASE + 0x00215048)) //shows interrupt status
#define AUX_MU_LCR      ((volatile uint32_t*)(MMIO_BASE + 0x0021504C))// controls line data format and access to baudrate reg
#define AUX_MU_MCR      ((volatile uint32_t*)(MMIO_BASE + 0x00215050)) //controls modem signal
#define AUX_MU_LSR      ((volatile uint32_t*)(MMIO_BASE + 0x00215054)) //shows data status
#define AUX_MU_CNTL     ((volatile uint32_t*)(MMIO_BASE + 0x00215060)) //single byte status
#define AUX_MU_BAUD     ((volatile uint32_t*)(MMIO_BASE + 0x00215068)) //direct access to 16 bit wide baudrate counter

void uart_init(void){
    gpio_funct_select(14,2);
    gpio_funct_select(15,2);
    *AUX_ENABLE |=1; 
    *AUX_MU_CNTL=0; //disable transmitter
    *AUX_MU_IER=0; //disables interrupts
    *AUX_MU_LCR=3; //11 : the UART works in 8-bit mode
    *AUX_MU_MCR=9;//
    *AUX_MU_BAUD=270;//waits 270 cycles between sending each bit
    *AUX_MU_CNTL=3;//enabling transmitter again
}

void uart_send(char c) {
    while(!(*AUX_MU_LSR & 0x20)); //uart is slow , cpu fast, asks cpu to wait to send each character
    *AUX_MU_IO=c;
}
 void uart_send_string(char* str){
    while(*str){ //loops till it reaches a null character
        if(*str=='\n') uart_send('\r'); //carries the cursor left next line
        uart_send(*str++); //send the character the fn
    }
 }
 void uart_send_hex(unsigned int n){
    char*digits="0123456789ABCDEF";
    uart_send('0'); uart_send('x'); //standard hex perfix
    for(int i=28;i>=0;i-=4){
        uart_send(digits[(n>>i)&0xF]);  //  (n >> i) & 0xF
                                       // Step A: Shift the bits we want to the far right.
                                      // Step B: Mask them so we ONLY see those 4 bits.
                                     // Step C: Look up the character in the 'digits' array.
    }

 }
 void uart_send_decimal(int num) {
    if (num == 0) {
        uart_send('0');
        return;
    }
    
    // Handle negative numbers (like negative gravity)
    if (num < 0) {
        uart_send('-');
        num = -num; // Make it positive for the math
    }
    
    char buffer[10];
    int i = 0;
    
    // Extract digits one by one
    while (num > 0) {
        buffer[i++] = (num % 10) + '0'; // Convert to ASCII character
        num /= 10;
    }
    
    // Print them out in reverse order
    while (i > 0) {
        uart_send(buffer[--i]);
    }
}
char uart_recv(void) {
    // Wait until the "RX FIFO Empty" flag is cleared
    // The exact register depends on whether you use PL011 or Mini UART.
    // Example for Mini UART (AUX_MU_LSR_REG):
    while(1) {
        if(*AUX_MU_LSR & 0x01) break; // Data is ready!
    }
    return (char)(*AUX_MU_IO & 0xFF);
}