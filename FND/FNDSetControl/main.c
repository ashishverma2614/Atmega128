/* SW1: set initial value / blink the first digit(fnd3)
 * SW2: change the digits of fnd (3 -> 2 -> 1 -> 0 -> 3...)
 * SW3: count the current digit up (0 ~ 9 -> 0...)
 * SW4: complete setting and start count up (0.1s)
 */

/* Conditions
 * 1) initial value: 0
 * 2) after setting, automatic counting up.
 * 3) If SW1 pressed, possible to setting based on the current count value
 * 4) 0.5s blink
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <math.h>

/*
#define FND1 PORTC = 0x01;
#define FND2 PORTC = 0x02;
#define FND3 PORTC = 0x04;
#define FND4 PORTC = 0x08;
#define FND_PORT PORTA;
*/



int cnt;
char fndNum; // 3, 2, 1, 0
char isBlink; // 1 for blink for setup mode
char isCountUp; // 1 for count-up
char blinkState; // 0 for ON, 1 for OFF
char temp[5]; 
char fndPattern[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x67};
char timerCnt;

// SW1
ISR(INT2_vect) //set initial val of count, blink start
{
	isBlink = 1; // start to blink(set-up mode)
	fndNum = 0; // select the first fnd[0]
	isCountUp = 0; // don't count up
	
	TCCR0 = 0x01; // turn on Timer INT
	TCNT0 = 0;
	EIMSK = 0x00; // stop external INT temporarily
}

// SW2
ISR(INT3_vect) // move fndNum 0 -> 1 -> 2 -> 3 -> 0
{
	if (isBlink == 1)
	{
		fndNum++;
		if(fndNum > 3) fndNum = 0; // move to the first fnd again
	}
	TCCR0 = 0x01;
	TCNT0 = 0;
	EIMSK = 0x00;
}
// SW3
ISR(INT6_vect) // count-up number at the present fnd 0 ~ 9 -> 0
{
	if(temp[3 - fndNum] == '9') temp[3 - fndNum] = '0';
	else temp[3 - fndNum]++;
	
	 // store current fnd num to the count(Remember: values in the temp are char)
	cnt = atoi(temp); // only can nums be converted, ignoring any kind of non-numerical value

	TCCR0 = 0x01;
	TCNT0 = 0;
	EIMSK = 0x00;
}
// SW4
ISR(INT7_vect) // start to count-up(blink off), 0.1 sec delay
{
	isBlink = 0; // blink off(end-setup)
	blinkState = 0; // blink state ON for 0.5s
	isCountUp = (isCountUp + 1) % 2; // toggle count-up state
	
	TCCR0 = 0x01;
	TCNT0 = 0;
	EIMSK = 0x00;
}

ISR(TIMER0_OVF_vect) // timer for debounce
{
	++timerCnt;
	if(timerCnt == 30)
	{
		timerCnt = 0; // reset counter
		EIMSK = (1 << INT2) | (1 << INT3) | (1 << INT6) | (1 << INT7); // re-start EXT. INT.
		TCCR0 = 0; // stop timer INT
		EIFR = 0x01; // make flag 0
	}
}

void INIT(void)
{
	// port setting
	DDRA = 0xFF; // 0~9, blank
	DDRC = 0xFF; // fnd 0 ~ 3
	DDRD = 0x00; // SW 1, 2
	DDRE = 0x00; // SW 3, 4
	
	// internal pull-up setting of all SW
	PORTD = 0x0C;
	PORTE = 0xC0;
	
	// EXT. INT. setting
	EICRA = (1 << ISC21) | (1 << ISC31);
	EICRB = (1 << ISC61) | (1 << ISC71);
	EIMSK = (1 << INT2) | (1 << INT3) | (1 << INT6) | (1 << INT7);
	
	//Timer setting
	TCCR0 = 0x00; // timer stop
	TIMSK = 0x01; // OVF enable
	TCNT0 = 0; // set bottom
	sei();	
}
void displayFnd(void)
{
	sprintf(temp, "%04d\0", cnt); // write count value to the temp array, temp stores *char
	
	for (int i = 0; i <= (25 * isBlink); i++) // if blink(isBlink = 1) mode on set-up mode, it blinks by 0.5s
	{
		PORTC = 0x01; // fnd1
		if (blinkState == 1 && fndNum == 0)
			PORTA = 0xFF; // off fnd
		else
			PORTA = ~fndPattern[temp[3] - '0'];
		_delay_ms(5); // time for dynamic display
		
		PORTC = 0x02; // fnd2
		if(blinkState == 1 && fndNum == 1)
			PORTA = 0xFF;
		else
			PORTA = ~fndPattern[temp[2] - '0'];
		_delay_ms(5);
		
		PORTC = 0x04; // fnd3
		if(blinkState == 1 && fndNum == 2)
			PORTA = 0xFF;
		else
			PORTA = ~fndPattern[temp[1] - '0'];
		_delay_ms(5);
		
		PORTC = 0x08; // fnd4
		if(blinkState == 1 && fndNum == 3)
			PORTA = 0xFF;
		else
			PORTA = ~fndPattern[temp[0] - '0'];
		_delay_ms(5);
	}
}
int main(void)
{
	INIT();

    while (1) 
    {
		displayFnd();
		if(isCountUp == 1) // count-up mode
		{
			cnt++; // increase from 0 to 9999
			if(cnt > 9999) cnt = 0; // count reset
		}

		if (isBlink == 1) // if set-mode
			blinkState = !blinkState; // turn on blink state mode
    }
	return 0;
}

