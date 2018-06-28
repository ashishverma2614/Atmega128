#define F_CPU 16000000UL
#include <avr/io.h>
#include "UART0.h"

int main(void)
{
	UART0_init();
	
	char str[] = "Test using UART0 Library";
	unit8_t num = 128;
	float floatNum = 3.14;
	
	UART0_print_string("\n\r");
	UART0_print_string(str);
	UART0_print_string("\n\r");
	
	UART0_print_1_byte_number(num);
	UART0_print_string("\n\r");

	UART0_print_float(floatNum);
	UART0_print_string("\n\r");
	while (1)
	{
	}
	return 0;
}
