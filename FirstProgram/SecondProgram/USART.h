#ifndef USART_H_
#define USART_H_

void USART0_Init(unsigned int ubrr);
void USART1_Init(unsigned int ubrr);
char USART0_GetCh(void);
char USART1_GetCh(void);
void USART0_GetS(char * str);
void USART1_GetS(char * str);
void USART0_PutCh(char ch);
void USART1_PutCh(char * ch);
void USART0_PutS(char * str);
void USART1_PutS(char * str);

#endif /* USART_H_ */