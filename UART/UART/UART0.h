
#ifndef UART0_H_
#define UART0_H_

typedef unsigned char unit8_t;

void UART0_init(unsigned int);
void TX0_ch(char);
unit8_t RX0_ch(void);
void TX0_str(unit8_t *);
void TX0_1byte_num(unit8_t n);
void TX0_float(float);
unit8_t RX0_ch(void);
void RX0_str(unit8_t *);
unit8_t RX0_isGetch(void);



#endif /* UART0_H_ */