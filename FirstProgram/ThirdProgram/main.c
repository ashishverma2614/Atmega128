/*
 * ThirdProgram.c
 *
 * Created: 2018-05-30 오후 4:06:43
 * Author : Woo
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "USART.h"

#define UBRR_9600_1X 103
#define TRUE 1

// file redirection
FILE OUTPUT = FDEV_SETUP_STREAM(USART0_PutCh, NULL, _FDEV_SETUP_WRITE);
//FILE INPUT = FDEV_SETUP_STREAM(NULL, USART0_GetCh, _FDEV_SETUP_READ);

void debug_LED(char led);

///////////////////////////////////////////////////////////////////////////////
int main(void)
{
    stdout = &OUTPUT;
    //stdin = &INPUT;
    USART0_Init(UBRR_9600_1X);
	char ledOnLocation = 0x10; // init LED
	DDRB = 0xF0;
    while (TRUE) 
    {
        PORTB = ledOnLocation;
        debug_LED(ledOnLocation);
        
        // check led position & come back to original state
		if (ledOnLocation == 0x80) // if 4th led is on
			ledOnLocation = 0x10; // come back to the 1st led
		else
			ledOnLocation <<= 1; // shift led from 0 to 8
		_delay_ms(2000);
    }
}
/* function definition */

void debug_LED(char led)
{
    switch(led) // serial monitor debug
    {
        case 0x10:
        printf("LED ON: X X X O\r\n");
        break;
        case 0x20:
        printf("LED ON: X X O X\r\n");
        break;
        case 0x40:
        printf("LED ON: X O X X\r\n");
        break;
        case 0x80:
        printf("LED ON: O X X X\r\n");
        break;
    }
}