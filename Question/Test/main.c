#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char fndPattern[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66 ,0x6D, 0x7C, 0x07 ,0x7F, 0x6F};
static unsigned int digit[] = {0, 0, 0, 0};
unsigned int cnt = 0;



// function prototype
void displayFND(unsigned int*);
void upCount(void);

// PORT setting
void INIT_PORT(void)
{
	DDRA = 0xFF; // bits in portA(a~g, dot)
	DDRC = 0xFF; // control FND[0:3]
}

// main method
int main(void)
{
	INIT_PORT();
	
	while (1)
	{
		upCount(); // calc count
		
		for (int i = 0; i < 50; i++)
		{
			displayFND(digit); // 5ms간격으로 점멸 및 쓰기(총20ms)
			
		} //1초 경과
		cnt++; //카운트 증가
	}
}

/* function implementation */


void displayFND(unsigned int * digit)
{
	for (int i = 0; i < 4; i++) // total 20ms
	{
		PORTC = 0x01 << i; // dynamic operation changing portC[0:4] output
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
		if (cnt == 10000) {cnt = 0;}
}