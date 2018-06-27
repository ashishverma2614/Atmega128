/*
*/
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define UBRR 103 // 9600bps at 1x mode

void USART0_INIT(unsigned int ubrr0) // 8bit, no-parity, 1 stop-bit, TX-enable
{
	UBRR0H = (unsigned char)(ubrr0 >> 8); // truncate upper 8 bits
	UBRR0L = (unsigned char)(ubrr0); // truncate lower 8 bits
	UCSR0B = (1 << TXEN0); // TX0 enable
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8bit unit transmit
}

// This function will be summoned to TX0_STR to send accumulated character string 
void TX0_ch (unsigned char chData)
{
	while(!((UCSR0A & (1 << UDRE0)))); // check the transmit enable status
	UDR0 = chData; // write chData, which means to transmit data
}

// Basically, string is the succession(sum) of character, so that
// it is advised to re-use TX0_ch to send sum of character
void TX0_STR(const unsigned char *str)
{
	while(*str) // until *str becomes 0
	{
		TX0_ch(*str++); // send character without stopping
	}
}

int main(void)
{
	const unsigned char str1[] = "Hello World\r\n"; // to circumvent data corruption, append "const"
	USART0_INIT(UBRR); // 9600 bps
    while (1) 
    {
		TX0_STR(str1);
		_delay_ms(1000);
    }
}

