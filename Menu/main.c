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
#include "../RFM/RFM69.h"
#include "../SPI/SPI_control.h"

#define redbut (1<<PD6)
#define greenbut (1<<PD7)
#define bluebut (1<<PB0)

#define ON 1 
#define OFF 0 

//global variables
uint8_t mode = 1;		//float so can print out for testing
uint8_t new_mode = 1; 	// this is a flag for entering into a new mode 
uint16_t steps = 0;		// step counter

// radio, screen and GPS structs
struct Screen screen; 
struct GPS gps;
struct RFM69 radio;

char data[15]; 
char buffer[23];

// heart rate struct
volatile struct HR_data HR = {0,0,0,0,0,0,0,0,0,0};

void init()
{
	// -------------------- screen --------------------------
	digitalWrite(25,0); // screen reset
	_delay_ms(10);
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

	// -------------------- Radio ------------------------------
	DDRD &= ~(1 << DDD2) ; 
	DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output
	PORTD |= (1 << PORTD2); 
	EICRA |= (1 << ISC00) | (1<<ISC01); // set it for rising edge 
	EIMSK |= (1 << INT0); 
	PCMSK0 |= 0x80;

	radio.slaveSelectPin = 24;
	radio.currentMode = 0;
	radio.buffer_length = 0;
	radio.packet_sent = 0;
	gps.sizeInputString = 0; 
	RFM_spiConfig(radio.slaveSelectPin);
	spi_init_master();			// SPI
	RFM_init(radio.slaveSelectPin);
}


void new_mode_test()
{
	if (new_mode)
	{
		new_mode = 0; 
		memset(screen.buffer,0,1024);
 		screen_sendBuffer(screen.buffer);
 		_delay_ms(2); 
 		radio.packet_sent = 0;

 		if (mode == 1)
 		{
 			RFM_setMode(&radio.currentMode, 0, radio.slaveSelectPin); 
 		}
 		else if (mode == 3) // want to start the heart rate measuring 
 		{
 			HR_start(&HR);
 		}
 		else if (mode == 4 || mode == 2)
 		{
 			HR_stop(&HR);
 			RFM_setMode(&radio.currentMode, 0, radio.slaveSelectPin); // idle 
 		}
 		else if (mode == 5)
 		{
 			RFM_setMode(&radio.currentMode, 1, radio.slaveSelectPin); // RX
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
 			

 			// current step is to draw some strings
			GPS_readSerialInput(&gps);
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
            char latitude_remote[10];
            char longitude_remote[10];
            
 			new_mode_test(); 
 			GPS_readSerialInput(&gps); 	

 			if (radio.currentMode = 1)
 			{
 				memset(buffer,0,sizeof(buffer));
 				sprintf(buffer,"dist: ");
 				screen_drawString(5, 30, buffer, screen.buffer);
 				screen_sendBuffer(screen.buffer);
 			}
 		
 			if (radio.receiveDataFlag)
 			{
 				radio.receiveDataFlag = 0; //reset the flag 
 				radio.buffer_length = Read_FIFO(radio.buffer, &radio.currentMode, radio.slaveSelectPin);
 				RFM_setMode(&radio.currentMode, 1, radio.slaveSelectPin); // if we want to continue recieving
                
 				char* split; 
 				char* split_string[2]; 
 				char i = 0; 
 				split = strtok(radio.buffer,'\n');
 				while (split != NULL)
    			{
        			split_string[i++] = split;
        			split = strtok(NULL, ",");
    			}

    			float dist = GPS_calculate(&gps, atof(split_string[0]), atof(split_string[1])); 
    			memset(buffer,0,sizeof(buffer));
    			FloatToStringNew(data, dist, 6);
    			screen_drawString(50, 30, data, screen.buffer);

 			}
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

ISR(INT0_vect) {
	serial_outputString("I ");
	// sets to idle, which is needed to know which packet was sent
	if (radio.currentMode == 2) // if in transmit 
	{
		radio.packet_sent = RFM_interruptHandler(&radio.currentMode, radio.slaveSelectPin);
	}
	else if (radio.currentMode == 1) // reciever 
	{
		radio.receiveDataFlag = RFM_interruptHandler(&radio.currentMode, radio.slaveSelectPin) ;
	}
}

