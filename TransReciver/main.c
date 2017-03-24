//
//  main.c
//  transReciever
//
//  Created by Christian Wagner on 3/9/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>

#include "DigitalIo.h"
#include "RFM69.h"
#include "SPI_control.h"
#include "serial.h"

#define Serial_rate 47

struct RFM69 radio; 


int main(int argc, const char * argv[]) {
	radio.slaveSelectPin = 24; 
	radio.currentMode = 0; 
	radio.buffer_length = 0;
	radio.receiveDataFlag = 0;
	
	sei(); 

	serial_init(Serial_rate); 

	spi_init_master(); 
	


	RFM_spiConfig(radio.slaveSelectPin) ;
	RFM_init(radio.slaveSelectPin);

	RFM_setMode(&radio.currentMode,1,cs); // RX

	

	while (1)
	{	
		if (radio.receiveDataFlag)
		{
			radio.receiveDataFlag = 0;
			radio.buffer_length = Read_FIFO(radio.buffer, radio.slaveSelectPin);
			RFM_setMode(&radio.currentMode,1,radio.slaveSelectPin); // RX
			serial_outputString(radio.buffer);
			_delay_ms(15000);
		}
		_delay_ms(5000);

	}



    return 0;
}

//pin chagne interrupt for port B 
ISR(PCINT0_vect)
{
	if (digitalRead(10) == 1)
	{
		radio.receiveDataFlag = RFM_interruptHandler(&radio.currentMode, radio.slaveSelectPin) ;
	}
}