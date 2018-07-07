#ifndef	__LED_H
#define	__LED_H


#define LED1	1
#define LED2	2
#define LED3	3
#define LED4	4
#define LED5	5
#define LED6	6
#define LED7	7
#define LED8	8
#define LED_ALL	0xFF


void led_light(unsigned char led);
void led_init(void);


#endif	// __LED_H
