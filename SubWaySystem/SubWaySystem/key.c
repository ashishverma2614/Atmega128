#include <avr/io.h>
#include <util/delay.h>

#include "key.h"

#define KEY_IN		PINE // PE[7:3], �Է��� ����
#define KEY_DDR		DDRE // E port�� �����Ͽ� ���� ������� üũ

volatile char key_flag; 

unsigned char getkey(unsigned char keyin) // ���� Ű ���� param���� �޾Ƽ� ���� ���� ���� ���Ͽ� ���ο� Ű�� ����
{
	unsigned char key;

	key_flag = 0;

	key = ~KEY_IN & 0xF8;
	if( key ) {
		_delay_ms(5);
		key = ~KEY_IN & 0xF8; // ��ٿ �Ǿ����� �ѹ� �� üũ�Ѵ�
		if( key ) {
			if( key == 0x08 )		key = KEY_1;
			else if( key == 0x10 )	key = KEY_2;
			else if( key == 0x20 )	key = KEY_3;
			else if( key == 0x40 )	key = KEY_4;
			else if( key == 0x80 )	key = KEY_5; // �ý��� �籸���� �̿�
		}
	}
	// if���ǹ������� 0�� �ƴ� ���� true(1)�� ó���Ѵ�
	// ���� Ű ���� ���� ���� Ű ���� �ٸ� ��쿡, �÷��׸� set -> ��ȿ�� Ű �Է°˻�
	if( key && (key != keyin) )	key_flag = 1;
	// �˻� ����� ���� �о�� Ű �� ����
	return key;
}

void key_init(void)
{
	KEY_DDR &= 0x07; //0b0000_0111 3~7������ �Է����� ����ش�
}