#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/wdt.h> // Watch Dog Timer

#define high(var, mask) ((var) |= (unit8_t)(1 << mask))
#define low(var, mask) ((var) &= (unit8_t) ~(1 << mask))

typedef unsigned char unit8_t;
typedef int unit16_t;

void delay(unit16_t i)
{
	unit16_t j, k;
	for (j = 0; j < i; j++)
	{
		for (k = 0; k < 300; k++)
		wdt_reset();
	}
}

void usart0_init(unit16_t baud) // 2x mode, 8bit, 1 stop bit, no-parity
{
	UBRR0H = (unit8_t)(baud << 8);
	UBRR0L = (unit8_t)(baud);
	UCSR0B = (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	//UCSR0A = (1 << U2X0); // 2x mode set
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8bit 

}

void usart0_tx_char(char ch)
{
	while (!(UCSR0A & ( 1 << UDRE0 ))); // wait for empty UDR
	UDR0 = ch;
}

unit8_t usart0_rx_char()
{
	while (!(UCSR0A & (1 << RXC0))) wdt_reset();
	return UDR0;
}

int main(void)
{
	unit16_t i = 0;
	usart0_init (103); // 9600 at 16000000Hz, 1x mode
    
    while (1) 
    {
		i = usart0_rx_char();
		usart0_tx_char(i); // echo back
    }
}

