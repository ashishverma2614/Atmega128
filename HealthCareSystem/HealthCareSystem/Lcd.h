#ifndef	__LCD_H
#define	__LCD_H


#define LCD_ON		0x0C
#define LCD_OFF		0x08
#define LCD_CLEAR		0x01
#define LCD_CURSOR	0x0F


void lcd_busycheck(void);
unsigned char lcd_command_read(void);
void lcd_command_write(unsigned char cmd);
unsigned char lcd_data_read(void);
void lcd_data_write(unsigned char data);
void lcd_string(char *str);
void lcd_gotoxy(char x, char y);
void lcd_init(void);


#endif	// __LCD_H
