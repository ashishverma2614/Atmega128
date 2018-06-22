/*
 * UART_FUNC.h
 *
 * Created: 2018-06-18 오후 4:04:44
 *  Author: Woo
 */ 


#ifndef UART_FUNC_H_
#define UART_FUNC_H_

void UART_Putch(char ch) // print one character
{
	while(!(UCSR0A & 0x20));

	UDR0 = ch;
}

char UART_Getch(void) // to receive one char
{
	while(!(UCSR0A & 0x80));

	return UDR0;
}

void UART_Puts(char * str) // print string
{
	int i = 0;
	while(str[i] != 0) // until encountering null character
	// store each character into str[]
	{
		UART_Putch(str[i++]);
	}
}

void UART_Gets(char * str) // to receive string character
{
	int i = 0;
	do
	{
		str[i] = UART_Getch();
		UART_Putch(str[i]); // echo to reflect
	} while(str[i++] != '\r');
	UART_Puts("\r\n"); // move cursor to the end of string character
	str[i - 1] = 0; // add NULL char in the '\r' position to avoid moving cursor to the first
}



#endif /* UART_FUNC_H_ */