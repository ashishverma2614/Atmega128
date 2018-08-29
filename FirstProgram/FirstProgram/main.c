#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "USART.h"

#define UBRR_9600_1X 103 // Baud rate
#define TRUE 1

// file redirection
FILE OUTPUT = FDEV_SETUP_STREAM(USART0_PutCh, NULL, _FDEV_SETUP_WRITE);
//FILE INPUT = FDEV_SETUP_STREAM(NULL, USART0_GetCh, _FDEV_SETUP_READ);

///////////////////////////////////////////////////////////////////////////////
int main(void)
{
	stdout = &OUTPUT;
	//stdin = &INPUT;

    USART0_Init(UBRR_9600_1X);
	DDRB = 0xFF; // set pin7 of PORTB as output
    while (TRUE) 
    {
		PORTB |= 0xFF; // turn on LED connected to PA0
		printf ("LED turned on!\r\n"); // for debug
		//USART0_PutS("LED turned on!");
        _delay_ms(500);
		PORTB &=  ~(0xFF); // turn off LED connected to PA0
		printf("LED turned off!\r\n");
        //USART0_PutS("LED turned off");
		_delay_ms(500);
    }
	
	return EXIT_SUCCESS; 
}

