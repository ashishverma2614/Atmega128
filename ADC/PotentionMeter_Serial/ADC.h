#ifndef ADC_H_
#define ADC_H_

#define BAUD 9600
#include <avr/io.h>
#include <avr/interrupt.h>
#include "UART.h"

static unsigned int ad_val; // 16bit space

ISR(ADC_vect)
{
	ad_val = (int) ADCL | ((int)ADCH << 8);
}

void ADC_init(void)
{
	UART0_Init(103); // to connect to serial monitor
	ADMUX = (0x01 << REFS0); // AVCC enable, Channel: ADC0(0x40)
	ADCSRA = 0xAF; // ADC enable, free-running, 128 pre-scale
	_delay_ms(5); // initial start duration
	ADCSRA = 0xEF; // ADIF clear to wait for the next ADC interrupt
	SREG |= 0x80; // global interrupt enable
}

unsigned int get_ADC_value() //
{
	return ad_val;
}



#endif /* ADC_H_ */