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

#include "DigitalIo.h"
#include "RFM69.h"
#include "SPI_control.h"
#include "serial.h"

#define Serial_rate 47


int main(int argc, const char * argv[]) {

	serial_init(Serial_rate); 

	spi_init_master(); 
	struct RFM69 radio; 
	radio.slaveSelectPin = 24; 
	radio.currentMode = 0; 
	radio.buffer_length = 0;


	RFM_spiConfig(radio.slaveSelectPin) ;
	RFM_init(radio.slaveSelectPin);

	RFM_setModeRx(&radio.currentMode, radio.slaveSelectPin); 

	

	while (1)
	{
		radio.buffer_length = Read_FIFO(radio.buffer, radio.slaveSelectPin);

		serial_outputString(radio.buffer);

		_delay_ms(5000);


	}



    return 0;
}
