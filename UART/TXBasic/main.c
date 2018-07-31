#define F_CPU 16000000UL
#define UBRR_9600_1X 103
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include "UART_FUNC.h"

typedef unsigned char unit8_t;

char buf[100], tx_buf[100];
int rx_index;
volatile char rx_complete = 0; // 1 for complete
int tx_index;
char tx_len;


FILE OUTPUT = FDEV_SETUP_STREAM(UART_Putch, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART_Getch, _FDEV_SETUP_READ);

ISR(USART0_RX_vect) // isr for RX
{
	buf[rx_index] = UDR0;
	if (buf[rx_index++] == '\r') // if line feed come at this index
	{
		buf[rx_index - 1] = 0; // add null to the end
		rx_index = 0; // re-new index
		rx_complete = 1; // flag for RX completion
	}
}


ISR(USART0_UDRE_vect) // if UDRE becomes empty, ISR(USART0_RX_vect) jumps here
{
	UDR0 = tx_buf[tx_index++]; // write the contents in the tx_buf[] to the UDR0(TX)

	if (tx_index == tx_len) // if the contents in the tx_buf becomes completely empty
		UCSR0B &= ~(1 << UDRIE0); // end of TX
}

void UART_PutInt(char str[])
{
	tx_len = strlen(str); // get the length of string delivered
	strcpy(tx_buf, str); // copy the string to tx_buf in order to transmit(TX occurs)
	tx_index = 0; // empty buffer
	UCSR0B |= (1 << UDRIE0); // it triggers the interrupt that makes UDR0 set(1), meaning UDR is empty
	// start of TX
	//UCSR0B = 0xA0;
}

void UART_init(unsigned int baud)
{
	// set for RX reg.
	UCSR0B = 0x98; // 0b1001_1000(1 << RXCIE + 1 << RXEN + 1 << TXEN)
	UCSR0C = 0x06; //0b0000_0110(1 << UCSZ01 + 1 << UCSZ00): 8bit mode
	//UBRR0H = 0;
	UBRR0L = 103; // baud rate
	rx_index = 0; // no contents in the buf
	rx_complete = 0; // no rx
	SREG |= 0x80;
}

int main(void)
{
	stdout = &OUTPUT;
	stdin = &INPUT;
	UART_init (UBRR_9600_1X);
	while (1)
	{
		if (rx_complete == 1) // only if RX takes place
		{
		UART_PutInt(buf); // confirm the contents received from RX
		rx_complete = 0;
		}
	}
}

