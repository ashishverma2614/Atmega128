#include <avr/io.h>
#include <util/delay.h>

#include "led.h"


#define LED_OUT	PORTD
#define LED_DDR	DDRD

// led를 전달된 param의 갯수대로 켜는 함수
void led_light(unsigned char led)
{
	unsigned char tmp = 0;

	LED_OUT |= 0xFF; // all off
	for( int i = 0; i < led; i++ )	tmp |= (0x01 << i);
		LED_OUT &= ~tmp;
}

// led의 초기 상태 설정 함수
void led_init(void)
{
	LED_DDR = 0xFF;
	LED_OUT = 0xFF;
}
