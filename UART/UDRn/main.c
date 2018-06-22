#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void)
{
    
    while (1) 
    {
		while ((UCSR0A & 0x80) == 0x00); // wait until RXC0 = 1
		char ch = UDR0; // RX

		if (ch >= 'a' && ch <= 'z')
			 ch = ch - 'a' + 'A'; // toggle case
		else if (ch >= 'A' && ch <= 'Z')
			ch = ch - 'A' + 'a';

		while((UCSR0A & 0x20) == 0x00); // wait until UERE0 = 1
		UDR0 = ch; // RX
    }
}

