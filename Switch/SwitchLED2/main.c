// When the switch1 is pressed, LED from 1 to 8 are turned on
// from LSB to MSB
// When the switch2 is pressed on, LEDs are turned on 
//, from MSB to LSB.

// LEDs[1:8] are connected to portB
// SW1 is connected to PD2, which uses external pull-up resist.
// SW2 is connected to PD3, which uses internal pull-up resist. 

#define F_CPU 16000000L // set Frequency of MCU
#include <avr/io.h>
#include <util/delay.h>

//TODO: It needs revising to decrease chattering
int main(void)
{
    /* Replace with your application code */

	unsigned char led = 0x01; // INIT LED blinking pattern
	//unsigned btnState = 0; // 0 is OFF(for decreasing chattering)

	unsigned char dir = 0; // set direction, 0 means to move from LSB to MSB
	DDRD = 0x00; // set SW1 as input
	PORTD = 0x08; // set SW2 to use internal pull-up resist
	DDRB = 0xFF; // set all LED as output
    
	while (1) 
    {
		PORTB = led; // write LED pattern
		if ((PIND & 0x04) == 0) dir = 0; // move from LSB to MSB
		if ((PIND & 0x08) == 0) dir = 1; // move from MSB to LSB

		if (dir == 0)
		{
			if (led == 0x80)
				led = 0x01;
			else
				led <<= 1;
		}
		else // if dir is 1
		{
			if (led == 0x01)
				led = 0x80;
			else
				led >>= 1;
		}
		_delay_ms(500);
    }
}

