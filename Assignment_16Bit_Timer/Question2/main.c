#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"

#define MAX_COUNT 10000

unit8_t fndPattern[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66 ,0x6D, 0x7C, 0x07 ,0x7F, 0x6F};
unit8_t fndPosition = 0x01;
unit32_t fndNum;
unit8_t cnt0; // for timer0
unit8_t cnt2; // for timer2

ISR(TIMER0_COMP_vect) // for dynamic operation(1ms)
{
	++cnt0;
	if (cnt0 == 5) // 5ms
	{
		fndPosition <<= 1;
		if (fndPosition == 0x10)
			fndPosition = 0x01;
		cnt0 = 0;
	}
}

ISR(TIMER2_COMP_vect) // for count-up(1ms)
{
	++cnt2;
	if (cnt2 == 100) // 100ms
	{
		fndNum++;
		if (fndNum == MAX_COUNT)
			fndNum = 0;
		cnt2 = 0;
	}
}

void main(void)
{
	INIT();
	while (1)
	{
		PORTC = fndPosition;
		displayFND(fndPosition);
	}
}

/* Function Implementations */

void INIT(void)
{
	// FND port
	DDRA = 0xFF; // a~g, dot
	DDRC = 0xFF; // fnd[0:3]
	
	// timer0, 2. CTC(Mode2), pre-scaler ratio 64, 1ms
	TIMSK |= (1 << OCIE0) | (1 << OCIE2);
	TCCR0 |= (1 << WGM01) | (1 << CS02);
	TCCR2 |= (1 << WGM21) | (1 << CS22);
	OCR0 = 249;
	OCR2 = 249;
	
	sei();
}
void displayFND(unit8_t pos)
{
	switch(pos)
	{
		case 1: // FND 0
			PORTA = ~fndPattern[fndNum % 10];
			break;
		case 2: // FND 1
			PORTA = ~fndPattern[(fndNum % 100) / 10];
			break;
		case 4: // FND 2
			PORTA = ~fndPattern[(fndNum % 1000) / 100];
			break;
		case 8: // FND 3
			PORTA = ~fndPattern[(fndNum / 1000)];
			break;
	}
}

