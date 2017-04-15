#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <avr/interrupt.h>

#include "../Serial/serial.h" 
#include "menu.h"

#define redbut (1<<PD6)
#define greenbut (1<<PD7)
#define bluebut (1<<PB0)

//global variables
float mode = 0;
int RBcount = 0;
int flag = 0;

ISR(PCINT2_vect)	{
	//if ((PIND & (1<<PD6))==0)	{
	if ((PIND & redbut)==0)	{
		mode++;
		char buffer[50];
		FloatToStringNew(&buffer, mode, 0);
		serial_outputString(buffer);	
		_delay_ms(50);
		while ((PIND & redbut)==0)	{}
		_delay_ms(50);
	}
	//else if ((PIND & (1<<PD7))==0)	{
	else if ((PIND & greenbut)==0)	{
		mode = mode+2;
		char buffer[50];
		FloatToStringNew(&buffer, mode, 0);
		serial_outputString(buffer);	
		_delay_ms(50);
		while ((PIND & greenbut)==0)	{}
		_delay_ms(50);
	}
	/*if ((PIND & (1<<PD6))==0 && flag==0)	{
		mode++;
		char buffer[50];
		FloatToStringNew(&buffer, mode, 0);
		serial_outputString(buffer);	
		flag = 1;
		RBcount = 0;
	}
	else if ((PIND & (1<<PD6))==1)	{
		flag = 0;
		RBcount = 0;
		char* str = "exit \n";
    	serial_outputString(str);
	}*/
}

ISR(PCINT0_vect)	{
	//if ((PINB & (1<<PB0))==0)	{
	if ((PINB & bluebut)==0)	{
		mode--;
		char buffer[50];
		FloatToStringNew(&buffer, mode, 0);
		serial_outputString(buffer);	
		_delay_ms(50);
		while ((PINB & bluebut)==0)	{}
		_delay_ms(50);
	}
}

int main (void)	{
	//DDRD |= (1<<DD6);	//Initialize D6 to output mode
	PORTD |= (1<<PD6);	//enable pull up on PD6
	PORTD |= (1<<PD7);	//enable pull up on PD7
	PORTB |= (1<<PB0);	//enable pull up on PB0
	
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
	PCMSK2 |= (1 << PCINT22);	//Red button
	PCMSK2 |= (1 << PCINT23);	//Green button
	PCICR |= (1 << PCIE2);		//pin change int control register
	
	PCMSK0 |= (1 << PCINT0); 	//Blue button
	PCICR |= (1 << PCIE0);
	
	sei();
	
	while(1)	{

	}
}
