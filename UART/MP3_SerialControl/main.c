/* Serial Communication Format
 * $S-VER-Len-[CMD]-Feedback-para1-para2-[checksum]-$O
*/

/* [Example of checksum]*******************************************************
 * Suppose that there is 4byte data: 0x25, 0x62, 0x3F, 0x52.
 * 1) Add all of the data: 0x118
 * 2) Truncate carry nibble: 0x18
 * 3) Get 2's complementary of 0x18: 0xFF - 0x18 + 1 = 0xE8 -> check-sum byte
 * 4) Add all4byte data and check-sum = 0x200
 * 5) Truncate carry nibble: 0x00, which means no error
 *****************************************************************************/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* define CMD */
#define CMD_NEXT		0x01
#define CMD_PRE		0x02
#define CMD_UPVOL	0X04
#define CMD_DOWNVOL	0X05
#define CMD_PLAY		0x0D
#define CMD_PAUSE	0x0E
#define CMD_STOP		0x16
#define CMD_TF		0x09 // playback source: U-Disk(0x00), TF Card(0x01), AUX(0x02), SLEEP(0x03), FLASH(0x04)

// cmd list for easy use by index
char cmd_list[10] = {CMD_PLAY, CMD_STOP, CMD_PAUSE, CMD_NEXT, CMD_PRE, CMD_UPVOL, CMD_DOWNVOL};
char * menu = "\r\n===========================\r\n= "
	"1 : Play Music          =\r\n= "
	"2 : Stop Music          =\r\n= 3 : Pause Music         =\r\n= "
	"4 : Next Music          =\r\n= 5 : Previous Music      =\r\n= "
	"6 : Volume UP           =\r\n= 7 : Volume DOWN         =\r\n"
	" ===========================\r\n"
	"\r\nCommand > ";

void UART_Init(void) // setting for UART0(for serial monitor) / UART1(for MP3)
{
	UCSR0B = 0x18;
	UCSR0C = 0x06;
	UBRR0L = 103;

	UCSR1B = 0x08;
	UCSR1C = 0x06;
	UBRR1L = 103;
}

void UART0_Putch(char ch) // TX of UART0
{
	while(!(UCSR0A & 0x20));

	UDR0 = ch;
}

char UART0_Getch(void) // RX of UART0
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

void UART1_Putch(char ch) // TX of UART1
{
	while(!(UCSR1A & 0x20));

	UDR1 = ch;
}

void SendCommand(unsigned char cmd) // abiding by Serial communication Format
{
	unsigned int checksum = 0; // check if the data 0f RX/TX is corrupted(2byte)
	//char temp[20];


	// Send $S-VER-Len-CMD-Feedback-para1-para2
	UART1_Putch(0x7E); //$S(start)
	UART1_Putch(0xFF); // VER
	UART1_Putch(0x06); // Len
	UART1_Putch(cmd);
	UART1_Putch(0x00); // feedback
	UART1_Putch(0x00); // para1(High), para2(Low)

	// calc check-sum
	if(cmd != CMD_TF){ // if there is not playback source
		UART1_Putch(0x00);
		checksum = 0 - (0xFF + 0x06 + cmd);
	}
	else{ // if there is a playback source
		UART1_Putch(0x02); // designate a playback source: TF(SD) Card
		checksum = 0 - (0xFF + 0x06 + cmd + 0x02);
	}
	
	// send check-sum
	UART1_Putch((char)(checksum >> 8)); // high byte of check-sum
	UART1_Putch(checksum & 0xFF); // low byte of check sum
	// send $O
	UART1_Putch(0xEF); // end bit

}

void DisplayMenu(void) // puts string of menu select to the serial monitor(UART0)
{
	UART0_Puts(menu);
}
int main(void)
{
	char cmd;

	UART_Init();

	_delay_ms(200); // delay for serial communication to receive command MSG

	SendCommand(CMD_TF); // specify playback source(check which playback source to use)

	while(1){
		DisplayMenu(); // open menu for select
		cmd = UART0_Getch();// get input(one character) from serial monitor(UART0)
		SendCommand(cmd_list[cmd -1 - '0']);  // send command converted from ASCII value to int index to via UART1
	}
}

