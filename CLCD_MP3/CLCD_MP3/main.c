#define F_CPU 16000000
#define BAUD 9600
#define MYUBRR ((F_CPU) / (16) / (BAUD - 1)) // 104.177 at 1x mode
/* Define directive for CLCD instruction */
#define FUNCTIONSET		0X28
#define ENTMODE			0X06 // ENTRY MODE
#define ALLCLR			0x01 // All Clear
#define DISPON			0x0C // Display On
#define LINE2			0XC0 // move to 2ed line
#define ENH				PORTC |= 0x04 // enable HIGH
#define ENL				PORTC &= 0xFB // enable Low
#define HOME				0X02 // move cursor home
#define RSHIFT			0x1C // display right shift
#define DISPOFF			0x80 // Display off
/* Define directive for MP3 */
#define CMD_NEXT			0x01
#define CMD_PRE			0x02
#define CMD_VOL_UP		0x04
#define CMD_VOL_DOWN		0x05
#define CMD_PLAY			0x0D
#define CMD_PAUSE		0x0E
#define CMD_STOP			0x16
#define CMD_TF			0x09
/**************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**************************************************************/
// printf/scanf for debug
//FILE OUTPUT = FDEV_SETUP_STREAM(UART1_transmit, NULL, _FDEV_SETUP_WRITE);
//FILE INPUT = FDEV_SETUP_STREAM(NULL, UART1_receive, _FDEV_SETUP_READ);

typedef unsigned char u_char;

volatile u_char location = 1; // current(selection) location centered on bracket
volatile u_char tick0; // for T/C OVF
volatile u_char isUp = 0; // flag for up/down
char temp_padding[16] = {0}; // temporary space to store string in compliance with format using sprintf()
char temp_bracket[16] = {0}; // same above, including bracket
char mp3_cmd_list[] = {
	CMD_PLAY, // 0
	CMD_STOP, // 1
	CMD_PAUSE, // 2
	CMD_NEXT, // 3
	CMD_PRE, // 4
};
char mp3_lcd_disp_list[][16] = {
	{"1_PLAY"},
	{"2_STOP"},
	{"3_PAUSE"},
	{"4_NEXT"},
	{"5_PRE"}
};

/**************************************************************/
// CLCD functions
void LCD_INIT(void); // setting
void Disp_LCD_String(char *); // display string
void Busy(void); // 2ms delay
void Send_LCD_Instruction(u_char); // to get command
void Send_LCD_Data(u_char); // send character to the LCD

// MP3 and UART functions
void Setting(void);
void UART1_Putch(char);
void Send_MP3_Command(unsigned char);
/**************************************************************/
ISR(TIMER0_OVF_vect) // de-bouncing
{
	++tick0;
	if (tick0 == 10)
	{
		tick0 = 0;
		EIMSK = 0b00010011; // 0, 1, 4
		TCCR0 = 0;
		EIFR = 1;
	}
}
ISR(INT0_vect) // SW1(PD0) : down-scroll
{
	isUp = 0;
	++location; // 1, 2, 3, 4, 5, 1
	if (location > 5)
	location = 1;
	TCCR0 = 1;
	TCNT0 = 0;
	EIMSK = 0;

}
ISR(INT1_vect) // SW2(PD1) : up-scroll
{
	isUp = 1;
	--location; // 1, 5, 4, 3, 2, 1
	if (location < 1)
	location = 5;
	TCCR0 = 1;
	TCNT0 = 0;
	EIMSK = 0;

}
ISR(INT4_vect) // SW3(PE4): send cmd based on the current location
{
	Send_MP3_Command(mp3_cmd_list[(location - 1)]);
	TCCR0 = 1;
	TCNT0 = 0;
	EIMSK = 0;
}
/**************************************************************/

int main(void)
{
	//stdout = &OUTPUT;
	//stdin = &INPUT;
	
	LCD_INIT();
	Setting();
	_delay_ms(200);
	Send_LCD_Data(CMD_TF); // check source of Music
	
	while (1)
	{
		switch(isUp)
		{
			case 0:
			if (location < 5)
			{
				Send_LCD_Instruction(HOME);
				sprintf(temp_bracket, "[%s]", mp3_lcd_disp_list[(location - 1)]);
				//printf(temp_bracket);
				sprintf(temp_padding, " %-14s", mp3_lcd_disp_list[location]); // padding white space by using format specifier in sprintf(), left alignment within 16 spaces
				// printf(temp);
				Disp_LCD_String(temp_bracket);
				Send_LCD_Instruction(LINE2);
				Disp_LCD_String(temp_padding);
			}
			else
			{
				Send_LCD_Instruction(HOME);
				sprintf(temp_bracket, "[%s]", mp3_lcd_disp_list[(location - 1)]);
				sprintf(temp_padding, " %-14s", mp3_lcd_disp_list[(location - 2)]);
				Disp_LCD_String(temp_padding);
				Send_LCD_Instruction(LINE2);
				Disp_LCD_String(temp_bracket);
			}
			break;
			
			case 1:
			if (location > 1)
			{
				Send_LCD_Instruction(HOME);
				sprintf(temp_bracket, "[%s]", mp3_lcd_disp_list[(location - 1)]);
				sprintf(temp_padding, " %-14s", mp3_lcd_disp_list[(location - 2)]);
				Disp_LCD_String(temp_padding);
				Send_LCD_Instruction(LINE2);
				Disp_LCD_String(temp_bracket);
			}
			else
			{
				Send_LCD_Instruction(HOME);
				sprintf(temp_bracket, "[%s]", mp3_lcd_disp_list[(location - 1)]);
				sprintf(temp_padding, " %-14s", mp3_lcd_disp_list[location]);
				Disp_LCD_String(temp_bracket);
				Send_LCD_Instruction(LINE2);
				Disp_LCD_String(temp_padding);
			}
			break;
		}
	}
	return EXIT_SUCCESS;
}



