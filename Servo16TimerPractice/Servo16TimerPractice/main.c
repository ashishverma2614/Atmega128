// Using T/C1, make 0.1ms interrupt
// move servo -90 -> 0 -> 90
//  normal mode
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define ZERO_DEG 375
#define N_90_DEG 125
#define P_90_DEG 625

void timer1_init(void)
{
    // mode 14
    TCCR1A = (0x01 << COM1A1) | (0x01 << WGM11);
    TCCR1B = (0x01 << WGM13) | (0x01 << WGM12) | (0x01 << CS11) | (0x01 << CS10);
    ICR1 = 4999; // TOP value
    OCR1A = P_90_DEG; // 0 degree
    TCNT1 = 0x00; // start point
}

int main(void)
{
    DDRB = 0xFF;
    PORTB = 0xFF;
    timer1_init();
    while(1)
    {
        OCR1A = N_90_DEG;
        _delay_ms(500);
        OCR1A = P_90_DEG;
        _delay_ms(500);
    }
}