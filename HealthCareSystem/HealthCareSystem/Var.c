#include <avr/io.h>
#include <util/delay.h>

#include "var.h"

// �Ƴ��α� ��(��������)�� �о�ͼ� ��ȯ(getter)
int var_read(void)
{
	ADCSRA |= 0x40;
	while( !(ADCSRA & 0x10) ); // AD��ȯ�� ���������� ���

	return ADC; // ADC(ADCH + ADCL)�� ����� �Ƴ��α� �Է°��� �о�´�
}

// �Ƴ��α� ���������� �⺻�� ����
void var_init(void)
{
	ADCSRA = 0x87; // 0b1000_0111, (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1 << ADPS0), 128 presale
	ADMUX = 0x00; // AREF���� ���� ���, ���� 10��Ʈ ������, �����Է� X, ADC0���
}
