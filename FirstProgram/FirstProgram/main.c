/*
 * FirstProgram.c
 *
 * Created: 2018-05-29 오후 10:47:44
 * Author : woo03
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    /* Replace with your application code */
	
	DDRB |= 0xFF; // set PA0 as output
    while (1) 
    {
		PORTB |= 0xFF; // turn on LED connected to PA0
		_delay_ms(1000); // pause .5 sec
		PORTB &=  ~(0xFF); // turn off LED connected to PA0
		_delay_ms(1000); // pause .5 sec
    }
	
	return 0; 
}

