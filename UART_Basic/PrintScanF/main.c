#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "UART0.h"

// instances of input and output for printf/scanf
FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART0_receive, _FDEV_SETUP_READ);

int main(void)
{
	unit8_t counter = 100;
	char buf[20] = ""; // receive buffer

	stdout = &OUTPUT;
	stdin = &INPUT;

	UART0_init();

	printf("\n");
	printf("Current Counter Value: ");
	printf("%d\r\n", counter);
	while (1)
	{
		scanf("%s", buf);

		if (strcmp(buf, "DOWN") == 0)
		{
			counter--;
			printf("Current Counter Value : ");
			printf("%d\r\n", counter);
		}
		else if (strcmp(buf, "UP") == 0)
		{
			counter++;
			printf("Current Counter Value : ");
			printf("%d\r\n", counter);
		}
		else // exception handling
		{
			printf("Invalid Command!! \r\n");
		}
	}
	return 0;
}

