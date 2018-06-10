/*
 * LED.c
 *
 * Created: 2018-05-30 PM 7:39:51
 * Author : Woo
 */ 
#define F_CPU 16000000L // set CPU operation frequency
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	DDRA = 0xFF; // set Port B as OUTPUT, which will use PORTB
		
    /* Replace with your application code */
    while (1) 
    {
		PORTA = 0x00; // turn off all LED
		_delay_ms(50); // pause .5 sec
		PORTA = 0xFF; // turn on all LED
		_delay_ms(50);
    }
}

