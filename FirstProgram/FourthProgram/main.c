/*
* FourthProgram.c
*
* Created: 2018-05-30 오후 4:27:39
* Author : Woo
*/
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	/* Replace with your application code */
	char led = 0x01;
	int dir = 0; // 0 is left shift

	DDRB = 0xFF; // set PORTB

	while(1)
	{
		PORTB = led;
		if (dir == 0) // left shift
		{
			if (led == 0x80) // led 8 is on
				dir = 1; //change direction to the right
			else
				led <<= 1; // move led-on to the left until 8th is on
		}
		else // right shift
		{
			 if (led == 0x01) // led 1st is on
				dir = 0; // change direction to the left
			else
				led >>= 1;
		}
		_delay_ms(500);
	}
	
}

