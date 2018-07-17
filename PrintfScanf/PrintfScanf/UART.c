#include <avr/io.h>
#include <stdio.h> // to use sprintf()

typedef unsigned char unit8_t;

void UART_init(unsigned int ubrr0)
{
	UBRR0H = (unsigned char) (ubrr0 >> 8);
	UBRR0L = (unsigned char) (ubrr0);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}


void Put_ch(char data)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

unsigned char Get_ch(void)
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void Put_str(const unsigned char *str)
{
	while(*str)
	{
		Put_ch(*str++);
	}
}

void Put_float_num(float f)
{
	char numString[20] = "0";
	sprintf(numString, "%f", f);
	Put_str(numString);

}

void Put_1byte_num(unit8_t n)
{
	char numString[4] = "0";
	sprintf(numString, "%d", n);
	Put_str(numString);
}