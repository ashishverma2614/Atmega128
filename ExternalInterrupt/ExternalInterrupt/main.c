/* When external interrupt occurs from switch, the number of interrupts stored is
 * represent to LED as a form of binary.
*/
#define F_CPU
#include <avr/io.h>
#include <avr/interrupt.h>

volatile char intNums = 0x00;

ISR(INT2_vect)
{
	intNums++;
	if (intNums >= 0xFF)
		intNums = 0x00;
}

void INIT(void)
{
	// Led port setting
	DDRB = 0xFF; // output
	DDRD = 0x00; // SW input
	PORTB = 0x00; // all led off
	PORTD = (1 << DDRD2); // internal pull-up resist
	
	// INT setting
	EIMSK |= (1 << INT2); // turn on external INT2
	EICRA |= (1 << ISC21); // detect INT from SW at falling-edge
	SREG |= 0x80; // GBL. INT. enable
}

int main(void)
{
	INIT();   
    while (1) 
    {
		PORTB = intNums;
    }
}

