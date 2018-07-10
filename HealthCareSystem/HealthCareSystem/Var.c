#include <avr/io.h>
#include <util/delay.h>

#include "var.h"

// 아날로그 값(가변저항)을 읽어와서 반환(getter)
int var_read(void)
{
	ADCSRA |= 0x40;
	while( !(ADCSRA & 0x10) ); // AD변환이 끝날때까지 대기

	return ADC; // ADC(ADCH + ADCL)에 저장된 아날로그 입력값을 읽어온다
}

// 아날로그 레지스터의 기본값 설정
void var_init(void)
{
	ADCSRA = 0x87; // 0b1000_0111, (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1 << ADPS0), 128 presale
	ADMUX = 0x00; // AREF단자 전압 사용, 하위 10비트 저장모드, 차동입력 X, ADC0사용
}
