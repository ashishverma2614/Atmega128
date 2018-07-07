#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define PORT_DATA PORTD // portD for data pin
#define PORT_CONTROL PORTC // portC for instruction pin
#define DDR_DATA DDRD // direction of portD
#define DDR_CONTROL DDRC // direction of portC

#define RS_PIN 0 // bit num of RS pin
#define RW_PIN 1 // bit num of RW pin
#define E_PIN 2 // bit num of E pin

#define COMMAND_CLEAR_DISPLAY 0x01
#define COMMAND_8BIT_MODE 0x38 // 8bit, 2-line, 5 x 8 font
#define COMMAND_4BIT_MODE 0X28 // 4bit, 2-line, 5 x 8 font

#define COMMAND_DISPLAY_ON_OFF_BIT 2 //D
#define COMMAND_CURSOR_ON_OFF_BIT 1 //C
#define COMMAND_BLINK_ON_OFF_BIT 0 //B

typedef unsigned char unit8_t;

void LCD_pulse_enable(void) // operates at falling edge
{
	PORT_CONTROL |= (1 << E_PIN); // E = HIGH
	_delay_us(1);
	PORT_CONTROL &= ~(1 << E_PIN); // E = LOW
	_delay_us(1);
}

void LCD_write_data(unit8_t data) // receive 8bits of info as param
{
	PORT_CONTROL |= (1 << RS_PIN); // enable data Reg.(1)
	PORT_DATA = data;
	LCD_pulse_enable();
	_delay_ms(2);
}

void LCD_write_command(unit8_t command)
{
	PORT_CONTROL &= ~(1 << RS_PIN); // enable instruction Reg.(0)
	PORT_DATA = command;
	LCD_pulse_enable();
	_delay_ms(2);
}

void LCD_clear(void)
{
	LCD_write_command(COMMAND_CLEAR_DISPLAY); // write 0x01 to clear the screen
	_delay_ms(2);
}

void LCD_INIT(void)
{
	_delay_ms(50); // ready duration
	
	DDR_DATA = 0xFF;
	PORT_DATA = 0x00; 
	DDR_CONTROL |= (1 << RS_PIN) | (1 << RW_PIN) | (1 << E_PIN);
	
	// set RW pin as write mode by producing LOW
	PORT_CONTROL &= ~(1 << RW_PIN);
	
	LCD_write_command(COMMAND_8BIT_MODE); // 8bit mode
	
	// display on/off control
	// lcd: on, cursor: off, blink: off
	unit8_t command = 0x80 | (1 << COMMAND_DISPLAY_ON_OFF_BIT);
	LCD_write_command(command);
	
	LCD_clear(); // clear the screen
	
	//Entry mode set
	// after displaying, move cursor to the right side, which means to increase the address of DDRAM
	// no screen moving
	LCD_write_command(0x06);
}

void LCD_write_string(char * string)
{
	unit8_t i;
	for (i = 0; string[i]; i++)
		LCD_write_data(string[i]);
}

void LCD_goto_XY(unit8_t row, unit8_t col)
{
	col %= 16;
	row %= 2;
	
	// start address of the first line: 0x00, second line: 0x40
	unit8_t address = (0x40 * row) + col;
	unit8_t command = 0x80 + address;
	
	LCD_write_command(command); // move cursor
}

int main(void)
{
	LCD_INIT();
	LCD_write_string("Hello World");
	_delay_ms(1000); // wait for 1 sec
	LCD_clear();
	
	//	
    /* Replace with your application code */
    while (1) 
    {
    }
}

