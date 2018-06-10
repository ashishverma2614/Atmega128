/*
 * BitShiftPattern2.c
 *
 * Created: 2018-05-30 오후 8:09:15
 * Author : Woo
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	char ledPattern = 0x01;
	DDRB = 0xFF; // set this port as output

    /* Replace with your application code */
    while (1) 
    {
		PORTB = ledPattern; // display current pattern

		// generate next pattern
		ledPattern <<= 1; // shift to the left led
		if (ledPattern == 0x00) // 0x00 means led pattern goes over 0x80(ater 0x80m it overflows)
			ledPattern = 0x01;
		_delay_ms(500);
    }
	return 0;
}

