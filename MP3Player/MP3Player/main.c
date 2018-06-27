#define F_CPU 16000000L
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/********* Command selection(refer to data sheets) *********/
#define CMD_NEXT	0x01
#define CMD_PRE		0x02
#define CMD_PLAY	0x0d
#define CMD_PAUSE	0x0E
#define CMD_STOP	0x11
#define CMD_TF		0x09

char cmd_list[10] = {CMD_PLAY, CMD_STOP, CMD_PAUSE, CMD_NEXT, CMD_PRE,};
char buf[100], tx_buf[300], tx1_buf[20];
int rx_index, tx_index, tx1_index, tx1_len;
volatile char rx_complete, disp_menu;


/************************************************************************/
/* Serial Communication                                                 */
/************************************************************************/
ISR(USART0_RX_vect)
{
	char ch;

	ch = UDR0;

	buf[0] = ch;

	rx_complete = 1;
}

ISR(USART0_UDRE_vect)
{
	UDR0 = tx_buf[tx_index];

	if(tx_buf[tx_index++] == 0)
	UCSR0B &= ~(1 << UDRIE0);
}

ISR(USART1_UDRE_vect)
{
	UDR1 = tx1_buf[tx1_index++];

	if(tx1_index == tx1_len){
		UCSR1B &= ~(1 << UDRIE1);
		disp_menu = 1;
	}
}

/************************************************************************/
/* Function Implementation                                              */
/************************************************************************/
void UART0_Puts(char str[])
{
	int i=0;

	while(str[i] != 0)
	tx_buf[tx_index++] = str[i++];
}

void UART_Init(void)
{
	UCSR0B = 0x98;
	UCSR0C = 0x06;
	UBRR0L = 103;

	UCSR1B = 0x08;
	UCSR1C = 0x06;
	UBRR1L = 103;
}

void SendCommand(unsigned char cmd, unsigned int param) // command consists of 6 successive protocol
{
	int checksum = 0;
	int i;

	tx1_buf[0] = 0x7E;
	tx1_buf[1] = 0xFF;
	tx1_buf[2] = 0x06;
	tx1_buf[3] = cmd;
	tx1_buf[4] = 0x00;
	tx1_buf[5] = (char)(param >> 8);
	tx1_buf[6] = (char)(param & 0xFF);
	
	for(i=1; i<=6; i++)
	checksum += tx1_buf[i];

	checksum = -checksum;
	
	tx1_buf[7] = (char)(checksum >> 8);
	tx1_buf[8] = (char)(checksum & 0xFF);
	tx1_buf[9] = 0xEF;

	tx1_len = 10;
	tx1_index = 0;

	UCSR1B |= (1 << UDRIE1);

	rx_complete = 0;
}

void DisplayMenu(void)
{
	UART0_Puts("\r\n===========================");
	UART0_Puts("\r\n= 1 : Play Music          =");
	UART0_Puts("\r\n= 2 : Stop Music          =");
	UART0_Puts("\r\n= 3 : Pause Music         =");
	UART0_Puts("\r\n= 4 : Next Music          =");
	UART0_Puts("\r\n= 5 : Previous Music      =");
	UART0_Puts("\r\n===========================");
	UART0_Puts("\r\n\r\nCommand > ");

	tx_buf[tx_index] = 0;
	tx_index = 0;

	UCSR0B |= (1 << UDRIE0);

	disp_menu = 0;
}


// main method
int main(void)
{
	UART_Init();

	_delay_ms(200); // initial waiting for starting serial communication

	SendCommand(CMD_TF, 2);
	
	SREG |= 0x80;
	
	while(1){
		if(rx_complete == 1)
		SendCommand(cmd_list[buf[0] - '0' - 1], 0);

		if(disp_menu == 1)
		DisplayMenu();
	}
}
