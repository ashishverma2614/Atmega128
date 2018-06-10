/*
 * SixthProgram.c
 *
 * Created: 2018-05-30 오후 5:18:28
 * Author : Woo
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    /* Replace with your application code */

	char led = 0x01; // INIT of led(first on)
	char swState = 0;
	
	DDRB = 0xFF; // led, output
	DDRD = 0x00; //SW, input
    while (1) 
    {
		PORTB = led; // set led to PORTB
		if ((PIND & 0x01) != 0 && swState == 0)
		{
			swState = 1; // change state of led
			if (led == 0x08) // if 8th led is on
				led = 0x01;
			else
				led <<= 1;

		}
		if ((PIND & 0x01) == 0 && swState == 1)
			swState = 0;

    }
}

