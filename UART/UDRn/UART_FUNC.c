#include <avr/io.h>
#include <stdio.h>

typedef unsigned char unit8_t;

void UART_init(unsigned int ubrr0)
{
	UBRR0H = (unsigned char) (ubrr0 >> 8);
	UBRR0L = (unsigned char) (ubrr0);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_Putch(char ch) 
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = ch;
}

void UART_Puts(char * str)
{
	int i = 0;
	while(str[i] != 0)
	{
		UART_Putch(str[i++]);
	}
}

char UART_Getch(void)	  
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
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
	str[i - 1] = 0; // append 'NULL' char at the '\r' position to avoid moving cursor to the first
}

