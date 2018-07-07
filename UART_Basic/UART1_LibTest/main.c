#define F_CPU 16000000UL
#include <avr/io.h>
#include "UART1.h"

int main(void)
{
    UART1_init();
	
	char str[] = "Test using UART1 Library";
	unit8_t num = 128;
	
	UART1_print_string(str);
	UART1_print_string("\n\r");
	
	UART1_print_1_byte_number(num);
	UART1_print_string("\n\r");
    while (1) 
    {
    }
	return 0;
}

