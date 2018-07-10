#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "UART1.h"


FILE OUTPUT = FDEV_SETUP_STREAM(UART1_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART1_receive, _FDEV_SETUP_READ);


void ADC_init(unsigned char channel)
{
	ADMUX |= (1 << REFS0); // set AVCC as criteria voltage
	ADCSRA |= 0x07; // pre-scaler:
	ADCSRA |= (1 << ADEN); // ADC enable
	ADCSRA |= (1 << ADFR); // "free-running" mode

	ADMUX = ((ADMUX & 0xE0) | channel);
	ADCSRA = ( 1 << ADSC); // start conversion
}

int read_ADC(void)
{
	while (!(ADCSRA & (1 << ADIF))); // wait for the completion of conversion
	return ADC; // once the conversion completes, it returns 10 bit int val
}

int main(void)
{
	int ad_val;

	stdout	= &OUTPUT;
	stdin = &INPUT;

	UART1_init(); // initialize UART1 communiaction
	ADC_init (0); // start ADC conversion using channel '0'

	while( 1 )
	{
		ad_val = read_ADC (); // read the analog value of potention-meter
		printf ("%d\r\n", ad_val);
		_delay_ms (1000);
	}
	return EXIT_SUCCESS;
}
