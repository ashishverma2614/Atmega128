#define MLED_PORT		PORTB
#define MLED_DDR			DDRB
#define MLED_DATA		0x01
#define MLED_SHIFT		0x02
#define MLED_CLEAR		0x04
#define MLED_LATCH1		0x10
#define MLED_LATCH2		0x20

// refer to 74HC595
// Using 4signals can control 16 data at once
void matrix_led(unsigned int data)
{
	unsigned int tmp = ~data; // active low -> toggle the signal
	int i;

	MLED_PORT |= MLED_CLEAR;
	MLED_PORT &= ~(MLED_LATCH1 | MLED_LATCH2);
	for( i = 0; i < 16; i++ ) {
		if( tmp & 0x8000 )	MLED_PORT |= MLED_DATA;
		else				MLED_PORT &= ~MLED_DATA;

		MLED_PORT |= MLED_SHIFT;
		tmp <<= 1;
		MLED_PORT &= ~MLED_SHIFT;
	}

	MLED_PORT |= (MLED_LATCH1 | MLED_LATCH2);
	MLED_PORT &= ~(MLED_LATCH1 | MLED_LATCH2);
}