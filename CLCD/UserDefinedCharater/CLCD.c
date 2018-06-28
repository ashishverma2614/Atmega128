#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "CLCD_Header.h"

// function implementations
void LCD_INIT(void) // refer to the data sheet
{
	DDRC = 0xFF;
	PORTC = 0x00;
	_delay_ms(15);
	CMD(0x20);
	_delay_ms(5);
	CMD(0x20);
	_delay_us(100);
	CMD(0x20);
	CMD(FUNCTIONSET);
	CMD(DISPON);
	CMD(ALLCLR);
	CMD(ENTMODE);
}

void Busy(void)
{
	_delay_ms(2);
}

void Disp_String(const char * str)
{
	//char i = 0;
	while(*str != 0) Send_Data(*str++); // until encountering, send data of *str
}

void CMD(u_char byte) // cmd set
{
	Busy(); // delay for stable start

	// instruction for upper 4bits
	PORTC = 0x00; // RS = 0(IR), RW = 0(Write)
	PORTC |= (byte & 0xF0); // masking upper 4bits

	_delay_us(1);
	ENH; // E = 1

	_delay_us(1);
	ENL; // E = 0
// Repeat same timing above

	// instruction for lower 4bits
	PORTC = 0x00; // RW = 0, RS = 0
	PORTC |= (byte << 4); // masking lower 4bits

	_delay_us(1);
	ENH; // E = 1

	_delay_us(1);
	ENL; // E = 0
}

void Send_Data(u_char byte)
{
	Busy();


	// send upper NIBL
	PORTC = 0x01; // RS = 1, RW = 0
	PORTC |= (byte & 0xF0);
	_delay_us(1);
	ENH; // E = 1
	_delay_us(1);
	ENL; // E = 0

	// send lower NIBL
	PORTC = 0x01; // RS = 1, RW = 0
	PORTC |= (byte << 4);
	_delay_us(1);
	ENH; // E = 1
	_delay_us(1);
	ENL; // E = 0
}


