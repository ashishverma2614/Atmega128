/*LIB for common initial setting for AVR project
 **/

#ifndef COMMON_H_
#define COMMON_H_
#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_9600_1X ((F_CPU) / (16) / (BAUD - 1)) // 9600 bps at 1x mode
                                                   // UBRR = 103 -> %ERR=0.16

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UART.h" // for debug on serial monitor

#endif /* COMMON_H_ */