/* This is another way to reduce chattering problem. In this method, to relieve bouncing pitfall, 
 * I check button state twice in a short time.
 */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

//unsigned char previousBtnState = 0, currentBtnstate;

char ShiftLedLeft(char ptn)
{
	return ((ptn << 1) | (ptn >> 7));
}

int getBtnState(void)
{
	if ((PIND & 0x04) >> 2 == 0x01) // read current button state
	{
		_delay_ms(10);
		if((PIND & 0x04) >> 2 == 0x01) // read again
		{
			return 1; // 1 means proper current state
		}
	}
	else
		return 0;
}

void INIT(void)
{
	DDRB = 0xFF;
	DDRD &= ~(0x04); // input
	PORTD = 0x04; // internal pull-up resist
}

int main(void)
{
	static unsigned char previousBtnState = 0, currentBtnState;
	char ledPattern = 0x01;
	
	INIT();
	PORTB = ledPattern; // initial setting
	
    /* Replace with your application code */
    while (1) 
    {
		currentBtnState = getBtnState(); // get current button state
		
		if (currentBtnState == 0x01 && previousBtnState == 0x00)
		{
			ledPattern = ShiftLedLeft(ledPattern);
			PORTB = ledPattern;
		}
		previousBtnState = currentBtnState;
    }
}

