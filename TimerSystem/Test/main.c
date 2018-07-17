#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#define FND1	PORTC = (0x01 << PC3)
#define FND2	PORTC = (0x01 << PC2)
#define FND3	PORTC = (0x01 << PC1)
#define FND4	PORTC = (0x01 << PC0)

const unsigned char FND[] = {0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};		//FND 0~9, off
char TEMP[5] = {0};																		//count변수 값을 문자 형태로 저장할 배열
unsigned volatile int count = 0;														//스위치를 누르면 1씩 증감하는 카운트 변수
unsigned char timer1A = 0;																//5ms를 맞추기 위한 변수
unsigned int timer0 = 0;																//500ms를 맞추기 위한 변수
unsigned char tr = 0;																	//출력할 FND 선택 변수
unsigned char sw_set = 0;																//셋팅 여부 확인 변수
unsigned int sw_count = 0;

/* 외부 인터럽트 */

/* SW1 - 카운트 초기값 설정 / 첫번째 자리 수 점멸 */
ISR(INT7_vect)
{
    tr = 1;
    sw_set = 1;

    /* SW1을 누르면 타이머0 인터럽트 설정 */
    TCCR0 = ((0x01 << WGM01) | (0x01 << CS01) | (0x01 << CS00));		//CTC모드, 분주 : 32
    OCR0 = 249;															//0.5ms = (1/16) * 32 * (249+1)
    TIMSK |= (0x01 << OCIE0);											//타이머0 출력 비교 매치 인터럽트 인에이블
}

/* SW2 - 자리수 변경 / 1 -> 2 -> 3 -> 4 -> 1 순서 / 변경된 자리 수 점멸 */
ISR(INT6_vect)
{
    if(sw_set == 1)
    tr++;
    if(tr == 5)
    tr = 1;
}

/* SW3 - 해당 자리수 카운트-업 / 0 -> 9 -> 0 */
ISR(INT3_vect)
{
    if(TEMP[4-tr] == '9')
    TEMP[4-tr] = '0';
    else
    TEMP[4-tr]++;

    count = atoi(TEMP);
}

/* SW4 - 설정 완료 후 카운트 업 / 0.1초 시간 지연 */
ISR(INT2_vect)
{
    sw_set = 0;
    tr = 0;
    TIMSK &= ~(0x01 << OCIE0);
    TCCR0 = 0;
    TCNT0 = 0;
}

/* 타이머 인터럽트 */

/* 5ms마다 timer1A 카운트 업*/
ISR(TIMER1_COMPA_vect)
{
    timer1A++;
    if(timer1A == 5)		//20ms 마다 초기화
    timer1A = 1;
}

/* 100ms마다 timer1A 카운트 업*/
ISR(TIMER3_COMPA_vect)
{
    if(sw_set == 0)
    {
        count++;
        if(count > 10000)
        count = 0;
    }
}

/* 50us마다 timer1A 카운트 업*/
ISR(TIMER0_COMP_vect)
{
    if(sw_set == 1)
    timer0++;
    if(timer0 > 2000)		//1초마다 초기화
    timer0 = 0;
}



/* 사용자 정의 함수 */

/* 스위치에 의해 선택된 FND 출력 없애기*/
void Blink(void)
{
    if(timer0 <= 1000)
    {
        if(tr == 1)
        PORTC &= ~(0x01 << PINC0);
        else if(tr == 2)
        PORTC &= ~(0x01 << PINC1);
        else if(tr == 3)
        PORTC &= ~(0x01 << PINC2);
        else if(tr == 4)
        PORTC &= ~(0x01 << PINC3);
    }
}

/* FND 출력하기*/
void Disp_FND(void)
{
    switch(timer1A)
    {
        case 1:
        FND1;
        PORTA = FND[TEMP[0] - '0'];
        Blink();
        break;
        case 2:
        FND2;
        PORTA = FND[TEMP[1] - '0'];
        Blink();
        break;
        case 3:
        FND3;
        PORTA = FND[TEMP[2] - '0'];
        Blink();
        break;
        case 4:
        FND4;
        PORTA = FND[TEMP[3] - '0'];
        Blink();
        timer1A = 0;
        break;
    }
}

int main(void)
{
    /*PORTA/PORTC 출력 설정 PORTD/PORTE 입력 설정 */
    DDRA = 0xFF;
    DDRC = ((0x01 << DDC3) | (0x01 << DDC2) | (0x01 << DDC1) | (0x01 << DDC0));
    DDRD = 0x00;
    DDRE = 0x00;

    /*SW 풀업 저항 설정*/
    PORTD = 0xFF;
    PORTE = 0xFF;
    
    /*외부 인터럽트 설정*/
    EICRA = ((0x01 << ISC21) | (0x01 << ISC31));									//trigger설정 : falling edge
    EICRB = ((0x01 << ISC61) | (0x01 << ISC71));									//trigger설정 : falling edge
    EIMSK = ((0x01 << INT2) | (0x01 << INT3) | (0x01 << INT6) | (0x01 << INT7));
    
    /* 타이머 인터럽트 설정*/
    TCCR1B = ((0x01 << WGM12) | (0x01 << CS11) | (0x01 << CS10));					//타이머1 CTC모드, 분주 : 64
    TCCR3B = ((0x01 << WGM32) | (0x01 << CS31) | (0x01 << CS30));					//타이머3 CTC모드, 분주 : 64
    TIMSK = ((0x01 << OCIE1A));														//타이머1 출력 비교 매치 A 인터럽트 인에이블
    ETIMSK = ((0x01 << OCIE3A));													//타이머3 출력 비교 매치 A 인터럽트 인에이블
    OCR1A = 1249;																	//5ms = (1/16) * 64 * (1249+1)
    OCR3A = 24999;																	//100ms = (1/16) * 64 * (24999+1)

    sei();

    while(1)
    {
        sprintf(TEMP, "%04d", count);
        Disp_FND();
    }

    return 0;
}
