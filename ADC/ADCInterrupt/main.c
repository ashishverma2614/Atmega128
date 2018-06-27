#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
char ledPattern [] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77};

unsigned int ad_val;


ISR(ADC_vect)
{
	ad_val = (int)ADCL + ((int)ADCH << 8); // 변환값 추출 및 갱신
	PORTA = ~ledPattern[(ad_val / 100)]; // 추출된 변환값 (계속)출력
}

int main(void)
{
	DDRA = 0xFF;
	DDRC = 0xFF;
	PORTC = 0x01;
	
	SREG |= 0x80;

	ADMUX = 0x40; // 0b0100_0000:  AVCC enable, 단극 ADC0입력
	ADCSRA = 0xAF; // 0b1100_1111: ADC동작허용, 프리러닝모드시작(1번만), 분주비128
	_delay_ms(5); // 처음 start 시간

	ADCSRA = 0xEF; //0b1110_1111, ADIF클리어(다음 인터럽트 발생 준비)

    while (1) 
    {}
}

