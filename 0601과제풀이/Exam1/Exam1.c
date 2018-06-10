#include <avr/io.h>
#include <util/delay.h>

#define setbit(x, y)      x |= (1 << y)
#define resetbit(x, y)    x &= ~(1 << y)

int main(void)
{
	DDRB = 0xFF;

	while(1){
		setbit(PORTB, 0); // PORTB의 bit0를 1로 출력
		_delay_ms(1000);

		resetbit(PORTB, 0); // PORTB의 bit0를 0으로 출력
		_delay_ms(1000);
	}
}
