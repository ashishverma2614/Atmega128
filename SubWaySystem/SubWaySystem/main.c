// start scrfeen -> Main Screen -> Select Class -> Select start/end station -> Calc fare

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include <stdio.h>

#include "key.h"
#include "fnd.h"
#include "var.h"
#include "lcd.h"
#include "serial.h"
#include "led.h"

#define ON		1
#define OFF		0

#define FORWARD		0
#define REWARD		1

enum { // 계층별 요금상수
	NONE, // 선택이전
	ADULT, // 어른
	KID // 꿈나무
};

// 프로그램 구동순서(모드)의 상수화
enum { // 모드 선택을 위한 enum상수 -> 상수의 번호순서로 프로그램이 실행된다
	SELECT_TICKET_TYPE, // 티켓 선택
	SELECT_START_STATION,// 출발역 선택
	SELECT_END_STATION,// 종착역 선택
	CALCULATE_FEE,// 요금 계산
	TRAIN_GO// 열차 출발
};

extern volatile char key_flag; // 키입력 감지 플래그
extern volatile char fnd_flag; // fnd 입력 플래그
extern volatile unsigned char fnd_buf[2]; // fnd출력 값 임시 변수
extern volatile int money; // 계산된 요금을 저장하는 임시 변수

// LED로 표현된 역 이름의 int값
// 2(Gnagnam St.)->3->4->5->9->13->17->16->12->8->7->11->15->14->10->6
unsigned int led_order[16] = {
	0x0001, 0x0002, 0x0004, 0x0008, 0x0080, 0x0800, 0x8000, 0x4000,
	0x0400, 0x0040, 0x0020, 0x0200, 0x2000, 0x1000, 0x0100, 0x0010
};
volatile char state;// 선택 모드를 체크하는 임시변수(enum의 모드 변수가 들어감)
volatile char ticket_type, direction; // 티켓 형태, 방향(정/역)
volatile int start_station, end_station, distance; // 출발역과 도착역을 기반으로 하여, 정방향/역방향 기준으로 거리가 산정
volatile int fee; // 계산된 요금을 저장하는 임시변수
int fee_tbl[2] = { 300, 500 }; // 요금 계산 테이블(꿈나무, 성인)

/* MCU구동 초기화 */
void mcu_init(void)
{
    //각 헤더 파일에서 설정된 변수와 설정을 초기화 한다
	key_init();
	fnd_init();
	var_init();
	lcd_init();
	serial_init(B9600); // 시리얼 헤더에서 설정(시리얼 통신의 초기화는 보드레이트 설정)
	matrix_init();
}

// main에서 선언된 변수(file scope declaration)의 초기화 및 fnd off
void variable_init(void)
{
	fnd_off();

	// 요금 결정 이전상태
	ticket_type = NONE;
	start_station = NONE;
	end_station = NONE;

	distance = 0;
	fee = 0;
}

// start_screen 이후에 이어서 구동되는 함수(start_screen함수의 마지막에서 호출됨)
void main_screen(void)
{
	fnd_off(); // fnd끄고
	matrix_alloff(); // LED끄고

	// LCD에 환영 인사를 띄운다
	lcd_gotoxy(0, 0);
	lcd_string(" Select Button! ");
	lcd_gotoxy(0, 1);
	lcd_string("S1:Adult S2:Kid");
}

void start_screen(void) // initial screen menu without nothing, preceding main screen 
{
	/* NONE상태에서 최초로 구동되는 화면(default) */
	int i;

	lcd_string("   Welcome To   ");
	lcd_gotoxy(0, 1); // go line2
	lcd_string("Gangnam Station!"); // current station

    // LEDdml 0.3초 간격 순차 점멸(LED1→LED2→LED3→LED4→LED8→LED12→LED16→LED15
    //				→LED11→LED7→LED6→LED10→LED14→LED13→LED9→LED5), FND는 소등한다(main_screen함수에 포함)
    for( i = 0; i < 16; i++ ) {
        matrix_led(*(led_order + i));
        _delay_ms(300);
	}	

	/* Stage over to next mode*/
	main_screen(); // By this function, all of the blinking led are to be off.
}

