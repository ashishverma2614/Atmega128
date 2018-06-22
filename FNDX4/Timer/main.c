#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define SEGMENT_DELAY 5
#define MILLIS_PER_OVF 16
#define MICROS_PER_OVF 384

typedef unsigned char unit8_t;
typedef unsigned long unit16_t;
typedef unsigned long long unit32_t;

unit8_t numbers[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x67 };
volatile unit32_t millis = 0;
volatile unit16_t micros = 0;	

ISR(TIMER0_OVF_vect)
{
	micros += MICROS_PER_OVF;
	millis += MILLIS_PER_OVF;
	
	millis += (micros / 1000);
	micros %= 1000;
}

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
    DDRE = 0x0F; // control pin
    DDRF = 0xFF; // segment pin
	
	TCCR0 |= (1 << CS02) | (1 << CS01) | (1 << CS00); // pre-scaler ratio 1024
	TIMSK |= (1 << TOIE0); // OVF INT enable
	sei();
	
    while (1) 
    {
		int temp = (millis / 100) % 10000;
		
		int thousands = temp / 1000;
		int hundreds = temp / 100 % 10;
		int tens = temp / 10 % 10;
		int ones = temp % 10;
		
		display_digit(1, thousands);
		_delay_ms(SEGMENT_DELAY);
		display_digit(2, hundreds);
		_delay_ms(SEGMENT_DELAY);
		display_digit(3, tens);
		_delay_ms(SEGMENT_DELAY);
		display_digit(4, ones);
		_delay_ms(SEGMENT_DELAY);
    }
}

