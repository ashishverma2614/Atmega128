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

// busy flag점검
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


unsigned char lcd_command_read(void) // 거의 쓸 일이 없다
{
	char cmd;

	LCD_DDR = 0x0F;
	LCD_OUT = 0x00;

	LCD_OUT |= LCD_RW;

	LCD_OUT |= LCD_E; // H
	asm	("nop");
	cmd = LCD_IN & 0xF0;
	LCD_OUT &= ~LCD_E; // L

	LCD_OUT |= LCD_E; // H
	asm	("nop");
	cmd |= LCD_IN >> 4;
	LCD_OUT &= ~LCD_E; // L

	return cmd;
}

// LCD에 명령어(CMD) 전달
// LCD에 데이터 전달(4비트 모드 이므로 H/L을 두 번 나누어서 전달한다)
// 명령어를 전달할 때는 포트 Enable신호가 H - > L사이에 전달된다(데이터 시트 참조)
void lcd_command_write(unsigned char cmd)
{
	lcd_busycheck(); // 잠깐의 지연시간(busy check)

	LCD_OUT = 0x00; // 포트를 끈다

	LCD_OUT |= cmd & 0xF0; // 명령어의 상위 바이트 마스킹
	LCD_OUT |= LCD_E; // H
	LCD_OUT &= ~LCD_E; // L

	LCD_OUT &= 0x0F;
	LCD_OUT |= cmd << 4; // 명령어의 하위 바이트 마스킹
	LCD_OUT |= LCD_E; // H
	LCD_OUT &= ~LCD_E; // L

	_delay_ms(2);
}


unsigned char lcd_data_read(void) // 쓸일이 없음
{
	char data;

	LCD_DDR = 0x0F;
	LCD_OUT = 0x00;

	LCD_OUT |= LCD_RS;
	LCD_OUT |= LCD_RW;
	//상위 바이트 전달
	LCD_OUT |= LCD_E; // 0b0000_0100(H)
	asm	("nop");
	data = LCD_IN & 0xF0;
	LCD_OUT &= ~LCD_E; // 0b1111_1011 (L)

	// 하위 바이트 전달
	LCD_OUT |= LCD_E; // H
	asm	("nop");
	data |= LCD_IN >> 4;
	LCD_OUT &= ~LCD_E; // L

	return data;
}

// LCD에 1바이트(한글자)를 씀
void lcd_data_write(unsigned char data)
{
	LCD_OUT = 0x00;

	LCD_OUT |= LCD_RS; // RS = 1, 데이터 쓰기 모드

	// 상위 바이트 데이터 전달
	LCD_OUT |= data & 0xF0;
	// 인에이블 신호 변화
	LCD_OUT |= LCD_E;
	LCD_OUT &= ~LCD_E;

	// 하위 바이트 데이터 전달
	LCD_OUT &= 0x0F;
	LCD_OUT |= data << 4;
	// 인에이블 신호 변환
	LCD_OUT |= LCD_E;
	LCD_OUT &= ~LCD_E;

	_delay_us(50);
}

// LCD에 여러 글자를 씀: lcd_data_write를 반복해서 호출하는 방식으로 문자열을 전달한다
void lcd_string(char *str)
{
	while( *str )	lcd_data_write(*str++);
	// while(str[i] != '\0') lcd_data_write(str[i++]);
}

// LCD의 커서 좌표 수정: DDRAM에 커서를 나타내는 주소를 수정한다
// 0x40은 lcd원점의 주소
void lcd_gotoxy(char x, char y)
{
	lcd_command_write(DDRAM | (0x40 * y) | x);
}

// LCD 초기화(데이터 시트 참조)
void lcd_init(void)
{
	LCD_DDR = 0xFF;

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
