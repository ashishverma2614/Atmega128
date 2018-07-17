#include <avr/io.h>
#include <util/delay.h>

#define setbit(x, y)      x |= (1 << y)
#define resetbit(x, y)    x &= ~(1 << y)

int main(void)
{
	DDRB = 0xFF;

	while(1){
		setbit(PORTB, 0); // PORTB�� bit0�� 1�� ���
		_delay_ms(1000);

		resetbit(PORTB, 0); // PORTB�� bit0�� 0���� ���
		_delay_ms(1000);
	}
}
