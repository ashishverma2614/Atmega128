/* Serial communication Format
 * $S: Start Byte(0x7E)
 * VER:Version Info(FF)
 * Len: nums of bytes after "Len"
 * CMD: Commands-> Indicate the specific operations(see below define directive)
 * Feedback: Command feedback
 * para1: query high data byte
 * para2: query low data byte
 * checksum: Accumulation and verification[exclude start bit]
 * $O: end bit(0xEF)
*/

// protocol: syntax concerning RX/TX ex) TTL level(0-5V), RS232 level

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* define CDM macro from data sheet */
#define CMD_NEXT	0x01
#define CMD_PRE		0x02
#define CMD_UPVOL	0X04
#define CMD_DOWNVOL	0X05
#define CMD_PLAY	0x0d
#define CMD_PAUSE	0x0E
#define CMD_STOP	0x16
#define CMD_TF		0x09

// cmd list for easy use by index
char cmd_list[10] = {CMD_PLAY, CMD_STOP, CMD_PAUSE, CMD_NEXT, CMD_PRE, CMD_UPVOL, CMD_DOWNVOL};

int main(void) // 세팅 + 반복동작
{
	char cmd;

	UART_Init(); //통신세팅

	_delay_ms(200); // delay for serial communication to receive command MSG

	SendCommand(CMD_TF);

	while(1){
		DisplayMenu(); // open menu for select
		cmd = UART0_Getch();// get input from serial
		SendCommand(cmd_list[cmd - '0' - 1]);  // send command converted from ASCII value to int index
	}
}

void UART_Init(void)
{
	UCSR0B = 0x18;
	UCSR0C = 0x06;
	UBRR0L = 103;

	UCSR1B = 0x08;
	UCSR1C = 0x06;
	UBRR1L = 103;
}

void UART0_Putch(char ch)
{
	while(!(UCSR0A & 0x20));

	UDR0 = ch;
}

char UART0_Getch(void)
{
	while(!(UCSR0A & 0x80));

	return UDR0;
}

void UART0_Puts(char str[])
{
	int i=0;

	while(str[i] != 0)
	UART0_Putch(str[i++]);
}

void UART1_Putch(char ch)
{
	while(!(UCSR1A & 0x20));

	UDR1 = ch;
}

void SendCommand(unsigned char cmd) // successively send serial format for communication(TX from MCU RX to MP3 module)
{
	unsigned int checksum = 0; // check if the data 0f RX/TX is corrupted(2byte)
	//char temp[20];


	// Serial communication format
	UART1_Putch(0x7E); //$S(start)
	UART1_Putch(0xFF); // VER
	UART1_Putch(0x06); // Len
	UART1_Putch(cmd); // refer to the data sheet
	UART1_Putch(0x00); // feedback
	UART1_Putch(0x00); // para1(High), para2(Low)

	// to calc check-some according to source
	if(cmd != CMD_TF){ // source is not SD Card
		UART1_Putch(0x00);
		checksum = 0 - (0xFF + 0x06 + cmd);
	}
	else{
		UART1_Putch(0x02); // source: TF(SD) Card
		checksum = 0 - (0xFF + 0x06 + cmd + 0x02);
	}
	
	UART1_Putch((char)(checksum >> 8)); // high byte of check-sum
	UART1_Putch(checksum & 0xFF); // low byte of check sum
	UART1_Putch(0xEF); // end bit

}

void DisplayMenu(void) // puts string of menu select to the serial monitor
{
	UART0_Puts("\r\n===========================");
	UART0_Puts("\r\n= 1 : Play Music          =");
	UART0_Puts("\r\n= 2 : Stop Music          =");
	UART0_Puts("\r\n= 3 : Pause Music         =");
	UART0_Puts("\r\n= 4 : Next Music          =");
	UART0_Puts("\r\n= 5 : Previous Music      =");
	UART0_Puts("\r\n===========================");
	UART0_Puts("\r\n\r\nCommand > ");
}