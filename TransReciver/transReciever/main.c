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


int main(int argc, const char * argv[]) {

	spi_init_master(); 
	struct RFM69 radio; 
	radio.slaveSelectPin = 24; 
	radio.currentMode = 1; 

	// RFM_spiConfig(radio.slaveSelectPin) ;
	pinMode(radio.slaveSelectPin, OUTPUT); 
	digitalWrite(radio.slaveSelectPin, 0);
	while (1)
	{
		// RFM_setModeTx(radio.currentMode, radio.slaveSelectPin); 
		SPI_transfer(0xa3);
		_delay_ms(5); 
	}



    return 0;
}
