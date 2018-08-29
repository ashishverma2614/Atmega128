#include <avr/io.h>
#include <stdio.h> // to use sprintf()

void USART0_Init(unsigned int ubrr)
{
    UCSR0A = 0x00;
    UCSR0B = (0x01 << RXCIE0) | (0x01 << RXEN0) | (0x01 << TXEN0);
    UCSR0C = (0x01 << UCSZ01) | (0x01 << UCSZ00);
    UBRR0H = (unsigned char)(ubrr >> 8); // mask upper NIBL
    UBRR0L = (unsigned char)ubrr; // upper NIBL truncated
    
}
void USART1_Init(unsigned int ubrr)
{
    UCSR1A = 0x00;
    UCSR0B = (0x01 << RXCIE1) | (0x01 << RXEN1) | (0x01 << TXEN1);
    UCSR1C = (0x01 << UCSZ11) | (0x01 << UCSZ10);
    UBRR1H = (unsigned char)(ubrr >>8);
    UBRR1L = (unsigned char)ubrr;
}
char USART0_GetCh(void)
{
    while(!(UCSR0A & (0x01 << RXC0)));
    return UDR0;
}
char USART1_GetCh(void)
{
    while(!(UCSR1A & (0x01 << RXC1)));
    return UDR1;
}
void USART0_GetS(char * str)
{
    static int i = 0;
    do 
    {
        str[i] = USART0_GetCh(); // accumulate each character in the array
    } while (str[i++] != '\r' && str[i++] != '\n');
    str[i - 1] = 0; // append NULL character
}
void USART1_GetS(char * str)
{
    static int i = 0;
    do 
    {
        str[i] = USART1_GetCh();
    } while (str[i++] != '\r' && str[i++] != '\n');
}
void USART0_PutCh(char ch)
{
    while (!(UCSR0A & (0x01 << UDRE0)));
    UDR0 = ch;
}
void USART1_PutCh(char * ch)
{
    while (!(UCSR1A & (0x01 << UDRE1)));
    UDR1 = ch;
}
void USART0_PutS(char * str)
{
    static int i = 0;
    while (str[i] != 0)
    {
        USART0_PutCh(str[i++]);
    }   
}
void USART1_PutS(char * str)
{
    static int i = 0;
    while(str[i] != 0)
    {
        USART1_PutCh(str[i++]);
    }
}