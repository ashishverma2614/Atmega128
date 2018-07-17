#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define UBRR_9600_1X 103 // 9600bps at 1x mode of 16000000Hz, unsigned int type
#define UBRR_9600_2x 207 // 9600bps at 2x mode of 16000000Hz

void USART0_INIT(unsigned int ubrr0) // 8bit, no-parity, 1 stop-bit, TX-enable
{
	UBRR0H = (unsigned char)(ubrr0 >> 8); // truncate upper 8 bits
	UBRR0L = (unsigned char)(ubrr0); // truncate lower 8 bits
	UCSR0B = (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}


void TX0_ch (unsigned char chData)
{
	while(!((UCSR0A & (1 << UDRE0)))); // check the transmit enable status
	UDR0 = chData; // write chData, which means to transmit data
}

void TX0_STR(const unsigned char *str)
{
	while(*str) // until *str becomes 0
	{
		TX0_ch(*str++); // send character until encountering NULL char
	}
}

int main(void)
{
	const unsigned char* str1 = "Bonjour le monde\r\n";
	unsigned char str2[] = "Hello World\r\n";
	USART0_INIT(UBRR_9600_1X); // 9600 bps
    while (1) 
    {
		TX0_STR(str1);
		_delay_ms(500);
		//TX0_ch('\n');
		TX0_STR(str2);
		_delay_ms(500);
    }
}

