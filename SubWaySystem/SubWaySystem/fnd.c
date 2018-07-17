#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "fnd.h"

#define FND_OUT	PORTB // �ڸ��� fnd�� PB2, PB3�� ����, ���� ����� PB[7:4]�� ����(74LS47, Decorder)
#define FND_DDR	DDRB

volatile unsigned char fnd_buf[2], digit; // digit = 0/1
volatile char fnd_flag; // 0: ����, 1: ����

//���̳��� ������ ���� Ÿ�̸� �����÷ο�
ISR(TIMER2_OVF_vect)
{
	TCNT2 = 256 - 125; // 0.5ms

	FND_OUT = 0xF0; // pb[7:4]�� 1�� ���� ���� -> ����

	if( fnd_flag ) { // �÷��� üũ
		FND_OUT = fnd_buf[digit] << 4; // *1�� fnd�� ���: 1 byte�� buf���� (������) �̵����Ѽ� ����Ѵ�
		FND_OUT |= 0x04 << digit; // 2�� fnd�� ���: PB2�� PB3�� ������ ���鼭 ����Ѵ�
	}

	digit ^= 0x01; // fnd�� �ڸ����� ��۽�Ų��
}

// 0.5ms
void timer2_init(void) //fnd ������ ���Ǵ� �������� �����̹Ƿ� �� ���� ���� ���� �ش�
{
	TCCR2 = 0x04; // (1<<CS22), 64 prescale
	TCNT2 = 256 - 125;
	TIMSK |= 0x40; // (1<<TOIE2)
}

void fnd_off(void)
{
	fnd_flag = 0; // �÷��׸� ����
	FND_OUT = 0x00; // fnd����
}

void fnd_init(void)
{
	FND_DDR = 0xFF;
	FND_OUT = 0xF0;
    
    // ó�� fnd�� ǥ�õǴ� ��
	fnd_buf[0] = 0xF0; // ù��° fnd�� ǥ�õǴ� �ʱⰪ ����
	fnd_buf[1] = 0xF0; // �ι�° fnd�� ǥ�õǴ� �ʱ갪 ����
	digit = 0;

	fnd_flag = 0; // �÷��� �� ���¿��� ����

	timer2_init(); // �ʱ�ȭ�� �Բ� Ÿ�̸Ӹ� ���ش�->ISR������ fnd������ ���۵ȴ�
}