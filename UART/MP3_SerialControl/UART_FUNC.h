#ifndef UART_FUNC_H_
#define UART_FUNC_H_

void UART_Init(void); // initial setting for UART0(Serial Monitor), 1(MP3)
void UART0_Putch(char ch); // TX char for serial monitor
char UART0_Getch(void); // RX for serial monitor
void UART0_Puts(char str[]); // TX string for serial monitor
void UART1_Putch(char ch); // TX for MP3
void SendCommand(unsigned char cmd); // TX for MP3 command using UART1_Putch
void DisplayMenu(void); // display menu on serial monitor
#endif /* UART_FUNC_H_ */