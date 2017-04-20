//
//  main.c
//  transReciever:: GET
//
//  Created by Christian Wagner on 3/9/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>

#include "../../Digital_IO/DigitalIo.h"
#include "../../RFM/RFM69.h"
#include "../../SPI/SPI_control.h"
#include "../../Serial/serial.h"

#define Serial_rate 47
#define slaveSelectPin 24

struct RFM69 radio; 

void interruptInit()
{
	DDRD &= ~(1 <<DDD2) ; 
	PORTD |= (1<<PORTD2); 
	EICRA |= (1<<ISC00) | (1<<ISC01); // set it for rising edge 
	EIMSK |= (1 << INT0); 
	sei(); 
	PCMSK0 |= 0x80;
}

int main(int argc, const char * argv[]) {
	
	// Initalize --------------------------------------------------------
	interruptInit();  // Interrupts
	serial_init(Serial_rate); //Serial 
	spi_init_master(); // SPI 
	
	// Radio Initalize and constants 
	radio.currentMode = 0; 
	radio.packet_sent = 0; 	
	
	RFM_spiConfig(slaveSelectPin) ;
	RFM_init(slaveSelectPin);

	//--------------------------------------------------------

	RFM_setMode(&radio.currentMode,1,slaveSelectPin); // RX

	while (1)
	{	
		// need this if case for reading the data
		if (radio.receiveDataFlag)
		{
			// need these three functions in this order to work. 
			radio.receiveDataFlag = 0; //reset the flag 
			Read_FIFO(radio.buffer,&radio.currentMode, slaveSelectPin);
			// have to do this after receiving somehting 
			RFM_setMode(&radio.currentMode,1,slaveSelectPin); // set mode to RX

			serial_outputString(radio.buffer); 
		}
		_delay_ms(2);
		// radio.buffer_length = Read_FIFO(radio.buffer,&radio.currentMode, radio.slaveSelectPin);
		// RFM_printMode(radio.slaveSelectPin);

	}



    return 0;
}

//Hardware interrupt
ISR(INT0_vect)
{
	// serial_outputString("I ");

		radio.receiveDataFlag = RFM_interruptHandler(&radio.currentMode, slaveSelectPin) ;

}