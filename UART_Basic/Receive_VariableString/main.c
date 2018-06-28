/* This is the pratice to test variable cmd(UP / DOWN) received via
 * UART0 RX scheme. For exception handling, when an user pressed wrong cmd,
 * append peripheral if-else statement to tackle this unexpected pitfall. 
*/

#define F_CPU 16000000L
#define TERMINATOR '\r'
#include <avr/io.h>
#include <string.h>
#include "UART0.h"



int main(void)
{
    unit8_t counter = 100; // counter
	int index = 0; // index of buf array
	int isProcessData = 0; // flag for commanding string process: 1 for process string
	char buf[20] = ""; // RX data buf to store the contents in the UDR0
	char data; // RX one char data

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
		if (data == TERMINATOR) // if meeting CR
		{
			buf[index] = '\0'; // append null to the end
			isProcessData = 1; // completion of receiving string with null
		}
		else
		{
			buf[index] = data; // write data to the receive buf
			index++; // increment of index
			//isProcessData = 0
		}

		// process string
		if (isProcessData == 1)
		{
			if(strcmp(buf, "DOWN") == 0) // compare cmd with buf contents. true returns 0
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
			// restore original state for repetition
			index = 0;
			isProcessData = 0;
		}
    }
}

