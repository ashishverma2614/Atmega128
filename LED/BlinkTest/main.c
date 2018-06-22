/*
 * BlinkTest.c
 *
 * Created: 2018-06-07 오후 7:52:07
 * Author : Woo
 */ 

#define F_CPU 16000000L 
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	DDRB = 0xff;
    /* Replace with your application code */
    while (1) 
    {
		PORTB = 0xFF;
		_delay_ms(500);
		PORTB = 0x00;
		_delay_ms(500);
    }
	return 0;
}

