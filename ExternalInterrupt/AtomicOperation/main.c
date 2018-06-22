/* It is about 8-bit micro-controler, which means that it operates basically by 8 bits(1 Byte).
 * To put it another way, when it reads 2byte data, and there is interrupt while reading lower
 * after upper higher 8bit reading, there should be a data corruption. Hence, 
 * in order to prevent it, we should guarantee enough time to read 2 byte data completely 
 * by keeping interrupt from arising during such process.
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include "init.h"

volatile unsigned int num = 0x0000; // 16bit(2byte), which is possible to break its data
									// num is not changed in the main loop, so it has been declared as volatile
unsigned char intFlag = 0x00;

ISR(INT2_vect)
{
	intFlag |= 0x01; // becomes 1
	num++;
	if (num > 0x80)
		num = 0x00;
}

int main(void)
{
	unsigned int localNum; // localize global var
	INIT(); // already defined in the header file
	
    while (1) 
    {
		/* start ATOMIC_BLOCK */
		
		SREG |= 0x00; // off GBL INT: while reading 16bit int num, turn-off external interrupt
		localNum = num;
		SREG |= 0x80; // restore INT: restore interrupt
		
		/* end ATOMIC_BLOCK */
		
		if (intFlag == 1) 
			PORTB = localNum;
    }
}

