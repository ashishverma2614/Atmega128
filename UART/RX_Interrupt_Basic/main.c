/* RX -> interrupt mode / TX -> polling
*/

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void TX_Data(char ch)
{
	while (!(UCSR0A & (0x01 << UDRE0))); // if UDR0 is empty, UDRE0 becomes 1 which means it is possible to do TX
	UDR0 = ch; // send ch by writing it at UDR0
}

ISR(USART0_RX_vect)
{
	char data;
	data = UDR0; // read the UDR buf and store it
	//TX_Data(data); // echo-back
	if (data == 'Y' || data == 'y')
	{
		PORTB = 0xFF;
		TX_Data('O');
		TX_Data('N');
		TX_Data('\r');
	}
	else if (data == 'N' || data == 'n')
	{
		PORTB = 0x00;
		TX_Data('O');
		TX_Data('F');
		TX_Data('F');
		TX_Data('\r');
	}
}



void INIT()
{
	DDRB = 0xFF;
	PORTB = 0x00;
	
	UBRR0H = 0x00; // 207 cannot be written at upper UBRRnH
	UBRR0L = 207; //9600bps at 2x mode
	UCSR0A = (0x01 << U2X0); // 2x mode
	UCSR0B = (0x01 << RXCIE0) | (0x01 << RXEN0) | (0x01 << TXEN0); // Rx complete interrupt enable, RX/TX enable
	UCSR0C = (0x01 << UCSZ00) | (0x01 << UCSZ01); // async, no-parity, 1 stop bit, 8bit RX/TX mode
	sei();
}


int main(void)
{
	INIT();
    while (1) 
    {    }
}

