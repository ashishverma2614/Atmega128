#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned int cnt = 0;
unsigned char ledState = 0;

ISR(TIMER0_COMP_vect)
{
	if(++cnt == 64)
	{
		TCNT0 = 0x00; // reset TCNT to forcefully empty it as soon as cnt reaches 128
		cnt = 0;
		ledState = !(ledState); // toggle ledState
		if (ledState == 1) PORTB |= 0xFF;
		else PORTB &= ~(0xFF); // same as PORTB |= 0x00;
	}
}

int main(void)
{
	/* PORT	and Interrupt setting */
	DDRB = 0xFF; // set portB as output
	PORTB = 0x00; // initial state: all led off

	TCCR0 |= (1 << CS02) | (1 << CS01) | (1 << CS00); // pre-scale 1024

	OCR0 = 128; // set comparison value as 128, which is half of 256

	TIMSK |= (1 << OCIE0); // allow compare & match interrupt

	SREG |= 0x80;

	while(1)
	{
		// nothing to repeat
		// due to time and match counter, turning on and off led occurs automatically
	}
	return 0;

}