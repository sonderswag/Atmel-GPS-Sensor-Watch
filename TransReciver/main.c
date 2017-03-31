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
	
	radio.slaveSelectPin = 24; 
	radio.currentMode = 0; 
	radio.buffer_length = 0;
	radio.packet_sent = 0; 
	
	interruptInit(); 
	
	serial_init(Serial_rate); 

	spi_init_master(); 
	


	RFM_spiConfig(radio.slaveSelectPin) ;
	RFM_init(radio.slaveSelectPin);

	// RFM_setModeRx(&radio.currentMode, radio.slaveSelectPin); 

	// char message[] = {0x11,0x22,0x33,0x44,0x55};
	char message[] = "hey you there" ;
	

	// radio.buffer_length = Read_FIFO(radio.buffer, radio.slaveSelectPin);


	while (1)
	{
		// serial_outputString(radio.buffer);

		RFM_send(message,&radio.currentMode, sizeof(message), radio.slaveSelectPin, &radio.packet_sent);


		_delay_ms(1);



		// char mess[2] = {0x11, 0x22};
		// RFM_writeReg(RH_RF69_REG_3C_FIFOTHRESH, RH_RF69_FIFOTHRESH_TXSTARTCONDITION_NOTEMPTY | 0x0f, 24);
		// digitalWrite(24,0);
		// SPI_multiWrite(mess,2);
		// digitalWrite(24,1);
		// RFM_writeReg(0x00,0x2d,24);
		// RFM_readReg(0x2f,24);


		// char synConfig = RFM_readReg(0x01,24) ; 

		// _delay_ms(3000);

	}



    return 0;
}

//pin chagne interrupt for port B 
ISR(INT0_vect)
{
	serial_outputString("I ");
	RFM_setMode(&radio.currentMode,0,radio.slaveSelectPin); // set to idle
	// this pin should change when something has been recieve

}
