#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define UBRR_9600_1X 103 
#define UBRR_9600_2x 207

void UART0_INIT(unsigned int ubrr0)
{
	UBRR0H = (unsigned char) (ubrr0 >> 8);
	UBRR0L = (unsigned char) (ubrr0);
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void TX0_ch(unsigned char data) // TX one char
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void TX0_str(const unsigned char *str)
{
	while(*str) // until *str becomes 0
	{
		TX0_ch(*str++); // send character until encountering NULL char
	}
}

unsigned char RX0_ch(void) // RX for one char
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

int main(void)
{
	UART0_INIT(UBRR_9600_1X);
	while(1)
	{
		TX0_ch(RX0_ch()); // echo back
	}
	return 0;
}