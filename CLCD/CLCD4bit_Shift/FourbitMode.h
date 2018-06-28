#ifndef 4BITMODE_H_
#define 4BITMODE_H_

typedef unsigned char u_char;
/**************************************************************/
#define	FUNCTIONSET 0X28
#define ENTMODE 0X06 // ENTRY MODE
#define ALLCLR 0x01 // All Clear
#define DISPON 0x0C // Display On
#define LINE2 0XC0 // move to 2ed line
#define ENH PORTC |= 0x04 // enable HIGH
#define ENL PORTC &= 0xFB // enable Low
#define HOME 0X02 // move cursor home
#define RSHIFT 0x1C // display right shift
#define DISPOFF 0x80 // Display off
/**************************************************************/
// function prototypes

void LCD_INIT(void); // setting
void Disp_String(const char * str); // display string
void Busy(void); // 2ms delay
void CMD(u_char); // to get command
void Send_Data(u_char); // send character to the LCD
/**************************************************************/



// function implementations
void LCD_INIT(void) // refer to the data sheet
{
	DDRC = 0xFF;
	PORTC = 0x00;
	_delay_ms(15);
	CMD(0x20);
	_delay_ms(5);
	CMD(0x20);
	_delay_us(100);
	CMD(0x20);
	CMD(FUNCTIONSET);
	CMD(DISPON);
	CMD(ALLCLR);
	CMD(ENTMODE);
}

void Disp_String(const char * str)
{
	//char i = 0;
	while(*str != 0) Send_Data(*str++);
}

void CMD(u_char byte) // cmd set
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

void Send_Data(u_char byte)
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


void Busy(void)
{
	_delay_ms(2);
}




#endif /* 4BITMODE_H_ */