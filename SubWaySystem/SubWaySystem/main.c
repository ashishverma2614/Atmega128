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

// 프로그램 구동순서의 상수화
enum { // 모드 선택을 위한 enum상수 -> 상수의 번호순서로 프로그램이 실행된다
	SELECT_TICKET_TYPE, // 티켓 선택
	SELECT_START_STATION,// 출발역 선택
	SELECT_END_STATION,// 종착역 선택
	CALCULATE_FEE,// 요금 계산
	TRAIN_GO// 열차 출발
};

extern volatile char key_flag; // 키입력 감지 플래그
extern volatile char fnd_flag;
extern volatile unsigned char fnd_buf[2]; // fnd에 출력되는 값을 임시로 저장
extern volatile int money; // 계산된 요금을 저장하는 임시 변수

// LED로 표현된 역 이름의 int값
// 2(Gnagnam St.)->3->4->5->9->13->17->16->12->8->7->11->15->14->10->6
unsigned int led_order[16] = {
	0x0001, 0x0002, 0x0004, 0x0008, 0x0080, 0x0800, 0x8000, 0x4000,
	0x0400, 0x0040, 0x0020, 0x0200, 0x2000, 0x1000, 0x0100, 0x0010
};
volatile char state;// 선택 모드를 넣어두는 임시변수
volatile char ticket_type, direction; // 티켓 형태, 방향(정/역)
volatile int start_station, end_station, distance;
volatile int fee;
int fee_tbl[2] = { 300, 500 }; // 요금 계산 테이블(꿈나무, 성인)

/* MCU구동 초기화 */
void mcu_init(void)
{
    //각 헤더 파일에서 설정된 변수와 설정을 초기화 한다
	key_init();
	fnd_init();
	var_init();
	lcd_init();
	serial_init(B9600); // 시리얼 헤더에서 설정
	matrix_init();
}

// main에서 선언된 변수의 초기화 및 fnd off
void variable_init(void) // variable constructor
{
	fnd_off();

	ticket_type = NONE;
	start_station = NONE;
	end_station = NONE;

	distance = 0;
	fee = 0;
}

// 사용자용 LCD의 초기 화면
void main_screen(void)
{
	fnd_off(); // fnd끄고
	matrix_alloff(); // LED끄고

	// LCD에 환영 인사를 띄운다
	lcd_gotoxy(0, 0);
	lcd_string(" Select Button! ");
	lcd_gotoxy(0, 1);
	lcd_string("S1:Adult  S2:Kid");
}

void start_screen(void) // initial screen menu without nothing, preceding main screen 
{
	/* Test System */
	int i;

	lcd_string("   Welcome To   ");
	lcd_gotoxy(0, 1); // go line2
	lcd_string("Gangnam Station!"); // current station

    // LEDdml 0.3초 간격 순차 점멸(LED1→LED2→LED3→LED4→LED8→LED12→LED16→LED15
    //				→LED11→LED7→LED6→LED10→LED14→LED13→LED9→LED5)    
    for( i = 0; i < 16; i++ ) {
        matrix_led(*(led_order + i));
        _delay_ms(300);
	}	

	/* Stage over to next mode*/
	main_screen(); // By this function, all of the blinking led are to be off.
}

