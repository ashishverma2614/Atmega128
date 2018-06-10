/*
* InterruptBasic.c
*
* Created: 2018-05-30 오전 10:30:44
* Author : Woo
*/

#include <avr/io.h>
#include <avr/interrupt.h>


ISR(INT6 _vect)
{
	//PORTE &= 0xFE;
	PORTE &= ~(0x01);
}

ISR(INT7 _vect)
{
	PORTE |= 0x01;
}

int main(void)
{
	/* Replace with your application code */
	
		DDRE = 0x3F;
		EICRB = 0x8A;
		EIMSK = 0xC0;
		SREG |= 0x80; // global interrupt enable


	while (1)
	{
	
	}
}

