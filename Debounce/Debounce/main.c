/* To decrease chattering phenomena when pressing a switch, there are basically two ways:
 * One is in software control; other in hardware one (add capacitor).
 * When it comes to the software way in dealing with chattering, first method is to use
 * delay function to deny new equivocal input during the function execution. Another way
 * is to check the button state twice in a short time.
 */



#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

unsigned char previousBtnState = 0, currentBtnState;

char ShiftLedLeft(char ptn)
{
	return ((ptn << 1) | (ptn >> 7));
}

void INIT(void)
{
	DDRB = 0xFF;
	DDRD &= ~(0x04);
	PORTD = 0x04; // internal pull-up resist
}

int main(void)
{
	INIT();
	
	char ledPattern = 0x01;
	PORTB = ledPattern; // initial state
	
    /* Replace with your application code */
    while (1) 
    {
		currentBtnState = (PIND & 0x04) >> 2; // read the current state of button
		if (currentBtnState == 1 && previousBtnState == 0)
		{
			_delay_ms(30); // during this delay, another input can be stopped
			ledPattern = ShiftLedLeft(ledPattern);
			PORTB = ledPattern;
		}
		previousBtnState = currentBtnState; // update button state
    }
}

