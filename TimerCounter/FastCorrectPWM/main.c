#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned int i = 0;

// 듀티비 증가 스위치 인터럽트
ISR(INT2_vect)
{
  if (++i == 255)
	i = 0;
 else 
	i++;
}

// 듀티비 감소 스위치 인터럽트

int main(void)
{
	//TIMSK = 0x00;
	DDRB = 0xff; // PB4(oc0: led)
	DDRD = 0x00; //스위치 입력 포트

	TCCR0 = 0b01101010; // 분주비 32, fastPWM
	OCR0 = 0;

	EIMSK = 0xCC;
	EICRA = 0xA0;
	EICRB = 0xA0;
	sei();

	/* Replace with your application code */
	while (1) 
    {
		OCR0 = i;
		/*
		// 딜레이 시간만큼 듀티비 변화
		for (int i = 0; i < 256; i++)
		{
			OCR0 = i;
			_delay_ms(1);
		}*/
    }
}

