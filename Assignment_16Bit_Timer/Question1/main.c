#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

typedef unsigned char unit8_t;
volatile unit8_t btnDir = 3; // initial state(nothing pressed)
volatile unit8_t ledPattern = 0x01;
volatile unit8_t cnt;
void INIT(void);


ISR(INT2_vect) // LSB to MSB
{
	btnDir = 0;
}

ISR(INT3_vect) // MSB to LSB
{
	btnDir = 1;
}

ISR(INT6_vect) // toggle blinking direction
{
	btnDir = (btnDir + 1) % 2;
}

ISR(INT7_vect) // stop led
{
	btnDir = 2;
}

ISR(TIMER0_COMP_vect)
{
	cnt++;
	TCNT0 = 0x00;
	if (cnt == 100)
	{
		if (btnDir == 0)
		{
			unit8_t rightPattern = (ledPattern >> 1) | (ledPattern << 7);
			ledPattern = rightPattern;
		}
			
		else if (btnDir == 1)
		{
			unit8_t leftPattern = (ledPattern << 1) | (ledPattern >> 7);
			ledPattern = leftPattern;
		}
			
		PORTB = ledPattern;
		cnt = 0;
	}
}




void main(void)
{
	INIT();
	while(1){}
}

void INIT(void)
{
	// led setting
	DDRB = 0xFF;
	// SW[1:4], internal pull-up
	DDRD = 0x00;
	DDRE = 0x00;
	PORTD = 0x08;
	PORTE = 0x80;
	
	// Ext. INT. 2, 3, 6, 7(falling edge detection)
	EIMSK |= (1 << INT7) | (1 << INT6) | (1 << INT3) | (1 << INT2);
	EICRA |= (1 << ISC31) | (1 << ISC21);
	EICRB |= (1 << ISC61) | (1 << ISC71);
	
	// timer0 INT, pre-scaler ratio 64
	TIMSK |= (1 << OCIE0);
	TCCR0 |= (1 << CS02);
	OCR0 = 249;
	
	sei(); // GLB. INT. enable
}

