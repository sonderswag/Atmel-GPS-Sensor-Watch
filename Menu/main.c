#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <avr/interrupt.h>

#include "../Serial/serial.h" 
#include "menu.h"

#define redbut (1<<PD6)

//global variables
float mode = 0;
int RBcount = 0;

ISR(PCINT2_vect)	{
	if (((PIND & (1<<PD6))==1) && RBcount >= 100)	{
		mode = mode+1;
		char buffer[50];
		FloatToStringNew(&buffer, mode, 0);
		serial_outputString(buffer);
		RBcount = 0;
	}	
	
}

int main (void)	{
	//DDRD |= (1<<DD6);	//Initialize D6 to output mode
	PORTD |= (1<<PD6);	//enable pull up on PD6
	serial_init(47);
	
	/*print hello when pressed
	while(1)	{
		char* str = "start \n";
    	serial_outputString(str);
    	_delay_ms(1000);
		if ((PIND & (1<<PD6))==0)	{
			char* str = "hello \n";
    		serial_outputString(str);
		}
		else	{
			char* str = "nope \n";
    		serial_outputString(str);
		}
	}*/
	/*debounced, count up
	float mode = 0;
	while(1)	{
		if (press_button())	{
			mode = mode+1;
			char buffer[50];
			FloatToStringNew(&buffer, mode, 0);
			serial_outputString(buffer);
		}
	}*/
	// pin change interrupts
	PCMSK2 |= (1 << PCINT22);
	PCICR |= (1 << PCIE2);
	sei();
	
	while(1)	{
		if ((PIND & (1<<PD6))==0)	{
			RBcount = RBcount+1;
		}
	}
}
