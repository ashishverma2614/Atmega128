#ifndef UART_H_
#define UART_H_

typedef unsigned char unit8_t;

void UART_init(unsigned int);
void Put_ch(char);
unsigned char Get_ch(void);
void Put_str(const unsigned char*);
void Put_1byte_num(unit8_t n);
void Put_float_num(float);

#endif /* UART_H_ */