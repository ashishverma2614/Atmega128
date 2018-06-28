#define F_CPU 16000000L
#define TERMINATOR '\r'
#include <avr/io.h>
#include <string.h>
#include "UART0.h"



int main(void)
{
    unit8_t counter = 100; // counter
	int index = 0; // location of receiving buf
	int process_data = 0; // flag for commanding string process: 1 for process string
	char buf[20] = ""; // RX data buf
	char data; // RX data

	UART0_init(); // INIT


	// header
	UART0_print_string("\n\r");
	UART0_print_string("===================================\n\r");
	UART0_print_string("Current Counter Value : ");
	UART0_print_1_byte_number(counter);
	UART0_print_string("\r\n");

    while (1) 
    {
		data = UART0_receive(); // receive data
		
		// check line end
		if (data == TERMINATOR)
		{
			buf[index] = '\0'; // append null to the end
			process_data = 1; // completion of receiving string with null
		}
		else
		{
			buf[index] = data; // write data to the receive buf
			index++; // increment of index
			//process_data = 0;
		}

		// process string
		if (process_data == 1)
		{
			if(strcmp(buf, "DOWN") == 0)
			{
				counter--;
				UART0_print_string("Current Counter Value : ");
				UART0_print_1_byte_number(counter);
				UART0_print_string("\n\r");
			}
			else if (strcmp(buf, "UP") == 0)
			{
				counter++;
				UART0_print_string("Current Counter Value : ");
				UART0_print_1_byte_number(counter);
				UART0_print_string("\n\r");
			}
			else // exception handling
			{
				UART0_print_string("!! Unknown Command!! ");
				UART0_print_string("\r\n");
			}
			// end of processing string data
			// recover original state
			index = 0;
			process_data = 0;
		}



    }
}

