#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <math.h>

#define FND1		PORTC = 0x01
#define FND2		PORTC = 0x02
#define FND3		PORTC = 0x04
#define FND4		PORTC = 0x08
#define FND_PORT	PORTA

int count = 0;
char sel = 0; // digit number
char blink = 0; //  0 : blink off, 1 : blink on
char cnt_up = 0; // 0 : count_stop, 1 : count_up
char blink_state = 0; // 0 : ON, 1 : OFF
char temp[5];
char up = 0;

char fnd[11] = {0b11000000, // 0
	0b11111001, // 1
	0b10100100, // 2
	0b10110000, // 3
	0b10011001, // 4
	0b10010010, // 5
	0b10000010, // 6
	0b11111000, // 7
	0b10000000, // 8
	0b10010000, // 9
	0b11111111, // Blank
};

ISR(INT2_vect)	// SW1
{
	blink = 1;
	sel = 0;
	cnt_up = 0;
}

ISR(INT3_vect) // SW2
{
	if(blink == 1)
	if(++sel == 4)
	sel = 0;
}

ISR(INT6_vect) // SW3
{
	//int num;

	if(temp[3-sel] == '9') temp[3-sel] = '0';
	else temp[3-sel]++;

	count = atoi(temp);
}

ISR(INT7_vect) // SW4
{
	blink = 0;
	blink_state = 0;

	if(cnt_up == 0) cnt_up = 1;
	else cnt_up = 0;
}

int main(void)
{
	int i, j;
	
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
		sprintf(temp,"%04d\0", count);
		
		for(i=0; i< (25 * blink + 1); i++){
			FND1;
			if(blink_state == 1 && sel == 0)
			FND_PORT = fnd[10];
			else
			FND_PORT = fnd[temp[3] - '0'];
			_delay_ms(5);

			FND2;
			if(blink_state == 1 && sel == 1)
			FND_PORT = fnd[10];
			else
			FND_PORT = fnd[temp[2] - '0'];
			_delay_ms(5);

			FND3;
			if(blink_state == 1 && sel == 2)
			FND_PORT = fnd[10];
			else
			FND_PORT = fnd[temp[1] - '0'];
			_delay_ms(5);

			FND4;
			if(blink_state == 1 && sel == 3)
			FND_PORT = fnd[10];
			else
			FND_PORT = fnd[temp[0] - '0'];
			_delay_ms(5);
		}

		if(cnt_up == 1){
			if(++count == 10000) count = 0;
		}

		if(blink == 1){
			blink_state = !blink_state;
		}
	}

	return 0;
}
