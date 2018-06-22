// This code is incomplete

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define DEBOUNCING_DELAY 5 // ms unit
typedef unsigned char unit8_t;

static unit8_t duty = 125; // initial duty ratio

void msec_delay(int n); // function for time delay

ISR(INT2_vect) // ISR to increase duty ratio, SW1 connected to PD2
{
	duty += 5;
	if (duty >= 250) duty = 250; // define max duty ratio
	msec_delay(DEBOUNCING_DELAY); // switch de-bouncing
	while(~PIND & 0x04); // wait for switch off
	EIFR |= (1 << INTF2); // *INT2 flag reset
}

ISR(INT3_vect) // decrease duty, SW2 connected to PD3
{
	duty -= 5;
	if (duty <= 5) duty = 5; // define min duty
	msec_delay(DEBOUNCING_DELAY);
	while(~PIND & 0x08);
	EIFR |= (1 << INTF3); // flag reset
}




int main(void)
{
    DDRB |= (1 << DDB4); // set PB4(OC0) as output
	DDRD = 0x00; // set SW1, 2 as input
	PORTD = 0b00001100; // use internal pull-up resist
	
	// timer0 setting: (1) phase correct PWM (2) timer stop(?)
	TCCR0 |= (0 << WGM01) | (1 << WGM00) | (2 << COM00);
	TCCR0 |= (1 << CS01); // pre-scaler ratio: 8
	
	// ext. INT setting, falling edge
	EICRA |= (2 << ISC20) | (2 << ISC30);
	EIMSK |= (1 << INT2) | (1 << INT3); // ext. INT2, 3, enable
    while (1) { OCR0 = duty; }
}

void msec_delay(int n)
{
	for ( ; n > 0; n--)
		_delay_ms(1);
}

