/* <Connection: *refer to the schematic>
 * RS(4): PF0
 * R/W(5): PF1
 * E: PF2
 *
 * PA[0:7]: D[0:7]
 *
 * <Instruction Process: refer to timing diagram>
 * RS, R/W setting -> E Set -> Inputs instruction to DB[0:7] -> EClear(Write instruction) -> RS, R/W recover
 * <Data Writing Process> : same above except for RS setting(1)
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// RS(0), R/W(0), DB7(0), DB6(0), DB5(1), DB4(DL), DB3(N), DB2(F), DB1(-, don't care), DB0(-)
void LCD_CMD_Write(char cmd)
{
	PORTF = 0x00; // RS = (instruction write mode), R/W = 0(write), E = 0;
	_delay_us(1);
	PORTF ^= 0x04; // toggle E1 as 1
	PORTD = cmd; // send CMD
	_delay_us(1);
	PORTF ^= 0x04; // toggle E1 as 0
	_delay_ms(5);
}

void LCD_Data_Write(char data)
{
	PORTF = 0x01; // RS = (data write mode), R/W = 0(write), E = 0;
	_delay_us(1);
	PORTF ^= 0x04; // toggle E1 as 1
	PORTD = data; // send CMD
	_delay_us(1);
	PORTF ^= 0x04; // toggle E1 as 0
	_delay_ms(5);
}

void LCD_SetXY(char x, char y) // set coordinate of cursor(x-axis, y-axis)
{
	if (y == 0) LCD_CMD_Write(0x80 + x);
	else if(y == 1) LCD_CMD_Write(0xC0 + y);
}

void LCD_PutString(const char * str) // const for preventing data corruption
{
	while (*str != '\0') 
	{
		LCD_Data_Write(*str++);
	}
}

void LCD_INIT(void) // 8bit initialization
{	/* power-on */
	_delay_ms(50); // wait for more than 30ms after VDD rises to 4.5V
	
	//function set
	LCD_CMD_Write(0x38); // 0b0011_1000
	_delay_ms(1); // wait for more than 39us after function set
	
	//display on/off control
	LCD_CMD_Write(0x0C); // 0b0000_1100: entire display-on(1), no_cursor(0), no-blink(0) 
	_delay_ms(1); //wait for more than 39us after display on/off control
	
	// display clear
	LCD_CMD_Write(0x01);
	_delay_ms(1); // wait for more than 1.53ms after display clear
	
	// entry mode set
	LCD_CMD_Write(0x06); // 0b0000_0110: move to the right(1), no display move mode(0)
	
	/* End of initialization */
	
}

int main(void)
{
	DDRD = 0xFF; PORTD = 0xFF;
	DDRF = 0xFF; PORTF = 0x00;
	char * str1 = "Hello World";
	char  * str2 = "Good Morning";
	
	LCD_INIT();
	LCD_SetXY(0, 0);
	LCD_PutString(str1);
	LCD_SetXY(0, 1);
	LCD_PutString(str2);
    while (1) 
    {
    }
}

