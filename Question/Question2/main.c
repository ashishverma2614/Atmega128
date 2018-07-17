#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile unsigned char btnDir = 3; // initial state(nothing pressed)
								  // define volatile not to use compiler optimization
unsigned char ledPattern = 0x01; 

// settings for sub-routines

ISR(INT2_vect)
{
	btnDir = 0; // move to LSB to MSB
}
ISR(INT3_vect)
{
	btnDir = 1; // move to MSB to LSB
}
ISR(INT6_vect)
{
	// toggle direction
	btnDir = (btnDir + 1) % 2;
}
ISR(INT7_vect)
{
	btnDir = 2; // stop position
}
// PORT setting for re-usability
void INIT_PORT(void)
{
	// led[0:7]: portB
	DDRB = 0xFF; // set portB as output
	
	// SW1(PD2) and SW2(PD3)
	DDRD = 0x00; // set portD as input
	PORTD = 0x08; // internal pull-up resist enable
	
	// SW3(PE6) and SW4(PE7)
	DDRE = 0x00; // set portE as input
	PORTE = 0x80; // internal pull-up resist enable
}

// interrupt setting for re-usability
void INIT_INT(void)
{
	sei(); // set SREG I bit
	EIMSK = 0xCC;
	EICRA = 0xA0; // use INT2, 3's falling edge(High -> Low, pull-up R)
	EICRB = 0xA0; // use INT6, 7's falling edge(High->Low, internal pull-up R)
}

// main method
int main(void)
{
	INIT_PORT(); // port setting
	INIT_INT(); // interrupt setting
	
	// event loop
	while(1)
	{
		PORTB = ledPattern; // first status: all led off by setting btnDir3 
		
		if (btnDir == 0)
		{ 
			char rightPattern = (ledPattern >> 1) | (ledPattern << 7); // used circular pattern moving
			ledPattern = rightPattern;
		}
		else if (btnDir == 1)
		{
			char leftPattern = (ledPattern << 1) | (ledPattern >> 7);
			ledPattern = leftPattern;	
		}
		else if(btnDir == 2)
		{
			// do nothing(led stop)
		}
		else if (btnDir == 3) // initial state(nothing pressed)
		{
			PORTB = 0x00; // all LED off
		}
		_delay_ms(100);
	}
}