/**************************************************************/
void LCD_INIT(void) // initial LCD setting
{
	DDRC = 0xFF;
	PORTC = 0x00;
	_delay_ms(15);
	Send_LCD_Instruction(0x20);
	_delay_ms(5);
	Send_LCD_Instruction(0x20);
	_delay_us(100);
	Send_LCD_Instruction(0x20);
	Send_LCD_Instruction(FUNCTIONSET);
	Send_LCD_Instruction(DISPON);
	Send_LCD_Instruction(ALLCLR);
	Send_LCD_Instruction(ENTMODE);
}

void Disp_LCD_String(char *str) // display string on LCD using Send data() func
{
	//u_char i = 0;
	while(*str != 0) Send_LCD_Data(*str++);
}

void Send_LCD_Data(u_char byte) // send data string
{
	Busy();


	// send upper NIBL
	PORTC = 0x01; // RS = 1, RW = 0
	PORTC |= (byte & 0xF0);
	_delay_us(1);
	ENH; // E = 1
	_delay_us(1);
	ENL; // E = 0

	// send lower NIBL
	PORTC = 0x01; // RS = 1, RW = 0
	PORTC |= (byte << 4);
	_delay_us(1);
	ENH; // E = 1
	_delay_us(1);
	ENL; // E = 0
}

void Send_LCD_Instruction(u_char byte) // send command for instruction
{
	Busy(); // delay for stable start

	// instruction for upper 4bits
	PORTC = 0x00; // RS = 0, RW = 0
	PORTC |= (byte & 0xF0); // masking upper 4bits

	_delay_us(1);
	ENH; // E = 1

	_delay_us(1);
	ENL; // E = 0


	// instruction for lower 4bits
	PORTC = 0x00; // RW = 0, RS = 0
	PORTC |= (byte << 4); // masking lower 4bits

	_delay_us(1);
	ENH; // E = 1

	_delay_us(1);
	ENL; // E = 0
}

void Busy(void)
{
	_delay_ms(2);
}

void Setting(void)
{
	DDRD = 0x00;
	PORTD = 0b00000011; // internal pull-up resist
	DDRE = 0x00;
	PORTE = 0b00010000;
	
	/* external interrupt setting */
	EIMSK = (0x01 << INT4) | (0x01 << INT1) | (0x01 << INT0);
	EICRA = (0x01 << ISC01) | (0x01 << ISC11); // falling edge trigger detection
	EICRB = (0x01 << ISC41);
	
	
	/* UART1 setting */
	// set Baud Rate
	UBRR1H = (unsigned char)(MYUBRR >> 8);
	UBRR1L = (unsigned char) MYUBRR;
	// set frame format(Asynchronous, no-parity, 1-stop bit, 8bit data frame)
	// [IDLE(HIGH)] - [st(LOW)] - [8bits data] - [no-parity] -[sp(HIGH)] - st/IDLE
	UCSR1B = (0x01 << TXEN1); // TX enable
	UCSR1C = (0x01 << UCSZ11) | (0x01 << UCSZ10);
	
	TCCR0 = 0x00;
	TIMSK = 0x01;
	TCNT0 = 0x00;
	
	SREG |= 0x80; // global interrupt enable
}

void UART1_Putch(char ch) // TX of UART1
{
	while(!(UCSR1A & 0x20));

	UDR1 = ch;
}

void Send_MP3_Command(const u_char cmd)
{
	unsigned int checksum = 0; // check if the data 0f RX/TX is corrupted(2byte)
	//char temp[20];


	// Send $S-VER-Len-CMD-Feedback-para1-para2
	UART1_Putch(0x7E); //$S(start)
	UART1_Putch(0xFF); // VER
	UART1_Putch(0x06); // Len
	UART1_Putch(cmd);
	UART1_Putch(0x00); // feedback
	UART1_Putch(0x00); // para1(High), para2(Low)

	// calc check-sum
	if(cmd != CMD_TF){ // if there is not playback source
		UART1_Putch(0x00);
		checksum = 0 - (0xFF + 0x06 + cmd);
	}
	else{ // if there is a playback source
		UART1_Putch(0x02); // designate a playback source: TF(SD) Card
		checksum = 0 - (0xFF + 0x06 + cmd + 0x02);
	}
	
	// send check-sum
	UART1_Putch((char)(checksum >> 8)); // high byte of check-sum
	UART1_Putch(checksum & 0xFF); // low byte of check sum
	// send $O
	UART1_Putch(0xEF); // end bit

}
