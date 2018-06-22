// Led connected to PB5 for PWM output blinks by 0.5 sec due to OCR1A value.
// Try to see the wave from PB5 by using oscilloscope

#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

typedef unsigned char unit8_t;

unit8_t ledState = 0; // 0 is off

ISR(TIMER1_COMPA_vect)
{
	TCNT1 = 0x00; // if it is not set as CTC, it cannot be changed to 0 automatically.
				  // try to use CTC mode instead
}


int main(void)
{
    DDRB = (1 << DDRB5); //set PB5 as output
	PORTB = 0x00; // start with led off

	OCR1A = 0x7FFF; // 0.5sec INT generate(32767)
	TCCR1B |= (1 << CS12); // pre-scale ratio 256
	//TCCR1B |= (1 << CS12) | (1 << CS10); // pre-scale ratio 1024 to make approx 2sec period
	//TCCR1B |= (1 << WGM12); // CTC Mode(Mode4)

	// toggle the output of OC1A when COMPA occurs
	TCCR1A |= (1 << COM1A0);
	TIMSK |= (1 << OCIE1A); // COMPA enable
	sei(); // global INT enable



    while (1) 
    { // nothing to repeat
    }
	return 0;
}

