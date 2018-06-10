/*
 * FifthProgram.c
 *
 * Created: 2018-05-30 오후 4:46:32
 * Author : Woo
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	char led[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	int i;
	DDRB = 0xFF; // set portB as output
    /* Replace with your application code */
    while (1) 
    {
		for (i = 0; i < 8; i++)
		{
		PORTB = led[i];
		_delay_ms(500);	
		}
		for (i = 7; i >= 0; i--)
		{
		PORTB = led[i];
		_delay_ms(500);
		}
    }
}

