/* This is the revised version of "OutputCompareInterrupt"
*/
#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned int cnt = 0; // set cnt as volatile b/c this value can be changed only in ISR
unsigned char ledState = 0;

ISR(TIMER0_COMP_vect)
{
	++cnt;
	TCNT0 = 0; // forcefully empty the number of timer interrupt
			   // since OCR0 is not 0
}

int main(void)
{
	DDRB = 0xFF; // set portB as output
	PORTB &= ~(0xFF); // initial state: all led off

	// set pre-scale of timer0 to use compare and match interrupt
	TCCR0 |= (1 << CS02)  | (1 << CS01) | (1 << CS00);
	
	// set bottom matching line
	OCR0 = 128;

	// compare and match interrupt enable
	TIMSK |= (1 << OCIE0); 

	// global interrupt enable
	SREG |= 0x80;

	while(1)
	{
		if(cnt == 64)
		{
			cnt = 0;
			ledState = !ledState;
			if (ledState == 1) PORTB |= 0xFF;
			else PORTB &= ~0xFF;
		}
	}
	return 0;
}