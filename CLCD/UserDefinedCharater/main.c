#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "CLCD_Header.h"

// user defined character
unsigned char cg_pat[24] =
{
	0x1F, 0x04, 0x0A, 0x11, 0x04, 0x1F, 0x00, 0x00,
	0x09, 0x17, 0x17, 0x09, 0x00, 0x06, 0x09, 0x06,
	0x1F, 0x04, 0x0A, 0x11, 0x1F, 0x04, 0x10, 0x1F
};

int main(void)
{
	LCD_INIT();
	for (int i = 0; i < 24; i++)
	{
		CMD(0x40 + i); // register CGRAM pattern
		Send_Data(*(cg_pat + i));
	}
	CMD(HOME); // move cursor to the home
	Disp_String("My name is ");
	Send_Data(0x00);
	Send_Data(0x01);
	Send_Data(0x02);
}

