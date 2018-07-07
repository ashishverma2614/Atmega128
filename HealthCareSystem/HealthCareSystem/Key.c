#include <avr/io.h>
#include <util/delay.h>

#include "key.h"


#define KEY_IN	PINE
#define KEY_DDR	DDRE


volatile unsigned char key_flag;

// ���� ������� Ű�� �Է��� while������ ȣ��ȴ�
unsigned char getkey(unsigned char keyin) 
{
	unsigned char key; // Ű ���� ���� ���� ����(Ű ���� ���)

	key_flag = 0; // Ű�� ������ �Է� Ȯ���� ���� �÷���(�����ϰ� �ԷµǸ� 1�� set)

	key = ~KEY_IN & 0xF0; //Ű ���� �д´�: Ű�� PF[7:5]�� Ǯ�� ����� �������� ���� -> �������Ѽ� �о�;� ��
	if( key ) { // Ű ���� �ִٸ�
		_delay_ms(5); // ���� �ð��� �ְ�
		key = ~KEY_IN & 0xF0; // ä�͸��� ���̱� ���ؼ� �ٽ� �д´�
		if( key ) { // Ű ���� ������(��, ������ Ű ���̸�), ������ Ű �Է��� �и��Ѵ�
			if( key == 0x10 )		key = KEY_START; //sw1
			else if( key == 0x20 )	key = KEY_STOP; //sw2
			else if( key == 0x40 )	key = KEY_UP; // sw3
			else if( key == 0x80 )	key = KEY_ENTER; //sw4
		}
	}
	//�Լ� �ȿ��� ���� Ű ����, �Ķ���ͷ� ���� Ű ���� ���� �ʴٸ�, "��, ���ο� Ű ���̶��" -> ����� �Է��� ���� ��
	if( key && (key != keyin) )	key_flag = 1; // �÷��׸� 1�� set(���ο� Ű ���� ��� �Դٴ� ��ȣ)

	return key; // �������� ���� ���� Ű ���� ��ȯ�Ѵ�.
}

void key_init(void)
{
	KEY_DDR &= 0x0F; // ����ġ ��Ʈ�� �Է����� �����Ѵ�
}
