#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#define _BV(bit) (1 << (bit))
#define set_bit(bit) ( PORTB |= _BV(bit) )
#define cli_bit(bit) ( PORTB &= ~_BV(bit) )

// PORTB
#define SHIFT_CLK 0 // move(shift) data
#define LATCH_CLK 1 // output data
#define DATA		 2

typedef unsigned char unit8_t;

void Shift_Clock(void) // at rising edge, data shifts
{
	set_bit(SHIFT_CLK); // shift bit
	cli_bit(SHIFT_CLK);
}

void Latch_Clock(void) // at rising, data TX
{
	set_bit(LATCH_CLK); // release data to port by unit8
	cli_bit(LATCH_CLK);
}

void Write_Byte_Data(unit8_t data)
{
	for (unit8_t i = 0; i < 8; i++)
	{
		if(data & 0x80) // from MSB
			set_bit(DATA);
		else
			cli_bit(DATA);

		Shift_Clock();
		data <<= 1; // to LSB
	} // storing serial data completed

	Latch_Clock(); // send data in a parallel way

}
int main(void)
{
	DDRB |= _BV(SHIFT_CLK) | _BV(LATCH_CLK) | _BV(DATA);
	unit8_t index = 0; // the location of led
    while (1) 
    {
		unit8_t pattern = (0x01) << index;
		index = (index + 1) % 8; // 1, 2, 3, 4, 5, 6, 7, 8(0) -> in  order to make circular pattern
		Write_Byte_Data(pattern);
		_delay_ms(500);
    }
	return EXIT_SUCCESS;
}

