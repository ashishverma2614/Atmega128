//#define __STDIO_FDEVOPEN_COMPAT_12

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>

#include "dc_motor.h"
#include "key.h"
#include "var.h"
#include "lcd.h"
#include "led.h"


struct time { //  �ð� �ν��Ͻ��� ���� -> ����ð�(current time) �� ��ð�(section time)
	unsigned char hour;
	unsigned char minute;
	unsigned char seconds;
};

extern volatile unsigned char key_flag; // ������ �Է��� Ȯ���� üũ�ϴ� flag����

volatile unsigned char tick0, section_tick, velocity; // �ð��� ���õǴ� ���� 1)MCU�������� Ÿ�̸��� ����ð� 2) start��忡�� ��ð� 3) Į�θ��� ���ϱ� ���� �ӵ�����(�������׿��� �о���� ADC��)
volatile struct time current = { 0, 0, 0 }; // mcu���������� �ð�
volatile struct time section = { 0, 0, 0 }; // �ð� ���� ���ĺ��� �帣�� �ð�
volatile char mode; // enum ����� ���ǵ� ��带 �޾��ִ� �ӽú���
volatile double kcal; // �Ҹ��� Į�θ��� ��



enum { // mode ������ ���� ���
	NO_MODE, // 0
	START_MODE, // 1
	STOP_MODE // 2
};


void timer0_init(void)
{
	TCCR0 = 0x06; // (1 << CS02) | (1 << CS01), normal mode(OC0 disconnected), 256 scale
	TCNT0 = 256 - 250;
	TIMSK |= 0x01; // (1 << TOIE0)
}

// 4ms x 250 = 1s
ISR(TIMER0_OVF_vect) // 1) �����÷ο� ���� 1���� ��� �� ���� �ʱ�ȭ ���� 2) ��ð��� ��� �� ���� �ʱ�ȭ ���� 3) �ʴ� Į�θ� ���
{
	TCNT0 = 256 - 250;

	// calc 1(sec) + time reset
	if( ++tick0 == 250 ) { // 4ms x 250 = 1sec
		tick0 = 0;
		// each time digit reset
		if( ++current.seconds == 60 ) {
			current.seconds = 0;
			if( ++current.minute == 60 ) {
				current.minute = 0;
				if( ++current.hour == 24 )	current.hour = 0;
			}
		}
	}
	
	if( mode == START_MODE ) {
		if( ++section_tick == 250 ) {
			section_tick = 0;

			if( ++section.seconds == 60 ) {
				section.seconds = 0;
				if( ++section.minute == 60 ) {
					section.minute = 0;
					if( ++section.hour == 24 )	section.hour = 0;
				}
			}
			// Ÿ�̸� ������ �����ð��� �ٰ��� �ӵ� �� Į�θ� �Һ�
			if( velocity > 0 && velocity < 15 )		kcal += 0.105; // 1�ʴ� .105�� �����ȴ�
			else									kcal += 0.14;
		}
	}
}

void mcu_init(void) // mcu���� �ʱ�ȭ(mcu�� �����ϴ� ���� �ܺ� ������ �Լ��鵵 ���ÿ� �ʱ�ȭ)
{
	/*parts init */
	dcmotor_init();
	key_init();
	var_init();
	lcd_init();
	led_init();
	/* timer init */
	timer0_init();
}

void variable_init(void) // Ÿ�̸� �ð��� ���۸���� �ʱ�ȭ
{
	tick0 = 0;
	mode = NO_MODE; // ���۵� ������ �ƴ� ����
}

