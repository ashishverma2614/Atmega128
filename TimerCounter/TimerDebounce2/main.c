#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int led = 0x01;
int cnt = 0;

ISR(INT2_vect) // left
{
	
	if (led == 0x80)
	led = 0x01;
	else
	led <<= 1;

	TCCR0 = 0x07 ; // 16.4ms 정도 돔 (분주비 1024 - 0000 0111)
	TCNT0 = 0;
	EIMSK = 0;

}

ISR(INT3_vect) // right
{
	
	if (led == 0x01)
	led = 0x80;
	else
	led >>= 1;

	TCCR0 = 0x07 ; // 16.4ms 정도 돔
	TCNT0 = 0;
	EIMSK = 0;      // 외부인터럽트 사용 금지시킴
}

ISR(TIMER0_OVF_vect)
{
	if(++cnt == 10)  // 너무 빠르기 때문에 10번정도 더 돌도록 164ms 돔
	{
		cnt = 0;
		EIMSK = 0x0C;   // 외부인터럽트를 다시 살리고,
		TCCR0 = 0x00;   // 타이머를 죽이고 분주비를 000으로 하면 카운터를 죽임.
	}
	
}


int main(void)
{
	
	DDRB = 0xff ; // B포트 모두 출력설정 LED출력

	DDRD = 0x00 ; // D포트 모두 입력설정 SW사용
	DDRE = 0x00 ; // E포트 모두 입력설정

	PORTD = 0x08 ;  // 1개는 input pull-up
	PORTE = 0x80 ;  // 1개는 input pull-up

	//========================================//
	// 타이머 설정

	TCCR0 = 0x07; // 분주비 1024 설정
	TIMSK = 0x01; // 오버플로우 인터럽트 인에이블 Overflow Interrrupt Enable (사용 신호)
	TCNT0 = 0x00;

	//========================================//
	// 인터럽트 설정

	
	EICRA = 0xA0; // 2,3번 비트에 하강엣지 사용
	EIMSK = 0x0C; // 3번, 7번 스위치를 인터럽트로 사용



	SREG |= 0x80;  // 인터럽트 사용

	while (1)
	{
		PORTB = led;
	}
}