#include <avr/io.h>
#include <stdio.h>

typedef unsigned char unit8_t;

//TODO: make UART1 ver for re-use
void UART0_init(void)
{
	UBRR0H = 0x00;
	UBRR0L = 207; // 9,600(2X)bps
	
	UCSR0A |= (1 << U2X0); // 2x TRX mode
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0); // TX/RX enable
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // async mode, no-parity, 1stop-bit, 8 data bit
}
///////////////////////////////////////////////////////////////////////////////
void UART0_transmit(char data)
{
	while(!(UCSR0A & (1 << UDRE0))); // check transmit status
	UDR0 = data; // write TX data to UDR0 buf
}

unsigned char UART0_receive(void)
{
	while(!(UCSR0A & (1 << RXC0))); // check receive status
	return UDR0; // need to set a var to receive this return val
}
///////////////////////////////////////////////////////////////////////////////


// TODO: use while instead
// This function should be used in any kind of print since
// basically a serial print treats all of data as ASCII
void UART0_print_string(char str[])
{
	for (int i = 0; str[i]; i++)
		UART0_transmit(str[i]); // transmit means write char
}


void UART0_print_float(float f)
{
	char numString[20] = "0";
	sprintf(numString, "%f", f); // f will be stored in the array in a float format
	UART0_print_string(numString);

}

void UART0_print_1_byte_number(unit8_t n) // max of unsigned num = 256, need 4spaces array including null char
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
	UART0_print_string(numString);
}