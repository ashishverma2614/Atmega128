#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


char ledPattern [] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77};


int main(void)
{
    int ad_val; // to store AD converted value

	DDRA = 0xFF;
	DDRC = 0xFF;
	PORTC = 0x01;

	ADMUX = 0x40; // 0b0100_0000: AVCC enable
	ADCSRA = 0x87; // 0b1000_0111: ADC enable, 단일변환모드, 128 pre-scale
	_delay_ms(5); // wait for first conversion

    while (1)
    {
		ADCSRA = 0xC7; // 0b1100_0111, ADSC enable(start conversion)
		//while((ADCSRA & 0x10) == 1);
		while((ADCSRA & 0x40) == 1); // ADSC이 변환중에는 1을 유지(대기)
		ad_val = (int)ADCL + ((int)ADCH << 8); // 상위 바이트와 하위 바이트 결합
		PORTA = ~ledPattern[(ad_val / 100)]; // 0-9 까지 값으로 mapping후에 출력
    }
	return 0;
}
