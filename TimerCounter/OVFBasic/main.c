#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

// In fact, sbi(), cbi() have already been defined in the avr/io.h
#define sbi(PORTX, BitX) PORTX |= ( 1<< BitX);
#define cbi(PORTX, BitX) PORTX &= ~( 1<< BitX);

typedef unsigned char unit8_t;
unit8_t count = 0;

ISR(TIMER0_OVF_vect)
{
	count++; // max = 125, total 126cnt
	
	// how to make 1sec by using 8bit OVF T/C
	if (count == 125) // 8ms * 125 = 1sec(1000ms)
	{
		switch (PINB)
		{
			case 0x00: // PINB = 0x00, all off
				PORTB = 0xFF;
				break;
			case 0xFF: // PINB = 0xFF, all on
				PORTB = 0x00;
				break;
		}
		count = 0; // reset counter
	}
	TCNT0 = 131; // enforce to reset TCNT0, which means to adjust the value of start 
				// 255 - 131  + 1 = 125
}

void INIT(void);

int main(void)
{
	INIT();
	while (1)
	{ /* nothing to repeat */ } return 0;
}

void INIT(void)
{
	/* Port Setting */
	cbi(SREG, 7); // same as cli(), all INT disabled
	PORTB = 0xFF; // set PB as output
	DDRB = 0xFF; // turn on all LED at first

	/* Timer Setting  */
	TCCR0 = (1 << CS02) | (1 << CS01) | (1 << CS00); // set Timer Counter Control Reg., pre-scale: 1024(approx. 64us)
	//TCCR0 = 0x07;
	TCNT0 = 131; // set start value, 1 / clkt0 = 64us, 64us X 125 = 8ms
	TIMSK = (1 << TOIE0); // set Timer0 Overflow Interrupt Enable
	sbi(SREG, 7); // global INT enable
}

