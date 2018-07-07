#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "fnd.h"

#define FND_OUT	PORTB // �ڸ��� fnd�� PB2, PB3�� ����, ��������� PB[7:4]�� ����
#define FND_DDR	DDRB

volatile unsigned char fnd_buf[2], digit;
volatile char fnd_flag; // 0: ����, 1: ����

//���̳��� ������ ���� Ÿ�̸� �����÷ο�
ISR(TIMER2_OVF_vect)
{
	TCNT2 = 256 - 125; // 116, ovf reset, 0.056(sec)

	FND_OUT = 0xF0; //  

	if( fnd_flag ) {
		FND_OUT = fnd_buf[digit] << 4;
		FND_OUT |= 0x04 << digit;
	}

	digit ^= 0x01; // fnd�� �ڸ����� ��۽�Ų��
}

void timer2_init(void)
{
	TCCR2 = 0x04; // (1<<CS22), 64 prescale
	TCNT2 = 256 - 125;
	TIMSK |= 0x40; // (1<<TOIE2)
}

void fnd_off(void)
{
	fnd_flag = 0;
	FND_OUT = 0x00;
}

void fnd_init(void)
{
	FND_DDR = 0xFF;
	FND_OUT = 0xF0;
    
    // ó�� fnd�� ǥ�õǴ� ��
	fnd_buf[0] = 0xF0;
	fnd_buf[1] = 0xF0;
	digit = 0;

	fnd_flag = 0;

	timer2_init(); // �ʱ�ȭ�� �Բ� Ÿ�̸Ӹ� ���ش�
}