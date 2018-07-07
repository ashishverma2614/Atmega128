#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "var.h"

volatile int money;

// 가변저항의 입력 발생을 인터럽트로 감지
ISR(ADC_vect) /// anal. convert interrupt trigger
{
	money = (int)(ADC / 20.46) * 100; // by dividing the value from potention meter, it can calc the sum of fare
	ADCSRA |= 0x40;
}

// 가변저항 순간의 입력값이 저장된 ADC를 리턴함
int var_read(void)
{
	ADCSRA |= 0x40;
	while( !(ADCSRA & 0x10) );

	return ADC;
}

// ?
void var_start(void)
{
	ADCSRA |= 0x40;
}
//ADC Reg의 초기화 
void var_init(void)
{
	ADCSRA = 0x8F;
	ADMUX = 0x00;
}