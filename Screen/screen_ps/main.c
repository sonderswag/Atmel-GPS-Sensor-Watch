//  This is a large program. This is done for testing reasons.
//  main.c
//  transReciever:: SEND 
//
//  Created by Christian Wagner on 3/9/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//
// 	Main code for displaying GPS data onto the screen

#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include <string.h> 
#include <stdlib.h>


#include "../Screen.h"
#include "../../Digital_IO/DigitalIo.h"
#include "../../Serial/serial.h"
#include "../../GPS/GPS.h"

#define ON 1 
#define OFF 0 

// structs needed
struct Screen screen; 
struct GPS gps; 

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
	
	// serial initialization
	serial_init(47);
	
	// GPS initialization
	gps.sizeInputString = 0; 
	gps.state = 0;
	char buffer[50];
	
	while (1)
	{
		_delay_ms(200);
		
		// current step is to draw some strings
		GPS_readSerialInput(&gps);
		// prints out GPS data into the serial screen
		GPS_printInfo(&gps); 
		
		// prints out latitude values
		screen_drawString(5, 0, "Lat:", screen.buffer);
		screen_sendBuffer(screen.buffer); 	// uploading the drawing
 		FloatToStringNew(buffer, gps.latitude , 6); 
		screen_drawString(50, 0, buffer, screen.buffer);
		screen_sendBuffer(screen.buffer); 	// uploading the drawing
		
		// print out longitude values
		screen_drawString(5, 10, "Long:", screen.buffer);
		screen_sendBuffer(screen.buffer); 	// uploading the drawing
 		FloatToStringNew(buffer, gps.longitude , 6); 
 		screen_drawString(50, 10, buffer, screen.buffer);
 		screen_sendBuffer(screen.buffer); 	// uploading the drawing
		
		// print out altitude values
		screen_drawString(5, 20, "Alt:", screen.buffer);
		screen_sendBuffer(screen.buffer); 	// uploading the drawing
 		FloatToStringNew(buffer, gps.altitude , 1); 
 		screen_drawString(50, 20, buffer, screen.buffer);
 		screen_sendBuffer(screen.buffer); 	// uploading the drawing// 
 		
 		// print out time values
 		sprintf(buffer, "hour %d, min %d, sec %d", gps.hour, gps.minute, gps.seconds);
 		screen_drawString(5, 30, buffer, screen.buffer);
 		
 		// print out satellites
 		sprintf(buffer, "satellites %d",gps.satellites); 
 		screen_drawString(5, 40, buffer, screen.buffer);
 		
 		_delay_ms(500);
	}

    return 0;
}
