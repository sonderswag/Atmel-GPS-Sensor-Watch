//
//  RMF69.c
//  transReciever
//
//  Created by Christian Wagner on 3/9/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include <avr/interrupt.h>

#include <stdio.h>
#include "../SPI/SPI_control.h"
#include "../Digital_IO/DigitalIo.h"
#include "RFM69.h"

//radio reset pin
#define RF_reset_pin 11 

// The crystal oscillator frequency of the RF69 module
#define RH_RF69_FXOSC 32000000.0

// The Frequency Synthesizer step = RH_RF69_FXOSC / 2^^19
#define RH_RF69_FSTEP  (RH_RF69_FXOSC / 524288)

#define Max_Message_length 60

/* Run to inialize the Radio communication 

*/ 
void RFM_init()
{
		// this is manulally resetting the transreciever 
	pinMode(RF_reset_pin,1); 
	digitalWrite(25,1);
	_delay_ms(100);
	digitalWrite(RF_reset_pin,0);
	_delay_ms(100);


	// Configure important RH_RF69 registers
	// defaults to fixed packet format 
    // Here we set up the standard packet format for use by the RH_RF69 library:
    // 4 bytes preamble
    // 2 SYNC words 2d, d4
    // 2 CRC CCITT octets computed on the header, length and data (this in the modem config data)
    // 0 to 60 bytes data
    // RSSI Threshold -114dBm
    // We dont use the RH_RF69s address filtering: instead we prepend our own headers to the beginning
    // of the RH_RF69 payload

	// RH_RF69_REG_3C_FIFOTHRESH : 0x3c == Tx start 
	// RH_RF69_FIFOTHRESH_TXSTARTCONDITION_NOTEMPTY : 0x80  setting threshold for fif0 to 0x8f as recommended 
    RFM_writeReg(RH_RF69_REG_3C_FIFOTHRESH, RH_RF69_FIFOTHRESH_TXSTARTCONDITION_NOTEMPTY | 0x0f); // thresh 15 is default

    RFM_writeReg(RH_RF69_REG_6F_TESTDAGC, RH_RF69_TESTDAGC_CONTINUOUSDAGC_IMPROVED_LOWBETAOFF);

    RFM_writeReg(RH_RF69_REG_5A_TESTPA1, RH_RF69_TESTPA1_NORMAL);
    RFM_writeReg(RH_RF69_REG_5C_TESTPA2, RH_RF69_TESTPA2_NORMAL);

    char syncwords[] = {0x2d, 0x4d};
    RFM_setSyncWords(syncwords);

    RHFM_setPreambleLength(4); 

    RFM_setFrequency(434.0);

}

/* This function is used to read what is currently stored in the FIFIO. 
   - It is used when reading the recieved message. 
   - Will set the mode to idle
   - Parameters: 
   		- buffer:: pointer to buffer that the FIFO info will be stored into 
   		- currentMode:: pointer to the current mode of the radio struct  
   	- Return :: the leangth of the message 
*/ 
char RFM_Read_FIFO(char* buffer, char* currentMode)
{
	cli();
	RFM_setMode(currentMode,0); // set to idle
	digitalWrite(cs, 0); 
	SPI_transfer(RH_RF69_REG_00_FIFO);

	char payload = SPI_transfer(0); //get length of bytes 
	int buf_len = 0 ; 

	if (payload != 0)
	{
		for (buf_len = 0 ; buf_len <payload ; buf_len++)
		{
			buffer[buf_len] = SPI_transfer(0); 
		}
	}
	digitalWrite(cs, 1); 
	sei(); 

	return payload; // the length of the message 
}

char RFM_recieve(struct RFM69* radio)
{
	RFM_setMode(&(radio->currentMode),1); // set mode to RX 
	if (radio->receiveDataFlag)
	{
		radio->receiveDataFlag = 0;
		radio->buffer_length = RFM_Read_FIFO(radio->buffer, &(radio->currentMode)); //getting the length of the message 
		return 0; // Note that the mode will be in idle at the end 
	}
	else 
	{
		return 1; 
	}
}


/* Call this function to send data via the radio communication 
	- the mode will change:: ->idle -> TX 
	- When the package is sent, an interrupt will happen on pin D0 (G0)
	- Parameters : 	
		- data :: pointer to the data that is going to be sent 
		- currentMode :: currentMode of the radio, is in radio struct 
*/ 
void RFM_send(char* data, char* currentMode)
{
	char length = sizeof(data); 
	if ( length > Max_Message_length)
	{
		return ; 
	}

	cli(); 
	 
	RFM_setMode(currentMode,0); // set mode to idle 
	while ( (RFM_readReg(RH_RF69_REG_27_IRQFLAGS1) & 0x80) == 0x00)
	{
// 		serial_outputString("stuck loop 1");
	} // wait for ModeReady in idle 
	
	digitalWrite(cs, 0); 
	char message[2] = {RH_RF69_REG_00_FIFO | RH_SPI_WRITE_MASK};
	
	SPI_multiWrite(message,2);
	
	while(length--)
	{
		// serial_out(*data);
		SPI_transfer(*data++); 

	}
	digitalWrite(cs, 1); 
	
	sei();
	
	RFM_setMode(currentMode,2); //TX 
	
	// waits until the mode of opperation is set to idle 
	while ((RFM_readReg(RH_RF69_REG_01_OPMODE) & 0x1C) != 0x04)
	{}
	
	// serial_outputString(" !!!Packet Sent!!! ");
	
}


