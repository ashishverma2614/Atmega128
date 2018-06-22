#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART_FUNC.h"

int main(void)
{
	// char ch;
	char str[100];
    
    UCSR0B = 0x18; //0b0001_1000 , RX/TX enable
    UCSR0C = 0x06; // 0b0000_0110, USCZ01, USCZ00 -> 8 bit(전송비트수 결정)
    UBRR0L = 103; // 9600 Baud Rate set, Async(+0.2%)

	// echo-back message to see successful connection
    //UART_Putch('T');
    //UART_Putch('E');
    //UART_Putch('S');
    //UART_Putch('T');

    while(1){
	    
	    //UART_Putch(UART_Getch());
		UART_Gets(str);
		UART_Puts(str);
    }

    return 1;
}