void start_healthcare(void) // �ｺ �ɾ� �ý����� LED, DC����, LCD�� �ʱ� ������ ���Ҿ� �����Ѵ�
{
	int i;
		
	// Q1: 1) 3�ʵ��� ���� ȸ��, 2) 3�ʵ��� LED��ü 0.5�ʰ� ����, 3) LCD�ʱ� ȭ�� ǥ�� �� 3�ʰ� ����
	
	dcmotor_spin(255); // speed 255�� ���͸� ȸ����Ű��(����)

	// lcd�ʱ� ����ȭ�� ����
	lcd_gotoxy(0, 0); // 1������ ùĭ���� ���� 
	lcd_string("  Health Care   "); // ���
	lcd_gotoxy(0, 1); // ��°�ٷ� �Űܼ�
	lcd_string("  Bike System..."); //���

	for( i = 0; i < 3; i++ ) { // 1�ʾ� 3�� �ݺ��Ѵ�.
		lcd_command_write(LCD_ON); // LCD�� 0.5�� ���� �Ѷ�
		//led_light(LED_ALL); // ��� LED�� 0.5�� ���� �Ѷ�
		led_light(0);
		_delay_ms(500); 
		//���¹���
		lcd_command_write(LCD_OFF); // LCD�� 0.5�� ���� ����
		//led_light(0); // �ƹ� LED�� 0.5�ʰ� ���� ����
		led_light(LED_ALL);
		_delay_ms(500);
	}
	dcmotor_spin(0); // 3�� ������ ���� ����
}

// �ð� ���� ���(��� 0): lcd���� ����!
void time_setting(void)
{
	unsigned char pos, tmp; // lcdĿ�� ��ġ�� 1ĭ�� ����ִ� ����, �ӽ� �ð� ���庯��
	unsigned char key = 0; // �ð� ������ ���� �Է�Ű ���� �޾��ֱ� ���� �ӽú���

	// ���� �ð��� �ʱ�ȭ
	current.hour = 0;
	current.minute = 0;
	current.seconds = 0;

	lcd_command_write(LCD_CURSOR); // Ŀ���� ǥ���϶�� lcd �ν�Ʈ���� �Լ�

	// �ʱ�ȭ�� ǥ�� 
	lcd_gotoxy(0, 0);
	lcd_string(" <Time Setting> ");
	lcd_gotoxy(0, 1);
	lcd_string("    00:00:00    "); // 4ĭ padding(4 + "8" + 4 = 16), ��� 8ĭ���� �ð��� �����Ѵ�

	lcd_gotoxy(4, 1); // '��'�� ù��° ĭ���� �̵�
	pos = 0; // �� ĭ�� 0���� �����Ѵ�(pos���� 0~7�� ��ǥ�� ������)
	tmp = 0; // �ð� ������ ���� �ӽú���
	
	while( 1 ) { // ���� ������� Ű ���� Ȯ���Ͽ� �ð��� ����(UP_KEY/ENTER_KEY�� ���� �ð��� ���� �� �����Ѵ�)
		key = getkey(key); // Ű ���� �÷��� Ȯ��
		if( key_flag ) { //			 �� < T i m e  S e t t i n g > �� 
            
            // �ð� �ڸ��� ����
			if( key == KEY_UP ) { // �� �� [0] [1] : [3] [4] : [6] [7] �� ��
				switch( pos ) {
					// Hour
					case 0 : // [0]: 0>1>2>0
						if( current.hour < 20 )	current.hour += 10;
						else						current.hour = 0; // hrs > 20 -> 0(Reset)
						lcd_data_write(current.hour / 10 + 0x30); // 0x30 in ASCII code = '0' / ���� ���� �ڸ��� ���
						break;
					case 1 : // [1]: 0>1>2>3>0
						if( (current.hour / 10) < 2 )	tmp = ++tmp % 10; // 01�� ~ 19��, tmp�� ���� ���� �ڸ��� �����ϴ� �ӽú���
						else								tmp = ++tmp % 4; // 21�� ~ 23��
						current.hour = (current.hour / 10) * 10 + tmp; // �� ��ü�� ����(���� ���� �ڸ� + ���� ���� �ڸ�)
						lcd_data_write(tmp + 0x30); // ���� ���� �ڸ��� ǥ��
						break;

					// Minute
					case 3 : // [3]
						if( current.minute < 50 )	current.minute += 10; // 1~5
						else							current.minute = 0;
						lcd_data_write(current.minute / 10 + 0x30); // ���� ���� �ڸ��� ���
						break;
					case 4 : // [4]
						tmp = ++tmp % 10; // temp���� ���� ���� �ڸ��� �����Ѵ�
						current.minute = (current.minute / 10) * 10 + tmp; // �� ��ü�� ����(���� ���� �ڸ� + ���� ���� �ڸ�)
						lcd_data_write(tmp + 0x30); // ���� ���� �ڸ��� ���
						break;

						// Second
					case 6 : // [6]
						if( current.seconds < 50 )	current.seconds += 10;
						else							current.seconds = 0;
						lcd_data_write(current.seconds / 10 + 0x30); // ���� ���� �ڸ��� ���
						break;
					case 7 : // [7]
						tmp = ++tmp % 10;
						current.seconds = (current.seconds / 10) * 10 + tmp; // �� ��ü�� ����(���� ���� �ڸ� + ���� ���� �ڸ�)
						lcd_data_write(tmp + 0x30); // ���� ���� �ڸ��� ���
						break;
				}

				lcd_gotoxy(pos + 4, 1); // 0x80(2��°�� ù°����)�������� �ؼ� 4�� �ּҸ� ���Ͽ� �̵�(������ ��Ʈ ����)
			}
            
            // �ð� �ڸ��� ���� �Ϸ�: �ڸ����� �̵���Ų��
			else if( key == KEY_ENTER ) {
				if( pos == 7 ) { // ���� 7��° �ڸ� ������ �Ϸ� ������, �ð� ���� ��ü �Ϸ� -> ���� Ż�� -> time_setting��� ���� 
					lcd_command_write(LCD_ON);
					return; // ����Ż��
				}
                // �ڸ��� �������� 7�� �ƴ϶��, �� ���� ������ �ð��� ���Ҵٸ�...
				tmp = 0;
				pos++;
				if( (pos == 2) || (pos == 5) )	pos++; // ":"��ġ�� Ŀ���� ��ġ�� ��쿡�� �ڸ��� ��ĭ�� ������ �Ű� 3�� 6�� ���������� �� ��
				lcd_gotoxy(pos + 4, 1); // ���ο� ��ġ���� �������� �� Ŀ�� �̵�(���� ��ġ�� �ּҰ��� 4�� ���Ѵ�)
			}
		}
	}
}

