// This is the revised version of "pulseOutputCTC" using CTC mode
// When using CTC(Clear Time on Compare/Match) Mode, we don't need to
// use TCNT0 = 0 to reset when TCNT matches OCR0. 
#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned int cnt = 0;
unsigned char ledState = 0; // 0 is off, 1 is on

ISR(TIMER0_COMP_vect)
{
	++cnt;
	
}

int main(void)
{
	/** port and timer interrupt setting **/
	
	DDRB = 0x10; // set OC0(PB4) as CTC output
	PORTB = 0x00; // start with all led off
	
	// pre-scale setting(1024), CTC mode(1 << WGM01), toggle(1 << COM0) output at TIMER0_COMP_vect
	TCCR0 |= ((1 << CS02) | (1 << CS01) | (1 << CS00)) 
				| (1 << COM00) | (1 << WGM01);
	
	// set compare and match standard
	OCR0 |= 255;
	
	// set compare and match interrupt mask of timer0
	TIMSK |= (1 << OCIE0); 
	
	// global interrupt setting
	SREG |= 0x80;
	while(1) 
	{
		// do nothing 
	}
	return 0;
	
}
