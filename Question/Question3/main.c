#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char fndPattern[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66 ,0x6D, 0x7C, 0x07 ,0x7F, 0x6F};
static unsigned int digit[] = {0, 0, 0, 0};
static unsigned int cnt = 0;;



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
		upCount(); //calc counter
		for (int i = 0; i < 50; i++) // for 1sec
		{
			displayFND(digit);
		}
		cnt++;
		
    }
}

/* function implementation */


void displayFND(unsigned int * digit)
{
	for (int i = 0; i < 4; i++)
	{
		PORTC = 0x01 << i; // dynamic operation changing portC[0:4] output
		PORTA = ~fndPattern[*(digit + (3 - i))];
		_delay_ms(5);
	}
}
void upCount(void)
{
		//static unsigned int digit[] = {0, 0, 0, 0};
		digit[0] = cnt / 1000;
		digit[1] = (cnt % 1000) / 100;
		digit[2] = (cnt % 100) / 10;
		digit[3] = cnt % 10;
		if (cnt == 10000)
			cnt = 0;
}