/* 폴링방식으로 수신 데이터가 있는지를 수신 함수를 루프내에서 계속 호출하여 검사하여 수신의 내용에 따라
 * LED를 켜고 끄는 예제
*/

#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

char RX_char(void) // 문자를 수신하여 UDR0에 저장
{
	while (!(UCSR0A & (0x01 << RXC0))); // RXC가 0이면 버퍼가 비어 있어 수신대기, 1이 되면 읽을 내용이 있으므로 루프 탈출 -> 버퍼에 있는 내용을 반환(하여 메인에서 읽어 옴)
	return UDR0;
}

void TX_char(char tx_data)
{
	while(!(UCSR0A & (1 << UDRE0))); // UDR이 비어 송신 가능상태가 되면 1 이 된다->루프 탈출
	UDR0 = tx_data; //전송할 새로운 데이터를 채워 넣는다(채워 넣으면 전송), UDR에 데이터가 있는 상황에서는 새로운 데이터를 전송할 수 없다.
}


int main(void)
{
	char data;
	
	// port setting
	DDRB = 0xFF;
	PORTB = 0x00; // 데이터 전송 받기 전-> led 꺼진상태
	
	// UART setting
	UBRR0H = 0x00; //하위비트에서 보드레이트 값 수용가능(상위 4비트로는 207을 수용할 수 없음)
	UBRR0L = 207; // 9600bps(2배속 모드)
	UCSR0A = (0x01 << U2X0); // 2배속 모드
	UCSR0B = (0x01 << RXEN0) | (0x01 << TXEN0); // 송수신 기능 활성화
	UCSR0C = (0x01 << UCSZ01) | (0x01 << UCSZ00); // 비동기, 패리티 없음, 1 정지비트, 8비트 송수신
	
    while (1) 
    {
		data = RX_char(); // 수신 할 내용이 있는지 계속 함수를 검사하여 있으면(대기 상태를 지나면), 임시 변수에 저장
		
		if (data == 'Y' || data == 'y')
		{
			PORTB = 0xFF; // 포트를 켠다
			
			// 문자 수신이 올바르게 되었는지를 포트를 통해 확인하고, 시리얼에 출력
			TX_char('_'); // 문자송신
			TX_char('Y');
			TX_char('E');
			TX_char('S');
			TX_char('\r');
		}
		else if (data == 'N' || data == 'n')
		{
			PORTB = 0x00; //포트를 끈다
			
			// 문자 수신이 올바르게 되었는지는 포트를 통해 확인하고, 시리얼에 확인 문자 출력
			TX_char('_'); // 문자송신
			TX_char('N');
			TX_char('O');
			TX_char('\r');
		}
    }
}

