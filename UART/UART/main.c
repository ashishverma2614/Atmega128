//FIXME: The code below is incomplete, so that it is to be revised!

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART0.h"

#define UBRR_9600_1X 103

int main(void)
{
	// char ch;
	unsigned char str[16];
    
    UART0_init (UBRR_9600_1X);

	
	// echo-back message to see successful connection
    //TX0_ch ('\n');
	//TX0_ch ('\r');
	TX0_str((unit8_t)("\n\r")); // same as the above
	TX0_ch('T');
    TX0_ch('E');
    TX0_ch('S');
    TX0_ch('T');

    while(1)
	{
	    
		//TX0_ch (RX0_ch ());
		RX0_str (str);
		TX0_str(str); // Unfortunately, come garbage strings are displayed if there is a space
		
    }

    return 0;
}
