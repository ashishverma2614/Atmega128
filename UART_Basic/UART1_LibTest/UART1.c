#include <avr/io.h>

typedef unsigned char unit8_t;
void UART1_init(void)
{
	UBRR1H = 0x00;
	UBRR1L = 207; // 9,600(2X)bps
	
	UCSR1A |= (1 << U2X1); // 2x TRX mode
	UCSR1B |= (1 << TXEN1) | (1 << RXEN1); // TX/RX enable
	UCSR1C |= 0x06; // async mode, no-parity, 1stop-bit, 8 data bit
}

void UART1_transmit(char data)
{
	while(!(UCSR1A & (1 << UDRE1))); // check transmit status
	UDR1 = data; // TX data
}

unsigned char UART1_receive(void)
{
	while(!(UCSR1A & (1 << RXC1))); // check receive status
	return UDR1;
}

void UART1_print_string(char str[])
{
	for (int i = 0; str[i]; i++)
		UART1_transmit(str[i]);
}

void UART1_print_1_byte_number(unit8_t n)
{
	char numString[4] = "0";
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
		UART1_transmit(numString[i]);
	}
}