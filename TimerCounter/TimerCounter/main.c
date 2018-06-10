/*	TCNTn(n=0, 2)[7:0]: stores the number of pulse(Time/Counter Register)
	TCCR0: FOC0 / WGM00 / COM01 / COM00 / WGM01 / CS02 / CS01 / CS 00
		   CS02, CS01, CS00[pre-scale] = 000[no clock, timer stop], 001[1]
		   010[8], 011[32], 100[64], 101[128], 110[256], 111[1024]
	TIMSK = OCIE2 / TOIE2 / OCIE1A / TOIE1A / OCIE1B / TOIE1 / OCIE0 / TOIE0
*/


#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned int cnt= 0; // counter set to measure time
unsigned char ledState = 0; // 0 is off, 1 is on

ISR(TIMER0_OVF_vect) // ISR of 8bit timer0
{
	if (++cnt == 32) // count 32 comes to approx 0.5s
	{
		cnt = 0; // reset timer
		ledState = !(ledState); // toggle ledState
		if (ledState == 1) PORTB = 0xFF;
		else PORTB = 0x00;
	}
}

int main(void)
{
	DDRB = 0xFF; // set portB as output
	PORTB = 0x00; // initial state: port off

	// set pre-sale as 1024(CS02, CS01, CS00: 1 1 1)
	TCCR0 |= (1 << CS02) | (1 << CS01) | (1 << CS00);

	TIMSK |= (1 << TOIE0); // allow overflow interrupt
	SREG |= 0x80; // global interrupt enable

	while(1)
	{
		// nothing to repeat
		// regardless of while loop, due to time overflow, 
		// bliking on and off led occurs periodically
	}
	return 0; // C convention
}