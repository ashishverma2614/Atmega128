#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>



int main(void)
{
	static unsigned int ad_val;
	// port setting
	DDRB = 0xFF; // led port

	// ADC setting
	ADMUX = 0x41; // 0x41
	ADCSRA = 0x87; // 0x87;
	_delay_ms(5);

    while (1) 
    {
		ADCSRA = 0xC7; // 0xC7
		while ((ADCSRA & 0x40) == 1); // 쓰고 있는 중에는 대기
		ad_val = (int)(ADCL) + ((int)(ADCH << 8));

		if (ad_val <= 128)
			PORTB = 0x01;
		else if (ad_val <= 256)
			PORTB = 0x02;
		else if (ad_val <= 384)
			PORTB = 0x04;
		else if (ad_val <= 512)
			PORTB = 0x08;
		else if (ad_val <= 640)
			PORTB = 0x10;
		else if (ad_val <= 768)
			PORTB = 0x20;
		else if (ad_val <= 896)
			PORTB = 0x40;
		else
			PORTB = 0x80;
    }
}

