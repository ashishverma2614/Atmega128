#ifndef UART_FUNC_H_
#define UART_FUNC_H_

void UART_Init(void);
void UART0_Putch(char ch);
char UART0_Getch(void);
void UART0_Puts(char str[]);
void UART1_Putch(char ch);
void SendCommand(unsigned char cmd);
void DisplayMenu(void);
#endif /* UART_FUNC_H_ */