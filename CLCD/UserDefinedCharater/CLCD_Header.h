#ifndef CLCD_HEADER_H_
#define CLCD_HEADER_H_

typedef unsigned char u_char;
/**************************************************************/
#define ALLCLR 0x01 // Clear Display
#define HOME 0x02 // Return Home
#define ENTMODE 0x06 // ENTRY MODE Set
#define DISPON 0x0C // Display ON(cursor, blink off)
#define DISPOFF 0x08 // Display OFF(cursor, blink off)
#define	FUNCTIONSET 0X28 // 4bit-length, 2lines, 5X7 dots
#define LINE2 0XC0 // move to 2ed line(0b1100_0000 = 0b1000_0000 + 0b0100_0000(40H)) 
#define RSHIFT 0x1C // display with right shift(shift entire display to the right, cursor follows the display shift)
#define LSHIFT 0X18 // display with left shift(shift entire display to the left, cursor follows the display shift)

// E connectec to PC2
#define ENH PORTC |= 0x04 // enable HIGH(Read)
#define ENL PORTC &= 0xFB // enable Low(Write) = PORTC &= ~(0x04)



/**************************************************************/
// function prototypes

void LCD_INIT(void); // setting
void Disp_String(const char * str); // display string
void Busy(void); // 2ms delay
void CMD(u_char); // to get command
void Send_Data(u_char); // send character to the LCD
/**************************************************************/
#endif /* CLCD_HEADER_H_ */