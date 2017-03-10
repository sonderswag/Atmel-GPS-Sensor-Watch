//
//  RMF69.c
//  transReciever
//
//  Created by Christian Wagner on 3/9/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include <avr/interrupt.h>

#include <stdio.h>
#include "SPI_control.h"
#include "DigitalIo.h"
#include "RFM69.h"


// The crystal oscillator frequency of the RF69 module
#define RH_RF69_FXOSC 32000000.0

// The Frequency Synthesizer step = RH_RF69_FXOSC / 2^^19
#define RH_RF69_FSTEP  (RH_RF69_FXOSC / 524288)


// void RFM_initialize(uint8_t freqBand, uint8_t nodeID, uint8_t networkID, char slaveSelectPin)
// {
 

// }


void RFM_spiConfig(char cs) 
{
	pinMode(cs, OUTPUT); 
	cli(); // stopping interrupts 
	
	// spi values corresponding to datasheet 
	SPI_setDataMode(SPI_MODE0); // setting the polarity of SPI 
	SPI_setBitOrder(1); //want MSB first 
	SPI_setClockDivider(SPI_CLOCK_DIV4);

	sei(); // starting interrupts 
}

void RFM_unselect(char slaveSelectPin)
{
	digitalWrite(slaveSelectPin,1); // turning off chip select 

	// will need to readjust SPI variables for screen or have a select for the screen to select the right values 

}

/* chip select must be on for this deivce 
	
	so would want to call 
	RFM_select(cs);
	RFM_wrtieREG 

*/ 

// write a single byte to a given register 
void RFM_writeReg(char address, char data, char cs)
{
	cli(); 
	//MSB == 1 for write it is 0 for read 
	// next 7 bits are address to write to 

	digitalWrite(cs, 0); 
	address |= RH_SPI_WRITE_MASK; // putting 1 in MSB 
	SPI_transfer(address); 
	SPI_transfer(data); 
	digitalWrite(cs, 1); 
	sei(); 
}

// read a single byte for a given register 
char RFM_readReg(char address, char cs)
{
	cli(); 
	digitalWrite(cs, 0);
	address &= ~RH_SPI_WRITE_MASK; // putting 0 in MSB
	SPI_transfer(address);
	char value = SPI_transfer(0x00); // transfer 0's since we just want to read 
	digitalWrite(cs, 1);
	sei(); 
	return value ; 
}

/* Write multiple bytes to radio 
address :: address to write multiple bytes (0x00 is fifo )
src :: data to be written 
len :: number of bytes to be writen 
radio :: holding the slave select pi 
*/ 
void RFM_burstWrtie(char address, char* src, char len, char cs)
{
	cli(); 
	digitalWrite(cs, 0);
	SPI_transfer(address |= RH_SPI_WRITE_MASK); // putting 1 in MSB )
	SPI_multiTransfer(src,len);
	digitalWrite(cs, 1);
	sei(); 
}


void RFM_burstRead(char address, char* dest, char len, char cs)
{
	cli(); 
	digitalWrite(cs, 0);
	SPI_transfer(address |= RH_SPI_WRITE_MASK); // putting 1 in MSB )

	while(len--) 
	{
		*dest++ = SPI_transfer(0x00); 
	}

	digitalWrite(cs, 1);
	sei(); 
}


void RFM_setFrequency(float centre, char cs )
{

	uint32_t frf = (uint32_t)((centre * 1000000.0) / RH_RF69_FSTEP);
	RFM_writeReg(RH_RF69_REG_07_FRFMSB, (frf >> 16) & 0xff, cs);
    RFM_writeReg(RH_RF69_REG_08_FRFMID, (frf >> 8) & 0xff,cs);
    RFM_writeReg(RH_RF69_REG_09_FRFLSB, frf & 0xff,cs);

}

/*	Modes of operation 
000 → Sleep mode (SLEEP) 			   :: RH_RF69_OPMODE_MODE_SLEEP  
001 → Standby mode (STDBY) 			   :: RH_RF69_OPMODE_MODE_STDBY  
010 → Frequency Synthesizer mode (FS)  :: RH_RF69_OPMODE_MODE_FS 
011 → Transmitter mode (TX) 		   :: RH_RF69_OPMODE_MODE_TX   
100 → Receiver mode (RX) 			   :: RH_RF69_OPMODE_MODE_RX   
*/ 

void RFM_setOpMode(char mode, char cs)
{
    char opmode = RFM_readReg(RH_RF69_REG_01_OPMODE, cs); // access 0x01 register which holds operation mode 
    opmode &= ~RH_RF69_OPMODE_MODE; // setting bits 4-2 to zero 
    opmode |= (mode & RH_RF69_OPMODE_MODE); // setting bits 4-2 to the mode we want 
    RFM_writeReg(RH_RF69_REG_01_OPMODE, opmode, cs);

    // Wait for mode to change. this could cause problems 
    while (!(RFM_readReg(RH_RF69_REG_27_IRQFLAGS1,cs) & RH_RF69_IRQFLAGS1_MODEREADY))
	;
}

char RFM_setModeRx(char currentMode, char cs)
{
	if (currentMode == 1 ) // checking to see if it is already in rx mode 
	{
		return; 
	}

	RFM_writeReg(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_BOOST, cs); // used to boost power to transmitter / reciever 
	RFM_writeReg(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_BOOST, cs); 

	RFM_setOpMode(RH_RF69_OPMODE_MODE_RX,cs); 

	return 1 ; // used to set the current mode to rx 
}

char RFM_setModeTx( char currentMode, char cs)
{
	if (currentMode != 0) // change this in the final. just checking to see if I get communication. 
	{
		return; 
	}

	RFM_writeReg(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_BOOST, cs); // used to boost power to transmitter / reciever 
	RFM_writeReg(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_BOOST, cs); 

	RFM_setOpMode(RH_RF69_OPMODE_MODE_TX,cs); 

	return 2; 


}
