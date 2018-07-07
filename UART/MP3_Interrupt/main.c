#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>

// define each CMD list
#define CMD_NEXT 0x01
#define CMD_PRE 0x02
#define CMD_VOLUP 0x04
#define CMD_VOLDOWN 0x05
#define CMD_PLAY 0x0D
#define CMD_PAUSE 0x0E
#define CMD_STOP 0x16
#define CMD_TF 0x09 // playback source

// store each list in an array to handle at ease
char cmd_list[10] ={CMD_PLAY, CMD_STOP, CMD_PAUSE, CMD_NEXT, CMD_PRE, CMD_VOLUP, CMD_VOLDOWN, };
// string for menu
char * menu = "\r\n===========================\r\n= "
	"1 : Play Music          =\r\n= "
	"2 : Stop Music          =\r\n= 3 : Pause Music         =\r\n= "
	"4 : Next Music          =\r\n= 5 : Previous Music      =\r\n= "
	"6 : Volume UP           =\r\n= 7 : Volume DOWN         =\r\n"
	" ===========================\r\n"
	"\r\nCommand > ";

char buf[100], tx_buf[100], tx1_buf[100];
char rx_index = 0;
unsigned int tx_len = 0, tx_index = 0;
unsigned int tx1_len = 0, tx1_index = 0;
volatile char rx_complete = 0; // 1 for complete
///////////////////////////////////////////////////////////////////////////////

ISR(USART0_RX_vect) // UART0(Serial monitor): RX0 interrupt
{
	char ch = 0;
	ch = UDR0; // read UDR0 one by one
	buf[rx_index++] = ch; // stack ch on a buf array
	if (ch == '\r') // check the end of RX0 interrupt
	{
		buf[rx_index - 1] = 0; // append NULL at the last index of buf
		rx_index = 0; // restore rx_index
		rx_complete = 1; // flag for RX complete
	}
}

ISR(USART0_UDRE_vect) // UART0(Serial monitor): TX0 interrupt
{
	UDR0 = tx_buf[tx_index++]; // write the contents in TX0 buf to UDR0 for TX
	if (tx_index == tx_len)
	{
		UCSR0B &= ~(1 << UDRIE0); // USART Data Register Empty Interrupt Enable, 송신데이터 레지스터 준비 완료 인터럽트 발생을 허용
	}
}

ISR(USART1_UDRE_vect) // UART1(mp3): TX interrupt
{
	UDR1 = tx1_buf[tx1_index++];
	if (tx1_index == tx1_len)
	{
		UCSR1B &= ~(1 << UDRIE0); //ready for interrupt arising of TX data reg.
	}
}
///////////////////////////////////////////////////////////////////////////////
void UART0_Puts(char * str)
{
	tx_len = strlen(str);
	strcpy(tx_buf, str); // copy content of str to tx_buf
	tx_index = 0; // ?
	
	UCSR0B |= (1 << UDRIE0); // signal buf is full
}

void SendCommand(char cmd) // function for sending TX msg to mp3 abiding by protocol, enumerating 10 protocol
{
	unsigned int checksum = 0;
	tx1_buf[0] = 0x7E; //$S(start bit)
	tx1_buf[1] = 0xFF; //VER
	tx1_buf[2] = 0x06; // Len
	tx1_buf[3] = cmd;
	tx1_buf[4] = 0x00; // feedback
	tx1_buf[5] = 0x00; // param1
	
	if(cmd == CMD_TF)
	{
		tx1_buf[6] = 0x02; // param2
		checksum = 0 - (0xFF + 0x06 + cmd + 0x02);
	}
	else
	{
		tx1_buf[6] = 0x00; // param2
		checksum = 0 - (0xFF + 0x06 + cmd);
	}
	tx1_buf[7] = ((char)checksum >> 8); // read High 8bit of checksum
	tx1_buf[8] = (checksum & 0x0F); // read Low bit 8bit of checksum
	tx1_buf[9] = 0xEF; // $0(end bit)
	tx1_len = 10;
	tx1_index = 0; // index for writing on buf in an ascending order
	
	UCSR1B != (1 << UDRIE1);
}

void UART_Init(void) // setting for UART0(for serial monitor) / UART1(for MP3)
{
	//UART0
	UCSR0B = ((1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0));
	UCSR0C = ((1 << UCSZ01) | (1 << UCSZ00));
	UBRR0L = 103; // 1x, 9600bps
	// UART1
	UCSR1B = (1 << TXEN1);
	UCSR1C = ((1 << UCSZ11) | (1 << UCSZ10));
	UBRR1L = 103;
	sei();
}

int main(void)
{
	UART_Init();
	_delay_ms(200);
	
	rx_index = 0;
	rx_complete = 0;
	tx_index = 0;
	tx1_index = 0;
	
	UART0_Puts(menu);
	SendCommand(CMD_TF); // specify playback source(check which playback source to use)

	while (1)
	{
		if (rx_complete == 1) // only if RX interrupt takes place
		{
			SendCommand(cmd_list[buf[0] - 1 - '0']);
			UART0_Puts(buf);
			UART0_Puts(menu); // re-display menu
			rx_complete = 0; // flag for waiting new RX interrupt
		}
	}
	return 0;
}

