#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// exclude compile optimization
// to prevent memory corruption, I used 8bit(1byte) data type
volatile char ledState = 0; // current status of LED(OFF)

// interrupt3: SW2(PD3)
ISR(INT3_vect)
{
	ledState = (ledState + 1) % 2; // convert led state
	EIFR = 0x01;
}

void INIT_PORT(void)
{
	// LED[1:8]: PORTB
	DDRB = 0xFF; // set PORTB as output
	PORTB = 0x00; // start with led off
	
	// SW1: connected to PD2, external resist
	// SW2: connected to PD3, pull-up resist
	DDRD = 0x00; // set PORTD as input
	PORTD = 0x08; // use internal pull-up of PD3
}


// interrupt3 setting
void INIT_INT3(void)
{
	EIMSK = 0x08;
	EICRA = 0x80; // ISC31 = 1, ISC30 = 0 -> falling edge
	SREG |= 0x80; // set 7 bit of SREG as 1 for global interrupt enable
	// sei();
}


int main(void)
{
	INIT_PORT(); // port setting
	INIT_INT3(); // external interrupt3 setting
	
    while (1) 
    {	
		if (ledState == 1)
		{
			PORTB = 0x01;
		}
		else // ledState == 0
			PORTB = 0x00;
    }
	return 0;
}

