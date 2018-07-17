/* This is the example to transmit(write) one character in the UDR0 by 1 sec.
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define UBRR0 103 // Baud: 9600 at 1x mode 

void USART0_INIT(unsigned int ubrr0) // 8bit, no-parity, 1 stop-bit, TX-enable
{
	UBRR0H = (unsigned char)(ubrr0 >> 8); // truncate upper 8bits
	UBRR0L = (unsigned char)(ubrr0); // truncate lower 8 bits
	UCSR0B = (1 << TXEN0); // TX0 enable
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8bit unit transmit
}

void TX0_ch (unsigned char chData)
{
	while(!((UCSR0A & (1 << UDRE0)))); // check the transmit enable status
	UDR0 = chData; // write chData, which means to transmit data
}

int main(void)
{
    USART0_INIT(UBRR0); // 9600 bps
    while (1) 
    {
		TX0_ch('A'); // send ch by 1sec
		_delay_ms(250);
    }
}

