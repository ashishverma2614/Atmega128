//#define __STDIO_FDEVOPEN_COMPAT_12

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>

#include "dc_motor.h"
#include "key.h"
#include "var.h"
#include "lcd.h"
#include "led.h"


struct time { //  시간 인스턴스의 생성 -> 현재시간(current time) 및 운동시간(section time)
	unsigned char hour;
	unsigned char minute;
	unsigned char seconds;
};

extern volatile unsigned char key_flag; // 정상적 입력의 확인을 체크하는 flag변수

volatile unsigned char tick0, section_tick, velocity; // 시간과 관련되는 변수 1)MCU구동이후 타이머의 진행시간 2) start모드에서 운동시간 3) 칼로리를 구하기 위한 속도변수(가변저항에서 읽어오는 ADC값)
volatile struct time current = { 0, 0, 0 }; // mcu구동이후의 시간
volatile struct time section = { 0, 0, 0 }; // 시간 설정 이후부터 흐르는 시간
volatile char mode; // enum 상수에 정의된 모드를 받아주는 임시변수
volatile double kcal; // 소모한 칼로리의 양



enum { // mode 변수에 들어가는 상수
	NO_MODE, // 0
	START_MODE, // 1
	STOP_MODE // 2
};


void timer0_init(void)
{
	TCCR0 = 0x06; // (1 << CS02) | (1 << CS01), normal mode(OC0 disconnected), 256 scale
	TCNT0 = 256 - 250;
	TIMSK |= 0x01; // (1 << TOIE0)
}

// 4ms x 250 = 1s
ISR(TIMER0_OVF_vect) // 1) 오버플로우 통한 1초의 계산 및 단위 초기화 설정 2) 운동시간의 계산 및 단위 초기화 설정 3) 초당 칼로리 계산
{
	TCNT0 = 256 - 250;

	// calc 1(sec) + time reset
	if( ++tick0 == 250 ) { // 4ms x 250 = 1sec
		tick0 = 0;
		// each time digit reset
		if( ++current.seconds == 60 ) {
			current.seconds = 0;
			if( ++current.minute == 60 ) {
				current.minute = 0;
				if( ++current.hour == 24 )	current.hour = 0;
			}
		}
	}
	
	if( mode == START_MODE ) {
		if( ++section_tick == 250 ) {
			section_tick = 0;

			if( ++section.seconds == 60 ) {
				section.seconds = 0;
				if( ++section.minute == 60 ) {
					section.minute = 0;
					if( ++section.hour == 24 )	section.hour = 0;
				}
			}
			// 타이머 내에서 단위시간에 근거한 속도 당 칼로리 소비량
			if( velocity > 0 && velocity < 15 )		kcal += 0.105; // 1초당 .105가 축적된다
			else									kcal += 0.14;
		}
	}
}

void mcu_init(void) // mcu동작 초기화(mcu와 연관하는 각종 외부 변수와 함수들도 동시에 초기화)
{
	/*parts init */
	dcmotor_init();
	key_init();
	var_init();
	lcd_init();
	led_init();
	/* timer init */
	timer0_init();
}

void variable_init(void) // 타이머 시간과 동작모드의 초기화
{
	tick0 = 0;
	mode = NO_MODE; // 시작도 정지도 아닌 상태
}

void start_healthcare(void) // 헬스 케어 시스템을 LED, DC모터, LCD의 초기 구동과 더불어 시작한다
{
	int i;
		
	// Q1: 1) 3초동안 모터 회전, 2) 3초동안 LED전체 0.5초간 점멸, 3) LCD초기 화면 표시 및 3초간 점멸
	
	dcmotor_spin(255); // speed 255로 모터를 회전시키라(시작)

	// lcd초기 구동화면 세팅
	lcd_gotoxy(0, 0); // 1라인의 첫칸으로 가서 
	lcd_string("  Health Care   "); // 써라
	lcd_gotoxy(0, 1); // 둘째줄로 옮겨서
	lcd_string("  Bike System..."); //써라

	for( i = 0; i < 3; i++ ) { // 1초씩 3번 반복한다.
		lcd_command_write(LCD_ON); // LCD를 0.5초 동안 켜라
		//led_light(LED_ALL); // 모든 LED를 0.5초 동안 켜라
		led_light(0);
		_delay_ms(500); 
		//상태반전
		lcd_command_write(LCD_OFF); // LCD를 0.5초 동안 꺼라
		//led_light(0); // 아무 LED로 0.5초간 켜지 마라
		led_light(LED_ALL);
		_delay_ms(500);
	}
	dcmotor_spin(0); // 3초 지난후 모터 스톱
}

