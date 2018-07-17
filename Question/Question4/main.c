#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char fndPattern[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66 ,0x6D, 0x7C, 0x07 ,0x7F, 0x6F};
static unsigned int digit[] = {0, 0, 0, 0};
static unsigned long cnt = 0;
unsigned char isINT = 0;
unsigned char isUpDown = 0; // 0 for nothing
	
ISR(INT2_vect)
{
	isUpDown = 1;
	isINT = 1;
	sei();
}
ISR(INT3_vect)
{
	isUpDown = 2;
	cnt = 9999;
	isINT = 1;
	sei();
}
ISR(INT6_vect)
{
	cnt = 0000;
	isINT = 1;
	sei();
}
ISR(INT7_vect)
{
	cnt = 9999;
	isINT = 1;
	sei();
}

void INIT_PORT(void); 
void INIT_INT(void); 
void displayFND(unsigned int*); // dynamic display
void upCount(void);
void downCount(void);

int main(void)
{
	INIT_PORT();
	INIT_INT();
	
    while (1) 
    {
		if (isINT == 0)
		{} // no Interrupt
		
		// interrupt case
		else if (isINT == 1)
		{
			if (isUpDown == 1)
				upCount();
			else if (isUpDown == 2)
				downCount();
		}
		
		// display number regardless of interrupt
		for (int i = 0; i < 50; i++)
		{
			displayFND(digit);
		}
		
		// counter control
		if (isINT == 1 && isUpDown ==1) cnt++;
		if (isINT== 1 && isUpDown == 2) cnt--;
		
    }
}

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
	
	// FND port setting
	DDRA = 0xFF; // bits in portA(a~g, dot)
	DDRC = 0xFF; // control FND[0:3]
}
void INIT_INT(void)
{
	SREG |= 0x80; // sei();
	EIMSK = 0xCC;
	EICRA = 0xA0; // falling edge(SW1, 2)
	EICRB = 0xA0; // falling edge(SW3, 4)
}
void displayFND(unsigned int * digit)
{
	for (int i = 0; i < 4; i++)
	{
		PORTC = 0x01 << i;
		PORTA = ~fndPattern[*(digit + (3 - i))];
		_delay_ms(5);
	}
}
void upCount(void)
{
		digit[0] = cnt / 1000;
		digit[1] = (cnt % 1000) / 100;
		digit[2] = (cnt % 100) / 10;
		digit[3] = cnt % 10;
		if (cnt == 10000) {cnt = 0 ;}
}
void downCount(void)
{
		digit[0] = (cnt % 10000) / 1000;
		digit[1] = (cnt % 1000) / 100;
		digit[2] = (cnt % 100) / 10;
		digit[3] = cnt % 10;
		if (cnt == 0) {cnt = 10000;}
}