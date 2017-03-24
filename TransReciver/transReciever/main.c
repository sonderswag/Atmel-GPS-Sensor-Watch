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


	RFM_spiConfig(radio.slaveSelectPin) ;
	RFM_init(radio.slaveSelectPin);

	RFM_setModeRx(radio.currentMode, radio.slaveSelectPin); 



	while (1)
	{
		// char mess[2] = {0x11, 0x22};
		// RFM_writeReg(RH_RF69_REG_3C_FIFOTHRESH, RH_RF69_FIFOTHRESH_TXSTARTCONDITION_NOTEMPTY | 0x0f, 24);
		// digitalWrite(24,0);
		// SPI_multiWrite(mess,2);
		// digitalWrite(24,1);
		// RFM_writeReg(0x00,0x2d,24);
		// RFM_readReg(0x2f,24);


		char synConfig = RFM_readReg(0x01,24) ; 

		_delay_ms(3000);

	}



    return 0;
}