// 시간 설정 모드(모드 0): lcd파일 참조!
void time_setting(void)
{
	unsigned char pos, tmp; // lcd커서 위치를 1칸씩 잡아주는 변수, 임시 시간 저장변수
	unsigned char key = 0; // 시간 설정을 위한 입력키 값을 받아주기 위한 임시변수

	// 현재 시간의 초기화
	current.hour = 0;
	current.minute = 0;
	current.seconds = 0;

	lcd_command_write(LCD_CURSOR); // 커서를 표시하라는 lcd 인스트럭션 함수

	// 초기화면 표시 
	lcd_gotoxy(0, 0);
	lcd_string(" <Time Setting> ");
	lcd_gotoxy(0, 1);
	lcd_string("    00:00:00    "); // 4칸 padding(4 + "8" + 4 = 16), 가운데 8칸에서 시간을 설정한다

	lcd_gotoxy(4, 1); // '시'의 첫번째 칸으로 이동
	pos = 0; // 그 칸을 0으로 설정한다(pos값은 0~7의 좌표를 가진다)
	tmp = 0; // 시간 저장을 위한 임시변수
	
	while( 1 ) { // 폴링 방식으로 키 값을 확인하여 시간을 설정(UP_KEY/ENTER_KEY두 개로 시간을 조정 및 세팅한다)
		key = getkey(key); // 키 값과 플래그 확인
		if( key_flag ) { //			 ▒ < T i m e  S e t t i n g > ▒ 
            
            // 시간 자리수 변경
			if( key == KEY_UP ) { // ▒ ▒ [0] [1] : [3] [4] : [6] [7] ▒ ▒
				switch( pos ) {
					// Hour
					case 0 : // [0]: 0>1>2>0
						if( current.hour < 20 )	current.hour += 10;
						else						current.hour = 0; // hrs > 20 -> 0(Reset)
						lcd_data_write(current.hour / 10 + 0x30); // 0x30 in ASCII code = '0' / 시의 십의 자리만 출력
						break;
					case 1 : // [1]: 0>1>2>3>0
						if( (current.hour / 10) < 2 )	tmp = ++tmp % 10; // 01시 ~ 19시, tmp는 시의 일의 자리를 저장하는 임시변수
						else								tmp = ++tmp % 4; // 21시 ~ 23시
						current.hour = (current.hour / 10) * 10 + tmp; // 시 전체를 저장(시의 십의 자리 + 시의 일의 자리)
						lcd_data_write(tmp + 0x30); // 시의 일의 자리만 표시
						break;

					// Minute
					case 3 : // [3]
						if( current.minute < 50 )	current.minute += 10; // 1~5
						else							current.minute = 0;
						lcd_data_write(current.minute / 10 + 0x30); // 분의 시의 자리만 출력
						break;
					case 4 : // [4]
						tmp = ++tmp % 10; // temp에는 분의 일의 자리를 저장한다
						current.minute = (current.minute / 10) * 10 + tmp; // 분 전체를 저장(분의 십의 자리 + 분의 일의 자리)
						lcd_data_write(tmp + 0x30); // 분의 일의 자리만 출력
						break;

						// Second
					case 6 : // [6]
						if( current.seconds < 50 )	current.seconds += 10;
						else							current.seconds = 0;
						lcd_data_write(current.seconds / 10 + 0x30); // 초의 십의 자리만 출력
						break;
					case 7 : // [7]
						tmp = ++tmp % 10;
						current.seconds = (current.seconds / 10) * 10 + tmp; // 초 전체를 저장(초의 십의 자리 + 초의 일의 자리)
						lcd_data_write(tmp + 0x30); // 초의 일의 자리만 출력
						break;
				}

				lcd_gotoxy(pos + 4, 1); // 0x80(2번째줄 첫째라인)기준으로 해서 4씩 주소를 더하여 이동(데이터 시트 참조)
			}
            
            // 시간 자리수 변경 완료: 자릿수를 이동시킨다
			else if( key == KEY_ENTER ) {
				if( pos == 7 ) { // 만약 7번째 자리 수까지 완료 지으면, 시간 세팅 전체 완료 -> 루프 탈출 -> time_setting모드 종료 
					lcd_command_write(LCD_ON);
					return; // 루프탈출
				}
                // 자릿수 포지션이 7이 아니라면, 즉 아직 세팅할 시간이 남았다면...
				tmp = 0;
				pos++;
				if( (pos == 2) || (pos == 5) )	pos++; // ":"위치에 커서가 위치할 경우에는 자리를 한칸씩 역으로 옮겨 3과 6의 포지션으로 갈 것
				lcd_gotoxy(pos + 4, 1); // 새로운 위치값을 기준으로 한 커서 이동(현재 위치의 주소값에 4를 더한다)
			}
		}
	}
}

// 설정한 시간을 lcd에 출력하여 확인시켜주는 함수
void init_screen(void)
{
	lcd_gotoxy(0, 0); // 원점으로 가서
	lcd_string("  Health Care   "); //써라
	lcd_gotoxy(0, 1); // 다음줄로 가서
	printf("    %02d:%02d:%02d    ", current.hour, current.minute, current.seconds); // 현재 시간을 형식에 맞게 써라(화면 구동후, 인터럽트가 구동되면서 흐르기 시작하는 시간)
	
}

