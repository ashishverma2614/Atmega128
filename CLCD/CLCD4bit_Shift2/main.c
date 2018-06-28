#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "CLCD_Header.h"

int main(void)
{
    LCD_INIT();

	// display at the first line
	Disp_String("Hello!! World -- Atmel ATmega128");
    while (1) 
    {
		CMD(LSHIFT); // string moves to the right
		_delay_ms(300);
    }
}

