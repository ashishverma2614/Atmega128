#ifndef CLCD_HEADER_H_
#define CLCD_HEADER_H_

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
#define RSHIFT 0x1C // display with right shift
#define LSHIFT 0X18 // display with left shift
#define DISPOFF 0x08 // Display off
/**************************************************************/
// function prototypes

void LCD_INIT(void); // setting
void Disp_String(const char * str); // display string
void Busy(void); // 2ms delay
void CMD(u_char); // to get command
void Send_Data(u_char); // send character to the LCD
/**************************************************************/



#endif /* CLCD_HEADER_H_ */