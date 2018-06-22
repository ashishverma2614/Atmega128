#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "UART_support.h"


FILE OUTPUT = FDEV_SETUP_STREAM(UART0_Transmit, NULL, _FDEV_SETUP_WRITE);
	
int main(void)
{
	UART0_init(); // init UART0
	stdout = &OUTPUT; // set printf use
	unsigned int count = 0;
    while (1) 
    {
		printf("%d\n\r", count++);
		_delay_ms(1000);
    }
	return 0;
}

