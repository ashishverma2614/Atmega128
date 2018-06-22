#define F_CPU
#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART_FUNC.h"

char buf[100];
char index;
volatile char rx_complete = 0; // flag: 1 for complete, 0 for waiting
/*
ISR(USART0_RX_vect)
{
	PORTB = UDR0;
}*/

ISR(USART0_RX_vect)
{
	buf[index] = UDR0;
	if (buf[index++] == '\r') // if line feed come at this index
	{
		buf[index - 1] = 0; // add null to the end
		index = 0; // re-new index
		rx_complete = 1; // Rx complete
	}
}



int main(void)
{
	DDRB = 0xFF;

	// set for RX reg.
    UCSR0B = 0x98;
	UCSR0C = 0x06;
	UBRR0L = 103;
	index = 0;
	rx_complete = 0;
	SREG |= 0x80;

    while (1) 
    {
		if(rx_complete == 1)
		{
			UART_Puts(buf);
			rx_complete = 0; // wait RX again
		}
		
    }
	return 0;
}

