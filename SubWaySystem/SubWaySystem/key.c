#include <avr/io.h>
#include <util/delay.h>

#include "key.h"

#define KEY_IN		PINE // PE[7:3], 입력핀 설정
#define KEY_DDR		DDRE // E port에 연결하여 폴링 방식으로 체크

volatile char key_flag; 

unsigned char getkey(unsigned char keyin) // 현재 키 값을 param으로 받아서 새로 들어온 값과 비교하여 새로운 키를 리턴
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
			else if( key == 0x80 )	key = KEY_5; // 시스템 재구동에 이용
		}
	}
	// if조건문내에서 0이 아닌 것은 true(1)로 처리한다
	// 이전 키 값과 새로 들어온 키 값이 다른 경우에, 플래그를 set -> 유효한 키 입력검사
	if( key && (key != keyin) )	key_flag = 1;
	// 검사 통과후 새로 읽어온 키 값 리턴
	return key;
}

void key_init(void)
{
	KEY_DDR &= 0x07; //0b0000_0111 3~7번핀을 입력으로 잡아준다
}