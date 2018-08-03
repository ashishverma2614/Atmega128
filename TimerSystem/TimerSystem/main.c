// dynamic display: 5ms
// count-up timer: 100ms
// blink on-off: 500ms respectively, total 1000ms
// 
// personal mission: (1)try to reduce the num of variables as much as possible
//                   (2)make ISR as short as possible

#include "common.h"
///////////////////////////////////////////////////////////////////////////////
#define TRUE 1
#define FALSE 0
#define FND_DATA PORTA
#define FND_CONTROL PORTC
/* printf() on serial monitor for debug */
FILE OUTPUT = FDEV_SETUP_STREAM(UART0_Putch, NULL, _FDEV_SETUP_WRITE);
///////////////////////////////////////////////////////////////////////////////

/* 1. Data Field */
enum MODE { TIMER_SET = 1, COUNT_UP };
enum MODE mode = TIMER_SET; // start mode

unsigned char fndPattern[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90}; // 0 ~ 9

volatile unsigned char fndIndex = 0; // 0 ~ 3
volatile unsigned char fndNum[4] = {'0', '0', '0', '0'};
volatile unsigned int fndStartNum = 0;
volatile unsigned char isTurnOn = FALSE;
volatile unsigned char isChangeFnd = FALSE;

///////////////////////////////////////////////////////////////////////////////
/* 2. Function prototypes */
void Debug(void);

void EXINT_init(void);
void fnd_init(void);
void key_init(void);
void timer_init(void);
void sys_init(void);

void timer0_init(void);
void timer0_off(void);
void timer1_init(void);
void timer3_init(void);

void disp_setMode_fnd(void);
void disp_countMode_fnd(void);

///////////////////////////////////////////////////////////////////////////////

/* 3. ISR */
ISR(INT2_vect) // key1: enter count set mode, blink the first position
{
	fndIndex = 0; // jump to the timer for blinking
	mode = TIMER_SET;
    
    /* For debug */
    Debug ();
    printf("%d \n", mode);
}
ISR(INT3_vect) // key2: change fnd blinking position
{
	fndIndex++;
	if (fndIndex > 3) fndIndex = 0; // reset index
    
    /* For debug */
    Debug ();
    printf("%d\n", fndIndex);
}
ISR(INT6_vect) // key3 : count-up fnd number at current position
{
	fndNum[fndIndex]++;
	if (fndNum[fndIndex] > '9') fndNum[fndIndex] = '0'; // reset value

    /* convert data to int for start count */
    // also possible to use atoi()
    fndStartNum = 1000 * (fndNum[3] - 0x30) + 100 * (fndNum[2] - 0x30) + 10 * (fndNum[1] - 0x30) + (fndNum[0] - 0x30);
    
    /* For debug */
    Debug ();
    printf("Stored fnd start number is: %d \n", fndStartNum);
}
ISR(INT7_vect)
{
	mode = COUNT_UP; // jump to count-up mode
    
    /* For debug */
    Debug ();
	printf("%d \n", mode);
}
///////////////////////////////////////////////////////////////////////////////
ISR(TIMER0_COMP_vect) // 1ms -> for 0.5sec interval blinking fnd
{
	static unsigned int tick0_blinking;
    
    tick0_blinking++;
    if (tick0_blinking <= 500) isTurnOn = TRUE;
    else if (tick0_blinking <= 1000) isTurnOn = FALSE;
    else tick0_blinking = 0;
}

ISR(TIMER1_COMPA_vect) // 1ms for 5ms dynamic display
{
    static unsigned char tick1_dynamicOp;
    
    tick1_dynamicOp++;
    if (tick1_dynamicOp <= 5) isChangeFnd = FALSE;
    else
    {
        isChangeFnd = TRUE;
        tick1_dynamicOp = 0;
    }
}
ISR(TIMER3_COMPA_vect) // for 100 ms count up
{  
}

/* 4. Main method */
int main(void)
{
	// 1. system setting
	sys_init ();
	// 2. system execution
	while(TRUE)
	{
		switch(mode)
		{
			case TIMER_SET:
			    // 1. turn on timer0 for blinking ON
			    timer0_init();
                
                // 2. display set mode fnd
                disp_setMode_fnd();
                
			    // 3. blink port
			    if (isTurnOn) // turn on the current port
                {
                    DDRA = 0xFF; // turn on output
                    PORTC = (0x01 << fndIndex);
                }
                else // turn off the current port
                {
                    PORTC = 0x00; // turn off fnd control port   
                    DDRA = 0x00; // turn off output
                }
			break;
            
            ///////////////////////////////////////////////////////////////////
			case COUNT_UP:
            // 1. turn off timer0 for blinking OFF
            timer0_off();
            
			// 2. turn on timer for counting-up
			
			
			break;
		}
	}
	return EXIT_SUCCESS;
}









///////////////////////////////////////////////////////////////////////////////
/*5. Function Implementation */

void Debug () // debug code using serial monitor and LED blinking
{
	UART0_Puts ("SW pressed!\r\n");
	DDRB = 0x01;
	PORTB = 0x01;
	_delay_ms(250);
	PORTB = 0x00;
}
void EXINT_init()
{
	DDRD = 0b11110011; // PIND[2:3] as input
	DDRE = 0b00111111; // PINE[6:7] as input
	// internal pull-up resistor
	PORTD = 0b00001100; 
	PORTE = 0b11000000;

	// Reg. setting
	EICRA = (1 << ISC21) + (1 << ISC31); // falling edge trigger detection
	EICRB = (1 << ISC61) + (1 << ISC71);
	EIMSK = (1 << INT2) + (1 << INT3) + (1 << INT6) + (1 << INT7);
}
void fnd_init ()
{
	DDRA = 0xFF;
	DDRC = 0x0F;
}
void sys_init ()
{
	stdout = &OUTPUT;
	UART0_Init(UBRR_9600_1X); // for debug
	EXINT_init();
	fnd_init ();
	SREG |= 0x80; // GRBL INT enable
}
void timer0_init()
{
    TCCR0 |= (1 << WGM01) + (1 << CS02); // 64 pre-scale
    OCR0 = 249; // 1ms
    TIMSK |= (1 << OCIE0);
}
void timer0_off()
{
    TCCR0 |= 0x00;
    OCR0 = 0x00;
    TIMSK &= ~(1 << OCIE0);
}
void disp_setMode_fnd(void)
{
    switch(fndIndex)
    {
        case 0:
            PORTC = 0x01;
            PORTA = fndPattern[fndNum[0] - 0x30];
            break;
        case 1:
            PORTC = 0x02;
            PORTA = fndPattern[fndNum[1] - 0x30];
            break;
        case 2:
            PORTC = 0x04;
            PORTA = fndPattern[fndNum[2] - 0x30];
            break;
        case 3:
            PORTC = 0x08;
            PORTA = fndPattern[fndNum[3] - 0x30];
            break;
    }
}

void disp_countMode_fnd()
{
    
}