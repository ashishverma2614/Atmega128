#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "define.h"

// TODO: try to reduce the number of var as much as possible
unit8_t fndPattern[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66 ,0x6D, 0x7C, 0x07 ,0x7F, 0x6F};
unit8_t digit[4] = {0, 0, 0, 0}; // to store each digit from set-up
unit8_t fndPosition = 0x01; // for dynamic operation
unit8_t isBlink; // flag for blinking(1 is blinking, 0 is count-up)
unit8_t nFnd; // index of digit[], 0 ~ 4
unit16_t cnt0, cnt1, cnt3; // to calc each timer
unit32_t fndNum = 0; // 0 ~ 10000 to calc each fnd digit

// start to blink
ISR(INT2_vect)
{
	isBlink = 1; // start blink
	nFnd = 0; // start from first fnd
	
	//start blink -> turn on blink timer
	TIMSK |= (1 << OCIE0);
	// turn off count-up timer until INT7 triggers
	ETIMSK |= 0x00;
}

// move fnd
ISR(INT3_vect)
{
	nFnd++; // move fnd position 0 -> 1 -> 2 -> 3
	if (nFnd == 4)
		nFnd = 0;	// 3 -> 0
}

// count up each digit of fnd
ISR(INT6_vect)
{
	digit[nFnd]++; // increment current fnd num
	if (digit[nFnd] == 10)
		digit[nFnd] = 0; // 9 -> 0
}

// start count-up and stop blinking
ISR(INT7_vect)
{
	isBlink = 0; // stop blink
	// stop blink -> turn off blink timer
	TIMSK |= 0x00;
	// start(turn-on) count-up timer
	ETIMSK |= (1 << OCIE1A);
	
		
}

// timer for count-up
ISR(TIMER0_COMP_vect) // 1ms
{
	cnt0++;
	if(cnt0 == 5) // 5ms for dynamic display
	{
		fndPosition <<= 1;
		if (fndPosition == 0x10)
			fndPosition = 0x01;
		cnt0 = 0;
		PORTC = fndPosition;
	}
}

// timer for blink
ISR(TIMER1_COMPB_vect) // 1ms
{
	cnt2++;
	if(isBlink == 1)
	{
		cnt2++;
		if (cnt2 == 500) // .5sec for blinking
		{
			
		}
		if (cnt2 == 1000) // 1 sec
			cnt2 = 0;
	}
}


int main(void)
{
    
    while (1) 
    {
    }
}

