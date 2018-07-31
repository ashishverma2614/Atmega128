#define F_CPU
#define UBRR_9600_1X 103

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "UART_FUNC.h"

char buf[100]; // store the unit8_t data in UDR0
char index;
volatile char rx_complete = 0; // 1 for complete

FILE OUTPUT = FDEV_SETUP_STREAM(UART_Putch, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART_Getch, _FDEV_SETUP_READ);

ISR(USART0_RX_vect) // RX means to write UDRn to buf
{
	buf[index] = UDR0;
	if (buf[index++] == '\r') // if line feed come at this index
	{
		buf[index - 1] = 0; // add null to the end
		index = 0; // re-new index
		rx_complete = 1; // Rx complete
	}
}


void UART_init(unsigned int baud)
{
	// set for RX reg.
	UCSR0B = 0x98; // 0b1001_1000(1 << RXCIE + 1 << RXEN + 1 << TXEN)
	UCSR0C = 0x06; //0b0000_0110(1 << UCSZ01 + 1 << UCSZ00): 8bit mode
	//UBRR0H = 0;
	UBRR0L = 103; // baud rate
	//rx_index = 0; // no contents in the buf
	rx_complete = 0; // no rx
	SREG |= 0x80;
}

int main(void)
{
	stdout = &OUTPUT;
	stdin = &INPUT;
	DDRB = 0xFF;

	// set for RX reg.
	UART_init(UBRR_9600_1X);
	/*
    UCSR0B = 0x98;
	UCSR0C = 0x06;
	UBRR0L = 103;
	index = 0;
	rx_complete = 0;
	SREG |= 0x80;
	*/
    while (1) 
    {
		if(rx_complete == 1)
		{
			UART_Puts(buf); // echo the buf read from UDR to confirm
			rx_complete = 0; // wait RX again
		}
		
    }
	return 0;
}

