#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "var.h"

volatile int money; // ���Ϲ����ص� �Ӵ�~ �Ӵϰ� ��������~~~~~

// ���������� �Է� �߻��� ���ͷ�Ʈ�� ����
// �������� �Է��� ��ȭ�� �� ���ҿ�� ����� ��ȭ�� �̾����� �Ѵ�.
ISR(ADC_vect) /// anal. convert interrupt trigger
{
	money = (int)(ADC / 20.46) * 100; // ADC�Է��� max���� 1023 / 20.46 * 100 = 5,000(�ִ� ���)
	ADCSRA |= 0x40;
}

// �������� ������ �Է°��� ADC�� �����Ѵ�(ADCH + ADCL)
int var_read(void)
{
	ADCSRA |= 0x40;
	while( !(ADCSRA & 0x10) ); //ADC�� ���ο� �Է°��� ���� �غ� �Ǿ������� üũ�Ѵ�(�����ϰ� ���߸� ������ �Ѽ� ���� ���ο� ���� ���� �� �ִ�)

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