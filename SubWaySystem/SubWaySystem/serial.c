#include <avr/io.h>
#include <util/delay.h>

// byte������ �ø��� ������ ����
void serial_transmit(unsigned char data)
{
	while( !( UCSR1A & (1 << UDRE1)) );

	UDR1 = data; // UDR�� �����͸� ���� ���� �� ������ �����ܰ�
}
// ���ڿ��� �ø��� ������ ����
void serial_string(char *str)
{
	while( *str )	serial_transmit(*str++);
}

// �ø��� ������ ����
unsigned char serial_receive(void)
{
	while ( !(UCSR1A & (1 << RXC1)) );

	return UDR1; // UDR�� ����� ���� �о� ���� ���� ������ �����ܰ�
}

// �ø��� ����� �ʱ�ȭ 
void serial_init(unsigned int baudrate)
{
	UCSR1A = 0x00;
	UCSR1B = 0x08;
	UCSR1C = 0x06;
    // ���� ����(H, L)
	UBRR1H = baudrate >> 8;
	UBRR1L = baudrate & 0x0FF;
}