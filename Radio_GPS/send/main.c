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

#include "../../Serial/serial.h" 
#include "../../GPS/GPS.h" 

// include other libraries from other folders
#include "../../Digital_IO/DigitalIo.h"
#include "../../RFM/RFM69.h"
#include "../../SPI/SPI_control.h"

#define Serial_rate 47
struct RFM69 radio;
struct GPS gps; 

void interruptInit()
{
	DDRD &= ~(1 << DDD2) ; 
	PORTD |= (1 << PORTD2); 
	EICRA |= (1 << ISC00) | (1<<ISC01); // set it for rising edge 
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

    DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output

	// initialize the radio and constants
	radio.slaveSelectPin = 24;
	radio.currentMode = 0;
	radio.buffer_length = 0;
	radio.packet_sent = 0;

	// setting up the GPS struct
	gps.sizeInputString = 0; 
	
	// setting up RFM parameters
	RFM_spiConfig(radio.slaveSelectPin);
	RFM_init(radio.slaveSelectPin);
	
	// initialize interrupts
	sei();
	
	// setting up the character buffer
	char buffer[50];
	char bufferMaster[60];

    while (1) 
    {
		PORTC |= 1 << PC0;      // Set PC0 to a 1
		PORTC &= ~(1 << PC0);   // Set PC0 to a 0
		
		GPS_readSerialInput(&gps); 			// reading serial input
		GPS_printInfo(&gps);  				// print out serial values of char
		
		// converting float to string: start with latitude
		/*
		FloatToStringNew(buffer, gps.latitude , 6); 
		
		// sending the char through the RFM69
		RFM_send(buffer, &radio.currentMode, sizeof(buffer), radio.slaveSelectPin);
		_delay_ms(2);

		FloatToStringNew(buffer, gps.longitude, 6); 
		RFM_send(buffer, &radio.currentMode, sizeof(buffer), radio.slaveSelectPin);
		_delay_ms(2);
		*/

		int i;

		PORTC |= 1 << PC0;      // Set PC0 to a 1
		PORTC &= ~(1 << PC0);   // Set PC0 to a 0
		
		GPS_readSerialInput(&gps); 			// reading serial input
		GPS_printInfo(&gps);  				// print out serial values of char

		// basically, combine the parsed GPS data into a master buffer
		// the master buffer will be sent through the radio to the receiver
		
		// processing latitude values onto the master buffer
		FloatToStringNew(buffer, gps.latitude , 6); 
		// from master[0] to master[5]
		// master[6] will become an empty space
		for (i = 0; i < 6; i++) {
			bufferMaster[i] = buffer[i];
		}
		bufferMaster[6] = '\n';

		// processing longiude values onto the master buffer
		FloatToStringNew(buffer, gps.longitude , 6); 
		// from master[7] to master[12]
		// master[13] will become an empty space
		for (i = 7; i < 13; i++) {
			bufferMaster[i] = buffer[i - 7];
		}
		bufferMaster[13] = ' \n';

		// process altitude to the master buffer
		FloatToStringNew(buffer, gps.altitude , 6); 
		// from master[14] to master[19]
		// master[20] becomes an empty space
		for (i = 14; i < 20; i++) {
			bufferMaster[i] = buffer[i - 14];
		}
		bufferMaster[20] = '\n';

		// process time to the master buffer
		// total chars needed: 24
		sprintf(buffer, "hour %d, min %d, sec %d", gps.hour, gps.minute, gps.seconds);
		for (i = 21; i < 44; i++) {
			bufferMaster[i] = buffer[i - 21];
		}
		bufferMaster[44] = '\n';

		// process satellite amount to master buffer
		// total chars needed: 14
		sprintf(buffer, "satellites %d",gps.satellites); 
		for (i = 45; i < 60; i++) {
			bufferMaster[i] = buffer[i - 45];
		}
		
		// sending the char through the RFM69
		RFM_send(bufferMaster, &radio.currentMode, sizeof(bufferMaster), radio.slaveSelectPin);
		_delay_ms(20);
    }

    return 0;   /* never reached */
}

// hardware interrupts, if needed

ISR(INT0_vect) {
	serial_outputString("I ");
	// sets to idle, which is needed to know which packet was sent
	radio.packet_sent = RFM_interruptHandler(&radio.currentMode, radio.slaveSelectPin);
}