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
// heart rate struct

char data[15]; 
char buffer[23];


volatile struct HR_data HR;




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
	HR_init(); 
	HR_start(&HR);

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
	RFM_setMode(&radio.currentMode, 1, slaveSelectPin); // RX

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
		
// 
//  		if (mode == 3) // want to start the heart rate measuring 
//  		{
//  			HR_start(&HR);
//  		}
//  		else if (mode == 4 || mode == 2)
//  		{
//  			HR_stop(&HR);
//  		}

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
		for (a=0; a<=10000; a++)	
		{ }
		if (mode==1)	// this is to show 
		{			

			new_mode_test();
			// current step is to draw some strings
			GPS_readSerialInput(&gps);
			// GPS_printInfo(&gps); 
			float temp; 
			LSM_getTemp(&temp);


 			// print out time values
 			memset(screen.buffer,0,sizeof(screen.buffer));
 			memset(data,0,sizeof(data));
 			sprintf(buffer, "%d:%d:%d", gps.hour, gps.minute, gps.seconds);


 			screen_drawString(50, 30, buffer, screen.buffer); 

 			memset(buffer,0,sizeof(buffer));
 			memset(data,0,sizeof(data));
 			FloatToStringNew(data,temp,2);
 			sprintf(buffer, "temp: "); 
 			strcat(buffer,data);
 			screen_drawString(5, 40, buffer, screen.buffer);

 			memset(data,0,sizeof(data));
 			sprintf(data,"%d",mode);
 			screen_drawString(110, 50, data, screen.buffer);  

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
 			sprintf(buffer, "steps : %d", steps);
 			screen_clear(screen.buffer);
 			screen_drawString(5, 30, buffer, screen.buffer);

 			memset(data,0,sizeof(data));
 			sprintf(data,"%d",mode);
 			screen_drawString(120, 50, data, screen.buffer);  

 			screen_sendBuffer(screen.buffer);
 		}
 		
 		// mode 3: track heart rate

 		else if (mode==3)	
 		{
 			
 			new_mode_test();
 			
 			memset(buffer,0,sizeof(buffer));
 			memset(data,0,sizeof(data));

 			sprintf(buffer, "BPM: %d", HR.BPM);
 			memset(screen.buffer,0,sizeof(screen.buffer));
 			screen_drawString(5, 30, buffer, screen.buffer);

 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer,"%d",mode);
 			screen_drawString(120, 50, buffer, screen.buffer);  
 			

 	// 		memset(data,0,sizeof(data));
