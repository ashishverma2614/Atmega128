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
void UART1_init(void) //보드레이트 설정(전송모드 고려) -> 전송모드(2배속) 설정 -> 동기/비동기, 전송 데이터비트(주로8), 패리티(주로0), 스톱비트(주로1비트)설정
					  // -> 송수신 Reg. 인에이블
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
void UART1_transmit(char data) //데이터를 파라미터로 전달하여 송신가능상태가 되면(그때까지 대기 하다가), UDR1에 데이터를 씀
{
	while(!(UCSR1A & (1 << UDRE1))); // wait for transmit enable
	UDR1 = data;
}

unsigned char UART1_receive(void) // 수신되는 데이터를 수신 가능 상태까지 대기 하다가(가능상태가 되면), UDR1에 있는 송신된 데이터를 리턴
{
	while(!(UCSR1A & (1 << TXC1)));
	return UDR1;
}

int main(void)
{
	UART1_init(); // 통신 초기화
	while(1)
	{
		UART1_transmit(UART1_receive()); // 통신으로 받은 것을 다시 전송
	}
	return 0;
}