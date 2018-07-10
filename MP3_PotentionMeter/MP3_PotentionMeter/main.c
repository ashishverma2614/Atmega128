#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

typedef unsigned char u_char;

#define FUNCSET 0x28 // Function Set
#define ENTMODE 0x06 // Entry Mode Set
#define ALLCLR 0x01 // All Clear
#define DISPON 0x0c // Display On
#define CURHOME	0x02
#define LINE2 0xC0 // 2nd Line Move
#define ENH  PORTC |= 0x04
#define ENL  PORTC &= 0xFB

#define CMD_NEXT	0x01
#define CMD_PRE		0x02
#define CMD_PLAY	0x0d
#define CMD_PAUSE	0x0E
#define CMD_STOP	0x16
#define CMD_TF		0x09
#define CMD_VOL 0x10

void LCD_init(void); // LCD 초기화
void LCD_String(char []); // 문자열 출력
void Busy(void); // 2ms 딜레이 함수
void Command(u_char); // 명령어 전송
void Data(u_char); // 데이터 전송

char menu[5][10] = {"1. PLAY ", "2. PAUSE", "3. STOP ", "4. NEXT ", "5. PRE. "};
char cmd_list[10] = {CMD_PLAY, CMD_PAUSE, CMD_STOP, CMD_NEXT, CMD_PRE,};

int cur_pos = 0, menu_pos = 0;
int old_cur = 9, old_menu = 9;
int sel_menu = 0;
char tx = 0;
int adc_value, old_adc;

ISR(INT4_vect) // UP
{
	if(cur_pos == 1)
	cur_pos = 0;
	else{
		if(menu_pos != 0)
		menu_pos--;
	}
}

ISR(INT5_vect)
{
	sel_menu = menu_pos + cur_pos; //현재 메뉴의 좌표와 커서 좌표를 더해서 선택 메뉴를 구성한다
	tx = 1;
}

ISR(INT6_vect) // DN
{
	if(cur_pos == 0)
	cur_pos = 1;
	else{
		if(menu_pos != 3)
		menu_pos++;
	}
}

ISR(ADC_vect) // ADV int free reuuning mode
{
	
}
void PortInit(void)
{
	DDRC = 0xFF; // LCD Port
}

void EXIntInit(void)
{
	EICRB = 0b00101010; // INT4 ~ INT6 : Falling Edge
	EIMSK = 0b01110000; // INT4 ~ INT6 External Interrupt Enable
}

void TimerInit(void)
{
	TCCR1B = 0x05;	// Prescaler : 1024
	TIMSK = 0x04;	// Timer1 OVF. Interrupt Enable
}

void UARTInit(void)
{
	// 9600bps, 8bit, 1bit Stop, No parity
	UCSR1A = 0x00;
	UCSR1B = 0b00001000; // Tx Enable
	UCSR1C = 0x06; // Data 8bit, Stop 1bit, No Parity

	UBRR1H = 0;
	UBRR1L = 103; // 9600bps
	
}

void UART1_Putch(char ch)
{
	while((UCSR1A & 0x20) != 0x20);

	UDR1 = ch;
}

void UART_Puts(char buf[], int len)
{
	int i;

	for(i=0; i<len; i++)
	UART1_Putch(buf[i]);
}

void ADCInit(void)
{
	ADMUX = 0b01000000; // REF : AVCC, MUX0
	ADCSRA = 0b10101111;	// ADC Enable, Free Running Mode, ADC Interrupt Enable, Prescale = 128
}

void LCD_Init(void)
{
	DDRC = 0xFF; // 포트 C 출력 설정
	PORTC = 0x0;
	_delay_ms(15);
	Command(0x20);
	_delay_ms(5);
	Command(0x20);
	_delay_us(100);
	Command(0x20);
	Command(FUNCSET);
	Command(DISPON);
	Command(ALLCLR);
	Command(ENTMODE);
}

void Command(u_char byte)
{
	Busy();
	// 인스트럭션 상위 4비트
	PORTC = 0x00; // RS=RW=0
	PORTC |= (byte & 0xF0); // 명령어 상위 4비트

	_delay_us(1);
	ENH; // E = 1

	_delay_us(1);
	ENL;// E = 0

	// 인스트럭션 하위 4비트
	PORTC = 0x00; // RS=RW=0
	PORTC |= (byte << 4); // 명령어 하위 4비트

	_delay_us(1);
	ENH; // E = 1

	_delay_us(1);
	ENL; // E = 0
}

void Data(u_char byte)
{
	Busy();
	// 데이터 상위 4비트
	PORTC = 0x01; // RS=1, RW=0
	PORTC |= (byte & 0xF0); // 데이터 상위 4비트
	_delay_us(1);
	ENH; // E = 1
	_delay_us(1);
	ENL; // E = 0

	// 데이터 하위 4비트
	PORTC = 0x01; // RS=1, RW=0
	PORTC |= (byte << 4); // 데이터 하위 4비트
	_delay_us(1);
	ENH; // E = 1
	_delay_us(1);
	ENL; // E = 0
}
// Busy Flag Check -> 일반적인 BF를 체크하는 것이 아니라
// 일정한 시간 지연을 이용한다.
void Busy(void)
{
	_delay_ms(2);
}

// 문자열 출력 함수
void LCD_String(char str[])
{
	char i=0;

	while(str[i] != 0) Data(str[i++]);
}

void DispMenu(void)
{
	Command(CURHOME);
	if(cur_pos == 0) LCD_String("[");
	else LCD_String(" ");
	LCD_String(menu[menu_pos]);
	if(cur_pos == 0) LCD_String("]");
	else LCD_String(" ");

	Command(LINE2);
	if(cur_pos == 1) LCD_String("[");
	else LCD_String(" ");
	LCD_String(menu[menu_pos+1]);
	if(cur_pos == 1) LCD_String("]");
	else LCD_String(" ");
}

void SendCommand(unsigned char cmd, unsigned int param)
{
	unsigned int checksum = 0;

	UART1_Putch(0x7E);
	UART1_Putch(0xFF);
	UART1_Putch(0x06);
	UART1_Putch(cmd);
	UART1_Putch(0x00);
	UART1_Putch((char)(param >> 8));
	UART1_Putch((char)(param & 0xFF));

	checksum = 0 - (0xFF + 0x06 + cmd + (char)(param >> 8) + (char)(param & 0xFF));
	
	UART1_Putch((char)(checksum >> 8));
	UART1_Putch(checksum&0xFF);
	UART1_Putch(0xEF);
}

int main(void)
{
	PORTE=0xFF;
	PortInit();
	EXIntInit();
	//TimerInit();
	UARTInit();
	//ADCInit();
	LCD_Init();

	SREG |= 0x80;
	//ADCSRA |= 0b01000000; //ADC Start

	SendCommand(CMD_TF, 0x02);

	while(1){
		if(old_cur != cur_pos || old_menu != menu_pos){
			DispMenu();
			old_cur = cur_pos;
			old_menu = menu_pos;
		}

		if(tx == 1){
			tx = 0;
			SendCommand(cmd_list[sel_menu], 0);
		}
		if (old_adc != adc_value)
		{
			vol = ((float)adc_value / 1023) * 30;
			SendCommand (CMD_VOL, vol);
			old_adc = adc_value;
		}
	}

	return 0;
}