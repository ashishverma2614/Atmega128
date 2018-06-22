#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "UART_FUNC.h"

typedef unsigned char unit8_t;

char buf[100], tx_buf[100];
int rx_index; // index for RX
volatile char rx_complete = 0; // flag: 1 for complete, 0 for waiting
int tx_index;
char tx_len;

ISR(USART0_RX_vect)
{
	buf[rx_index] = UDR0;
	if (buf[rx_index++] == '\r') // if line feed come at this index
	{
		buf[rx_index - 1] = 0; // add null to the end
		rx_index = 0; // re-new index
		rx_complete = 1; // Rx complete
	}
}


ISR(USART0_UDRE_vect) // INT for RX
{
	UDR0 = tx_buf[tx_index++];

	if (tx_index == tx_len)
		UCSR0B &= ~(1 << UDRIE0); // turn-off RX INT
	
}


void UART_PutInt(char str[])
{
	tx_len = strlen(str); // calc the length of string delivered
	strcpy(tx_buf, str); // copy the string delivered to transmit buffer
	tx_index = 0; // empty buffer
	UCSR0B |= (1 << UDRIE0); // turn-on RX INT
	//UCSR0B = 0xA0;
}

int main(void)
{
	
	// set for RX reg.
	UCSR0B = 0x98;
	UCSR0C = 0x06;
	UBRR0L = 103;
	rx_index = 0;
	rx_complete = 0;
	SREG |= 0x80;
	
	while (1)
	{
		if (rx_complete == 1)
		{
		UART_PutInt(buf);
		rx_complete = 0;
		}
	}
}

