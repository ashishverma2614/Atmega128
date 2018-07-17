#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LEFT_SHIFT		0
#define RIGHT_SHIFT		1
#define CHANGE_SHIFT	2
#define STOP_SHIFT		3
#define LED_PORT		PORTB

char dir = STOP_SHIFT;
unsigned char led = 0x01;

ISR(INT2_vect)
{
	dir = RIGHT_SHIFT;
}

ISR(INT3_vect)
{
	dir = LEFT_SHIFT;
}

ISR(INT6_vect)
{
	if(dir != STOP_SHIFT){
		if(dir == LEFT_SHIFT) dir = RIGHT_SHIFT;
		else if(dir == RIGHT_SHIFT) dir = LEFT_SHIFT;
	}
}

ISR(INT7_vect)
{
	dir = STOP_SHIFT;
}

int main(void)
{
	DDRB = 0xFF;
	DDRD = 0x00;
	DDRE = 0x00;

	PORTD = 0x08;
	PORTE = 0x80;

	EICRA = 0b10100000;
	EICRB = 0b10100000;
	EIMSK = 0b11001100;

	SREG |= 0x80;

	while(1)
	{
		LED_PORT = led;

		if(dir == RIGHT_SHIFT){
			if(led == 0x80)
				led = 0x01;
			else
				led <<= 1;			
		}
		else if(dir == LEFT_SHIFT){
			if(led == 0x01)
				led = 0x80;
			else
				led >>= 1;
		}

		_delay_ms(100);
	}
}

	
