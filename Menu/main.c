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


	// -------------------- Serial --------------------------
	serial_init(47);


	// -------------------- GPS --------------------------

	// initialize GPS values
	gps.sizeInputString = 0; 
	gps.state = 0;


	// -------------------- Accelerometer --------------------------
	// hardware interrupt 1 for the acc 
	DDRD  &= ~(1 << DDD3); 
	PORTD |= (1<<PORTD3);
	EICRA |= (1<<ISC10) | (1 << ISC11);
	EIMSK |= (1<< INT1);  
	LSM_init(); 

	// -------------------- Heart_rate --------------------------
	HR_init(); 


}


void new_mode_test()
{
	if (new_mode)
	{
		new_mode = 0; 
		memset(screen.buffer,0,1024);
 		screen_sendBuffer(screen.buffer);
 		_delay_ms(2); 

 		if (mode == 3) // want to start the heart rate measuring 
 		{
 			HR_start(&HR);
 		}
 		else if (mode == 4 || mode == 2)
 		{
 			HR_stop(&HR);
 		}
	}
	else 
	{
		return ; 
	}
}

int main (int argc, const char * argv[])	{
	init();
	sei();
	char buffer[23];

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
 			screen_clear(screen.buffer);
 			screen_drawString(5, 30, buffer, screen.buffer);
 			screen_sendBuffer(screen.buffer);
 		}
 		
 		// mode 3: track heart rate

 		else if (mode==3)	
 		{
 			
 			new_mode_test();
 			char hr[15];
 			sprintf(hr, "Heart Rate : %d", HR.BPM);
 			memset(screen.buffer,0,1024);
 			screen_drawString(5, 30, hr, screen.buffer);
 			screen_sendBuffer(screen.buffer);
 			// screen_drawFillCircle(10,10,10,1,screen.buffer);
 			// screen_sendBuffer(screen.buffer);
 		}

 		// mode 4: GPS data 
 		else if (mode==4)	
 		{
 			new_mode_test();
 			GPS_readSerialInput(&gps);
 			char data[15]; 

 			// current step is to draw some strings
			GPS_readSerialInput(&gps);
			// prints out GPS data into the serial screen
			//GPS_printInfo(&gps); 
		
			// prints out latitude values
			// screen_drawString(5, 0, "Lat:", screen.buffer);
			// screen_sendBuffer(screen.buffer); 	// uploading the drawing
 		// 	FloatToStringNew(lat_data, gps.latitude , 6); 
			// screen_drawString(50, 0, lat_data, screen.buffer);
			// screen_sendBuffer(screen.buffer); 	// uploading the drawing
		
			// // print out longitude values
			// screen_drawString(5, 10, "Long:", screen.buffer);
			// screen_sendBuffer(screen.buffer); 	// uploading the drawing
 		// 	FloatToStringNew(log_data, gps.longitude , 6); 
 		// 	screen_drawString(50, 10, log_data, screen.buffer);
 		// 	screen_sendBuffer(screen.buffer); 	// uploading the drawing
		
			// // print out altitude values
			// screen_drawString(5, 20, "Alt:", screen.buffer);
			// screen_sendBuffer(screen.buffer); 	// uploading the drawing
 		// 	//FloatToStringNew(buffer, gps.altitude , 1); 
 		// 	FloatToStringNew(alt_data, gps.altitude , 1); 
 		// 	screen_drawString(50, 20, alt_data, screen.buffer);
 		// 	screen_sendBuffer(screen.buffer); 	// uploading the drawing// 
 		// 	// print out satellites
 		// 	sprintf(buffer, "satellites %d",gps.satellites); 
 		// 	screen_drawString(5, 30, buffer, screen.buffer);
 		// 	screen_sendBuffer(screen.buffer); 
			memset(data,0,sizeof(data));
 		    FloatToStringNew(data,gps.longitude , 6); 
		    sprintf(buffer,"Longitude: ");
		    strcat(buffer,data); 
		    screen_drawString(5, 5, buffer, screen.buffer);
			
			memset(data,0,sizeof(data));
		    FloatToStringNew(data,gps.latitude , 6); 
		    sprintf(buffer,"Latitude: ");
		    strcat(buffer,data); 
		    screen_drawString(5, 20, buffer, screen.buffer);
			
			memset(data,0,sizeof(data));
		    FloatToStringNew(data,gps.altitude , 1); 
		    sprintf(buffer,"Altitude: ");
		    strcat(buffer,data); 
		    screen_drawString(5, 35, buffer, screen.buffer);

		    memset(data,0,sizeof(data));
 			sprintf(buffer, "satellites %d",gps.satellites); 
 			screen_drawString(5, 50, buffer, screen.buffer);
 			screen_sendBuffer(screen.buffer);


 		}

 		else if (mode == 5)
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

		// char bufferbut[10];
		// sprintf(bufferbut,"redbut %d",mode);
		// serial_outputString(bufferbut);	
		_delay_ms(1);
		while ((PIND & redbut)==0)	{}
		_delay_ms(1);
	}
	
	else if ((PIND & greenbut)==0)	
	{
		mode = mode-1;
		if (mode == 0)	{
			mode = 5;
		}
		// char bufferbut[10];
		// sprintf(bufferbut,"green %d",mode);
		// serial_outputString(bufferbut);	
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
		if (mode == 6)	{
			mode = 1;
		}
		// char bufferbut[10];
		// sprintf(bufferbut,"blue %d",mode);
		// serial_outputString(bufferbut);	
		_delay_ms(1);
		while ((PINB & bluebut)==0)	{}
		_delay_ms(1);
	}
}


//Hardware interrupt for the accelerometer 
ISR(INT1_vect)
{
	steps++ ; 
}

// for the adc 
ISR(ADC_vect)
{
	HR_read(&HR); 
}

ISR(TIMER1_COMPA_vect)
{
	serial_outputString("time");
	HR_calc_BPM(&HR);
}



