#ifndef INIT_H_
#define INIT_H_

// GPIO and External Interrupt setting
void INIT(void)
{
	// Led port setting
	DDRB = 0xFF; // output
	DDRD = 0x00; // SW input
	PORTB = 0x00; // all led off
	PORTD = (1 << DDRD2); // internal pull-up resist
	
	// INT setting
	EIMSK |= (1 << INT2); // turn on external INT2
	EICRA |= (1 << ISC21); // detect INT from SW at falling-edge
	SREG |= 0x80; // GBL. INT. enable
}


#endif /* INIT_H_ */