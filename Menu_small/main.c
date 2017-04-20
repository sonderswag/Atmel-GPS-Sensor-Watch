#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <stdint.h>
#include <avr/interrupt.h>

#include "../Serial/serial.h" 
#include "../Heart_rate/Heart_rate.h"
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
#define slaveSelectPin 24

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


	// -------------------- Serial --------------------------
	serial_init(47);

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



	// -------------------- GPS --------------------------

	// initialize GPS values
	// gps.sizeInputString = 0; 
	// gps.state = 0;
	gps.hour = 12; 
	gps.minute = 0;
	gps.seconds = 0; 


	// -------------------- Accelerometer --------------------------
	// hardware interrupt 1 for the acc 
	DDRD  &= ~(1 << DDD3); 
	PORTD |= (1<<PORTD3);
	EICRA |= (1<<ISC10) | (1 << ISC11);
	EIMSK |= (1<< INT1);  
	LSM_init(); 

	// -------------------- Heart_rate --------------------------
	HR_init(); //need it to init the timer 

	// -------------------- Radio ------------------------------
	DDRD &= ~(1 << DDD2) ; 
	DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output
	PORTD |= (1 << PORTD2); 
	EICRA |= (1 << ISC00) | (1<<ISC01); // set it for rising edge 
	EIMSK |= (1 << INT0); 
	PCMSK0 |= 0x80;

	// radio.slaveSelectPin = 24;
	radio.currentMode = 0;
	// radio.buffer_length = 0;
	radio.packet_sent = 0;
 
	RFM_spiConfig(slaveSelectPin);
	spi_init_master();			// SPI
	RFM_init(slaveSelectPin);



}


void new_mode_test()
{
	if (new_mode)
	{

 		// sprintf(buffer,"mode: %d",mode);
		// serial_outputString(buffer);	
		new_mode = 0; 
		memset(screen.buffer,0,1024);
 		screen_sendBuffer(screen.buffer);
 		_delay_ms(2); 
 		radio.packet_sent = 0;
	}
	else 
	{
		return ; 
	}
}

