#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

// In fact, sbi(), cbi() have already been defined in the avr/io.h
#define sbi(PORTX, BitX) PORTX |= ( 1<< BitX);
#define cbi(PORTX, BitX) PORTX &= ~( 1<< BitX);


/* Timer/Counter2 Compare Match */
#define TIMER2_COMP_vect			_VECTOR(9)
#define SIG_OUTPUT_COMPARE2		_VECTOR(9)
/* Timer/Counter2 Overflow */
#define TIMER2_OVF_vect			_VECTOR(10)
#define SIG_OVERFLOW2			_VECTOR(10)
/* Timer/Counter1 Capture Event */
#define TIMER1_CAPT_vect			_VECTOR(11)
#define SIG_INPUT_CAPTURE1		_VECTOR(11)
/* Timer/Counter1 Compare Match A */
#define TIMER1_COMPA_vect		_VECTOR(12)
#define SIG_OUTPUT_COMPARE1A		_VECTOR(12)
/* Timer/Counter Compare Match B */
#define TIMER1_COMPB_vect		_VECTOR(13)
#define SIG_OUTPUT_COMPARE1B		_VECTOR(13)
/* Timer/Counter1 Overflow */
#define TIMER1_OVF_vect			_VECTOR(14)
#define SIG_OVERFLOW1			_VECTOR(14)
/* Timer/Counter0 Compare Match */
#define TIMER0_COMP_vect			_VECTOR(15)
#define SIG_OUTPUT_COMPARE0		_VECTOR(15)
/* Timer/Counter0 Overflow */
#define TIMER0_OVF_vect			_VECTOR(16)
#define SIG_OVERFLOW0			_VECTOR(16)

typedef unsigned char unit8_t;
//unit8_t tick0_OvF = 0;

ISR(TIMER0_OVF_vect)
{
	volatile static unsigned int tick0_OVF; // better way than using global var
	tick0_OVF++; // max = 125, total 126cnt
	
	// how to make 1sec by using 8bit OVF T/C
	if (tick0_OVF == 125) // 8ms * 125 = 1sec(1000ms)
	{
		switch (PINB) // read the value of PINB
		{
			case 0x00: // PINB = 0x00, if PINB is off then turn on
				PORTB = 0xFF;
				break;
			case 0xFF: // PINB = 0xFF, if PINB is on then turn off
				PORTB = 0x00;
				break;
		}
		tick0_OVF = 0; // reset counter
	}
	TCNT0 = 131; // enforce to reset TCNT0, which means to adjust the value of start 
				// 255 - 131  + 1 = 125
}

void INIT(void);

int main(void)
{
	INIT();
	while (1) {	} 
	return 0;
}

void INIT(void)
{
	/* Port Setting */
	cbi(SREG, 7); // same as cli(), all INT off
	PORTB = 0xFF; // set PB as output
	DDRB = 0xFF; // turn on all LED at first

	/* Timer Setting  */
	TCCR0 = (1 << CS02) | (1 << CS01) | (1 << CS00); // set Timer Counter Control Reg., pre-scale: 1024(approx. 64us)
	//TCCR0 = 0x07;
	TCNT0 = 131; // set start value, 1 / clkt0 = 64us, 64us X 125 = 8ms
	TIMSK = (1 << TOIE0); // set Timer0 Overflow Interrupt Enable
	sbi(SREG, 7); // global INT enable
}

