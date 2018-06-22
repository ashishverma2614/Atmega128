#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

typedef unsigned char unit8_t;

unit8_t state0 = 0, state1 = 0, state2 = 0; // each led state


// COMPA to make 1/4(.25) sec
ISR(TIMER1_COMPA_vect)
{
	state2 = !state2;
	if (state2) PORTB |= 0x04;
	else PORTB &= ~ 0x04; 
}

// COMPB to make 1/2(.50) sec
ISR(TIMER1_COMPB_vect)
{
	state2 = !state2; 
	if (state2) PORTB |= 0x04; 
	else PORTB &= ~0x04; 

	state1 = !state1; 
	if (state1) PORTB |= 0x02;
	else PORTB &= ~0x02;

}

//COMPC to make 3/4(.75) sec
ISR(TIMER1_COMPC_vect)
{
	state2 = !state2;
	if (state2) PORTB = 0x04; 
	else PORTB &= ~0x04;
}


// OVF of timer1 to make 1 sec
ISR(TIMER1_OVF_vect)
{
	state2 = !state2;
	if(state2) PORTB |= 0x04;
	else PORTB &= ~0x04;

	state1 = !state1;
	if (state1) PORTB |= 0x02;
	else PORTB &= ~0x02;

	state0 = !state0;
	if (state0) PORTB |= 0x01;
	else PORTB &= ~0x01;

}


int main(void)
{
	DDRB = (1 << DDRB0) | (1 << DDRB1) | (1 << DDRB2); // set port[0:2] as output
	PORTB = 0x00; // initial state(all led[0:2] off)

	// set each OCR1n value(max)
	OCR1A = 0x3FFF; // .25sec
	OCR1B = 0x7FFF; // .50sec
	OCR1C = 0xBFFF; // .75sec
	
	TCCR1B |= (1 << CS12); // pre-scale ratio 256

	// compare & match A, B, and overflow INT of timer1 enable
	TIMSK |= (1 << OCIE1A) | (1 << OCIE1B) | (1 << TOIE1);
	// compare and match C INT of timer1 enable
	ETIMSK |= (1 << OCIE1C);
	sei();
	while (1)
	{
	}
}

