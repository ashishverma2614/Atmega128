#include <avr/io.h>
#include <util/delay.h>

#include "lcd.h"


#define LCD_OUT		PORTC
#define LCD_IN		PINC
#define LCD_DDR		DDRC

#define LCD_RS		0x01
#define LCD_RW		0x02
#define LCD_E		0x04

#define DDRAM		0x80


// busy flag check 함수
void lcd_busycheck(void)
{
	LCD_DDR = 0x0F;

	LCD_OUT = 0x00;
	LCD_OUT |= LCD_RW;
	LCD_OUT |= LCD_E;
	while( LCD_IN & 0x80 );
	LCD_OUT &= ~LCD_E;

	LCD_DDR = 0xFF;
}

// 명령어 읽기 함수(NC)
unsigned char lcd_command_read(void)
{
	char cmd;

	LCD_DDR = 0x0F;
	LCD_OUT = 0x00;

	LCD_OUT |= LCD_RW;

	LCD_OUT |= LCD_E;
	asm	("nop");
	cmd = LCD_IN & 0xF0;
	LCD_OUT &= ~LCD_E;

	LCD_OUT |= LCD_E;
	asm	("nop");
	cmd |= LCD_IN >> 4;
	LCD_OUT &= ~LCD_E;

	return cmd;
}

// 명령어 전달함수
void lcd_command_write(unsigned char cmd)
{
	lcd_busycheck();

	LCD_OUT = 0x00;
	// upper NIBL
	LCD_OUT |= cmd & 0xF0;
	LCD_OUT |= LCD_E;
	LCD_OUT &= ~LCD_E;
	// lower NIBL
	LCD_OUT &= 0x0F;
	LCD_OUT |= cmd << 4;
	LCD_OUT |= LCD_E;
	LCD_OUT &= ~LCD_E;

	_delay_ms(2);
}
// 1바이트 데이터 읽어오기(4비트 모드)
unsigned char lcd_data_read(void) // 하단의 출력함수와 더불어서 함수 순서 구성이 올바른지 확인할 것
{
	char data;

	LCD_DDR = 0x0F;
	LCD_OUT = 0x00;

	LCD_OUT |= LCD_RS;
	LCD_OUT |= LCD_RW;

	LCD_OUT |= LCD_E;
	asm	("nop");
	data = LCD_IN & 0xF0;
	LCD_OUT &= ~LCD_E;

	LCD_OUT |= LCD_E;
	asm	("nop");
	data |= LCD_IN >> 4;
	LCD_OUT &= ~LCD_E;

	return data;
}

// 1바이트 데이터 쓰기(4비트 모드)
void lcd_data_write(unsigned char data) // 올바르게 설정 되었는지 비교해서 확인할 것(현재 쓰레기 값 표현 됨)
{
	LCD_OUT = 0x00;

	LCD_OUT |= LCD_RS;

	LCD_OUT |= data & 0xF0;
	LCD_OUT |= LCD_E;
	LCD_OUT &= ~LCD_E;

	LCD_OUT &= 0x0F;
	LCD_OUT |= data << 4;
	LCD_OUT |= LCD_E;
	LCD_OUT &= ~LCD_E;

	_delay_us(50);
} 

// 문자열 출력(1바이트 데이터 쓰기 함수 내부호출)
void lcd_string(char *str)
{
	while( *str )	lcd_data_write(*str++); // 문자열에 0(널문자)이 발견되는 순간 루프 빠져나와서 데이터를 씀
}

// 커서 이동함수(0x40: 첫번째 라인의 첫번째 칸 주소값, 0x80: 둘째 라인 첫번째 칸) -> 포인터 변수 이동값으로 생각
void lcd_gotoxy(char x, char y) // set cursor(y = 1, 2)-> 0x40(0, 0), 0x80(0, 1)
{
	lcd_command_write(DDRAM | (0x40 * y) | x); // same as DDRAM + (0x40 * y) + x
}

// lcd초기화 
void lcd_init(void)
{
	LCD_DDR = 0xFF; // lcd 포트 출력설정

	lcd_command_write(0x20);
	_delay_ms(10);
	lcd_command_write(0x20);
	_delay_ms(10);
	lcd_command_write(0x20);
	_delay_ms(10);
	lcd_command_write(0x28);
	lcd_command_write(0x08);
	lcd_command_write(0x01);
	lcd_command_write(0x06);
	_delay_ms(10);
	lcd_command_write(0x0C);
}