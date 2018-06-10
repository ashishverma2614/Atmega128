#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned int cnt = 0;
unsigned char ledState = 0;

ISR(TIMER0_COMP_vect)
{
	++cnt;
	TCNT0 = 0x00; // empty TCNT register to restart the counter
				  // It is advised to use CTC Mode, b/c this process isn't needed.
}

int main(void)
{
	DDRB = 0x10; // PB4(OC0) is only on to see waves of compare and match counter
	PORTB = 0x00; // initial state: led off

	// set pre-scale, mormal mode(WGM01, WGM00, 0, 0)
	// pre-scale = 1024
	TCCR0 |= (1 << CS02) | (1 << CS01) | (1 << CS00);
	TCCR0 |= (1 << COM00); // toggle mode(when TCNT0 = OCR0, produce 1)
	OCR0 = 255; // set top(0xFF)

	TIMSK |= (1 << OCIE0); // compare and match interrupt enable
	SREG |= 0x80; // global interrupt enable
	
    /* Replace with your application code */

    while (1) 
    { // nothing to happen
    }
	return 0;
}