// char RFM_interruptHandler(char* currentMode) 
// {
// 	// serial_outputString("interrupt handeler");
// 	if (*currentMode == 1 && (RFM_readReg(RH_RF69_REG_28_IRQFLAGS2,cs) & 0x04))
// 	{
// 		// serial_outputString("new data ");
// 		return 1;
// 	}
// 	else 
// 	{
// 		return 0;
// 	}
// }

/* This function configures the SPI communication for the radio 

*/ 
void RFM_spiConfig() 
{
	pinMode(cs, OUTPUT); 
	cli(); // stopping interrupts 
	
	// spi values corresponding to datasheet 
	SPI_setDataMode(SPI_MODE0); // setting the polarity of SPI 
	SPI_setBitOrder(1); //want MSB first 
	SPI_setClockDivider(SPI_CLOCK_DIV4);

	sei(); // starting interrupts 
}



/* This function will write to a given register on the radio 
	- address :: of the register to write to 
	- data :: what you want to write to the register 
	
*/ 

// write a single byte to a given register 
void RFM_writeReg(char address, char data)
{
	cli(); // disable global 

	//MSB == 1 for write it is 0 for read 
	// next 7 bits are address to write to 

	digitalWrite(cs, 0); // select 

	char message[2] = {address | RH_SPI_WRITE_MASK, data };

	address |= RH_SPI_WRITE_MASK; // putting 1 in MSB 
	SPI_multiWrite(message,2);
	digitalWrite(cs, 1); 
	sei(); 
}


/* read a single byte for a given register 
	
	- address :: address of the register to read 
	- returns :: the char of what the register is 
*/ 
char RFM_readReg(char address)
{
	cli(); 
	digitalWrite(cs, 0);
	address &= ~RH_SPI_WRITE_MASK; // putting 0 in MSB

// 	char message[] = {address, 0x00};
	SPI_transfer(address); 
	char new = SPI_transfer(0x00); 
// 	SPI_multiTransfer(message,2); 

	digitalWrite(cs, 1);
	sei(); 
	return new ; 
}

/* Will se the syncwords for the radio communication 
 	- syncwords:: should be 2 bytes long containg the sync bytes 

*/ 
void RFM_setSyncWords(char* syncwords)
{
	// restricting number of sync words to 2 for now 
	// getting the current syncConfig
	// default number of sync words is 2 
	// syncwords is on by default  

	// currently not changing any of the default values 
	// char synConfig = RFM_read(RH_RF69_REG_2E_SYNCCONFIG,cs) ; 

	// setting the sync words 
	RFM_writeReg(0x2f,syncwords[0]);
	RFM_writeReg(0x30,syncwords[1]);

	
}

void RHFM_setPreambleLength(uint16_t bytes)
{
    RFM_writeReg(RH_RF69_REG_2C_PREAMBLEMSB, bytes >> 8);
    RFM_writeReg(RH_RF69_REG_2D_PREAMBLELSB, bytes & 0xff);
}

void RFM_setFrequency(float centre)
{

	uint32_t frf = (uint32_t)((centre * 1000000.0) / RH_RF69_FSTEP);
	RFM_writeReg(RH_RF69_REG_07_FRFMSB, (frf >> 16) & 0xff);
    RFM_writeReg(RH_RF69_REG_08_FRFMID, (frf >> 8) & 0xff);
    RFM_writeReg(RH_RF69_REG_09_FRFLSB, frf & 0xff);

}

/*	Modes of operation 
000 → Sleep mode (SLEEP) 			   :: RH_RF69_OPMODE_MODE_SLEEP  
001 → Standby mode (STDBY) 			   :: RH_RF69_OPMODE_MODE_STDBY  
010 → Frequency Synthesizer mode (FS)  :: RH_RF69_OPMODE_MODE_FS 
011 → Transmitter mode (TX) 		   :: RH_RF69_OPMODE_MODE_TX   
100 → Receiver mode (RX) 			   :: RH_RF69_OPMODE_MODE_RX   
*/ 
void RFM_modeSetter(char mode)
{
    char opmode = RFM_readReg(RH_RF69_REG_01_OPMODE); // access 0x01 register which holds operation mode 
    opmode &= ~RH_RF69_OPMODE_MODE; // setting bits 4-2 to zero 
    opmode |= (mode & RH_RF69_OPMODE_MODE); // setting bits 4-2 to the mode we want 
    RFM_writeReg(RH_RF69_REG_01_OPMODE, opmode);

    // Wait for mode to change. this could cause problems 
    // while (!(RFM_readReg(RH_RF69_REG_27_IRQFLAGS1,cs) & RH_RF69_IRQFLAGS1_MODEREADY));
	
}

