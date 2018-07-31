#include <avr/io.h>
#ifndef UART_H_
#define UART_H_

void UART0_Init(unsigned int ubrr)
{
	UCSR0A = 0x00;
	UCSR0B = (0x01 << RXCIE0) | (0x01 << RXEN0) | (0x01 << TXEN0);
	UCSR0C = (0x01 << UCSZ01) | (0X01 << UCSZ00);
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
}

void UART1_Init(unsigned int ubrr)
{
	UCSR1B = (0x01 << RXCIE1) | (0x01 << RXEN1) | (0x01 << TXEN1);
	UCSR1C = (0x01 << UCSZ11) | (0x01 << UCSZ10);
	UBRR1H = (unsigned char) (ubrr >> 8);
	UBRR1L = (unsigned char) ubrr;
}

char UART0_Getch(void)
{
	while (!(UCSR0A & (0x01 << RXC0)));
	return UDR0;
}

char UART1_Getch(void)
{
	while (!(UCSR1A & (0x01 << RXC1)));
	return UDR1;
}

void UART0_Gets(char * str)
{
	static int i = 0;
	do
	{
		str[i] = UART0_Getch();
	}while( str[i++] != '\r' && str[i++] != '\n');
	str[i - 1] = 0;
}

void UART1_Gets(char * str)
{
	static int i = 0;
	do
	{
		str[i] = UART0_Getch ();
	} while (str[i++] != '\r' && str[i++] != '\n');
	str[i - 1] = 0;
}

void UART0_Putch(char ch)
{
	while(!(UCSR0A & (0x01 << UDRE0)));
	UDR0 = ch;
}

void UART1_Putch(char ch)
{
	while(!(UCSR1A & (0x01 << UDRE1)));
	UDR1 = ch;
}

void UART0_Puts(char * str)
{
	int i = 0;
	while(str[i] != 0)
	{
		UART0_Putch (str[i++]);
	}
}

void UART1_Puts(char * str)
{
	int i = 0;
	while(str[i] != 0)
	{
		UART1_Putch(str[i++]);
	}
}





#endif /* UART_H_ */