// (1) 타이머 설정 -> (2) 모터의 초기화(타이머 설정을 함수내에 포함) -> (3) 모터를 제어 받은 값의 속도로 구동
#include <avr/io.h>
#include <util/delay.h>

#include "dc_motor.h"


#define DCMOTOR_OUT	PORTB
#define DCMOTOR_DDR	DDRB

// timer1의 설정: 모터를 구동하기 위해 PWM신호를 이용할 것임
// 모터 초기화 함수에서 호출된다
void timer1_init(void)
{
	TCCR1A = 0x83; // (1<<COM1A1) | (1<<WGM11) | (1<<WGM10)
	TCCR1B = 0x0B; // (1<<WGM12) | (1<<CS12)
	TCCR1C = 0x00;
	TCNT1 = 0;
	OCR1A = 0;
}

// 모터회전을 속도에 맞추어서 구동하는 함수
void dcmotor_spin(unsigned char speed)
{
	if( speed > 50 )	speed = 50;// 최고 속도를 50으로 제한한다
	OCR1A = (unsigned int)(speed * 20.46); //OCR1A 값을 통한 해당 속도의 출력
}

// 모터를 초기화 시키는 함수
void dcmotor_init(void)	
{
	DCMOTOR_DDR |= 0x60; // 모터가 연결된 포트의 출력설정(0b0110_0000: PB[5:6])
	DCMOTOR_OUT &= ~0x40; // 6번포트의 출력을 끈다("PB5번은 출력된다")(0b0100_0000) cf.모터는 PB에 연결되어 있음.

	timer1_init(); //모터 구동의 타이머 초기화
}
