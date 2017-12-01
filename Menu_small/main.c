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
void draw_compass();

//global variables
uint8_t mode = 1;		//float so can print out for testing
uint8_t new_mode = 1; 	// this is a flag for entering into a new mode
uint8_t vibrate     = 10; 		// this is to control the vibrate
char vibrate_notify = 22;
uint8_t update_GPS_count  = 0;

uint16_t steps = 0;		// step counter

float last_lat        = 0;		//this is for caluclating cumulative distence
float last_long       = 0;
float distence_travled = 0;

float float_value = 0.0;

// radio, screen and GPS structs
struct Screen screen;
struct GPS gps;
struct RFM69 radio;


char buffer[28];

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
	RFM_setMode(&radio.currentMode, 1, slaveSelectPin); // RX

	// -------------------- Timer ------------------------------
	// Set the Timer Mode to CTC
    TCCR0A |= (1 << WGM01);
 //    // .06*7372800 / 1024 = 219.88 so 60ms
    OCR0A = 220;
    TIMSK0 |= (1 << OCIE0A); // tunring on interrupts
    TCCR0B |= (1 << CS02) | (1 << CS00); //this starts the timer and restarts it if neceassry. for 1024
 //    // TCCR0B |= (1 << CS02); for 254
    DDRD |= (1 << 4);
    PORTD |= (1 << 4);
    vibrate_notify = 1; // for testing



}

void update_GPS()
{
	if (update_GPS_count == 10)
	{
			GPS_readSerialInput(&gps);
			update_GPS_count = 0;
	}
	else
	{
		update_GPS_count++;
	}
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
 		if (mode != 5)
 		{
 			RFM_setMode(&radio.currentMode, 1, slaveSelectPin); // RX
 		}
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
			// GPS_readSerialInput(&gps);
			update_GPS();
			// GPS_printInfo(&gps);



 			// print out time values

 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer, "%d:%d:%d", gps.hour, gps.minute, gps.seconds);
 			screen_drawString(50, 30, buffer, screen.buffer);

 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer,"%d",mode);
 			screen_drawString(120, 60, buffer, screen.buffer);

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
 			update_GPS();
 			memset(screen.buffer,0,sizeof(screen.buffer));

 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer, "steps : %d", steps);
 			screen_drawString(0, 20, buffer, screen.buffer);

 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer,"%d",mode);
 			screen_drawString(120, 60, buffer, screen.buffer);


			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"Dist Travled: ");
			FloatToStringNew(&(buffer[14]),distence_travled,5);
			screen_drawString(0, 40, buffer, screen.buffer);

 			if (gps.satellites < 2)
 			{
 				screen_drawFillCircle(120, 5, 2, ON, screen.buffer);
 			}


 			screen_sendBuffer(screen.buffer);

 		}

 		// mode 3: display temp

 		else if (mode==3)
 		{

 			new_mode_test();

 			LSM_getHeading(&float_value);
 			memset(buffer,0,sizeof(buffer));
 			FloatToStringNew(buffer,float_value,3);
 			strcat(buffer, " degrees");
			screen_drawString(0, 5, buffer, screen.buffer);

			LSM_getTemp(&float_value);
			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer, "temp: ");
 			FloatToStringNew(&(buffer[6]),float_value,2);
 			screen_drawString(0, 40, buffer, screen.buffer);



 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer,"%d",mode);
 			screen_drawString(120, 60, buffer, screen.buffer);


 			if (gps.satellites == 0)
 			{
 				screen_drawFillCircle(120, 10, 2, ON, screen.buffer);
 			}


 			screen_sendBuffer(screen.buffer);


 		}

 		// mode 4: GPS data
 		else if (mode==4)
 		{
 			new_mode_test();
 			// GPS_readSerialInput(&gps);
 			update_GPS();

 			// current step is to draw some strings
			GPS_readSerialInput(&gps);
			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"long: ");
 		    FloatToStringNew(&(buffer[6]),gps.longitude , 6);
		    screen_drawString(5, 5, buffer, screen.buffer);

			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"lat: ");
		    FloatToStringNew(&(buffer[5]),gps.latitude , 6);
		    screen_drawString(5, 20, buffer, screen.buffer);

			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"altitude: ");
		    FloatToStringNew(&(buffer[10]),gps.altitude , 1);
		    screen_drawString(5, 35, buffer, screen.buffer);

		    memset(buffer,0,sizeof(buffer));
 			sprintf(buffer, "satellites %d",gps.satellites);
 			screen_drawString(5, 50, buffer, screen.buffer);

 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer,"%d",mode);
 			screen_drawString(120, 60, buffer, screen.buffer);


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
 			// GPS_readSerialInput(&gps);
 			update_GPS();

 			//to get acutal gps location
 			char data[15];
 			memset(buffer,0,sizeof(buffer));
 			memset(data,0,sizeof(data));
 			FloatToStringNew(data,gps.latitude,6);
 			strcat(buffer,data);
 			strcat(buffer,",");
 			memset(data,0,sizeof(data));
 			FloatToStringNew(data,gps.longitude,6);
 			strcat(buffer,data);

 			//for fake locaiton
 			// memset(buffer,0,sizeof(buffer));
 			// sprintf(buffer,"34.024212,-118.28814");



 			RFM_send(buffer,&radio.currentMode, sizeof(buffer), slaveSelectPin);


 			draw_compass();



 			memset(buffer,0,sizeof(buffer));
 			sprintf(buffer,"%d",mode);
 			screen_drawString(120, 60, buffer, screen.buffer);


 			if (gps.satellites == 0)
 			{
 				screen_drawFillCircle(120, 10, 2, ON, screen.buffer);
 			}

 			screen_sendBuffer(screen.buffer);


 		}
 		else if (mode == 6)
 		{
 			memset(buffer,0,sizeof(buffer));
 			// sprintf(buffer,"%d",buffer);
 			screen_drawString(50, 30, "got it", screen.buffer);
 			screen_sendBuffer(screen.buffer);
 			if (radio.receiveDataFlag)
 			{
 				mode = 5;
 				new_mode = 1;
 			}
 			// RFM_setMode(&radio.currentMode, 1, slaveSelectPin); // RX
 		}
	}


	return 0;
}

