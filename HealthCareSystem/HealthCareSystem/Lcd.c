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


// busy flag check �Լ�
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

// ��ɾ� �б� �Լ�(NC)
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

// ��ɾ� �����Լ�
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
// 1����Ʈ ������ �о����(4��Ʈ ���)
unsigned char lcd_data_read(void) // �ϴ��� ����Լ��� ���Ҿ �Լ� ���� ������ �ùٸ��� Ȯ���� ��
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

// 1����Ʈ ������ ����(4��Ʈ ���)
void lcd_data_write(unsigned char data) // �ùٸ��� ���� �Ǿ����� ���ؼ� Ȯ���� ��(���� ������ �� ǥ�� ��)
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

// ���ڿ� ���(1����Ʈ ������ ���� �Լ� ����ȣ��)
void lcd_string(char *str)
{
	while( *str )	lcd_data_write(*str++); // ���ڿ��� 0(�ι���)�� �߰ߵǴ� ���� ���� �������ͼ� �����͸� ��
}

// Ŀ�� �̵��Լ�(0x40: ù��° ������ ù��° ĭ �ּҰ�, 0x80: ��° ���� ù��° ĭ) -> ������ ���� �̵������� ����
void lcd_gotoxy(char x, char y) // set cursor(y = 1, 2)-> 0x40(0, 0), 0x80(0, 1)
{
	lcd_command_write(DDRAM | (0x40 * y) | x); // same as DDRAM + (0x40 * y) + x
}

// lcd�ʱ�ȭ 
void lcd_init(void)
{
	LCD_DDR = 0xFF; // lcd ��Ʈ ��¼���

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