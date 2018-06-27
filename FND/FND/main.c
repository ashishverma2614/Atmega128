#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	// define led pattern
	unsigned char fnd[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x67};
    int count = 0;
	DDRA = 0xFF; // set portA as output
	
    while (1) 
    {
		PORTA = ~(fnd[count]);
		count = (count + 1) % 10;
		_delay_ms(1000);
    }
	return 0;
}

