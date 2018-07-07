#ifndef UART1_H_
#define UART1_H_

typedef unsigned char unit8_t;

void UART1_init(void);
void UART1_transmit(char data);
unsigned char UART1_receive(void);
void UART1_print_string(char str[]);
void UART1_print_1_byte_number(unit8_t n);



#endif /* UART1_H_ */