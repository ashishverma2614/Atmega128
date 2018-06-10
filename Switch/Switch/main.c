/*
 * Switch.c
 *
 * Created: 2018-05-31 AM 12:26:34
 * Author : woo03
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    /* Replace with your application code */
	// setting for LED: set PB0~PB2 as OUTPUT and the rest as default(0000_0111)
	DDRB |= 0x07;
	
	// setting for switch: set PF0~PF2 as INPUT  and the rest as default(1111_1000)
	DDRF &= ~(0x07); // 0000_0000
	
	// set PF0 as internal pull-up resist
	// To make it, output 1 from PF0
	PORTF |= 0x01;
	
    while (1) 
    {
		// display result to LED
		PORTB = (PORTB & 0xF8) + (PINF & 0x07);
    }
	return 0;
}

