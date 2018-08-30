#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "USART.h"

#define UBRR_9600_1X 103
#define TRUE 1
#define SW_ON 1
#define SW_OFF 0

// file redirection
FILE OUTPUT = FDEV_SETUP_STREAM(USART0_PutCh, NULL, _FDEV_SETUP_WRITE);
//FILE INPUT = FDEV_SETUP_STREAM(NULL, USART0_GetCh, _FDEV_SETUP_READ);
void debug_LED(char led);
///////////////////////////////////////////////////////////////////////////////

int main(void)
{
    // initialization
    stdout = &OUTPUT;
    USART0_Init(UBRR_9600_1X);
	static char ledOnLocation = 0x10;
	static char swState = SW_OFF;
	
	DDRB = 0xF0; // led, output
	DDRD = 0x00; //SW, input
    PORTD = 0xFF; // internal pull-up resistor
    
    // execution loop
    while (TRUE) 
    {
		if ((PIND & 0x01) != 0 && swState == SW_OFF)
		{
            PORTB = ledOnLocation;
			swState = SW_ON;
            printf("Switch is pressed!: ");
            debug_LED(ledOnLocation);
			if (ledOnLocation == 0x80)
				ledOnLocation = 0x10;
			else
				ledOnLocation <<= 1;
		}
		if ((PIND & 0x01) == 0 && swState == SW_OFF)
			swState = SW_OFF;
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
