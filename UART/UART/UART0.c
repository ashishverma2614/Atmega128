#include <avr/io.h>
#include <stdio.h>

typedef unsigned char unit8_t;

void UART0_init(unsigned int ubrr0)
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

unit8_t RX0_ch(void)
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void TX0_str(unit8_t * str)
{
	while(*str)
	{
		TX0_ch(*str++);
	}
}

void RX0_str(unit8_t * str)
{
	int i = 0;
	do
	{
	 str[i] = RX0_ch();
	 TX0_ch (str[i]); // echo to confirm successful RX
	} while (str[i++] != '\r' && str[i++] != '\n');
	TX0_str ("\r\n"); // move to the end of string character
	str[i - 1] = 0; // append null character
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
	sprintf(numString, "%d", n);
	TX0_str(numString);
}

unit8_t RX0_isGetch(void) // func to check whether to receive char or not
{
	if ((UCSR0A & (1 << RXC0))) return 1;
	else return 0;
}
