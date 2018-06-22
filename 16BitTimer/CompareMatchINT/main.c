#define F_CPU 16000000L

#include <avr/io.h>
#include <avr/interrupt.h>

typedef unsigned char unit8_t;

unit8_t ledState= 0; // 0 is off

ISR(TIMER1_COMPA_vect) // to use compare & match A of timer1
{
	ledState = !ledState; // toggle the ledState
	if (ledState) PORTB = 0x01 << 4; // led on
	else PORTB = 0x00;

	TCNT1 = 0; // clear TCNT1, reset the number of timer1 INT
}


int main(void)
{
    DDRB |= (1 << DDRB4); // set PB4 as output
	PORTB = 0x00;

	OCR1A = 0x7FFF; // set the criteria of compare and match, 32767
	TCCR1B |= (1 << CS12); // set pre-scale ratio as 256
	TIMSK |= (1 << OCIE1A); // compare and matchA enable
	sei(); // global INT enable
	while(1);
}

