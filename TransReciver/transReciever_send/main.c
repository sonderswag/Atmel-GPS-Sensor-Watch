//
//  main.c
//  transReciever:: SEND 
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

void interruptInit()
{
	DDRD &= ~(1 <<DDD2) ; 
	PORTD |= (1<<PORTD2); 
	EICRA |= (1<<ISC00) | (1<<ISC01); // set it for rising edge 
	EIMSK |= (1 << INT0); 
	sei(); 
	PCMSK0 |= 0x80;
}

struct RFM69 radio; 

	
int main(int argc, const char * argv[]) {
	
	// Initalize --------------------------------------------------------
	interruptInit();  // Interrupts
	serial_init(Serial_rate); //Serial 
	spi_init_master(); // SPI 
	
	// Radio Initalize and constants 
	radio.slaveSelectPin = 24; 
	radio.currentMode = 0; 
	radio.buffer_length = 0;
	radio.packet_sent = 0; 	
	
	RFM_spiConfig(radio.slaveSelectPin) ;
	RFM_init(radio.slaveSelectPin);

	//--------------------------------------------------------

	sei(); // start interrupts 

	char message[] = "hey,you" ;
	

	while (1)
	{
		// serial_outputString(radio.buffer);

		RFM_send(message,&radio.currentMode, sizeof(message), radio.slaveSelectPin);


		_delay_ms(1);


	}

    return 0;
}

//Hardware interrupt
ISR(INT0_vect)
{
	serial_outputString("I ");
	// set to idle
	radio.packet_sent = RFM_interruptHandler(&radio.currentMode,radio.slaveSelectPin);

}
