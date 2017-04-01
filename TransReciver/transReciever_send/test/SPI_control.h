//
//  SPI_control.h
//  SPI
//
//  Created by Christian Wagner on 3/8/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#ifndef SPI_control_h
#define SPI_control_h

#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>

#endif /* SPI_control_h */

// default clock is div 16 
#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06

#define SPI_MODE0 0x00 //use this mode for the transreciver 
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

// if questions about registers see : see: http://maxembedded.com/2013/11/the-spi-of-the-avr/

// mask to select individual bits of a register 
#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

void spi_init_master (void);
char SPI_transfer(char data) ; 
void SPI_multiTransfer(void *data, size_t count);
void SPI_multiWrite(void *data, size_t count);

void SPI_enableDevice(char port); 
void SPI_disableDevice(char port); 

/* this handels slecting the clock polarity and the clock phase 
	give is one of the above modes to change CPOL and CPHA 
	
*/ 
void SPI_setDataMode(char dataMode);

/* DORD stands for Data ORDer. Set this bit to 1 if you want to transmit 
	LSB first, else set it to 0, in which case it sends out MSB first. 
*/
void SPI_setBitOrder(char bitOrder) ;
// This function is deprecated.  New applications should use
// beginTransaction() to configure SPI settings.
void SPI_setClockDivider(char clockDiv) ;