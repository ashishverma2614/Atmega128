#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "fnd.h"

#define FND_OUT	PORTB // 자릿수 fnd는 PB2, PB3에 연결, 문자제어는 PB[7:4]에 연결
#define FND_DDR	DDRB

volatile unsigned char fnd_buf[2], digit;
volatile char fnd_flag; // 0: 켜짐, 1: 꺼짐

//다이나믹 구동을 위한 타이머 오버플로우
ISR(TIMER2_OVF_vect)
{
	TCNT2 = 256 - 125; // 116, ovf reset, 0.056(sec)

	FND_OUT = 0xF0; //  

	if( fnd_flag ) {
		FND_OUT = fnd_buf[digit] << 4;
		FND_OUT |= 0x04 << digit;
	}

	digit ^= 0x01; // fnd의 자리수를 토글시킨다
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
    
    // 처음 fnd에 표시되는 값
	fnd_buf[0] = 0xF0;
	fnd_buf[1] = 0xF0;
	digit = 0;

	fnd_flag = 0;

	timer2_init(); // 초기화와 함께 타이머를 켜준다
}