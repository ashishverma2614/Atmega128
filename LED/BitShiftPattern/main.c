/*
 * BitShiftPattern.c
 *
 * Created: 2018-05-30 오후 7:49:11
 * Author : Woo
 */ 

#define F_CPU 16000000 // set CPU frequency as 16MHz
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    /* Replace with your application code */
	char current_pattern_value;
	char next_pattern_value = 0x01;

	DDRB = 0xFF; // set PortB as OUTPUT

	while (1) 
    {
		current_pattern_value = next_pattern_value;
		PORTB = current_pattern_value; // display current bit pattern

		// generate next pattern
		next_pattern_value = current_pattern_value << 1; // bit shift to left by 1 bit on
		if (next_pattern_value == 0) next_pattern_value = 0x01; // return to the first LED
		_delay_ms(500);
    }
}

