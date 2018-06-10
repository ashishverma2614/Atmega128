#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#define LED_TIME 20

void turn_on_led_PWM(int dim); // adjust the brightness of led

int main(void)
{
	DDRB = 0x01; // set PB0 as output

	int dim = 0; // current brightness of led
	int direction = 1; // 1: increase brightness, -1: decrease

	/* Replace with your application code */
	while (1)
	{
		turn_on_led_PWM(dim);
		dim += direction; // change the brightness
		if (dim == 0) direction = 1;
		else if (dim == 255) direction = -1; // toggle direction, start to decrease brightness
	}
}

void turn_on_led_PWM(int dim)
{
	PORTB = 0x01; // led on

	for (int i = 0; i < 256; i++)
	{
		if (i > dim) PORTB = 0x00; // i is greater than 256, then turn off led
		_delay_us(LED_TIME);
	}
}

