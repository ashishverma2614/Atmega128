#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char state = 0; // led off

ISR(TIMER1_OVF_vect) // overflow takes place about 1s
{
	state = !state; // toggle led state
	if (state) PORTB = 1; // led on
	else PORTB = 0; // led off
}


int main(void)
{	
	DDRB |= (1 << DDRB0); // set PB0 as output
	PORTB = 0x00; // start with led off
	TCCR1B |= (1 << CS12); // pre-scaler ratio 256
	TIMSK |=  (1 << TOIE1); // timer1 overflow INT
	sei(); // global INT enable
	
    /* Replace with your application code */
    while (1) 
    { // nothing to repeat
    }
}

