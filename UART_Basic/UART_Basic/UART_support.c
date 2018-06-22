#include <avr/io.h>

// initialization of UART communication
void UART0_init(void)
{
	UBRR0H = 0x00; // set Baud rate 9,600
	UBRR0L = 207;
	
	UCSR0A |= (1 << U2X0); // 2X mode
	
	//async, 8bit data, no parity, 1bit stop mode
	UCSR0C |= 0x06;
	
	UCSR0B |= (1 << RXEN0); // RX enable
	UCSR0B |= (1 << RXEN0); // TX enable
}

void UART0_Transmit(char data)
{
	while(!(UCSR0A & (1 << UDRE0))); // wait until it is possible for RX
	UDR0 = data;
}