/* This is the function to call in order to change the mode. 
	- checks if already in the mode 
	- boosts power for recieving and transmitting 
	- enable to the right interrupt pin mapping 
	- calls modeSetter to change to the desire mode 

*/ 
void RFM_setMode(char* currentMode, char mode)
{
	if (*currentMode == mode ) // checking to see if it is already in rx mode 
	{
		return ; 
	}

	if (mode == 0) // idle
	{
		*currentMode = 0; 
		RFM_writeReg(RH_RF69_REG_5A_TESTPA1, 0x55); // used to boost power to transmitter / reciever 
		RFM_writeReg(RH_RF69_REG_5C_TESTPA2, 0x70); 
		RFM_modeSetter(RH_RF69_OPMODE_MODE_STDBY);
	}

	else if (mode == 1) //recieve 
	{
		RFM_writeReg(RH_RF69_REG_5A_TESTPA1, 0x55); // used to boost power to transmitter / reciever 
		RFM_writeReg(RH_RF69_REG_5C_TESTPA2, 0x70); 
		RFM_modeSetter(RH_RF69_OPMODE_MODE_RX); 
		RFM_writeReg(RH_RF69_REG_25_DIOMAPPING1, 0x40); // set DIO0 to "PAYLOADREADY" in receive mode
		RFM_setHighPower(0);
		*currentMode = 1 ; 
	}

	else if (mode == 2) // transmit 
	{
		RFM_writeReg(RH_RF69_REG_5A_TESTPA1, 0x5d); // used to boost power to transmitter / reciever 
		RFM_writeReg(RH_RF69_REG_5C_TESTPA2, 0x7c); 
		RFM_modeSetter(RH_RF69_OPMODE_MODE_TX); 
		RFM_writeReg(RH_RF69_REG_25_DIOMAPPING1, 0x00); // setting DIO0 to packetsent for TX 
		RFM_setHighPower(1);

		*currentMode = 2; 
	}
	
}

// set *transmit/TX* output power: 0=min, 31=max
// this results in a "weaker" transmitted signal, and directly results in a lower RSSI at the receiver
// the power configurations are explained in the SX1231H datasheet (Table 10 on p21; RegPaLevel p66): http://www.semtech.com/images/datasheet/sx1231h.pdf
// valid powerLevel parameter values are 0-31 and result in a directly proportional effect on the output/transmission power
// this function implements 2 modes as follows:
//       - for RFM69W the range is from 0-31 [-18dBm to 13dBm] (PA0 only on RFIO pin)
//       - for RFM69HW the range is from 0-31 [5dBm to 20dBm]  (PA1 & PA2 on PA_BOOST pin & high Power PA settings - see section 3.3.7 in datasheet, p22)
void RFM_setPowerLevel(char powerLevel)
{
  powerLevel = (powerLevel > 31 ? 31 : powerLevel);
  powerLevel /= 2;
  RFM_writeReg(RH_RF69_REG_11_PALEVEL, (RFM_readReg(RH_RF69_REG_11_PALEVEL) & 0xE0) | powerLevel);
}

// for RFM69HW only: you must call setHighPower(true) after initialize() or else transmission won't work
void RFM_setHighPower(char onOff)
{

	RFM_writeReg(RH_RF69_REG_13_OCP,onOff ? 0x0F : 0x1A); // turning off the overload current protection for PA 
	if (onOff)
	{
		RFM_writeReg(RH_RF69_REG_11_PALEVEL, (RFM_readReg(RH_RF69_REG_11_PALEVEL) & 0x1F) | 0x40 | 0x20);
	}
	else 
	{
		RFM_writeReg(RH_RF69_REG_11_PALEVEL,(RFM_readReg(RH_RF69_REG_11_PALEVEL) & 0x1F & ~0x40 & ~0x20 ));
	}
}

// get the received signal strength indicator (RSSI)
int RFM_readRSSI() 
{
  int rssi = 0;
 
  RFM_writeReg(RH_RF69_REG_23_RSSICONFIG, 0x01); //start the measurements 
  while ((RFM_readReg(RH_RF69_REG_23_RSSICONFIG) & 0x02) == 0x00); // wait for RSSI_Ready
  
  rssi = -RFM_readReg(RH_RF69_REG_24_RSSIVALUE);
  rssi >>= 1;
  return rssi;
}


