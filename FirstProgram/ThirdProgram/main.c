/*
 * ThirdProgram.c
 *
 * Created: 2018-05-30 오후 4:06:43
 * Author : Woo
 */ 

 #define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    /* Replace with your application code */

	char led = 0x80; // init LED
	DDRB = 0xFF; // set as output
    while (1) 
    {
		PORTB = led;
		if (led == 0x01) // if 8th led is on
			led = 0x80;
		else
			led >>= 1; // shift led from 0 to 8
		_delay_ms(500);
    }
}

