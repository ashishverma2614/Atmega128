#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define FUNCSET 	0x28
#define ENTMODE 	0x06
#define ALLCLR 		0x01
#define DISPON 		0x0C
#define LINE2 		0xC0
#define LINE1 		0x80
#define ENH 		(PORTC |= 0x04)
#define	ENL			(PORTC &=0xFB)
#define HOME 		0x02
#define RSHIFT 		0x1C
#define LSHIFT 		0x1B
#define DISPOFF 	0x08

#define CMD_NEXT			0x01
#define CMD_PRE				0x02
#define CMD_VOL_UP			0x04
#define CMD_VOL_DOWN		0x05
#define CMD_PLAY			0x0D
#define CMD_PAUSE			0x0E
#define CMD_STOP			0x16
#define CMD_TF				0x09

void INT_init(void);
void LCD_init(void);
void LCD_String(char str[]);
void Command(unsigned char byte);
void Data(unsigned char byte);
void Busy(void);
void Disp_menu(void);
void Send_tx_data(char cmd);

volatile char menu = 0;
volatile char state_menu = 0;
volatile char select_menu = 0;
volatile char cmd = 0;
char tx1_buf[50] = {0};
char tx1_index = 0;
char tx1_len = 0;
int count = 0;

char cmd_list[] = {CMD_PLAY, CMD_STOP, CMD_PAUSE, CMD_NEXT, CMD_PRE, CMD_VOL_UP, CMD_VOL_DOWN};

char Disp_play0[] 		= "  Play ";
char Disp_play1[] 		= "[ Play ]";
char Disp_stop0[] 		= "  Stop ";
char Disp_stop1[] 		= "[ Stop ]";
char Disp_pause0[] 		= "  Pause ";
char Disp_pause1[] 		= "[ Pause ]";
char Disp_next0[] 		= "  Next ";
char Disp_next1[] 		= "[ Next ]";
char Disp_previous0[] 	= "  Previous ";
char Disp_previous1[] 	= "[ Previous ]";
char Disp_vol_up0[] 	= "  Volume UP ";
char Disp_vol_up1[] 	= "[ Volume UP ]";
char Disp_vol_down0[] 	= "  Volume DOWN ";
char Disp_vol_down1[] 	= "[ Volume DOWN ]";


//외부 인터럽트

ISR(INT1_vect)
{
	menu--;
	state_menu = 1;
	if(menu < 0)
	menu = 0;

	EIMSK &= ~((0x01 << INT1) | (0x01 << INT6) | (0x01 << INT7));
	TCCR0 = ((0x01 << CS02) | (0x01 << CS00));
}


ISR(INT6_vect)
{
	select_menu = 1;

	EIMSK &= ~((0x01 << INT1) | (0x01 << INT6) | (0x01 << INT7));
	TCCR0 = ((0x01 << CS02) | (0x01 << CS00));
}


ISR(INT7_vect)
{
	menu++;
	state_menu = 1;
	if(menu > 11)
	menu = 11;

	EIMSK &= ~((0x01 << INT1) | (0x01 << INT6) | (0x01 << INT7));
	TCCR0 = ((0x01 << CS02) | (0x01 << CS00));
}


//타이머 인터럽트(채터링 방지)
ISR(TIMER0_COMP_vect)
{
	if(++count == 100)
	{
		TCNT0 = 0x00;
		TCCR0 = 0x00;
		count = 0;
		EIFR |= ((0x01 << INTF1) | (0x01 << INTF6) | (0x01 << INTF7));
		EIMSK |= ((0x01 << INT1) | (0x01 << INT6) | (0x01 << INT7));
	}
}


//UART1 인터럽트
ISR(USART1_UDRE_vect)
{
	UDR1 = tx1_buf[tx1_index++];

	if(tx1_index == tx1_len)
	{
		UCSR1B &= ~(0x01 << UDRIE1);
	}
}


int main(void)
{

	LCD_init();
	INT_init();
	UART1_init();
	TIMER0_init();
	Disp_menu();

	_delay_ms(200);

	Send_tx_data(CMD_TF);
	SREG |= (0x01 << 7);

	while(1)
	{
		if(state_menu)
		{
			state_menu = 0;
			Disp_menu();
		}

		if(select_menu)
		{
			select_menu = 0;
			Send_tx_data(cmd_list[cmd]);
		}
	}

	return 0;
}


// 외부 인터럽트 초기화
void INT_init(void)
{
	// 스위치 입력 설정
	DDRD = 0x00;
	DDRE = 0x00;

	//내부 풀업 저항
	PORTD = 0xFF;
	PORTE = 0xFF;

	//트리거 설정(falling edge)
	EICRA = ((0x01 << ISC11));
	EICRB = ((0x01 << ISC61) | (0x01 << ISC71));

	//외부인터럽트 마스크 설정
	EIMSK = ((0x01 << INT1) | (0x01 << INT6) | (0x01 << INT7));
}

void TIMER0_init(void)
{
	TIMSK = ((0x01 << OCIE0));
	OCR0 = 124; 	//1ms
}


// UART1 초기화
void UART1_init(void)
{
	UCSR1B = (0x01 << TXEN1);
	UCSR1C = ((0x01 << UCSZ11) | (0x01 << UCSZ10));
	UBRR1L = 103;
}


