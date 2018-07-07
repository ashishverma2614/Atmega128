// (1) Ÿ�̸� ���� -> (2) ������ �ʱ�ȭ(Ÿ�̸� ������ �Լ����� ����) -> (3) ���͸� ���� ���� ���� �ӵ��� ����
#include <avr/io.h>
#include <util/delay.h>

#include "dc_motor.h"


#define DCMOTOR_OUT	PORTB
#define DCMOTOR_DDR	DDRB

// timer1�� ����: ���͸� �����ϱ� ���� PWM��ȣ�� �̿��� ����
// ���� �ʱ�ȭ �Լ����� ȣ��ȴ�
void timer1_init(void)
{
	TCCR1A = 0x83; // (1<<COM1A1) | (1<<WGM11) | (1<<WGM10)
	TCCR1B = 0x0B; // (1<<WGM12) | (1<<CS12)
	TCCR1C = 0x00;
	TCNT1 = 0;
	OCR1A = 0;
}

// ����ȸ���� �ӵ��� ���߾ �����ϴ� �Լ�
void dcmotor_spin(unsigned char speed)
{
	if( speed > 50 )	speed = 50;// �ְ� �ӵ��� 50���� �����Ѵ�
	OCR1A = (unsigned int)(speed * 20.46); //OCR1A ���� ���� �ش� �ӵ��� ���
}

// ���͸� �ʱ�ȭ ��Ű�� �Լ�
void dcmotor_init(void)	
{
	DCMOTOR_DDR |= 0x60; // ���Ͱ� ����� ��Ʈ�� ��¼���(0b0110_0000: PB[5:6])
	DCMOTOR_OUT &= ~0x40; // 6����Ʈ�� ����� ����("PB5���� ��µȴ�")(0b0100_0000) cf.���ʹ� PB�� ����Ǿ� ����.

	timer1_init(); //���� ������ Ÿ�̸� �ʱ�ȭ
}
