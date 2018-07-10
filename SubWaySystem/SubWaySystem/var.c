#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "var.h"

volatile int money; // 뭐니뭐니해도 머니~ 머니가 좋겠지만~~~~~

// 가변저항의 입력 발생을 인터럽트로 감지
// 가변저항 입력의 변화는 곧 지불요금 계산의 변화로 이어져야 한다.
ISR(ADC_vect) /// anal. convert interrupt trigger
{
	money = (int)(ADC / 20.46) * 100; // ADC입력의 max값인 1023 / 20.46 * 100 = 5,000(최대 요금)
	ADCSRA |= 0x40;
}

// 가변저항 순간의 입력값을 ADC에 저장한다(ADCH + ADCL)
int var_read(void)
{
	ADCSRA |= 0x40;
	while( !(ADCSRA & 0x10) ); //ADC가 새로운 입력값을 받을 준비가 되었는지를 체크한다(완전하게 비어야만 데이터 훼손 없이 새로운 값을 받을 수 있다)

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