//LCD 초기화
void LCD_init(void)
{
	DDRC = 0xFF;		//PORTC에 LCD 연결
	PORTC = 0x00;
	_delay_ms(15);
	Command(0x20);		//펑션 셋
	_delay_ms(5);
	Command(0x20);		//펑션 셋
	_delay_us(100);
	Command(0x20);		//펑션 셋
	Command(FUNCSET);	//FUNCSET(0x28) : 4비트 데이터 버스(DL=0), 표시라인 - 2줄(N=1), 글자 폰트 - 5*7 도트(F=*)
	Command(DISPON);	//DISPON (0x0C) : LCD 전체 ON(D=1), 커서 끄기(C=0),커서위치 문자 깜빡임 끄기(B=0)
	Command(ALLCLR);	//ALLCLR (0x01) : 표시 클리어
	Command(ENTMODE);	//ENTMODE (0x06) : 어드레스를 +1하면서 커서를 오른쪽으로 이동(I/D=1), 뷰 포인트를 이동하지 않음(S=0)
}


// 문자열 출력 함수
void LCD_String(char str[])
{
	char i = 0;

	while(str[i] != 0)
	Data(str[i++]);
}

// 인스트럭션 쓰기 함수
void Command(unsigned char byte)
{
	Busy();

	// 인스트럭션 상위 4비트 쓰기
	PORTC = 0x00;	//RS = 0, RW = 0
	PORTC |= (byte & 0xF0);		//상위 4비트 마스크

	_delay_us(1);
	ENH;		// Enable = 1;

	_delay_us(1);
	ENL;		// Enable = 0;	=> 데이터가 써짐

	// 인스트럭션 하위 4비트
	PORTC = 0x00;	//RS = 0, RW = 0
	PORTC |= (byte << 4);		//하위 4비트 쉬프트

	_delay_us(1);
	ENH;		// ENH	(PORTC |= 0x04) : Enable(PC2)핀 1로 설정

	_delay_us(1);
	ENL;		// ENL	(PORTC &=0xFB)	: Enable(PC2)핀 0로 설정
}

// 데이터 쓰기 함수
void Data(unsigned char byte)
{
	Busy();

	// 데이터 상위 4비트
	PORTC = 0x01;	//RS=1, RW=0
	PORTC |=(byte & 0xF0);
	
	_delay_us(1);
	ENH;
	
	_delay_us(1);
	ENL;
	
	//데이터 하위 4비트
	PORTC = 0x01;	//RS=1, RW=0
	PORTC |= (byte << 4);

	_delay_us(1);
	ENH;

	_delay_us(1);
	ENL;
}

// Busy Flag를 채크하는 것이 아닌 LCD가 명령을 처리하는데 소요되는 시간 만큼 delay 함수로 시간 지연
void Busy(void)
{
	_delay_ms(2);
}

void Disp_menu(void)
{
	switch(menu)
	{
		case 0:
		Command(ALLCLR);
		LCD_String(Disp_play1);
		Command(LINE2);
		LCD_String(Disp_stop0);
		cmd = 0;
		break;
		case 1:
		Command(ALLCLR);
		LCD_String(Disp_play0);
		Command(LINE2);
		LCD_String(Disp_stop1);
		cmd = 1;
		break;
		case 2:
		Command(ALLCLR);
		LCD_String(Disp_stop1);
		Command(LINE2);
		LCD_String(Disp_pause0);
		cmd = 1;
		break;
		case 3:
		Command(ALLCLR);
		LCD_String(Disp_stop0);
		Command(LINE2);
		LCD_String(Disp_pause1);
		cmd = 2;
		break;
		case 4:
		Command(ALLCLR);
		LCD_String(Disp_pause1);
		Command(LINE2);
		LCD_String(Disp_next0);
		cmd = 2;
		break;
		case 5:
		Command(ALLCLR);
		LCD_String(Disp_pause0);
		Command(LINE2);
		LCD_String(Disp_next1);
		cmd = 3;
		break;
		case 6:
		Command(ALLCLR);
		LCD_String(Disp_next1);
		Command(LINE2);
		LCD_String(Disp_previous0);
		cmd = 3;
		break;
		case 7:
		Command(ALLCLR);
		LCD_String(Disp_next0);
		Command(LINE2);
		LCD_String(Disp_previous1);
		cmd = 4;
		break;
		case 8:
		Command(ALLCLR);
		LCD_String(Disp_previous1);
		Command(LINE2);
		LCD_String(Disp_vol_up0);
		cmd = 4;
		break;
		case 9:
		Command(ALLCLR);
		LCD_String(Disp_previous0);
		Command(LINE2);
		LCD_String(Disp_vol_up1);
		cmd = 5;
		break;
		case 10:
		Command(ALLCLR);
		LCD_String(Disp_vol_up1);
		Command(LINE2);
		LCD_String(Disp_vol_down0);
		cmd = 5;
		break;
		case 11:
		Command(ALLCLR);
		LCD_String(Disp_vol_up0);
		Command(LINE2);
		LCD_String(Disp_vol_down1);
		cmd = 6;
		break;
	}
}

void Send_tx_data(char cmd)
{
	int checksum = 0;
	
	tx1_buf[0] = 0x7E;							//Start byte(0x7E)
	tx1_buf[1] = 0xFF;							//Version(0xFF)
	tx1_buf[2] = 0x06;							//Len(command~checksum)
	tx1_buf[3] = cmd;							//Command
	tx1_buf[4] = 0x00;							//Feedback
	tx1_buf[5] = 0x00;							//Parameter1

	if(cmd == CMD_TF)
	{
		tx1_buf[6] = 0x02;						//Parameter2
		checksum = 0 - (0xFF + 0x06 + cmd + 0x02);
	}
	else
	{
		tx1_buf[6] = 0x00;						//Parameter2
		checksum = 0 - (0xFF + 0x06 + cmd);
	}

	tx1_buf[7] = ((char)(checksum >> 8));		//ChecksumH
	tx1_buf[8] = (checksum & 0xFF);				//ChecksumL
	tx1_buf[9] = 0xEF;							//End
	
	tx1_len = 10;
	tx1_index = 0;

	UCSR1B |= (0x01 << UDRIE1);
}




