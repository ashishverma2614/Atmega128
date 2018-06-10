// LEDs[1:8] are connected to portB
// SW1 is connected to PD2, which uses pull-up resist(Source)
// , so HIGH value is 0;
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
    /* Replace with your application code */
	unsigned led = 0x01; // INIT LED
	unsigned state = 0; // state of SW, 0 is off(false)
	DDRD = 0x00; // set portD as input(external pull-up resist)
	DDRB = 0xFF; // set portB as output 

    while (1) 
    {
	PORTB = led; // write led

	// de-bounce chattering by detecting LED state
	if ((PIND & 0x04) == 0x00 && state == 0) // 0x04 is position of SW1
	{
		state = 1;
		if (led == 0x80)
			led = 0x01; // change led position to the first
		else
			led <<= 1; // move LED from LSB to MSB
	}
	if ((PIND & 0x04) != 0x00 && state == 1)
		state = 0;
    }
	_delay_ms(500);
}

