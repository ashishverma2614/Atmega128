/* ServoMotor.c
 * Created: 2018-06-08 ¿ÀÈÄ 5:47:34
 * Author : Seungan Woo
 */


/******* Signal width and Degree Legend ********/
// 0.5ms ---- 0 degree (.5ms HIGH + 19.5ms LOW)
// 1.0ms ---- 45 degree (1.0ms HIGH + 19.0ms LOW)
// 1.5ms ---- 90 degree (1.5ms HIGH + 18.5ms LOW)
// 2.0ms ---- 135 degree (2.0ms HIGH +18.0ms LOW)
// 2.5ms ---- 180 degree (2.5ms HIGH + 17.5ms LOW)

#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>

#define ZERO_ANGLE 5 // 5 means 0.5ms HIGH

volatile unsigned char moveAngle = 0;
volatile unsigned char count = 0;

// per 1 switch pressing, 0.1ms of high width increases
// , which is 9 degree(45degree / 5).
// If I had set unit of a degree as "0.9" degree, servo would not operate due to "DEAD BAND"
// In order to control servo by 0.9 degree, we need to use 16bit Timer1 for high resolution.
ISR(INT2_vect) // TODO: switch de-bouncing
{
	++moveAngle;
	if (moveAngle > 20)
		moveAngle = 20; // 180 degree (ZERO_DEGREE + 2.0ms = 2.5ms), exception handling
}

// per 1 switch pressing, 0.1ms of high width decrease
ISR(INT3_vect) // TODO: switch de-bouncing
{
	--moveAngle;
	if (moveAngle < 0)
		moveAngle = 0; // 0 degree (ZERO_DEGREE = 0.5ms), exception handling
}

ISR(TIMER0_COMP_vect) // 100us(0.1ms)
{
	if (++count == 200) // period criteria(20ms = 0.1ms X 200)
	{
		count = 0; // re-count timer interrupt
		PORTD = 0xFF; // start servo moving
	}
	if (count > (moveAngle + ZERO_ANGLE))
		PORTD = 0x00; // stop servo moving
}

void INIT(void)
{
	// pin setting
	DDRB = 0xFF; // set portB as output
	DDRD = 0x00; // set portD as input(SW)
	PORTD = 0x0C; // set internal pull-up resist
	
	// external interrupt enable
	EICRA |= (1 << ISC31) | (1 << ISC21); // set INT2, 3 as falling edge detection
	EIMSK |= (1 << INT2) | (1 << INT3); // INT2, 3 enable
	
	// timer interrupt enable
	TCCR0 |= (1 << WGM01) | (1 << CS01); // CTC mode, pre-scale ratio 8
	TIMSK |= (1 << OCIE0); // compare and match interrupt enable
	OCR0 = 199; // 1/16us X 8 X (1 + 199) = 100us
	
	sei(); // global interrupt enable
}

int main(void) // nothing to return(no OS, no return)
{
	INIT();
    while (1) 
    { /*nothing to repeat*/ }
}

