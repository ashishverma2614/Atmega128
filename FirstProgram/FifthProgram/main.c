/*
 * FifthProgram.c
 *
 * Created: 2018-05-30 오후 4:46:32
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
    USART0_Init(UBRR_9600_1X);
	unsigned char ledOnLocation[4] = {0x10, 0x20, 0x40, 0x80};
	DDRB = 0xF0;
    
    while (TRUE)
    {
		for (int i = 0; i < 4; i++)
		{
		PORTB = ledOnLocation[i];
        debug_LED(ledOnLocation[i]);
		_delay_ms(500);	
		}
		for (int i = 2; i > 0; i--)
		{
		PORTB = ledOnLocation[i];
        debug_LED(ledOnLocation[i]);
		_delay_ms(500);
		}
    }
    return EXIT_SUCCESS;
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
