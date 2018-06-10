/* This is another solution of BitShiftPattern2.
 * To address this problem in a new perspective, I used the circular shift way
 * which moves each LSB to MSB and concatenate the two.
 * CircularShift.c
 *
 * Created: 2018-05-30 오후 8:59:26
 * Author : William Woo
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>


char circular_shift_right(char pattern);

int main(void)
{
    /* Replace with your application code */
	char pattern = 0xA0;
	DDRB = 0xFF; // set port B as output
    while (1) 
    {
		PORTB = pattern; // display current pattern

		// generate next pattern
		pattern = circular_shift_right(pattern);
		_delay_ms(1000);
    }
	return 0; // it it useless at MCU since this unit doesn't have OS system
}

/*
char circular_shift_right(char pattern)
{
	char LSB = pattern & 0x01; // LSB of each pattern
	char MSB = LSB << 7; // move LSB to the location of MSB

	char new_pattern = pattern >> 1; // pattern shift to right by 1bit

	new_pattern |= MSB; // new pattern = previous pattern + MSB

	return new_pattern;

}*/

// this digested function is same as the above
char circular_shift_right(char pattern)
{
	char new_pattern = (pattern >> 1) | (pattern << 7);
	
	return new_pattern;
}

