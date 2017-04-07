//
//  main.c
//  transReciever:: SEND 
//
//  Created by Christian Wagner on 3/9/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>


#include "../Screen.h"
#include "../../Digital_IO/DigitalIo.h"

#define ON 1 
#define OFF 0 
struct Screen screen; 

// Must call these lines in this order 
// The screen size is 128 x 64 pixels 
void init()
{
	digitalWrite(25,0);
    digitalWrite(25,1);
	screen_init(); 
	screen_clear(screen.buffer);
}

int main(int argc, const char * argv[]) {
	
	init(); // do this first 

	
	// draw horizontal line 
	screen_drawHLine(2,60,120,screen.buffer);
	// draw verticle line 
	screen_drawVLine(100,20,30,screen.buffer);
	//upload 
	screen_sendBuffer(screen.buffer);
	//wait 
	_delay_ms(2000);
	//clear 
	screen_clear(screen.buffer);
	screen_sendBuffer(screen.buffer);
	_delay_ms(2000);

	//draw individual pixel 
	screen_drawPixel(100,50,1,screen.buffer);
	
	//draw the letter A 
	screen_drawChar(100, 30, 'A',screen.buffer);
	//draw the string Hey 
	screen_drawString(100, 20, "Hey",screen.buffer); 

	//draw the outline of a circle
	// NOTE:: saying on will turn the leds on, saying OFF will turn those same pixels off
	screen_drawCircle(70,30,11,ON,screen.buffer);
	//draw a filled circle 
	screen_drawFillCircle(70,30,10,ON,screen.buffer);
	//draw the outline of a rectangle 
	screen_drawRectangle(19,19,51,50,screen.buffer); 
	//draw filled rectangle 
	screen_drawFillRectangle(20,20,50,50,ON,screen.buffer); 

	
	screen_sendBuffer(screen.buffer);


	while (1)
	{
		screen_drawFillRectangle(20,20,50,50,ON,screen.buffer); 
		screen_drawFillCircle(70,30,10,ON,screen.buffer);
		screen_sendBuffer(screen.buffer);
		_delay_ms(1000);

		screen_drawFillRectangle(20,20,50,50,OFF,screen.buffer); 
		screen_drawFillCircle(70,30,10,OFF,screen.buffer);
		screen_sendBuffer(screen.buffer);
		_delay_ms(1000);


	}

    return 0;
}
