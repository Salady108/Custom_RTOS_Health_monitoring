#ifndef UART_H
#define UART_H
void uart_init(void);
void uart_send(char c);
void uart_send_string(char* str);
void uart_send_hex(unsigned int n);
void uart_send_decimal(int num);
char uart_recv(void);
#endif