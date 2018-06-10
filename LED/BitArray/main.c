/*
 * BitArray.c
 *
 * Created: 2018-05-30 오후 8:16:20
 * Author : Woo
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    /* Replace with your application code */

	// This array contains irregular bit pattern
	char ledPattern[] = {
		0xA0, 0x50, 0x28, 0x14, 0x0A,
		0x05, 0x82, 0x41
	};

	int index = 0; // pattern to display this time
	DDRB = 0xFF; // set port B as output

    while (1) 
    {
		PORTB = ledPattern[index];
		index = (index + 1) % 8; // to make next pattern index as well as
								 // prevent index overflow by using modulus operation
		_delay_ms(2000);
    }
}

