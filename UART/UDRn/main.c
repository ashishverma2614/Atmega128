/* UCSRnA: [RXCn][TXCn][UDREn][FEn][DORn][UPEn][U2Xn][MPCMn]
 *
 **/


#define F_CPU 16000000UL
#define UBRR_9600_1X 103

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "UART_FUNC.h"

FILE OUTPUT = FDEV_SETUP_STREAM(UART_Putch, NULL, _FDEV_SETUP_WRITE);
FILE INPUT = FDEV_SETUP_STREAM(NULL, UART_Getch, _FDEV_SETUP_READ);

int main(void)
{

	stdout = &OUTPUT;
	stdin = &INPUT;
	UART_init (UBRR_9600_1X);
	printf("Case Converting RX/TX \r\n");
    
    while (1) 
    {
		/* Wait for RX enable*/

		//while ((UCSR0A & 0x80) == 0x00); // wait until RXC0 = 1
		while (!(UCSR0A & (1 << RXC0)));
		char ch = UDR0; // RX(store the contents in the UDRn to the var)

		/* Convert case */
		printf("\r\n");
		
		if (ch >= 'a' && ch <= 'z')
			 ch = ch - 'a' + 'A'; // toggle case
		else if (ch >= 'A' && ch <= 'Z')
			ch = ch - 'A' + 'a';
		
		/* Wait for TX enable */
		
		//while((UCSR0A & 0x20) == 0x00); // wait until UERE0 = 1
		while(!(UCSR0A & (1 << UDRE0)));
		UDR0 = ch; // TX(Write data to UDRn)
		
    }
}

