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

struct Screen screen; 

int main(int argc, const char * argv[]) {
	
	digitalWrite(25,0);
	digitalWrite(25,1);
	screen_init(); 
	screen_clear(screen.buffer);
	// screen_sendBuffer(screen.buffer);

	// screen_drawHLine(2,2,120,screen.buffer);
	// screen_drawHLine(2,20,120,screen.buffer);
	// screen_drawPixel(50,50,1,screen.buffer);
	screen_drawRectangle(19,19,51,50,screen.buffer); 
	screen_drawFillRectangle(20,20,50,50,1,screen.buffer); 
	// screen_drawHLine(2,40,100,screen.buffer);

	
	screen_sendBuffer(screen.buffer);


	while (1)
	{
		screen_drawFillRectangle(20,20,50,50,1,screen.buffer); 
		screen_sendBuffer(screen.buffer);
		_delay_ms(1000);

		screen_drawFillRectangle(20,20,50,50,0,screen.buffer); 
		screen_sendBuffer(screen.buffer);
		_delay_ms(1000);


	}

    return 0;
}
