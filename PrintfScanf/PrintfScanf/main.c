// This is the test version of printf/scanf for re-usability
// 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "UART.h"


#define UBRR_9600_1X 103

FILE OUTPUT = FDEV_SETUP_STREAM(Put_ch, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, Get_ch, _FDEV_SETUP_READ);


int main(void)
{
    stdout = &OUTPUT;
	stdin = &INPUT;
	UART_init (UBRR_9600_1X);

	printf("\n");
	printf ("Hello World!\n\r");
	printf ("Bonjour le monde\n\r");

    while (1) 
    {
    }
}

