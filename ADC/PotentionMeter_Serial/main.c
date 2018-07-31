#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "ADC.h"


FILE OUTPUT = FDEV_SETUP_STREAM(UART0_Putch, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_Getch, _FDEV_SETUP_READ);


int main(void)
{
	int ad_val;

	stdout	= &OUTPUT;
	stdin = &INPUT;

	
	ADC_init(); // start ADC conversion using channel '0'

	while( 1 )
	{
		ad_val = get_ADC_value (); // read the analog value of potention-meter
		printf ("%d\r\n", ad_val);
		_delay_ms (500);
	}
	return EXIT_SUCCESS;
}
