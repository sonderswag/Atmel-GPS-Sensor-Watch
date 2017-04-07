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

#include "../Serial/serial.h" 
#include "../GPS/GPS.h" 

// include other libraries from other folders
#include "../Digital_IO/DigitalIo.h"
#include "../RFM/RFM69.h"
#include "../SPI/SPI_control.h"

#define Serial_rate 47
struct RFM69 radio;

void interruptInit()
{
	DDRD &= ~(1 <<DDD2) ; 
	PORTD |= (1<<PORTD2); 
	EICRA |= (1<<ISC00) | (1<<ISC01); // set it for rising edge 
	EIMSK |= (1 << INT0); 
	sei(); 					// set enable interrupts
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
	
    /* calculate from clock freq and baud see handout 
 	UBRR = [f_osc / (16*BUAD)] - 1 
 	UBRR = 47 
 	*/

	// setting up the GPS struct
	struct GPS gps; 
	gps.sizeInputString = 0; 
	
	// setting up RFM parameters
	RFM_spiConfig(radio.slaveSelectPin);
	RFM_init(radio.slaveSelectPin);
	
	// initialize interrupts
	sei();

    while (1) 
    {
		PORTC |= 1 << PC0;      // Set PC0 to a 1
		PORTC &= ~(1 << PC0);   // Set PC0 to a 0
		
		readSerialInput(serial_in(), &gps); 	// reading serial input
		serial_out((char) gps.latitude); 	// print out serial values of char
		
		// sending the char through the RFM69
		RFM_send(((char) gps.latitude), &radio.currentMode);
		_delay_ms(1);
    }

    return 0;   /* never reached */
}

// hardware interrupts, if needed

/*
ISR(INT0_vect) {
	serial_outputString("I ");
	// sets to idle, which is needed to know which packet was sent
	radio.packet_sent = RFM_interruptHandler(&radio.currentMode);
}
*/

