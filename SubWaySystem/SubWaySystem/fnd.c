#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "fnd.h"

#define FND_OUT	PORTB // 자릿수 fnd는 PB2, PB3에 연결, 문자 제어는 PB[7:4]에 연결(74LS47, Decorder)
#define FND_DDR	DDRB

volatile unsigned char fnd_buf[2], digit; // digit = 0/1
volatile char fnd_flag; // 0: 켜짐, 1: 꺼짐

//다이나믹 구동을 위한 타이머 오버플로우
ISR(TIMER2_OVF_vect)
{
	TCNT2 = 256 - 125; // 0.5ms

	FND_OUT = 0xF0; // pb[7:4]에 1을 전부 쓴다 -> 끈다

	if( fnd_flag ) { // 플래그 체크
		FND_OUT = fnd_buf[digit] << 4; // *1개 fnd의 출력: 1 byte씩 buf값을 (빠르게) 이동시켜서 출력한다
		FND_OUT |= 0x04 << digit; // 2개 fnd의 출력: PB2와 PB3을 번갈아 가면서 출력한다
	}

	digit ^= 0x01; // fnd의 자리수를 토글시킨다
}

// 0.5ms
void timer2_init(void) //fnd 구동에 사용되는 레지스터 설정이므로 한 파일 내에 묶어 준다
{
	TCCR2 = 0x04; // (1<<CS22), 64 prescale
	TCNT2 = 256 - 125;
	TIMSK |= 0x40; // (1<<TOIE2)
}

void fnd_off(void)
{
	fnd_flag = 0; // 플래그를 끈다
	FND_OUT = 0x00; // fnd끈다
}

void fnd_init(void)
{
	FND_DDR = 0xFF;
	FND_OUT = 0xF0;
    
    // 처음 fnd에 표시되는 값
	fnd_buf[0] = 0xF0; // 첫번째 fnd에 표시되는 초기값 저장
	fnd_buf[1] = 0xF0; // 두번째 fnd에 표시되는 초깃값 저장
	digit = 0;

	fnd_flag = 0; // 플래그 끈 상태에서 시작

	timer2_init(); // 초기화와 함께 타이머를 켜준다->ISR내에서 fnd구동이 시작된다
}