//  			sprintf(data,"%d",steps);
//  			screen_drawString(10, 50, data, screen.buffer);  

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
		    sprintf(buffer,"Long: ");
		    strcat(buffer,data); 
		    screen_drawString(5, 5, buffer, screen.buffer);
			
			memset(data,0,sizeof(data));
		    FloatToStringNew(data,gps.latitude , 6); 
		    sprintf(buffer,"lat: ");
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
 			screen_drawString(120, 50, data, screen.buffer);  

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
 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer,"red button to track");
 			screen_drawString(5, 30, buffer, screen.buffer);
 			
 			screen_sendBuffer(screen.buffer);
 		}

 		else if (mode == 6)
 		{
 			new_mode_test(); 
 			GPS_readSerialInput(&gps); 	

 			if (radio.currentMode == 1)
 			{
 				memset(buffer,0,sizeof(buffer));
 				sprintf(buffer,"dist (km): ");
 				screen_drawString(5, 5, buffer, screen.buffer);
 				
 			}
 		
 			if (radio.receiveDataFlag)
 			{
 				radio.receiveDataFlag = 0; //reset the flag 
 				Read_FIFO(radio.buffer, &radio.currentMode, slaveSelectPin);
 				RFM_setMode(&radio.currentMode, 1, slaveSelectPin); // if we want to continue recieving
                
 				char* token; 
 				char* token_list[2]; 
 				char i = 0; 
 				token = strtok(radio.buffer,",");
 				while (token != NULL)
    			{
        			token_list[i++] = token;
        			token = strtok(NULL, ",");
    			}	
    			// float dist = GPS_calculate(&gps, atof(split_string[0]), atof(split_string[1])); 
//     			memset(buffer,0,sizeof(buffer));
//     			FloatToStringNew(data, dist, 6);
    			//screen_drawString(50, 5, token_list[0], screen.buffer);
    			
    			float dist = GPS_calculate(&gps, atof(token_list[0]), atof(token_list[1])); 
    			memset(buffer,0,sizeof(buffer));
    			FloatToStringNew(buffer, dist, 6);
    			screen_drawString(60, 5, buffer, screen.buffer);
				 			
 				//cheating way of finding bearing, not true bearing
				if (gps.latitude <= atof(token_list[0]) && fabsf(gps.longitude) >= fabsf(atof(token_list[1])))	{
 					screen_drawFillCircle(75, 20, 5, ON, screen.buffer);
 				}
 				else if (gps.latitude <= atof(token_list[0]) && fabsf(gps.longitude) < fabsf(atof(token_list[1])))	{
 					screen_drawFillCircle(45, 20, 5, ON, screen.buffer);
 				}
 				else if (gps.latitude > atof(token_list[0]) && fabsf(gps.longitude) < fabsf(atof(token_list[1])))	{
 					screen_drawFillCircle(45, 40, 5, ON, screen.buffer);
 				}
 				else if (gps.latitude > atof(token_list[0]) && fabsf(gps.longitude) >= fabsf(atof(token_list[1])))	{
 					screen_drawFillCircle(75, 40, 5, ON, screen.buffer);
 				}
 			}
		
		 	memset(data,0,sizeof(data));
 			sprintf(data,"N");
 			screen_drawString(60, 10, data, screen.buffer);  
 			screen_sendBuffer(screen.buffer);
 			memset(data,0,sizeof(data));
 			sprintf(data,"W");
 			screen_drawString(30, 30, data, screen.buffer);  
 			screen_sendBuffer(screen.buffer);
 			memset(data,0,sizeof(data));
 			sprintf(data,"E");
 			screen_drawString(90, 30, data, screen.buffer);  
 			screen_sendBuffer(screen.buffer);
 			memset(data,0,sizeof(data));
 			sprintf(data,"S");
 			screen_drawString(60, 50, data, screen.buffer);  
 			screen_sendBuffer(screen.buffer);
 					 	
 			memset(data,0,sizeof(data));
 			sprintf(data,"%d",mode);
 			screen_drawString(120, 50, data, screen.buffer);  
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
		if (mode == 5)
		{
			mode++ ; 
		}

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
		if (mode <= 0)	{
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
		if (mode >= 6)	{
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

ISR(TIMER1_COMPA_vect) //happens every 5 s 
{
	// serial_outputString("time");
	// if (gps.satellites == 0) // doesn't have a fix 
	// {
	// 	gps.seconds += 5; 
	// 	if (gps.seconds >= 60)
	// 	{
	// 		gps.seconds = 0; 
	// 		gps.minute += 1; 
	// 	}	
	// 	if (gps.minute >= 60)
	// 	{
	// 		gps.minute = 0; 
	// 		gps.hour += 1; 
	// 	}
	// 	if (gps.hour >= 24)
	// 	{
	// 		gps.hour = 0; 
	// 	}
	// }

	if (mode == 3)
	{
		HR_calc_BPM(&HR);
	}

}

ISR(INT0_vect) {
	serial_outputString("I ");
	// sets to idle, which is needed to know which packet was sent
	if (radio.currentMode == 2) // if in transmit 
	{
		radio.packet_sent = RFM_interruptHandler(&radio.currentMode, slaveSelectPin);
	}
	else if ((radio.currentMode == 1)) // reciever 
	{
		radio.receiveDataFlag = RFM_interruptHandler(&radio.currentMode, slaveSelectPin) ;
		if (mode != 6)
		{
			mode = 6;
			new_mode = 1;
		}
		
	}
}

