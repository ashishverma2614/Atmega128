#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

typedef unsigned char unit8_t; // tried to use typedef for parsimony

unit8_t state = 0; // led off

ISR(TIMER1_OVF_vect) // overflow takes place about 1s
{
	state = !state; // toggle led state
	if (state) PORTB = 0x01 << 1; // led[1] on
	else PORTB = 0x00; // led off
}


int main(void)
{	
	DDRB |= (1 << DDRB1); // set PB2 as output
	PORTB = 0x00; // start with led off
	TCCR1B |= (1 << CS12); // pre-scaler ratio 256(b100)
	TIMSK |=  (1 << TOIE1); // timer1 overflow INT
	sei(); // global INT enable
	while(1){} // nothting to repeat
	return 0;
}

