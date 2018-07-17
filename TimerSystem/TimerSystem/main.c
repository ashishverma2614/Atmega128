
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
///////////////////////////////////////////////////////////////////////////////
#define FND_PORT PORTA //[7:0]
#define SW_PORT1 PORTD //2, 3
#define SW_PORT2 PORTE //6, 7
#define FND1 PORTC = 0X08 // PC3
#define FND2 PORTC = 0x04 // PC2
#define FND3 PORTC = 0x02 // PC1
#define FND4 PORTC = 0x01 // PC0
#define FND_WRITE PORTA //[7:0]

///////////////////////////////////////////////////////////////////////////////
enum {TIMER_SET = 1, COUNT_UP};

unsigned char mode;
unsigned char digit = 0; // digit of fnd
unsigned char fndNum[4] = {0, 0, 0, 0}; // initial value = 0, 0, 0, 0
unsigned char FND[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
unsigned int fndCnt = 0; // change fndNum into one number
unsigned int tick0_blink = 0; // for blinking(.5s)
unsigned int tick1_fnd_sel = 0; // for dynamic display
unsigned int tick2_debounce = 0; // for SW de-bounce

ISR(INT2_vect) // sw1: enter set mode -> blink the (first) digit
{
    mode = TIMER_SET; // blink set-up mode
    digit = 0;
    
    // turn on timer0 for blinking
    TCCR0 |= (0x01 << CS02) | (0x01 << CS01) | (0x01 << CS00); // 1024 pre-scale
    OCR0 = 128;
    TIMSK |= (0x01 << OCIE0);
    
    // chattering de-bounce code
    
}
ISR(INT3_vect) // sw2: change digit
{
    //digit++;
    if (digit == 4) digit = 0;
    else digit++;
    
    // chattering de-bounce code
}
ISR(INT6_vect) // sw3: count up the current digit
{
    if (fndNum[digit - 1] == 10) fndNum[digit - 1] = 0;
    else fndNum[digit - 1]++;
    
    // convert elements in the array to one integer number
    fndCnt = fndNum[3] * 1000 + fndNum[2] * 100 + fndNum[1] * 10 + fndNum[0];
    
    // chattering de-bounce code
}
ISR(INT7_vect) //sw4: count-up mode
{
    mode = COUNT_UP; // enter count-up mode, no-blinking
    
    // turn off timer0
    TCCR0 |= 0x00;
    TCNT0 |= 0x00;
    TIMSK |= 0x00;
    // chattering de-bounce code
    
    
}

ISR(TIMER0_COMP_vect) // for 0.5sec "blinking"
{
    tick0_blink++;
    TCNT0 = 0;
    if (tick0_blink == 64 && mode == TIMER_SET)
    {
        tick0_blink = 0;
        PORTC &= ~(0x01 << (3 - digit)); // turn off fnd of the current digit
    }
}

// TODO: optional T/C
ISR(TIMER2_OVF_vect) // SW de-bounce
{
    tick2_debounce++;
    if (tick2_debounce == 30)
    {
        tick2_debounce = 0;
        EIMSK = 0xCC;
        TCCR2 = 0x00;
        EIFR = 0xCC;
    }
}

ISR(TIMER1_COMPA_vect) // for dynamic display at a high speed
{
    
}
ISR(TIMER1_COMPB_vect) // for count calculation
{
    
}

void init(void);

int main(void)
{
    
    while (1)
    {
    }
}

void init(void)
{
    // fnd port
    DDRA = 0xFF;
    DDRC = 0x0F;
    
    // SW port
    DDRD = 0xF3;
    DDRE = 0x3F;
    PORTD &= ~0xF3; // internal pull-up resist
    PORTE &= ~0x3F;
    
    // EXINT setup
    EICRA = (0x01 << ISC21) | (0x01 << ISC31); // falling edge detection
    EICRB = (0x01 << ISC61) | (0x01 << ISC71);
    EIMSK = (0x01 << INT2) | (0x01 << INT3) | (0x01 << INT6) | (0x01 << INT7); // EXINT[2, 3, 6, 7] enable
    
    
    // timer0 ovf setting
    
    
    // GRBL INT enable
    SREG |= 0x80;
}

