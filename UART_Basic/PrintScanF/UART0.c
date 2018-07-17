#include <avr/io.h>
#include <stdio.h> // to use sprintf()

typedef unsigned char unit8_t;

void UART0_INIT(unsigned int ubrr0)
{
	UBRR0H = (unsigned char) (ubrr0 >> 8);
	UBRR0L = (unsigned char) (ubrr0);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}


void TX0_ch(char data)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

unsigned char RX0_ch(void)
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void TX0_str(const unsigned char *str)
{
	while(*str)
	{
		TX0_ch(*str++);
	}
}

void TX0_float(float f)
{
	char numString[20] = "0";
	sprintf(numString, "%f", f);
	TX0_str(numString);

}

void TX0_1byte_num(unit8_t n)
{
	char numString[4] = "0";
	/*
	int index = 0;
	int i;
	if (n > 0)
	{
		for (i = 0; n != 0; i++)
		{
			numString[i] = n % 10 + '0';
			n = n / 10;
		}
		numString[i] = '\0';
		index = i - 1; // exclude null char
	}
	for (i = index; i >= 0; i--) // print value in the array in an reverse order
	{
		UART0_transmit(numString[i]);
	}*/
	sprintf(numString, "%d", n);
	TX0_str(numString);
}