// ������ �ð��� lcd�� ����Ͽ� Ȯ�ν����ִ� �Լ�
void init_screen(void)
{
	lcd_gotoxy(0, 0); // �������� ����
	lcd_string("  Health Care   "); //���
	lcd_gotoxy(0, 1); // �����ٷ� ����
	printf("    %02d:%02d:%02d    ", current.hour, current.minute, current.seconds); // ���� �ð��� ���Ŀ� �°� ���(ȭ�� ������, ���ͷ�Ʈ�� �����Ǹ鼭 �帣�� �����ϴ� �ð�)
	
}

int main(void)
{
	unsigned char oldsec = 0xFF; // MAX: 256
	unsigned char key = 0; // ��� ����ġ�� �Էµ� ���� ���� ����
	
	/* constructor */
	mcu_init(); //�� ������ �ʱ�ȭ
	variable_init(); // �ð�(Ÿ�̸�)�� ����� �ʱ�ȭ(��� ��嵵 �ƴ�:default)
	/* End of constructor*/
	
	fdevopen((void *)lcd_data_write, 0); //pritnf()�� LCD�� ����� �� �ֵ��� �����ش�(����� �߰�/�������̽�)

	start_healthcare();// �ｺ�ɾ� �ʱ�ȭ������ ��ȯ
	time_setting(); // ����ġ�� �Է��� �߻��ϴ� ���, �ð��� �����ϴ� ���� ����
	init_screen(); // ������ �ð��� ȭ�鿡 ǥ��(������ �ð��� ��μ� �帣�� �����Ѵ�)-> �Ϲ����� �ð踦 ������ ��(�����ϱ� ������ �ð��� �帥��)

	sei(); // ���ͷ�Ʈ�� ���Ǹ鼭 Ÿ�̸Ӱ� ���ư��� ������ ���̴�.

	while( 1 ) { // �������, ����ġ �Է°˻�, �ش��� ����
		key = getkey(key); // Ű ���� ����("Ű ���� �ְ� Ű ���� ����", �ʱ� ���´� 0) -> switch�������� ���� �ٸ� Ű ���� �ޱ� ���ؼ� ����
		if( key_flag ) { // ������ Ű �Է��� Ȯ�� �÷��� üũ(���� Ű ���� �ٸ� Ű ���� ������)
			switch( key ) { // (��������)Ű �Է¿� ���� ��� �и� -> (1)Ű ���� ���� ����, (2) ���� ��Ȳ�� ���� LCD���
				// ���� Ű
				case KEY_START :
					if( mode == NO_MODE ) { // ���� ���°� ���۵� ������ �ƴ� (�ʱ�) �����
						// ���۸����� ���� ��� �ð� ������ �̿� �����ϴ� Į�θ� ��(�����ð��� � �Ҹ�Į�θ� �̹Ƿ�)�� �ʱ�ȭ �Ѵ�
						section.hour = 0;
						section.minute = 0;
						section.seconds = 0;
						section_tick = 0;
						kcal = 0.;
						// �ʱ�ȭ�� ������
						lcd_gotoxy(0, 0); // Ŀ���� �������� �Űܼ�
						lcd_string("Velocity:   Km/H"); // �ʱ� �ӵ� ȭ���� lcd�� �ѷ��ش�

						mode = START_MODE; // ��������, ���۸��� �����ϰ�, ����ġ ������ ���� ������ ���� �������� �б��Ѵ�
					}
					break;
				// ���� Ű
				case KEY_STOP :
					if( mode == START_MODE ) { // ���� ���� ��� �϶� �ǹ̰� �ִ� Ű �̹Ƿ�, ���� ���� ������� �ƴ����� üũ�Ѵ�
						// ����Ű ������ 1) ��ð��� 2) �Ҹ� Į�θ����� ǥ�õȴ�
						
						lcd_gotoxy(0, 0); //lcd ù°������ ���
						printf("Time    %02d:%02d:%02d", section.hour, section.minute, section.seconds); // � �Ҹ� �ð�
						

						lcd_gotoxy(0, 1); // lcd��° ������ ���
						printf("Calorie %4.1fKcal", kcal); // Į�θ�(ISR������ 1�ʴ� ���������� �̷����)
						
						mode = STOP_MODE; // ����Ű�� �Է¿� ���� ����� �����ϰ� ����Ǹ�, ������尡 �Ǹ鼭 ����ġ ������ ����������(�Ұ� ���ϰ� ����������)
					}
					break;
				// ���� Ű
				case KEY_ENTER : // 3) ����Ű
					if( mode == STOP_MODE ) { // ���� ��尡 ���������
						init_screen(); //�ʱ� ���� �����Ѵ�
						mode = NO_MODE;
					}
					break;
			}
		}
		// ���۸�忡���� ����
		// ����ġ ������ �������� ��Ȳ����, 
		// ���۸����(��� �ϸ�ũ ����), 
		if( mode == START_MODE ) { // ���� ��忡�� �ӵ� ������ �ǽð� ��Ȳ ������Ʈ
	
		// �������� �Է°��� �о� ������ �ӵ����� �� LCD�� ��Ȳǥ��
		velocity = (unsigned char)(var_read() / 20.4); // �������� ��ȭ�� ���� ADC���� �о��
		dcmotor_spin(velocity); // �о�� ���� ���� ȸ�� ������ ����

		lcd_gotoxy(10, 0); // ù�� 10��° ĭ���� ���� ������Ʈ �� �ӵ��� ���
		printf("%2d", velocity); // ������ 2ĭ
		

		led_light((unsigned char)(8 - (velocity / 6.2))); // led�����ε� �ӵ���Ȳ�� ���(��, LED�� ������ ����Ͽ� ����� ������ ����)
		}
		
		// �ǽð� �ð��� ǥ��
		// oldsec�� �ʱⰪ�� �ִ밪�� 0xFF(=256)
		if( oldsec |= current.seconds ) { // ���� ������ �ð��� oldsec�ð��� �ٸ��ٸ�
			oldsec = current.seconds; // oldsec�� ���������� while�ȿ��� ������Ʈ�ϰ� 
			if( mode != STOP_MODE ) { // stop��尡 �ƴ�, �ʱ��峪 ���۸����, (*������忡���� section time�� ǥ�õȴ�)
				lcd_gotoxy(0, 1); // �ι�° �ٷ� ����
				printf("    %02d:%02d:%02d    ",
						current.hour, current.minute, current.seconds); // �ð��� ��� ǥ���ض�
			}
		}
	}
	return 0;
}
