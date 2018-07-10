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


unsigned char lcd_command_read(void) // ���� �� ���� ����
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

// LCD�� ��ɾ�(CMD) ����
// LCD�� ������ ����(4��Ʈ ��� �̹Ƿ� H/L�� �� �� ����� �����Ѵ�)
// ��ɾ ������ ���� ��Ʈ Enable��ȣ�� H - > L���̿� ���޵ȴ�(������ ��Ʈ ����)
void lcd_command_write(unsigned char cmd)
{
	lcd_busycheck(); // ����� �����ð�(busy check)

	LCD_OUT = 0x00; // ��Ʈ�� ����

	LCD_OUT |= cmd & 0xF0; // ��ɾ��� ���� ����Ʈ ����ŷ
	LCD_OUT |= LCD_E; // H
	LCD_OUT &= ~LCD_E; // L

	LCD_OUT &= 0x0F;
	LCD_OUT |= cmd << 4; // ��ɾ��� ���� ����Ʈ ����ŷ
	LCD_OUT |= LCD_E; // H
	LCD_OUT &= ~LCD_E; // L

	_delay_ms(2);
}


unsigned char lcd_data_read(void) // ������ ����
{
	char data;

	LCD_DDR = 0x0F;
	LCD_OUT = 0x00;

	LCD_OUT |= LCD_RS;
	LCD_OUT |= LCD_RW;
	//���� ����Ʈ ����
	LCD_OUT |= LCD_E; // 0b0000_0100(H)
	asm	("nop");
	data = LCD_IN & 0xF0;
	LCD_OUT &= ~LCD_E; // 0b1111_1011 (L)

	// ���� ����Ʈ ����
	LCD_OUT |= LCD_E; // H
	asm	("nop");
	data |= LCD_IN >> 4;
	LCD_OUT &= ~LCD_E; // L

	return data;
}

// LCD�� 1����Ʈ(�ѱ���)�� ��
void lcd_data_write(unsigned char data)
{
	LCD_OUT = 0x00;

	LCD_OUT |= LCD_RS; // RS = 1, ������ ���� ���

	// ���� ����Ʈ ������ ����
	LCD_OUT |= data & 0xF0;
	// �ο��̺� ��ȣ ��ȭ
	LCD_OUT |= LCD_E;
	LCD_OUT &= ~LCD_E;

	// ���� ����Ʈ ������ ����
	LCD_OUT &= 0x0F;
	LCD_OUT |= data << 4;
	// �ο��̺� ��ȣ ��ȯ
	LCD_OUT |= LCD_E;
	LCD_OUT &= ~LCD_E;

	_delay_us(50);
}

// LCD�� ���� ���ڸ� ��: lcd_data_write�� �ݺ��ؼ� ȣ���ϴ� ������� ���ڿ��� �����Ѵ�
void lcd_string(char *str)
{
	while( *str )	lcd_data_write(*str++);
	// while(str[i] != '\0') lcd_data_write(str[i++]);
}

// LCD�� Ŀ�� ��ǥ ����: DDRAM�� Ŀ���� ��Ÿ���� �ּҸ� �����Ѵ�
// 0x40�� lcd������ �ּ�
void lcd_gotoxy(char x, char y)
{
	lcd_command_write(DDRAM | (0x40 * y) | x);
}

// LCD �ʱ�ȭ(������ ��Ʈ ����)
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