void select_station(char keyin) // station select mode
{
	char key = keyin; // receive adult/kid key for calculating fare
	int station;
	_delay_ms(500);

	start_station = 1; // set start station
	station = start_station; // substitution

	distance = 0; // set distance
	fee = 0; // set fare

	// display initial LCD
	lcd_gotoxy(0, 0);
	lcd_string(" Start Station  ");
	lcd_gotoxy(0, 1);
	lcd_string("Station Num : ");

	while( 1 ) { 
		key = getkey(key); // get "station moving"
		if( key_flag ) { // check key flag
			switch( key ) { // according to each key input: key1(move+), key2(move-), key3(select)
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				//	[문제 2] 스위치를 이용하여 LED를 정방향 또는 역방향으로 이동
				//		나. LED
				//			SW1를 1번 누를 때마다 1정거장씩 정방향으로 이동되고,
				//			SW2을 1번 누를 때마다 1정거장씩 역방향으로 이동되게 하시오.
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				case KEY_1 : // move to forward direction
                    station++;
                    if (station == 17) station = 1;
                    break;

				case KEY_2 : // move to reverse direction
                    station--;
                    if (station == 0) station = 16;
                    break;

				case KEY_3 : // select station
				if( state == SELECT_START_STATION ) { // 1: case of selecting start station
					start_station = station;
					end_station = 1;
					station = end_station;

					lcd_gotoxy(0, 0);
					lcd_string(" End   Station  ");

					state = SELECT_END_STATION; // move to selecting end station
				}
				else if( state == SELECT_END_STATION ) { // 2: case of selecting end station
					end_station = station;

					// display chosen result: start/end station
					lcd_gotoxy(0, 0);
					printf("Start:%02d Station", start_station);
					lcd_gotoxy(0, 1);
					printf("End  :%02d Station", end_station);

					// blink led start and end station 
					matrix_led(led_order[start_station - 1] | led_order[end_station - 1]);

					_delay_ms(500);

					// exception handling!
					if( start_station == end_station ) {
						lcd_gotoxy(0, 0);
						lcd_string("    Error!!     ");
						lcd_gotoxy(0, 1);
						lcd_string("Return To First!"); // it implies that selecting station is not the first process
						_delay_ms(500);

						
						start_station = NONE;
						end_station = NONE;

						main_screen();

						state = SELECT_TICKET_TYPE; // ticket type mode -> station select mode -> fare calculation
					}
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

				case KEY_5 : // watch dog
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

	fnd_flag = ON; // 1, toggle fnd flag
	for( i = 0; i <= distance; i++ ) {
		matrix_led(led_order[pos]); // blink start station

		if( i < distance ) {
			lcd_gotoxy(0, 1);
			if( direction == FORWARD )	printf("----> : %d Minutes", distance - i);
			else					printf("<---- : %d Minutes", distance - i);
		}
		else {
			lcd_gotoxy(0, 0);
			lcd_string("Train Is Arrived");
			lcd_gotoxy(0, 1);
			lcd_string(" Thanks A Lot!  ");
		}

		fnd_buf[0] = i; // current position
		fnd_buf[1] = distance - i; // remaining distance

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	[문제 5] 통신
		//		출력 양식에 맞춰서 실시간으로 PC 터미널에 출력하시오.
		//		(1)이동시간, (2)남은거리, (3)출발역, 도착역, 현재역, 요금을 터미널에 출력
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		serial_transmit('\f'); // 커서를 한 페이지 이동한다(form feed)
        sprintf(str, "Time/m        : %d\r\n", distance - i);
        serial_string(str);
        sprintf(str, "Distance      : %d\r\n", i);
        serial_string(str);
        sprintf(str, "Start Station : %02d\r\n", start_station);
        serial_string(str);
        sprintf(str, "End Station   : %02d\r\n", end_station);
        serial_string(str);
        sprintf(str, "Now Station   : %02d\r\n", pos + 1);
        serial_string(str);
        sprintf(str, "Money         : %04d\r\n", fee);
        serial_string(str);
		
        if( direction == FORWARD ) {
			if( pos < 15 )	pos++;
			else			pos = 0;
		}
		else {
			if( pos )	pos--;
			else		pos = 15;
		}

		_delay_ms(1000);
	}
	_delay_ms(1000);

	ticket_type = NONE;
	start_station = NONE;
	end_station = NONE;

	distance = 0;
	fee = 0;

	main_screen();

	state = SELECT_TICKET_TYPE;
}

int main(void) // main method: initialization + loop
{
	unsigned char key = 0xFF; // all of the SW are off(pull-down resist)

	mcu_init();
	variable_init();

	sei();

	fdevopen((void *)lcd_data_write, 0);

	start_screen();
	state = SELECT_TICKET_TYPE; // first menu

	var_start();

	while( 1 ) {
		key = getkey(key);
		if( key_flag ) {
			switch( key ) {
				case KEY_1 :
				case KEY_2 : // key1, 2 for age menu select
				if( state == SELECT_TICKET_TYPE ) { // ticket type select -> select station
					state = SELECT_START_STATION;

					lcd_command_write(0x01);
					lcd_gotoxy(0, 0);
					if( key == KEY_1 )		lcd_string("You Select Adult");
					else if( key == KEY_2 )	lcd_string("You Select Kid  ");

					ticket_type = key;
					select_station(key); // enter select station mode
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

						lcd_gotoxy(0, 0);
						lcd_string("  [S4] Go!!     ");
						lcd_gotoxy(0, 1);
						printf("  Distance : %d  ", distance);

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

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//	[문제 4] 가변저항을 이용하여 지불 요금  표시
		//		1. VR1의 ADC 입력값을 지불 금액으로 LCD에 표시ㅣ
		//		2. 입력된 금액을 요금과 비교하여 크거나 같으면 'OK'라고 표시하고
		//			그 전에는 공백이 되게 하시오.
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if( state == CALCULATE_FEE ) 
        {
            lcd_gotoxy(9, 1);
            printf("%04d   ", money);
			if( money >= fee ) 
            {
                lcd_gotoxy(14, 1);
                lcd_string  ("OK");
			}
		}
	}
	return 0;
}

