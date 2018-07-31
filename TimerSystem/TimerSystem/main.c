// dynamic display: 5ms
// count-up timer: 100ms
// blink on-off: 500ms respectively, total 1000ms
// 
// TODO: try to use struct for tick for simplicity instead
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
///////////////////////////////////////////////////////////////////////////////
#define F_CPU 16000000UL
#define FND_DATA PORTA //[7:0]
#define SW_PORT1 PORTD //2, 3
#define SW_PORT2 PORTE //6, 7
#define FND_SEL PORTC // PC3, 2, 1, 0
///////////////////////////////////////////////////////////////////////////////
typedef unsigned char unit8_t;
typedef unsigned int unit16_t;

enum {TIMER_SET = 1, COUNT_UP};

unit8_t mode;
unit8_t digit = 0; // digit of fnd
unit8_t fndNum[4] = {0, 0, 0, 0}; // initial value = 0, 0, 0, 0
unit8_t FND_Pattern[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
unit16_t fndCnt = 0; // change fndNum into one number
unit16_t time_count = 0; // counter-up var
unit16_t tick0_blink = 0; // for blinking(.5s)
//unit16_t tick2_debounce = 0; // for SW de-bounce
unit16_t tick3_fnd_sel = 0; // for dynamic display

void init(void);
void fnd_blink(void);
void fnd_display(void);

ISR(INT2_vect) // sw1: enter set mode -> blink the (first) digit
{
    mode = TIMER_SET; // blink set-up mode
    digit = 0;
    
    // turn on timer0 for blinking
    TCCR0 |= (0x01 << WGM01) | (0x01 << CS02); // 64 pre-scale
    OCR0 = 249; // 1ms
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
    if (fndNum[digit] > 9) fndNum[digit] = 0;
    else fndNum[digit]++;
    
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
// T/C for blinking
ISR(TIMER0_COMP_vect) // 1 ms -> "1sec"(0.5 off + 0.5 on)
{
    if (mode == TIMER_SET) tick0_blink++;
    if (tick0_blink > 1000) tick0_blink = 0;
}
/*
// TODO: optional T/C, incorporate it into timer0 to reduce the num of timer
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
}*/
// T/C for count-up
ISR(TIMER1_COMPA_vect) // 100 ms
{
    if (mode == COUNT_UP)
    {
        time_count++;
        if (time_count == 10000) time_count = 0; // time count reset
    }
}
// T/C for dynamic display for both mode
ISR(TIMER3_COMPA_vect) // 5ms
{
    if (tick3_fnd_sel < 4) tick3_fnd_sel++;
    else tick3_fnd_sel = 0;
}



void main(void)
{
    init();
    
    while (1)
    {
        sprintf(); // convert array into count var
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
    SW_PORT1 &= ~0xF3; // internal pull-up resist
    SW_PORT2 &= ~0x3F;
    
    // EXINT setup
    EICRA = (0x01 << ISC21) | (0x01 << ISC31); // falling edge detection
    EICRB = (0x01 << ISC61) | (0x01 << ISC71);
    EIMSK = (0x01 << INT2) | (0x01 << INT3) | (0x01 << INT6) | (0x01 << INT7); // EXINT[2, 3, 6, 7] enable
    
    
    // timer1_COMPA: 100ms for count-up number
    TCCR1B = (0x01 << WGM12) | (0x01 << CS12); //CTC, 256 pre-scale 
    OCR1A = 6249;
    TIMSK = (0x01 << OCIE1A);
    
    // timer3_COMPA: 5ms for dynamic display
    TCCR3B
    OCR3A
    ETIMSK
    // GRBL INT enable
    SREG |= 0x80;
}

void fnd_blink(void)
{
    if (tick0_blink <= 500) PORTC &= ~(0x01 << digit); //현재 자릿수에 해당하는 fnd 끔
}

void fnd_display(void)
{
    // TODO
}