/* Last but not least, third way to reduce chattering problem is to use timer/counter interrupt.
*/

#define F_CPU 160000L
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char ledPattern = 0x01; //초기패턴결정
unsigned int cnt = 0; // 카운트 초기화
unsigned char dir = 0; // 정지상태

// 오버플로 타이머에 의한 디바운스 시간설정
// 일정시간 입력무시
ISR(TIMER0_OVF_vect)
{
	++cnt;
	if (cnt == 10) // 10 ~ 30
	{
		cnt = 0; //카운트 리셋
		EIMSK = 0x0C; // 인터럽트를 살린다
		TCCR0 = 0x00; // 타이머 인터럽트 정지
		EIFR = 0x01; //혹시 중간에 들어올 인터럽트를 위해 지워둔다
	}
}

ISR(INT2_vect) // move from LSB to MSB
{
	char rightPattern = (ledPattern >> 1) | (ledPattern << 7);
	ledPattern = rightPattern;

	TCCR0 = 0x01; //타이머 인터럽트를 다시 켠다(일정시간 지연을 위해)
	TCNT0 = 0; // 타이머 인터럽트의 초기화(처음부터 센다)
	EIMSK = 0; // 외부 인터럽트를 잠시 중단한다(타이머 인터럽트가 돌아가는 시간만큼)
}
ISR(INT3_vect) // move from MSB to LSB
{
	char leftPattern = (ledPattern << 1) | (ledPattern >> 7);
	ledPattern = leftPattern;
	TCCR0 = 0x01;
	TCNT0 = 0;
	EIMSK = 0;
}


int main(void)
{
	//포트 및 오버플로우 인터럽트 설정
	DDRB = 0xff;
	PORTD = 0xff; // led패턴 출력

	EICRA = 0xa0; // 하강엣지설정
	EIMSK = 0x0c; // 0000_1100

	TCCR0 = 0x00; // 타이머 정지 상태
	TIMSK = 0x01; // 오버플로우 인터럽트 인에이블
	TCNT0 = 0x00;
	SREG |= 0x80;

	while (1)
	{
		PORTB = ledPattern;
	}
}
