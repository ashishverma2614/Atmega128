#include <avr/io.h>
#include <util/delay.h>

#include "var.h"

// 아날로그 값(가변저항)을 읽어와서 반환(getter)
int var_read(void)
{
	ADCSRA |= 0x40;
	while( !(ADCSRA & 0x10) );

	return ADC; // ADC에 저장된 아날로그 입력값을 읽어온다
}

// 아날로그 레지스터의 기본값 설정
void var_init(void)
{
	ADCSRA = 0x87;
	ADMUX = 0x00;
}
