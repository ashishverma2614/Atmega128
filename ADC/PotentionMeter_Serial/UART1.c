#include <avr/io.h>
#include <string.h>
#include <stdio.h>
typedef unsigned char unit8_t;

void UART1_init ()
{
	// Baud Rate set
	UBRR1H = 0x00;
	UBRR1L = 207; // at 2x mode

	UCSR1A = (1 << U2X1); // 2x mode
	UCSR1B = (1 << RXEN1) | (0x01 << TXEN1); // RX/TX enable
	UCSR1C =  0x06; // 8bit data mode, 1stop bit, no parity
}

void UART1_transmit (char data)
{
	while (!(UCSR1A & (0x01 << UDRE1))); // wait for TX enable until UDR is empty
	UDR1 = data; // writing data means sending data
}

unsigned char UART1_receive ()
{
	while(!(UCSR1A & (0x01 < RXC1))); // wait for RX enable
	return UDR1;
}

void UART1_print_string (char * str)
{
	for (int i = 0; str[i]; i++) // until encountering NULL char
		UART1_transmit (str[i]); // repeatedly transmit one char data
}

void UART1_print_1byte_number (unit8_t n)
{
	char numString[4] = "0";
	sprintf(numString, "%d", n);
	UART1_print_string (numString);
}

// need to add library to use float
void UART1_printf_float(float f)
{
	char numString[20] = "0";
	sprintf (numString, "%f", f);
	UART1_print_string (numString);
}