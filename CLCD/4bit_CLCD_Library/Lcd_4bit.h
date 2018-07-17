#ifndef LCD_4BIT_H_
#define LCD_4BIT_H_

#ifndef F_CPU
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define PORT_LCD PORTC
#define DDR_LCD DDRC

#define RS_PIN 0
#define RW_PIN 1
#define E_PIN 2

#define CLR_DISP 0x01
#define MODE_EIGHT 0X38 // 8 bit, 2line, 5 X 8
#define MODE_FOUR 0X28 // 4bit, 2line, 5 X 8

// display on/off control(0b0000_1/D/C/B)
#define DISP_ON_OFF_BIT 2
#define CURSOR_ON_OFF_BIT 1
#define BLINK_ON_OFF_BIT 0

typedef unsigned char unit8_t;
extern unit8_t MODE; // to decide 4 or 8bit bus mode in the main

void lcd_pulse_enable(void);
void lcd_write_data(unit8_t);
void lcd_write_cmd(unit8_t);
void lcd_clear(void);
void lcd_init(void);
void lcd_write_string(char *);
void lcd_gotoXY(unit8_t, unit8_t);


#endif /* LCD_4BIT_H_ */