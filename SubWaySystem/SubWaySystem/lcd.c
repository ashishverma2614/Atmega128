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

// busy flag����
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

// LCD�� ��ɾ�(CMD) ����
void lcd_command_write(unsigned char cmd)
{
	lcd_busycheck();

	LCD_OUT = 0x00;

	LCD_OUT |= cmd & 0xF0;
	LCD_OUT |= LCD_E;
	LCD_OUT &= ~LCD_E;

	LCD_OUT &= 0x0F;
	LCD_OUT |= cmd << 4;
	LCD_OUT |= LCD_E;
	LCD_OUT &= ~LCD_E;

	_delay_ms(2);
}

// LCD�� ������ ����
unsigned char lcd_data_read(void)
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

// LCD�� 1����Ʈ(�ѱ���)�� ��
void lcd_data_write(unsigned char data)
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

// LCD�� ���� ���ڸ� ��
void lcd_string(char *str)
{
	while( *str )	lcd_data_write(*str++);
}

// LCD�� Ŀ�� ��ǥ ����
void lcd_gotoxy(char x, char y)
{
	lcd_command_write(DDRAM | (0x40 * y) | x);
}

// LCD �ʱ�ȭ
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
