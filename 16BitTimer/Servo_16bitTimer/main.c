#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

#define ZERO_ANGLE 31.225 // 0.5ms for 0 degree
#define FLAT_ANGLE (31.225 * 5) // 2.5ms for 180 degree

// increase angle(SW1-PD2)
ISR(INT2_vect)
{
	if (OCR1B != FLAT_ANGLE)
		OCR1B++;	
}

// decrease angle(SW2-PD3)
ISR(INT3_vect)
{
	 if (OCR1B != ZERO_ANGLE)
		OCR1B--;	
}

ISR(TIMER1_COMPA_vect) {}
ISR(TIMER1_COMPB_vect) {}
void INIT(void);

int main(void)
{
    INIT();
    while (1) {}
}

void INIT(void)
{
	// motor output GPIO
	DDRB |= (1 << DDRB5);
	PORTB = 0x20;
	
	// Ext. INT2, 3
	DDRD = 0x00;
	PORTD = 0xFF; // internal pull-up
	EIMSK |= (1 << INT2) | (1 << INT3);
	EICRA |= (1 << ISC21) | (1 << ISC31); // falling-edge detection
	
	// timer1 setting. (1)fast PWM,  (2) toggle output wave of both OCR, (3) pre-scaler ratio 256 -> 1.048576 sec
	TCCR1A |= (1 << WGM11) | (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1);
	TCCR1B |= (1 << WGM13) | (1 << WGM12) | (1 << CS12);
	OCR1A = 1250; // approx. 20ms(65,535 / 52.4288) -> ZERO_ANGLE = 1250 / 40 = 31.225
	OCR1B = 31.225;
	
	sei(); //GLB. INT. enable
	
}

