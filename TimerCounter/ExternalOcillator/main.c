// To verify this code, connect 32,768KHz ocillator with PB3 and PB4.
// This code is to blink a LED by 0.5sec using external ocillator.

#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

int state = 0; // led off

ISR (TIMER0_OVF_vect) // overflow timer
{
	state = !state; //toggle led state
	if (state) PORTB |= 0xFF; // led on 
	else PORTB &= ~(0xFF); // led off
}

int main(void)
{
	DDRB = 0xFF; // set port as output
	PORTB = 0x00; // initial led state: all off

	ASSR |= (1 << AS0);

	TCCR0 |= (1 << CS02) | (1 << CS01) | (1 << CS00);
	TIMSK |= (1 << TOIE0); // overflow interrupt enable
	SREG |= 0x80; // global interrupt enable

    /* Replace with your application code */
    while (1) 
    {
		// nothing to repeat, since according to overflow interrupt
		// turning on and off led occurs periodically.
    }
	return 0; // conventional expression
}