int main (void)	{
	
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
		uint16_t a;
		for (a=0; a<=5000; a++)	
		{ }
		if (mode==1)	// this is to show 
		{			
			new_mode_test();
			// current step is to draw some strings
			GPS_readSerialInput(&gps);
			// GPS_printInfo(&gps); 
			


 			// print out time values
 			
 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer, "%d:%d:%d", gps.hour, gps.minute, gps.seconds);
 			screen_drawString(50, 30, buffer, screen.buffer);  

 			memset(data,0,sizeof(data));
 			sprintf(data,"%d",mode);
 			screen_drawString(120, 50, data, screen.buffer);  

 			if (gps.satellites == 0)
 			{
 				screen_drawFillCircle(120, 10, 2, ON, screen.buffer);
 			}

 			screen_sendBuffer(screen.buffer);
 		}
 		
 		// mode 2: track steps
 		else if (mode==2)	
 		{
 			new_mode_test();
 			sei();
 			sprintf(buffer, "steps : %d", steps);
 			memset(screen.buffer,0,sizeof(screen.buffer));
 			screen_drawString(5, 30, buffer, screen.buffer);

 			memset(data,0,sizeof(data));
 			sprintf(data,"%d",mode);
 			screen_drawString(120, 50, data, screen.buffer);  

 			if (gps.satellites == 0)
 			{
 				screen_drawFillCircle(120, 10, 2, ON, screen.buffer);
 			}

 			screen_sendBuffer(screen.buffer);
 			sei();

 		}
 		
 		// mode 3: display temp

 		else if (mode==3)	
 		{
 			
 			new_mode_test();
 			float temp; 
			LSM_getTemp(&temp);
			memset(buffer,0,sizeof(buffer));
 			memset(data,0,sizeof(data));
 			FloatToStringNew(data,temp,2);
 			sprintf(buffer, "temp: "); 
 			strcat(buffer,data);
 			screen_drawString(5, 40, buffer, screen.buffer);
 			


 			memset(data,0,sizeof(data));
 			sprintf(data,"%d",mode);
 			screen_drawString(120, 50, data, screen.buffer); 


 			if (gps.satellites == 0)
 			{
 				screen_drawFillCircle(120, 10, 2, ON, screen.buffer);
 			}


 			screen_sendBuffer(screen.buffer);
 			
 			// char hr[15];
 			// sprintf(hr, "Heart Rate : %d", HR.BPM);
 			// memset(screen.buffer,0,sizeof(screen.buffer));
 			// screen_drawString(5, 30, hr, screen.buffer);
 			// screen_sendBuffer(screen.buffer);
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
		    sprintf(buffer,"longitude: ");
		    strcat(buffer,data); 
		    screen_drawString(5, 5, buffer, screen.buffer);
			
			memset(data,0,sizeof(data));
		    FloatToStringNew(data,gps.latitude , 6); 
		    sprintf(buffer,"latitude: ");
		    strcat(buffer,data); 
		    screen_drawString(5, 20, buffer, screen.buffer);
			
			memset(data,0,sizeof(data));
		    FloatToStringNew(data,gps.altitude , 1); 
		    sprintf(buffer,"altitude: ");
		    strcat(buffer,data); 
		    screen_drawString(5, 35, buffer, screen.buffer);

		    memset(data,0,sizeof(data));
 			sprintf(buffer, "satellites %d",gps.satellites); 
 			screen_drawString(5, 50, buffer, screen.buffer);

 			memset(data,0,sizeof(data));
 			sprintf(data,"%d",mode);
 			screen_drawString(110, 50, data, screen.buffer);  


 			if (gps.satellites == 0)
 			{
 				screen_drawFillCircle(120, 10, 2, ON, screen.buffer);
 			}


 			screen_sendBuffer(screen.buffer);
 		}

 		else if (mode == 5)
 		{
            // char latitude_remote[10];
            // char longitude_remote[10];
            
 			new_mode_test(); 
 			GPS_readSerialInput(&gps); 	

 			//to get acutal gps location

 			// memset(buffer,0,sizeof(buffer)); 
 			// memset(data,0,sizeof(data)); 	
 			// FloatToStringNew(data,gps.latitude,6);
 			// strcat(buffer,data);
 			// strcat(buffer,",");
 			// memset(data,0,sizeof(data)); 
 			// FloatToStringNew(data,gps.longitude,6);
 			// strcat(buffer,data);

 			//for fake locaiton 
 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer,"-118.28814,34.024212");



 			RFM_send(buffer,&radio.currentMode, sizeof(buffer), slaveSelectPin);

 			// if (radio.currentMode = 1)
 			// {
 			// 	memset(buffer,0,sizeof(buffer));
 			// 	sprintf(buffer,"dist: ");
 			// 	screen_drawString(5, 30, buffer, screen.buffer);
 			// 	screen_sendBuffer(screen.buffer);
 			// }
 		
 			// if (radio.receiveDataFlag)
 			// {
 			// 	radio.receiveDataFlag = 0; //reset the flag 
 			// 	radio.buffer_length = Read_FIFO(radio.buffer, &radio.currentMode, radio.slaveSelectPin);
 			// 	RFM_setMode(&radio.currentMode, 1, radio.slaveSelectPin); // if we want to continue recieving
                
 			// 	char* split; 
 			// 	char* split_string[2]; 
 			// 	char i = 0; 
 			// 	split = strtok(radio.buffer,'\n');
 			// 	while (split != NULL)
    // 			{
    //     			split_string[i++] = split;
    //     			split = strtok(NULL, ",");
    // 			}

    // 			// float dist = GPS_calculate(&gps, atof(split_string[0]), atof(split_string[1])); 
    // 			memset(buffer,0,sizeof(buffer));
    // 			// FloatToStringNew(data, dist, 6);
    // 			screen_drawString(50, 30, split_string[0], screen.buffer);

 			// }

 			memset(data,0,sizeof(data));
 			sprintf(data,"%d",mode);
 			screen_drawString(110, 50, data, screen.buffer);  


 			if (gps.satellites == 0)
 			{
 				screen_drawFillCircle(120, 10, 2, ON, screen.buffer);
 			}

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
		_delay_ms(2);
		while ((PIND & redbut)==0)	{}
		_delay_ms(2);
	}
	
	else if ((PIND & greenbut)==0)	
	{
		mode = mode-1;
		if (mode <= 0)	{
			mode = 5;
		}
		// char bufferbut[10];
		// sprintf(bufferbut,"green %d",mode);
		// serial_outputString(bufferbut);	
		_delay_ms(2);
		while ((PIND & greenbut)==0)	{}
		_delay_ms(2);
	}
}

ISR(PCINT0_vect)	
{
	new_mode = 1; 
	if ((PINB & bluebut)==0)	{
		mode = mode+1;
		if (mode >= 6)	{
			mode = 1;
		}
		// char bufferbut[10];
		// sprintf(buffer,"blue %d",mode);
		// serial_outputString(buffer);	
		_delay_ms(2);
		while ((PINB & bluebut)==0)	{}
		_delay_ms(2);
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
	// HR_read(&HR); 
}

ISR(TIMER1_COMPA_vect)
{
	// serial_outputString("time");
	if (gps.satellites == 0) // doesn't have a fix 
	{
		gps.seconds += 5; 
		if (gps.seconds >= 60)
		{
			gps.seconds = 0; 
			gps.minute += 1; 
		}	
		if (gps.minute >= 60)
		{
			gps.minute = 0; 
			gps.hour += 1; 
		}
		if (gps.hour >= 24)
		{
			gps.hour = 0; 
		}
	}

	// if (mode == 3)
	// {
	// 	// HR_calc_BPM(&HR);
	// }

}

ISR(INT0_vect) {
	serial_outputString("I ");
	// sets to idle, which is needed to know which packet was sent
	if (radio.currentMode == 2) // if in transmit 
	{
		radio.packet_sent = RFM_interruptHandler(&radio.currentMode, slaveSelectPin);
	}
	else if (radio.currentMode == 1) // reciever 
	{
		radio.receiveDataFlag = RFM_interruptHandler(&radio.currentMode, slaveSelectPin) ;
	}
}

