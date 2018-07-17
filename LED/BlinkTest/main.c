#define F_CPU 16000000L 
#include <avr/io.h>
#include <util/delay.h>

// change to use define directive on purpose
#define LED_OUTPUT (DDRB = 0XFF)
#define LED_PORT PORTB

int main(void)
{
	LED_OUTPUT; // set port as output
    /* Replace with your application code */
    while (1) 
    {
        // try to use pin in a diverse way
		LED_PORT = 1;
		_delay_ms(500);
		LED_PORT = 0;
		_delay_ms(500);
    }
	return 0;
}

