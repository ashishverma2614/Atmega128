#include <avr/io.h>
#include <util/delay.h>

// byte데이터 전송(문자열 전송에서 호출)
void serial_transmit(unsigned char data)
{
	while( !( UCSR1A & (1 << UDRE1)) );

	UDR1 = data;
}

// 문자열 전송(byte데이터 전송 사용)
void serial_string(char *str)
{
	while( *str )	serial_transmit(*str++);
}

// 시리얼 데이터 수신
unsigned char serial_receive(void)
{
	while ( !(UCSR1A & (1 << RXC1)) );

	return UDR1;
}
// 시리얼 통신 초기화 함수
void serial_init(unsigned int baudrate)
{
	UCSR1A = 0x00;
	UCSR1B = 0x08;
	UCSR1C = 0x06;
	UBRR1H = baudrate >> 8;
	UBRR1L = baudrate & 0x0FF;
}