void draw_compass()
{
	LSM_getHeading(&float_value);

	char dir_1[2];
	char dir_2[2];
	char dir_3[2];
	char dir_4[2];

	memset(buffer,0,sizeof(buffer));
	if ( float_value > 290 || float_value < 10 ) //facing north
	{

		strcpy(dir_1,"n");
		strcpy(dir_2,"w");
		strcpy(dir_3,"s");
		strcpy(dir_4,"e");
	}


	else if (float_value <= 290 && float_value > 150 ) // facing west
	{

		strcpy(dir_1,"w");
		strcpy(dir_2,"s");
		strcpy(dir_3,"e");
		strcpy(dir_4,"n");
	}
	else if (float_value <= 150 && float_value > 80 ) //facing south
	{
		strcpy(dir_1,"s");
		strcpy(dir_2,"e");
		strcpy(dir_3,"n");
		strcpy(dir_4,"w");
	}
	else if (float_value <= 80 && float_value >= 10) // facing east
	{
		strcpy(dir_1,"e");
		strcpy(dir_2,"n");
		strcpy(dir_3,"w");
		strcpy(dir_4,"s");
	}



	screen_drawString(60, 10, dir_1, screen.buffer);
	screen_drawString(30, 30, dir_2, screen.buffer);
	screen_drawString(60, 50, dir_3, screen.buffer);
	screen_drawString(90, 30, dir_4, screen.buffer);


	memset(buffer,0,sizeof(buffer));
	FloatToStringNew(buffer,float_value,3);
	strcat(buffer, " deg");
	screen_drawString(0, 60, buffer, screen.buffer);

	screen_sendBuffer(screen.buffer);

}


// button interrupt commands --------------------------------------------
ISR(PCINT2_vect)
{

}

ISR(PCINT0_vect)
{
	if ((PINB & bluebut)==0)
	{
		vibrate = 1;
		digitalWrite(6,1);

		new_mode = 1;

		mode = mode+1;
		if (mode >= 6)
		{
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

ISR(TIMER1_COMPA_vect) //every 5 seconds
{

	if (gps.satellites > 1)
	{
		if ((last_long != last_lat) && (last_lat != gps.latitude) && (last_long != gps.longitude))
		{
			distence_travled += GPS_calculate(&gps, last_lat, last_long);
		}
		last_lat  = gps.latitude;
		last_long = gps.longitude;
	}

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

ISR (TIMER0_COMPA_vect)  // every .06s
{

	// serial_outputString("time");
	if (vibrate <= 5)
	{
		digitalWrite(6,1);
		vibrate++;
	}
	else if (vibrate == 6)
	{
		digitalWrite(6,0);
	}

	if (vibrate_notify <= 10)
	{
		digitalWrite(6,1);
		vibrate_notify++;
	}
	else if (vibrate_notify == 11)
	{
		digitalWrite(6,0);
	}

}

ISR(INT0_vect) {
	// serial_outputString("I ");
	// sets to idle, which is needed to know which packet was sent
	// memset(buffer,0,sizeof(buffer));
 // 	screen_drawString(50, 30, "interrupt", screen.buffer);
 // 	screen_sendBuffer(screen.buffer);

	if (radio.currentMode == 2) // if in transmit
	{
		radio.packet_sent = RFM_interruptHandler(&radio.currentMode, slaveSelectPin);
	}
	else if (radio.currentMode == 1) // reciever
	{
		radio.receiveDataFlag = RFM_interruptHandler(&radio.currentMode, slaveSelectPin) ;
		if (mode != 6)
		{
			mode = 6;
			new_mode = 1;
			vibrate_notify = -4;
			digitalWrite(6,1);
		}
	}
}
