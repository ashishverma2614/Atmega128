#define F_CPU 16000000L // default 16Mhz operation
					   // F_CPU means the number of clock per 1 sec
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "USART.h"

#define UBRR_9600_1X 103 // Baud rate
#define TRUE 1

FILE OUTPUT = FDEV_SETUP_STREAM(USART0_PutCh, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, USART0_GetCh, _FDEV_SETUP_READ);

///////////////////////////////////////////////////////////////////////////////
int main(void)
{
	stdout = &OUTPUT;
	USART0_Init(UBRR_9600_1X);
	DDRB = 0xFF;
	static unsigned int count = 0;

    while (TRUE) 
    {
        count++;
		if ((count % 2) == 0) 
        {
            // increase number from 0 by 1 during 1 sec
            printf("count : %d \n\r", count);
            PORTB |= 0xFF; // led on at even num    
        }
		else 
        {
            printf("count : %d \n\r", count);
            PORTB &= ~0xFF; // led off at odd num
        }
		_delay_ms(500);
    }
	return EXIT_SUCCESS;
}

