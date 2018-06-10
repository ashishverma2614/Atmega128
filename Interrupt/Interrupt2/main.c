/*
 * Interrupt2.c
 *
 * Created: 2018-06-03 오전 12:48:50
 * Author : woo03
 */ 
#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
volatile unsigned char led = 0x01;
volatile unsigned char dir = 0;

// LEDs turns on from LSB to MSB
ISR(INT2_vect)
{
	sei(); // for multiple interrupt
	while(1)
	{
		PORTB = led;
		led = (led << 1) | (led >> 7);
		_delay_ms(100);
	}
	EIFR = 0x01;
}

// LEDs turns on from MSB to LSB
ISR(INT3_vect)
{
	sei(); // for multiple interrupt
	led = 0x80;
	while(1)
	{
		PORTB = led;
		led = (led >> 1) | (led << 7);
		_delay_ms(100);
	}
	EIFR = 0x01;
}

// Shift LED direction
ISR(INT6_vect)
{
	
}

// Stop LED

int main(void)
{
	DDRB = 0xFF;
	PORTB = 0xFF;
	DDRD = 0x00; //set portD as input
	PORTD = 0x08; // set PD3 as internal pull-up
	EIMSK = 0x0C; // set INT2 and 3(00001100)
	EICRA = 0xA0; // falling edge
	sei(); // global interrupt enable
	
    /* Replace with your application code */
    while (1) 
    {
		PORTB = 0x00;
		
    }
}

