#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

#define SEGMENT_DELAY 5

typedef unsigned char unit8_t;
unit8_t numbers[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x67 };
	
void display_digit(int position, int number)
{
	// position: 0 ~ 3
	// number: 0 ~ 9
	PORTE |= 0x0F;
	PORTE &= ~(1 << (position - 1));
	PORTF = numbers[number];
}


int main(void)
{
    DDRE = 0x0F;
	DDRF = 0xFF;
    while (1) 
    {
		display_digit(1, 1);
		_delay_ms(SEGMENT_DELAY);
		display_digit(2, 2);
		_delay_ms(SEGMENT_DELAY);
		display_digit(3, 3);
		_delay_ms(SEGMENT_DELAY);
		display_digit(4, 4);
		_delay_ms(SEGMENT_DELAY);
    }
}

