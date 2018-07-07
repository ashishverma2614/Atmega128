#ifndef	__SERIAL_H
#define	__SERIAL_H

#define B2400		416
#define B4800		207
#define B9600		103
#define B19200		51
#define B38400		25
#define B76800		12

void serial_transmit(unsigned char data);
void serial_string(char *str);
unsigned char serial_receive(void);
void serial_init(unsigned int baudrate);

#endif	// __SERIAL_H