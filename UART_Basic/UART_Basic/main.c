/* This is the basic program that it echos back received data from computer
 * to the same computer.
 */

/* <Baud rate equation>
 * Async 1x(U2Xn = 0) = 16MHz / (16 * Baud) - 1
 * Async 2X(U2Xn = 1) = 16MHz / (8 * Baud) -1
 * Sync = 16MHz / (2 * Baud) -1
*/
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// set commnication mode and its control
void UART1_init(void) //���巹��Ʈ ����(���۸�� ���) -> ���۸��(2���) ���� -> ����/�񵿱�, ���� �����ͺ�Ʈ(�ַ�8), �и�Ƽ(�ַ�0), �����Ʈ(�ַ�1��Ʈ)����
					  // -> �ۼ��� Reg. �ο��̺�
{
	// set Baud Rate: the rate varies depending on the transmit mode
	// UBRR(USART Baud Rate Reg) consists of two byte using 12 bits.
	// UBRRnH uses 4 bits; UBRRnL 8bits.
	UBRR1H = 0x00; // empty bits(don't use any bits in the upper register)
	UBRR1L = 207; // 2x Mode, 9600Baud Rate, precisely 207.3333333([16MHz / (8 * 9600)] -1)

	// set 2x Mode
	UCSR1A |= (1 << U2X1); // UCSR1A |= 0x02

	// Async Mode, 8bit data, no-parity, 1bit-stop mode
	UCSR1C |= (1 << UCSZ11) | (1 << UCSZ10); // 0x06, UCSZnx represent 8bit data
	
	// RX/TX enable
	UCSR1B |= (1 << RXEN1) | (1 << TXEN1); // 0x18
}
void UART1_transmit(char data) //�����͸� �Ķ���ͷ� �����Ͽ� �۽Ű��ɻ��°� �Ǹ�(�׶����� ��� �ϴٰ�), UDR1�� �����͸� ��
{
	while(!(UCSR1A & (1 << UDRE1))); // wait for transmit enable
	UDR1 = data;
}

unsigned char UART1_receive(void) // ���ŵǴ� �����͸� ���� ���� ���±��� ��� �ϴٰ�(���ɻ��°� �Ǹ�), UDR1�� �ִ� �۽ŵ� �����͸� ����
{
	while(!(UCSR1A & (1 << TXC1)));
	return UDR1;
}

int main(void)
{
	UART1_init(); // ��� �ʱ�ȭ
	while(1)
	{
		UART1_transmit(UART1_receive()); // ������� ���� ���� �ٽ� ����
	}
	return 0;
}