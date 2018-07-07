#include <avr/io.h>

void UART_Putch(char ch) // print one character
						 // this function should be called to print string
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = ch;
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
///////////////////////////////////////////////////////////////////////////////
char UART_Getch(void) // to receive one char
					  // this function should be called to read string
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

//TODO: analise this algorithm
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

