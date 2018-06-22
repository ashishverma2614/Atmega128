//TODO: switch debouncing

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"

unit8_t fndPattern[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66 ,0x6D, 0x7C, 0x07 ,0x7F, 0x6F};
unit8_t fndPosition = 0x01;
unit32_t fndNum = 0;
unit8_t cnt1; // for timer1

ISR(INT2_vect)
{
	if (fndNum == 10000)
		fndNum = 0;
	else
		fndNum++;
}
ISR(INT3_vect)
{
	if (fndNum < 0)
		fndNum = 9999;
	else
		fndNum--;
}
ISR(INT6_vect)
{
	fndNum = 0;
}
ISR(INT7_vect)
{
	fndNum = 9999;
}

ISR(TIMER1_COMPA_vect) // 1ms
{
	++cnt1;
	if (cnt1 == 5) // 5ms
	{
		fndPosition <<= 1;
		if (fndPosition == 0x10)
			fndPosition = 0x01;
		cnt1 = 0;
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
	
	// timer1 setting: CTC, 64 pre-scaler ratio
	TIMSK = (1 << OCIE1A);
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
	OCR1A = 249;
	
	// Ext. INT. 2, 3, 6, 7(falling edge detection)
	EIMSK = (1 << INT7) | (1 << INT6) | (1 << INT3) | (1 << INT2);
	EICRA = (1 << ISC31) | (1 << ISC21);
	EICRB = (1 << ISC61) | (1 << ISC71);
	
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

