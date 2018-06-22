#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>

typedef unsigned char unit8_t;

int main(void)
{
	
	unit8_t numbers[] =
	{
		0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 0x7F, 0x67
	};
	
	DDRF = 0xFF; // control segment
	DDRE = 0x0F; // control digit(0~4)
	
    while (1) 
    {
		for (int i = 0; i < 4; i++)
		{
			PORTE |= 0x0F;
			PORTE &= ~(1 << i);
			
			for (int j = 0; j < 10; j++)
			{
				PORTF = numbers[i];
				_delay_ms(100); // 0.1 sec
			}
		}
    }
}

