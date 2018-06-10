#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#define setbit(x, y) ((x) |= (0x01 << (y)))
#define resetbit(x, y) ((x) &= ~(0x01 << (y)))


int main(void)
{
    /* Replace with your application code */
	DDRB = 0xFF; // set portB as OUTPUT
    while (1) 
    {
	setbit(PORTB, 0); // set 1 to bit0 of PORTB
	_delay_ms(1000);
	resetbit(PORTB, 0); // set 0 to bit0 of PORTB
		_delay_ms(1000);
    }
}