int main(void)
{
	unsigned char oldsec = 0xFF; // MAX: 256
	unsigned char key = 0; // 어떠한 스위치의 입력도 받지 않은 상태
	
	/* constructor */
	mcu_init(); //각 동작의 초기화
	variable_init(); // 시간(타이머)와 모드의 초기화(어떠한 모드도 아님:default)
	/* End of constructor*/
	
	fdevopen((void *)lcd_data_write, 0); //pritnf()를 LCD에 사용할 수 있도록 열어준다(기능의 추가/인터페이스)

	start_healthcare();// 헬스케어 초기화면으로 전환
	time_setting(); // 스위치의 입력이 발생하는 경우, 시간을 설정하는 모드로 진입
	init_screen(); // 설정한 시간을 화면에 표시(설정한 시간이 비로소 흐르기 시작한다)-> 일반적인 시계를 생각할 것(세팅하기 전에도 시간은 흐른다)

	sei(); // 인터럽트가 허용되면서 타이머가 돌아가기 시작할 것이다.

	while( 1 ) { // 폴링방식, 스위치 입력검사, 해당명령 수행
		key = getkey(key); // 키 값을 읽음("키 값을 주고 키 값을 읽음", 초기 상태는 0) -> switch구문에서 서로 다른 키 값을 받기 위해서 설정
		if( key_flag ) { // 성공적 키 입력의 확인 플래그 체크(이전 키 값과 다른 키 값이 들어오면)
			switch( key ) { // (그제서야)키 입력에 따른 명령 분리 -> (1)키 값에 따른 제어, (2) 제어 상황에 따른 LCD출력
				// 시작 키
				case KEY_START :
					if( mode == NO_MODE ) { // 현재 상태가 시작도 정지도 아닌 (초기) 모드라면
						// 시작모드부터 사용될 모든 시간 변수와 이와 관련하는 칼로리 값(단위시간당 운동 소모칼로리 이므로)을 초기화 한다
						section.hour = 0;
						section.minute = 0;
						section.seconds = 0;
						section_tick = 0;
						kcal = 0.;
						// 초기화가 끝나면
						lcd_gotoxy(0, 0); // 커서를 원점으로 옮겨서
						lcd_string("Velocity:   Km/H"); // 초기 속도 화면을 lcd에 뿌려준다

						mode = START_MODE; // 이제서야, 시작모드로 진입하고, 스위치 구문을 빠져 나가서 다음 구문으로 분기한다
					}
					break;
				// 정지 키
				case KEY_STOP :
					if( mode == START_MODE ) { // 현재 시작 모드 일때 의미가 있는 키 이므로, 현재 시작 모드인지 아닌지를 체크한다
						// 정지키 쿨렀을때 1) 운동시간과 2) 소모 칼로리량이 표시된다
						
						lcd_gotoxy(0, 0); //lcd 첫째라인의 출력
						printf("Time    %02d:%02d:%02d", section.hour, section.minute, section.seconds); // 운동 소모 시간
						

						lcd_gotoxy(0, 1); // lcd둘째 라인의 출력
						printf("Calorie %4.1fKcal", kcal); // 칼로리(ISR내에서 1초당 누적적으로 이루어짐)
						
						mode = STOP_MODE; // 정지키의 입력에 따른 출력을 수행하고 종료되면, 정지모드가 되면서 스위치 구문을 빠져나간다(할거 다하고 빠져나간다)
					}
					break;
				// 선택 키
				case KEY_ENTER : // 3) 선택키
					if( mode == STOP_MODE ) { // 현재 모드가 정지모드라면
						init_screen(); //초기 모드로 진입한다
						mode = NO_MODE;
					}
					break;
			}
		}
		// 시작모드에서의 진행
		// 스위치 구문을 빠져나온 상황에서, 
		// 시작모드라면(상단 북마크 참조), 
		if( mode == START_MODE ) { // 시작 모드에서 속도 조절시 실시간 상황 업데이트
	
		// 가변저항 입력값을 읽어 모터의 속도조절 및 LCD에 상황표시
		velocity = (unsigned char)(var_read() / 20.4); // 가변저항 변화에 따른 ADC값을 읽어옴
		dcmotor_spin(velocity); // 읽어온 값을 모터 회전 값으로 전달

		lcd_gotoxy(10, 0); // 첫줄 10번째 칸으로 가서 업데이트 된 속도를 출력
		printf("%2d", velocity); // 정수형 2칸
		

		led_light((unsigned char)(8 - (velocity / 6.2))); // led값으로도 속도상황을 출력(단, LED의 갯수를 고려하여 계산한 변수를 전달)
		}
		
		// 실시간 시간의 표시
		// oldsec의 초기값과 최대값은 0xFF(=256)
		if( oldsec |= current.seconds ) { // 현재 시점의 시간과 oldsec시간이 다르다면
			oldsec = current.seconds; // oldsec을 지속적으로 while안에서 업데이트하고 
			if( mode != STOP_MODE ) { // stop모드가 아닌, 초기모드나 시작모드라면, (*정지모드에서는 section time이 표시된다)
				lcd_gotoxy(0, 1); // 두번째 줄로 가서
				printf("    %02d:%02d:%02d    ",
						current.hour, current.minute, current.seconds); // 시간을 계속 표시해라
			}
		}
	}
	return 0;
}
