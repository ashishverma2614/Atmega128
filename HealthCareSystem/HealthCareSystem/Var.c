#include <avr/io.h>
#include <util/delay.h>

#include "var.h"

// �Ƴ��α� ��(��������)�� �о�ͼ� ��ȯ(getter)
int var_read(void)
{
	ADCSRA |= 0x40;
	while( !(ADCSRA & 0x10) );

	return ADC; // ADC�� ����� �Ƴ��α� �Է°��� �о�´�
}

// �Ƴ��α� ���������� �⺻�� ����
void var_init(void)
{
	ADCSRA = 0x87;
	ADMUX = 0x00;
}
