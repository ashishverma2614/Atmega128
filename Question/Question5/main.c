#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char fndPattern[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66 ,0x6D, 0x7C, 0x07 ,0x7F, 0x6F};
static unsigned int digit[] = {0, 0, 0, 0};
static unsigned long cnt = 0;
unsigned char isINT = 0; // 1 is adjust mode, 2 is counting-up
unsigned char btnSelect = 0;
unsigned fndNum;

void INIT_PORT(void);
void INIT_INT(void);
void displayFND(unsigned int*); // dynamic display
void upCount(void);
void downCount(void);


ISR(INT2_vect)
{
	isINT = 1;
	fndNum = 0;
	cnt = 0;
	PORTC = 0x01;
	sei();
}
ISR(INT3_vect)
{
	fndNum++;
	if (fndNum == 4)
		fndNum = 0;
	sei();
}
ISR(INT6_vect)
{
	digit[fndNum]++;
	if (digit[fndNum] == 10)
		digit[fndNum] = 0;
}
ISR(INT7_vect)
{
	isINT = 2;
}

int main(void)
{
	INIT_PORT();
	INIT_INT();
	
	while (1)
	{	
		// set mode
		if (isINT == 1) // blink and fnd adjust
		{
			for(int i = 0; i < 4; i++)
			{
				PORTA = ~fndPattern[*(digit + (3 - i))];
			}
			PORTC = 0x01 << fndNum;
			_delay_ms(500);
			PORTA = 0xFF;
			_delay_ms(500);
		}
		
		// count mode
		else if(isINT == 2)
		{
			upCount();
			for (int i = 0; i < 5; i++) // 0.1 sec
			{
				displayFND(digit);
			}
			cnt++;
		}
	}
}

void INIT_PORT(void)
{
	DDRB = 0xFF;
	
	DDRD = 0x00;
	PORTD = 0x08;
	
	DDRE = 0x00;
	PORTE = 0x80;
	
	DDRA = 0xFF; 
	DDRC = 0xFF;
}
void INIT_INT(void)
{
	SREG |= 0x80;
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