// 역선택 메뉴
void select_station(char keyin) // station select mode
{
	char key = keyin; // 이전 단계에서 키를 받으면 지하철 역 선택 모드로 넘어온다
	int station;
	_delay_ms(500); // .5초의 지연시간(메뉴얼 참조)

	start_station = 1; // 초기 값(강남역)
	station = start_station; // 출발역을 임시변수에 대입

	distance = 0; // 종착역에 설정되지 않았으므로 거리는 0
	fee = 0; // 차비도 0

	// 역 선택 입력을 받기 위한 화면을 출력한다
	lcd_gotoxy(0, 0);
	lcd_string(" Start Station  ");
	lcd_gotoxy(0, 1);
	lcd_string("Station Num : ");
///////////////////////////////////////////////////////////////////////////////
	while( 1 ) { 
		key = getkey(key); // 정방향(S1)또는 역방향(S2)를 눌러 역 선택
		if( key_flag ) { // 정상적 키 입력의 확인
			switch( key ) { // 키 입력 분류
				case KEY_1 : // 정방향 이동키
                    station++;
                    if (station == 17) station = 1; // 17번째 역은 없으므로 처음으로 돌아간다
                    break;

				case KEY_2 : // 역방향 이동키
                    station--;
                    if (station == 0) station = 16; // 0번쨰 역은 없으므로 마지막으로 돌아간다
                    break;

				case KEY_3 : // (출발)역 선택키
					if( state == SELECT_START_STATION )
					{ // 출발역 결정 모드
						start_station = station; // 출발역 결정
						end_station = 1; // 도착역 설정을 위한 도착역의 초기화
						station = end_station; // 임시 변수에 도착역 초기값 대입

						// 도착역 입력을 받기 위한 사용자 인터페이스 화면 출력
						lcd_gotoxy(0, 0);
						lcd_string(" End   Station  ");

						state = SELECT_END_STATION; // 도착역 모드 진입(도착역 키 값을 다시 받으러 간다)
					}
					else if( state == SELECT_END_STATION ) 
					{ // 도착역 결정모드
						end_station = station;

						// 출도착 역 확인 출력
						lcd_gotoxy(0, 0);
						printf("Start:%02d Station", start_station);
						lcd_gotoxy(0, 1);
						printf("End  :%02d Station", end_station);

						// 출도착역의 LED확인 
						matrix_led(led_order[start_station - 1] | led_order[end_station - 1]);
						_delay_ms(500);

					// 예외처리(exception handling)
					if( start_station == end_station ) 
					{
						lcd_gotoxy(0, 0);
						lcd_string("    Error!!     ");
						lcd_gotoxy(0, 1);
						lcd_string("Return To First!"); // it implies that selecting station is not the first process
						_delay_ms(500);

						// 오류가 생기면 출/도착 역이 초기화 된다
						start_station = NONE;
						end_station = NONE;
						// 메인 화면이 (다시) 호출된다
						main_screen();

						state = SELECT_TICKET_TYPE; // 티켓 선택 모드로 다시 진입한다
					} // 오류가 일어난 것이 아니라면, 요금계산모드로 진입한다
					else { // calc optimized fare based on shortest distance
						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//	[문제 3] 출발역과 도착역 선택 후 최단거리와 요금을 계산하여 LCD에 표시
						//		가. LCD
						//			거리 : 출발역과 도착역 간의 최단 거리를 계산하여 표시
						//			요금 : Adult = 500 X 거리, Kid = 300 X 거리
						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						distance = end_station - start_station;
						if( distance < 0 ) 
                        {
                            distance *= (-1); //거리의 절댓값
                            if (distance < 8)
                            {
                                direction = REWARD;
                            }
                            else
                            {
                                if (distance == FORWARD)
                                distance = 16 - distance;
                            }
						}
						else {
							if( distance > 8 ) {
                                direction = FORWARD;
                                distance = 16 - distance;
							}
							else 
                            {
                                distance = FORWARD;
							}
						}

						// 거리값 출력
						lcd_gotoxy(0, 0);
                        printf("Distance:%d      ", distance);
                        // 요금표에 따른 요금 계산
                        fee = fee_tbl[ticket_type % 2] * distance;
						lcd_gotoxy(0, 1);
                        //요금 인쇄
                        printf("Fee : %04d Won  ", fee);
                        
						_delay_ms(1000); // 다음 화면으로 넘어가기 위한 1초의 지연시간
                        // 지불요구 및 지불요금 출력
						lcd_gotoxy(0, 0);
						lcd_string("Insert Money!!  ");
						lcd_gotoxy(0, 1);
						printf("Fee:%04d-0000   ", fee);

						state = CALCULATE_FEE;
					}
					return;
				}
				break;
                
                //exception handling
				case KEY_5 : // watch dog(비정상적 행동의 감지시 reset시킴)
				wdt_enable(WDTO_30MS);
				while( 1 );
				break;
			}
		}
		// display end station to remind
		matrix_led(led_order[station - 1]);
		lcd_gotoxy(14, 1);
		printf("%02d", station);
	}
}

