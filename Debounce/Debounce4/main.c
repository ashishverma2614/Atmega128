#define F_CPU
#include <avr/io.h>

unsigned char led = 0x01;

ISR(INT0_vect)
{
	if (led == 0x80)
		led = 0x01;
	else
		led <<= 1;
		EIMSK = 0;
		TCCR0
}

int main(void)
{
	DDRB = 0xFF;
	EICRA = 0x02; // falling edge
	EIMSK = 0x01;
	TCCR0 = 0x00; // timer off, as soon as pre-scaler is set, timer starts
	TCNT0 = 0;
	TIMSK = 0x01;
	SREG = 0x80;
    /* Replace with your application code */
    while (1) 
    {
    }
}

