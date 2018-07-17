#ifndef UART0_H_
#define UART0_H_

typedef unsigned char unit8_t;

void UART0_init(void);
void TX0_ch(char);
unsigned char RX0_ch(void);
void TX0_str(const unsigned char*);
void TX0_1byte_num(unit8_t n);
void TX0_float(float);

#endif /* UART0_H_ */