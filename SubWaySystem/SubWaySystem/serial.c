#include <avr/io.h>
#include <util/delay.h>

// byte단위의 시리얼 데이터 전송(write data to UDR)
void serial_transmit(unsigned char data)
{
	while( !( UCSR1A & (1 << UDRE1)) ); // UDR이 비었는지를 확인한다(비어 있지 않으면 중간에 데이터를 write할 수 없다)

	UDR1 = data; // UDR에 데이터를 쓰는 것이 곧 전송의 최종단계
}

// 문자열의 시리얼 데이터 전송
// 여러 바이트를 전달하기 위해, serial_transmit()함수를 내부에서 호출한다
void serial_string(char *str)
{
	while( *str )	serial_transmit(*str++);
}

// 시리얼 데이터 수신
unsigned char serial_receive(void)
{
	while ( !(UCSR1A & (1 << RXC1)) );

	return UDR1; // UDR에 저장된 값을 읽어 오는 것이 수신의 최종단계
}

// 시리얼 통신의 초기화 
void serial_init(unsigned int baudrate)
{
	UCSR1A = 0x00;
	UCSR1B = 0x08;
	UCSR1C = 0x06;
    // 보율 설정(H, L)
	UBRR1H = baudrate >> 8; // 상위 바이트의 값을 읽는다
	UBRR1L = baudrate & 0x0FF; // 하위 바이트의 값을 읽는다
}