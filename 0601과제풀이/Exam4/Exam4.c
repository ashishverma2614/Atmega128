#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define FND1		PORTC = 0x01
#define FND2		PORTC = 0x02
#define FND3		PORTC = 0x04
#define FND4		PORTC = 0x08
#define FND_PORT	PORTA


int count = 0;

char fnd[10] = {0b11000000, // 0
				0b11111001, // 1
				0b10100100, // 2
				0b10110000, // 3
				0b10011001, // 4
				0b10010010, // 5
				0b10000010, // 6
				0b11111000, // 7
				0b10000000, // 8
				0b10010000, // 9
				};
ISR(INT2_vect)
{
	if(++count == 10000)
		count = 0;
	
}

ISR(INT3_vect)
{
	if(count != 0) count--;
	else count = 9999;
}

ISR(INT6_vect)
{
	count = 0;
}

ISR(INT7_vect)
{
	count = 9999;
}

int main(void)
{
	int i, j;
	char temp[5];
	
	DDRA = 0xFF;
	DDRC = 0xFF;
	DDRD = 0x00;
	DDRE = 0x00;

	PORTD = 0x08;
	PORTE = 0x80;

	EICRA = 0b10100000;
	EICRB = 0b10100000;
	EIMSK = 0b11001100;

	SREG |= 0x80;
	
	while(1){
		sprintf(temp,"%04d", count);
		
		FND1;
		FND_PORT = fnd[temp[3] - '0'];
		_delay_ms(5);

		FND2;
		FND_PORT = fnd[temp[2] - '0'];
		_delay_ms(5);

		FND3;
		FND_PORT = fnd[temp[1] - '0'];
		_delay_ms(5);

		FND4;
		FND_PORT = fnd[temp[0] - '0'];
		_delay_ms(5);
	}
}
