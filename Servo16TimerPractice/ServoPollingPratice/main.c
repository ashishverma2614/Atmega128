// this exmaple is to try to understand the core of servo motor moving
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>




int main(void)
{
    DDRB = 0x80; // set pin 7 as output
  
    while (1) 
    {
        // -90degree
        for (int i = 0; i < 25; i++)
        {
            PORTB = 0xFF;
            _delay_us(500); // 0.5ms
            PORTB = 0x00;
            _delay_us(19500); // 1.5ms
        }
        // 0 degree
        /*for (int i = 0; i < 50; i++)
        {
            PORTB = 0xFF;
            _delay_us(1500); // 1.5ms
            PORTB = 0x00;
            _delay_us(18500);
        }*/
        // + 90 degree
        for (int i = 0; i < 25; i++)
        {
            
            PORTB = 0xFF;
            _delay_us(2500);
            PORTB = 0x00;
            _delay_us(17500);
        }
    }
}

