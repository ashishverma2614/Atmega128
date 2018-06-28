#ifndef UART0_H_
#define UART0_H_

typedef unsigned char unit8_t;

void UART0_init(void);
void UART0_transmit(char data);
unsigned char UART0_receive(void);
void UART0_print_string(char str[]);
void UART0_print_1_byte_number(unit8_t n);
void UART0_print_float(float f);



#endif /* UART0_H_ */