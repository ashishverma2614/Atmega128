#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


int main(void)
{
	//TIMSK = 0x00;
	DDRB = 0xff; // PB4(oc0: led)
	TCCR0 = 0b01101011; // 분주비 32(011), phaseCorrect PWM
	OCR0 = 0;

	/* Replace with your application code */
	while (1)
	{
		for (int i = 0; i < 256; i++)
		{
			OCR0 = i;
			_delay_ms(10);
		}
	}
}
