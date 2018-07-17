#include "Lcd_4bit.h"

void lcd_pulse_enable(void) // make a  falling edge to write instruction/data
{
    PORT_LCD |= (0x01 << E_PIN); // H
    _delay_us(1);
    PORT_LCD &= ~(0x01 << E_PIN); // L
    _delay_us(1);
}

void lcd_write_data(unit8_t data)
{
    PORT_LCD |= (1 << RS_PIN); // RS = 1 for data write
    if (MODE == 8)
    {
        PORT_
    }
}