void train_go(void)
{
	int i, pos = start_station - 1;
	char str[30];

	lcd_gotoxy(0, 0);
	lcd_string("Train Is Going..");

	fnd_flag = ON; // 1, toggle fnd flag(fnd가 켜지는 모드임을 알림)
	for( i = 0; i <= distance; i++ ) {
		matrix_led(led_order[pos]); // 현재 위치를 led로 표시

		if( i < distance ) { // 전철이 이동하는 상태: 남은 시간표시
			lcd_gotoxy(0, 1);
			if( direction == FORWARD )	printf("----> : %d Minutes", distance - i); // 정방향
			else					printf("<---- : %d Minutes", distance - i); // 역방향
		}
		else { // 전철이 도착해 있거나 지나간 상태
			lcd_gotoxy(0, 0);
			lcd_string("Train Is Arrived");
			lcd_gotoxy(0, 1);
			lcd_string(" Thanks A Lot!  ");
		}
        // fnd에 현재 지나가고 있는 역과 현재역과 남은 역 사이의 거리를 (실시간으로) 표시한다
		fnd_buf[0] = i; // 현재 위치
		fnd_buf[1] = distance - i; // 남은거리
        
        // 제반 운행 정보를 (관리자의 입장에서) 실시간으로 시리얼 모니터로 받아본다(문항지에서 제시하는 양삭에 맞게)
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	[문제 5] 통신
		//		출력 양식에 맞춰서 실시간으로 PC 터미널에 출력하시오.
		//		(1)이동시간, (2)남은거리, (3)출발역, 도착역, 현재역, 요금을 터미널에 출력
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		serial_transmit('\f'); // form feed:새로운 화면에서 출력시작
        sprintf(str, "Time/m        : %d\r\n", distance - i); // 남은거리(현재 위치를 기준으로)
        serial_string(str);
        sprintf(str, "Distance      : %d\r\n", i); // 현재 위치
        serial_string(str);
        sprintf(str, "Start Station : %02d\r\n", start_station); // 출발역
        serial_string(str);
        sprintf(str, "End Station   : %02d\r\n", end_station); // 도착역
        serial_string(str);
        sprintf(str, "Now Station   : %02d\r\n", pos + 1); // 곧 도착하는 역(현재 위치 + 1)
        serial_string(str);
        sprintf(str, "Money         : %04d\r\n", fee); // 요금
        serial_string(str);
		
        // 변화하는 위치의 업데이트
        if( direction == FORWARD ) { // 정방향일 경우
			if( pos < 15 )	pos++;
			else			pos = 0;
		}
		else { // 역방향일 경우
			if( pos )	pos--;
			else		pos = 15;
		}

		_delay_ms(1000);
	}
	_delay_ms(1000);
    // i = distance가 되어 루프를 탈출한경우(즉, 거리만큼 이동해서 도착역이 현재역이 되어 도착한 경우)
	// 제반 요소들을 초기화 한다
    ticket_type = NONE;
	start_station = NONE;
	end_station = NONE;

	distance = 0;
	fee = 0;

	main_screen();
    // 티켓 선택 모드로 다시 진입
	state = SELECT_TICKET_TYPE;
}

