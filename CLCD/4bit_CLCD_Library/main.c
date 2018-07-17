#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

typedef unsigned char u_char;

// instructions
#define ALLCLR 0X01
#define HOME 0x02
#define ENTMODE 0x06 // 0b0000_0110
#define DISPON 0x0C // DispON, cursor off, blink off
#define DISPOFF 0x08
#define FUNCTIONSET 0X28 // 4bit, 2line, 5 x 8 font size

int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

