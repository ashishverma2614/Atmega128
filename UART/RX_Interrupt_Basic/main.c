/* RX -> interrupt mode / TX -> polling
 * 
*/

#define F_CPU 16000000L
#define UBRR_9600_1X 103
#define UBRR_9600_2X 207
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define LED_PORT PORTB



void TX_Data(char ch)
{
	while (!(UCSR0A & (0x01 << UDRE0))); // check UDR0 is empty ready to write new data to transmit
	UDR0 = ch; // send ch by writing it at UDR0
}

// isr for RX
ISR(USART0_RX_vect)
{
	char data; // var to store 8bit data from RX
	data = UDR0; // read the UDR buf and store it
	//TX_Data(data); // echo-back to confirm


	// better to separate this code into main
	if (data == 'Y' || data == 'y')
	{
		LED_PORT = 0xFF;
		TX_Data('O');
		TX_Data('N');
		TX_Data('\r');
	}
	else if (data == 'N' || data == 'n')
	{
		LED_PORT = 0x00;
		TX_Data('O');
		TX_Data('F');
		TX_Data('F');
		TX_Data('\r');
	}
}

void INIT()
{
	DDRB = 0xFF;
	PORTB = 0x00; //  initial state: all off
	
	UBRR0H = 0x00;
	UBRR0L = UBRR_9600_2X; 
	UCSR0A = (0x01 << U2X0); // 2x mode enable
	UCSR0B = (0x01 << RXCIE0) | (0x01 << RXEN0) | (0x01 << TXEN0); // Rx complete interrupt enable, RX/TX enable
	UCSR0C = (0x01 << UCSZ00) | (0x01 << UCSZ01); // async, no-parity, 1 stop bit, 8bit RX/TX mode
	sei();
}

FILE OUTPUT = FDEV_SETUP_STREAM(TX_Data, NULL, _FDEV_SETUP_WRITE);

int main(void)
{
	stdout = &OUTPUT;
	INIT();
    while (1) 
    {  // nothing to repeat b/c it uses interrupt
	}
}

