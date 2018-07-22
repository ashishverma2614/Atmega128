#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _USART_H_
#define _USART_H_

// <USART0>
// Input the Baud Rate value.
// 9600, 14400, 19200, 28800, 38400, 57600, 76800, 115200
void USART0_init (unsigned long int bitrate)
{
    UCSR0A = 0x00;
    UCSR0B = (1 << RXCIE0) + (1 << RXEN0) + (1 << TXEN0); //RX complete Interrupt Enable
    UCSR0C = (1 << UCSZ01) + (1 << UCSZ00);                //  Set number of Bit for Transmitting (8bit per once)
    
    if(bitrate==9600){
        UBRR0H = 0x00;
        UBRR0L = 103;
    }
    else if(bitrate==14400){
        UBRR0H = 0x00;
        UBRR0L = 68;
    }
    else if(bitrate==19200){
        UBRR0H = 0x00;
        UBRR0L = 51;
    }
    else if(bitrate==28800){
        UBRR0H = 0x00;
        UBRR0L = 34;
    }
    else if(bitrate==38400){
        UBRR0H = 0x00;
        UBRR0L = 25;
    }
    else if(bitrate==57600){
        UBRR0H = 0x00;
        UBRR0L = 16;
    }
    else if(bitrate==76800){
        UBRR0H = 0x00;
        UBRR0L = 12;
    }
    else if(bitrate==115200){
        UBRR0H = 0x00;
        UBRR0L = 8;
    }
    else;
}

// <USART1>
// Input the Baud Rate value.
// 9600, 14400, 19200, 28800, 38400, 57600, 76800, 115200
void USART1_init(unsigned long int bitrate)
{
    UCSR1A = 0x00;
    UCSR1B = (1<<RXEN1) + (1<<TXEN1) + (1<<RXCIE1); //RX complete Interrupt Enable
    UCSR1C = (1<<UCSZ11) + (1<<UCSZ10);              //  Set number of Bit for Transmitting (8bit per once)

    if(bitrate==9600){
        UBRR1H = 0x00;
        UBRR1L = 103;
    }
    else if(bitrate==14400){
        UBRR1H = 0x00;
        UBRR1L = 68;
    }
    else if(bitrate==19200){
        UBRR1H = 0x00;
        UBRR1L = 51;
    }
    else if(bitrate==28800){
        UBRR1H = 0x00;
        UBRR1L = 34;
    }
    else if(bitrate==38400){
        UBRR1H = 0x00;
        UBRR1L = 25;
    }
    else if(bitrate==57600){
        UBRR1H = 0x00;
        UBRR1L = 16;
    }
    else if(bitrate==76800){
        UBRR1H = 0x00;
        UBRR1L = 12;
    }
    else if(bitrate==115200){
        UBRR1H = 0x00;
        UBRR1L = 8;
    }
    else;
}

//**********************
// USART0 Functions.
char USART0_getch(void)
{
    while( !(UCSR0A & (1<<RXC0)) );
    return UDR0;
}

void USART0_putch(char ch)
{
    while( !(UCSR0A & (1<<UDRE0)) );
    UDR0 = ch;  
}

void USART0_puts(char * str)
{
    for(int i = 0; str[i] ; i++)
    {
        USART0_putch(str[i]);
    }
}

//********************
// USART1 Functions.
char USART1_getch(void)
{
    while( !(UCSR1A & (1<<RXC1)) );
    return UDR1;
}

void USART1_putch(char ch)
{
    while( !(UCSR1A & (1 << UDRE1)) )
    {
        UDR1 = ch;
    }
}

void USART1_puts(char str[])
{
    static int i=0;
    while( str[i++] )
    {
        USART1_putch( str[i++] );
    }
}
#endif
