#include <avr/io.h>
#include <util/delay.h>

#include "key.h"

#define KEY_IN		PINE // PE[7:3]
#define KEY_DDR		DDRE

volatile char key_flag; 

unsigned char getkey(unsigned char keyin)
{
	unsigned char key;

	key_flag = 0;

	key = ~KEY_IN & 0xF8;
	if( key ) {
		_delay_ms(5);
		key = ~KEY_IN & 0xF8; // 디바운스 되었는지 한번 더 체크한다
		if( key ) {
			if( key == 0x08 )		key = KEY_1;
			else if( key == 0x10 )	key = KEY_2;
			else if( key == 0x20 )	key = KEY_3;
			else if( key == 0x40 )	key = KEY_4;
			else if( key == 0x80 )	key = KEY_5;
		}
	}

	if( key && (key != keyin) )	key_flag = 1;

	return key;
}

void key_init(void)
{
	KEY_DDR &= 0x07; //0b0000_0111 3~7번핀을 입력으로 잡아준다
}