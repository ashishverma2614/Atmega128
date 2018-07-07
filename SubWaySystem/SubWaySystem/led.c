#include <avr/io.h>
#include <util/delay.h>

#include "led.h"

#define MLED_PORT		PORTA
#define MLED_DDR			DDRA
#define MLED_DATA		0x01
#define MLED_SHIFT		0x02
#define MLED_CLEAR		0x04
#define MLED_LATCH1		0x10
#define MLED_LATCH2		0x20

// refer to 74HC595 data sheet
// Using 4signals can control 16 data at once
// 16개 LED의 정보를 int값(0xNNNN)의 형태로 받아서 해당 LED를 켜 주는 함수
void matrix_led(unsigned int data)
{
	unsigned int tmp = ~data; // active low(full-down resist) -> toggle the signal befor saving
	int i;

	MLED_PORT |= MLED_CLEAR; // port clear
	MLED_PORT &= ~(MLED_LATCH1 | MLED_LATCH2); // latch clear
	for( i = 0; i < 16; i++ ) { // from MSB
		if( tmp & 0x8000 )	MLED_PORT |= MLED_DATA; // ENLH
		else				MLED_PORT &= ~MLED_DATA; //ENLL

		MLED_PORT |= MLED_SHIFT; // set shift clk(H)
		tmp <<= 1; // to LSB
		MLED_PORT &= ~MLED_SHIFT; // clear shift clk(L)
	}

	MLED_PORT |= (MLED_LATCH1 | MLED_LATCH2); // set latch(H)
	MLED_PORT &= ~(MLED_LATCH1 | MLED_LATCH2); // set latch(L)
}

// 모든 LED를 꺼주는 함수
void matrix_alloff(void)
{
	int i;

	MLED_PORT |= MLED_CLEAR;
	MLED_PORT &= ~(MLED_LATCH1 | MLED_LATCH2);
	for( i = 0; i < 16; i++ ) { // no data passed
		MLED_PORT |= MLED_DATA;

		MLED_PORT |= MLED_SHIFT;
		MLED_PORT &= ~MLED_SHIFT;
	}

	MLED_PORT |= (MLED_LATCH1 | MLED_LATCH2);
	MLED_PORT &= ~(MLED_LATCH1 | MLED_LATCH2);
}

// 모든 LED를 끄는 함수
void matrix_allon(void) // At the beginning, there are stored 0s
						// Between LatchH and LatchL, this func produce all 0(active-low)
{
	MLED_PORT &= ~MLED_CLEAR;
	MLED_PORT |= (MLED_LATCH1 | MLED_LATCH2);
	MLED_PORT &= ~(MLED_LATCH1 | MLED_LATCH2);
}

// LED의 초기화 
void matrix_init(void)  
{
	MLED_DDR = 0xFF; // output
	MLED_PORT = 0x00; // all led[15:0] off 

	matrix_alloff();
}