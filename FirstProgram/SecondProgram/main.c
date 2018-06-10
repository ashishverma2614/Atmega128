/*
 * SecondProgram.c
 *
 * Created: 2018-05-29 오후 11:49:33
 * Author : woo03
 *
 * The structure of a program for MCU usually consists of three process:
 * (1) Preprocessing
 * (2) Initialization / constructing
 * (3) Data Processing
 */ 

// preprocessing field
#define F_CPU 16000000L // default 16Mhz operation
					   // F_CPU means the number of clock per 1 sec
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>


void UART0_init(void);
void UART0_transmit(char data);
FILE OUTPUT \
		= FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

void UART0_init(void)
{
	
	UBRR0H = 0x00; // set Baud rate as 9600
	UBRR0L = 207;
	
	UCSR0A |= _BV(U2X0); // 2times speed mode
	// asynchronous, 8bit, no-parity, 1bit stop mode
	UCSR0C |= 0x06;
	
	UCSR0B |= _BV(RXEN0); // RX/TX available
	UCSR0B |= _BV(TXEN0);
}

void UART0_transmit(char data)
{
	while(!(UCSR0A & (1 << UDRE0))); // delay for receive available
	UDR0 = data;
}

// event loop
int main(void)
{
    /* Replace with your application code */
	
	// INIT field
	UART0_init(); // initialization of UART0
	stdout = &OUTPUT;
	
	unsigned int count = 0;
	
	// data processing field
    while (1) 
    {
		// increase number from 0 by 1 during 1 sec
		printf("%d\n\r", count++);
		_delay_ms(1000);
    }
	
	return 0; // it will not be used, since MCU has no OS system.
}

