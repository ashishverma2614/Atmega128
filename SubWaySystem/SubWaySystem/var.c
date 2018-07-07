#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "var.h"

volatile int money;

// ���������� �Է� �߻��� ���ͷ�Ʈ�� ����
ISR(ADC_vect) /// anal. convert interrupt trigger
{
	money = (int)(ADC / 20.46) * 100; // by dividing the value from potention meter, it can calc the sum of fare
	ADCSRA |= 0x40;
}

// �������� ������ �Է°��� ����� ADC�� ������
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
//ADC Reg�� �ʱ�ȭ 
void var_init(void)
{
	ADCSRA = 0x8F;
	ADMUX = 0x00;
}