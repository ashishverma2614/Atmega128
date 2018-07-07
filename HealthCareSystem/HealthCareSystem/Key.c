#include <avr/io.h>
#include <util/delay.h>

#include "key.h"


#define KEY_IN	PINE
#define KEY_DDR	DDRE


volatile unsigned char key_flag;

// 폴링 방식으로 키의 입력이 while문에서 호출된다
unsigned char getkey(unsigned char keyin) 
{
	unsigned char key; // 키 값을 받을 변수 설정(키 값은 양수)

	key_flag = 0; // 키의 완전한 입력 확인을 위한 플래그(완전하게 입력되며 1로 set)

	key = ~KEY_IN & 0xF0; //키 값을 읽는다: 키는 PF[7:5]에 풀업 방식의 저항으로 연결 -> 반전시켜서 읽어와야 함
	if( key ) { // 키 값이 있다면
		_delay_ms(5); // 일정 시간을 주고
		key = ~KEY_IN & 0xF0; // 채터링을 줄이기 위해서 다시 읽는다
		if( key ) { // 키 값이 있으면(즉, 동일한 키 값이면), 각각의 키 입력을 분리한다
			if( key == 0x10 )		key = KEY_START; //sw1
			else if( key == 0x20 )	key = KEY_STOP; //sw2
			else if( key == 0x40 )	key = KEY_UP; // sw3
			else if( key == 0x80 )	key = KEY_ENTER; //sw4
		}
	}
	//함수 안에서 읽은 키 값과, 파라미터로 들어온 키 값이 같지 않다면, "즉, 새로운 키 값이라면" -> 제대로 입력이 들어온 것
	if( key && (key != keyin) )	key_flag = 1; // 플래그를 1로 set(새로운 키 값이 들어 왔다는 신호)

	return key; // 그제서야 새로 들어온 키 값을 반환한다.
}

void key_init(void)
{
	KEY_DDR &= 0x0F; // 스위치 포트를 입력으로 설정한다
}