int main(void) // main method: initialization + loop
{
	unsigned char key = 0xFF; // all of the SW are off(pull-down resist)

	mcu_init();
	variable_init();

	sei();

	fdevopen((void *)lcd_data_write, 0); // LCD쓰기 허용

	start_screen(); // main_screen 함수가 내부에서 호출되고 있음
	state = SELECT_TICKET_TYPE; // first menu

	var_start(); // 가변저항 입력값의 초기화 

	while( 1 ) {
		key = getkey(key); // 키 입력을 읽어옴
		if( key_flag ) { // 온전하게 키 입력을 받은 것인지 확인한다
			switch( key ) { // 키 값을 분류한다
				case KEY_1 :
				case KEY_2 : // key1이나 2를 누르면, 티켓 선택 모드로 진입한다
				if( state == SELECT_TICKET_TYPE ) { // 모드에 진입했다면
					state = SELECT_START_STATION; // 다음 모드(출발역 선택)로 미리 변경해 두고

					//선택 모드에서 받은 키 값에 따른 결과를 출력한다
					lcd_command_write(0x01);
					lcd_gotoxy(0, 0);
					if( key == KEY_1 )		lcd_string("You Select Adult");
					else if( key == KEY_2 )	lcd_string("You Select Kid  ");

					ticket_type = key; // 현재 받은 키 값(성인인지 꿈나무인지)을 '티켓타입'변수에 저장한다
					select_station(key); // key1 또는 key2의 값을 받으면, 티켓 설정을 완료하고 "역 선택 모드를 호출한다"
				}
				break;

				case KEY_3 :
				if( state == CALCULATE_FEE ) {
					if( money >= fee ) {
						if( money > fee ) {
							lcd_gotoxy(0, 0);
							lcd_string("Here Is Change. ");
							lcd_gotoxy(0, 1);
							printf("Change : %04d   ", money - fee);
						}
						_delay_ms(1000);
						// 요금 결정 및 거스름을 받고 출발모드(TRAIN_GO)로 들어간다

						lcd_gotoxy(0, 0);
						lcd_string("  [S4] Go!!     ");
						lcd_gotoxy(0, 1);
						printf("  Distance : %d  ", distance); // 거리 표시

						state = TRAIN_GO;
					}
				}
				break;

				case KEY_4 :
				if( state == TRAIN_GO )	train_go();
				break;

				case KEY_5 :
				wdt_enable(WDTO_30MS);
				while( 1 );
				break;
			}
		}

		// CALCULATE_FEE모드에서의 요금 결정(ADC 사용): 아날로그 값을 변환해 읽어서 요금 결정
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	[문제 4] 가변저항을 이용하여 지불 요금  표시
		//		1. VR1의 ADC 입력값을 지불 금액으로 LCD에 표시ㅣ
		//		2. 입력된 금액을 요금과 비교하여 크거나 같으면 'OK'라고 표시하고
		//			그 전에는 "공백"이 되게 하시오.
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if( state == CALCULATE_FEE ) // 요금 표시 모드
        {
            lcd_gotoxy(9, 1);
            printf("%04d   ", money);
			if( money >= fee ) //fee는 가변저항의 업데이트 되는 값으로 표현된다
            {
                lcd_gotoxy(14, 1); // 요금 확인 이전에[14:16]은 공백으로 표시된다
                lcd_string  ("OK"); // 요금 확인
			}
		}
	}
	return 0;
}

