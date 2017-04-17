#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <stdint.h>
#include <avr/interrupt.h>

#include "../Serial/serial.h" 
#include "../Heart_rate/Heart_rate.h"
#include "menu.h"
#include "../Screen/Screen.h"
#include "../GPS/GPS.h"
#include "../Digital_IO/DigitalIo.h"
#include "../LSM/LSM.h"

#define redbut (1<<PD6)
#define greenbut (1<<PD7)
#define bluebut (1<<PB0)

#define ON 1 
#define OFF 0 

//global variables
uint8_t mode = 1;		//float so can print out for testing
uint8_t new_mode = 1; 	// this is a flag for entering into a new mode 
int steps = 0;		// step counter
// screen and GPS structs
struct Screen screen; 
struct GPS gps;

// heart rate struct
volatile struct HR_data HR = {0,0,0,0,0,0,0,0,0,0};

void init()
{
	// -------------------- screen --------------------------
	digitalWrite(25,0); // screen reset
    digitalWrite(25,1);
	screen_init(); 
	screen_clear(screen.buffer);
	// ------------------------------------------------------

	// -------------------- buttons --------------------------
	PORTD |= (1<<PD6);	//enable pull up on PD6
	PORTD |= (1<<PD7);	//enable pull up on PD7
	PORTB |= (1<<PB0);	//enable pull up on PB0

	// pin change interrupts
	PCMSK2 |= (1 << PCINT22);	//Red button
	PCMSK2 |= (1 << PCINT23);	//Green button
	PCICR  |= (1 << PCIE2);		//pin change int control register
	PCMSK0 |= (1 << PCINT0); 	//Blue button
	PCICR  |= (1 << PCIE0);
	// ------------------------------------------------------

	// -------------------- Serial --------------------------
	serial_init(47);
	// ------------------------------------------------------


	// -------------------- GPS --------------------------

	// initialize GPS values
	gps.sizeInputString = 0; 
	gps.state = 0;

	// ------------------------------------------------------

	// -------------------- Accelerometer --------------------------
	// hardware interrupt 1 for the acc 
	DDRD  &= ~(1 << DDD3); 
	PORTD |= (1<<PORTD3);
	EICRA |= (1<<ISC10) | (1 << ISC11);
	EIMSK |= (1<< INT1);  
	LSM_init(); 
	// ------------------------------------------------------
}


void new_mode_test()
{
	if (new_mode)
	{
		new_mode = 0; 
		screen_clear(screen.buffer);
 		screen_sendBuffer(screen.buffer);
 		_delay_ms(1); 
	}
	else 
	{
		return ; 
	}
}

int main (int argc, const char * argv[])	{
	init();
	sei();
	char buffer[30];

	// mode settings
	// mode = 1 --> read GPS data and time
	// mode = 2 --> track number of steps
	// mode = 3 --> track heart rate
	// mode = 4 --> track others around
	while(1)	
	{
		//_delay_ms(50);
		int a;
		for (a=0; a<=5000; a++)	
		{ }
		if (mode==1)	// this is to show 
		{			
			new_mode_test();
			// current step is to draw some strings
			GPS_readSerialInput(&gps);

 			// print out time values
 			sprintf(buffer, "hour %d, min %d, sec %d", gps.hour, gps.minute, gps.seconds);
 			screen_drawString(5, 30, buffer, screen.buffer);
 			screen_sendBuffer(screen.buffer);
 		}
 		
 		// mode 2: track steps
 		else if (mode==2)	
 		{
 			new_mode_test();
 			sprintf(buffer, "steps : %d", steps);
 			screen_drawString(5, 30, buffer, screen.buffer);
 			screen_sendBuffer(screen.buffer);
 		}
 		
 		// mode 3: track steps
 		else if (mode==3)	
 		{
 			new_mode_test();
 			screen_drawFillCircle(10,10,10,1,screen.buffer);
 			screen_sendBuffer(screen.buffer);
 		}

 		// mode 4: tracking people
 		else if (mode==4)	
 		{
 			new_mode_test();
 			screen_drawFillRectangle(10,10,30,20,1,screen.buffer);
 			screen_sendBuffer(screen.buffer);
 		}
	}
	
	return 0;
}

// button interrupt commands --------------------------------------------
ISR(PCINT2_vect)	
{
	new_mode = 1; 
	if ((PIND & redbut)==0)	{
		mode++;

		char bufferbut[10];
		sprintf(bufferbut,"redbut %d",mode);
		serial_outputString(bufferbut);	
		_delay_ms(1);
		while ((PIND & redbut)==0)	{}
		_delay_ms(1);
	}
	
	else if ((PIND & greenbut)==0)	
	{
		mode = mode-1;
		if (mode == 0)	{
			mode = 4;
		}
		char bufferbut[10];
		sprintf(bufferbut,"green %d",mode);
		serial_outputString(bufferbut);	
		_delay_ms(1);
		while ((PIND & greenbut)==0)	{}
		_delay_ms(1);
	}
}

ISR(PCINT0_vect)	
{
	new_mode = 1; 
	if ((PINB & bluebut)==0)	{
		mode = mode+1;
		if (mode == 5)	{
			mode = 1;
		}
		char bufferbut[10];
		sprintf(bufferbut,"blue %d",mode);
		serial_outputString(bufferbut);	
		_delay_ms(1);
		while ((PINB & bluebut)==0)	{}
		_delay_ms(1);
	}
}
// -----------------------------------------------------------------------

//Hardware interrupt for the accelerometer 
ISR(INT1_vect)
{
	steps++ ; 
}