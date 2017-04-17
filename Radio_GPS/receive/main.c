/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h> 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// include other libraries from other folders
#include "../../Serial/serial.h" 
#include "../../Digital_IO/DigitalIo.h"
#include "../../RFM/RFM69.h"
#include "../../SPI/SPI_control.h"

// include the GPS struct as well
#include "../../GPS/GPS.h"

#define Serial_rate 47
struct RFM69 radio;
struct GPS gps_local;		// receiver GPS
struct GPS gps_remote;		// sender GPS

void interruptInit()
{
	DDRD &= ~(1 <<DDD2) ; 
	PORTD |= (1<<PORTD2); 
	EICRA |= (1<<ISC00) | (1<<ISC01); // set it for rising edge 
	EIMSK |= (1 << INT0); 
	sei(); 
	PCMSK0 |= 0x80;
}

int main(int argc, const char **argv)
{
	// initialize the interrupt
	interruptInit();			// Interrupts
	serial_init(Serial_rate);	// Serial
	spi_init_master();			// SPI

	// initialize the radio and constants
	radio.slaveSelectPin = 24;
	radio.currentMode = 0;
	radio.buffer_length = 0;
	radio.packet_sent = 0;
	
	// setting up RFM parameters
	RFM_spiConfig(radio.slaveSelectPin);
	RFM_init(radio.slaveSelectPin);
	
	RFM_setMode(&radio.currentMode, 1, radio.slaveSelectPin); // RX

	// remote latitude & longitude elements
	char latitude_remote[50];
	char longitude_remote[50];

	// other buffer
	char buffer[50];

    while (1) 
    {
    	PORTC |= 1 << PC0;      		// Set PC0 to a 1
		PORTC &= ~(1 << PC0);   		// Set PC0 to a 0

		// need this if case for reading the data
		if (radio.receiveDataFlag)
		{
			// need these three functions in this order to work. 
			radio.receiveDataFlag = 0; //reset the flag 
			radio.buffer_length = Read_FIFO(radio.buffer, &radio.currentMode, radio.slaveSelectPin);
			// have to do this after receiving somehting 
			RFM_setMode(&radio.currentMode, 1, radio.slaveSelectPin); // set mode to RX

			serial_outputString(radio.buffer); 

			// next part: calculating distance based on parsed data
			int index = 0;

			// subpart 1: extracting latitude from sent data
			while (radio.buffer[index] != '\n') {
				latitude_remote[index] = radio.buffer[index];
				index++;
			}

			index += 1;
			int secIndex = 0;

			// subpart 2: extracting longitude from sent data
			while (radio.buffer[index] != '\n') {
				longitude_remote[secIndex] = radio.buffer[index];
				secIndex++;
				index++;
			}

			// ***** do this the old-school way **********//

			// both sent latitude & longitude should be extracted
			// FloatToStringNew(latitude_local, gps_local.latitude, 6);
			// FloatToStringNew(longitude_local, gps_local.longitude, 6);

			// update remote GPS struct values
			gps_remote.latitude = atof(latitude_remote);
			gps_remote.longitude = atof(longitude_remote);

			float R = 6371000;		// value in meters
			float phi1 = (gps_local.latitude) * M_PI / 180;
			float phi2 = (atof(latitude_remote)) * M_PI / 180;
			float lambda1 = (gps_local.longitude) * M_PI / 180;
			float lambda2 = (atof(longitude_remote)) * M_PI / 180;

			float a = sin((phi1-phi2)/2)*sin((phi1-phi2)/2) + cos(phi1)*cos(phi2)*sin((lambda1-lambda2)/2)*sin((lambda1-lambda2)/2);
			float c = 2*atan2(sqrt(a),sqrt(1-a));
			float d = (R*c) / 1000;		// obtain value in km

			// calculating GPS distance and displaying on the serial screen, for now
			FloatToStringNew(buffer, d , 6); 
			serial_outputString("Distance in km: ");
			serial_outputString(buffer);
		}
		_delay_ms(2);
    }

    return 0;   /* never reached */
}

// hardware interrupts, if needed

ISR(INT0_vect) {
	// sets to idle, which is needed to know which packet was sent
	radio.receiveDataFlag = RFM_interruptHandler(&radio.currentMode, radio.slaveSelectPin) ;
}