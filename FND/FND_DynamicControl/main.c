#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define FND1 0
#define FND2 1
#define FND1_ON (0 << FND1)
#define FND2_ON (0 << FND2)
#define FND1_OFF (1 << FND1)
#define FND2_OFF (1 << FND2)

char fndPattern[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66 ,0x6D, 0x7C, 0x07 ,0x7F, 0x6F};
unsigned int cnt = 0;

ISR(TIMER0_OVF_vect) // timer for counting number
{
	static int timer = 0;
	timer++;
	if (timer == 250)
	{
		cnt++;
		timer = 0;
		if (cnt == 99) cnt = 0;
	}
	TCNT0 += 0x06;
}


void INIT(void)
{
	DDRA = 0xFF; // 0 ~ 9
	DDRC = 0xFF; // fnd 1 ~4
	
	TCCR0 = 0x04; // 64 pre-scale, OVF interrupt
	TCNT0 = 0x06;
	TIMSK = 0x01;
	sei();
}

void displayFnd(unsigned count) // dynamic display
{
	PORTC = (FND1_ON | FND2_OFF);
	PORTA = ~fndPattern[(count % 100) / 10];
	_delay_ms(5);
	PORTC = (FND1_OFF | FND2_ON);
	PORTA = ~fndPattern[(count % 10)];
	_delay_ms(5);
}


int main(void)
{
	INIT();
    
    while (1) 
    {
		displayFnd(cnt);
    }
	return 0;
}

