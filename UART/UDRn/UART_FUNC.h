/*
 * UART_FUNC.h
 *
 * Created: 2018-06-18 오후 4:04:44
 *  Author: Woo
 */ 


#ifndef UART_FUNC_H_
#define UART_FUNC_H_
void UART_init(unsigned int);
void UART_Putch(char ch);
char UART_Getch(void);
void UART_Puts(char * str);
void UART_Gets(char * str);

#endif /* UART_FUNC_H_ */