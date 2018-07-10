#include <avr/io.h>
#include <util/delay.h>

#include "led.h"


#define LED_OUT	PORTD
#define LED_DDR	DDRD

// led�� ���޵� param�� ������� �Ѵ� �Լ�
void led_light(unsigned char led)
{
	unsigned char tmp = 0;

	LED_OUT |= 0xFF; // all off
	for( int i = 0; i < led; i++ )	tmp |= (0x01 << i);
		LED_OUT &= ~tmp;
}

// led�� �ʱ� ���� ���� �Լ�
void led_init(void)
{
	LED_DDR = 0xFF;
	LED_OUT = 0xFF;
}
