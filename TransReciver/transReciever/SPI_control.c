//
//  SPI_control.c
//  SPI
//
//  Created by Christian Wagner on 3/8/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "SPI_control.h"
#include "DigitalIo.h"


// Initialize SPI Master Device (with SPI interrupt)
void spi_init_master (void)
{
    cli(); 
    // Set MOSI, SCK as Output
    DDRB=(1<<5)|(1<<3);
    
    // Enable SPI, Set as Master
    // Prescaler: Fosc/16, Enable Interrupts
    //The MOSI, SCK pins are as per ATMega8
    SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPIE);
    
    // Enable Global Interrupts
    sei();
}


// Write to the SPI bus (MOSI pin) and also receive (MISO pin)
  char SPI_transfer(char data) 
  {
    SPDR = data;
    /*
     * The following NOP introduces a small delay that can prevent the wait
     * loop form iterating when running at the maximum speed. This gives
     * about 10% more speed, even if it seems counter-intuitive. At lower
     * speeds it is unnoticed.
     */
    asm volatile("nop");
    while(!(SPSR & (1<<SPIF) ));
    return SPDR;
  }

//Function to transfer a list of chars 
  // count is in bytes 
void SPI_multiTransfer(void *data, size_t count)
{
    if (count == 0) return;
    
    char *buf = (char *)data; // loading in the buffer
    SPDR = *buf; // SPDR is the registered that the data will be transfered from
    while (--count > 0)
    {
        char out = *(buf+1);
        while(!(SPSR & (1<<SPIF) )); // where the actual transfer happens
        char in = SPDR; // getting the input
        SPDR = out; // loading the next byte to pushed out
        *buf += in; // loading into the buffer the recieved data
        
    }
    while(!(SPSR & (1<<SPIF) )); ; // get the last bit 
    *buf = SPDR;
}


void SPI_enableDevice(char port)
{
    digitalWrite(port, 0);
}

void SPI_disableDevice(char port)
{
    digitalWrite(port, 1);
}

/* this handels slecting the clock polarity and the clock phase 
    give is one of the above modes to change CPOL and CPHA 
    
*/ 
void SPI_setDataMode(char dataMode)
{
    SPCR = (SPCR & ~SPI_MODE_MASK) | dataMode;
}

/* DORD stands for Data ORDer. Set this bit to 1 if you want to transmit 
    LSB first, else set it to 0, in which case it sends out MSB first. 
*/
void SPI_setBitOrder(char bitOrder) 
{
    if (bitOrder == 0) SPCR |= (1 << DORD); // transmitting LSB first 
    else SPCR &= ~( 1 << DORD); // transmitting MSB first 
}

// This function is deprecated.  New applications should use
// beginTransaction() to configure SPI settings.
void SPI_setClockDivider(char clockDiv) 
{
    SPCR = (SPCR & ~SPI_CLOCK_MASK) | (clockDiv & SPI_CLOCK_MASK); // this is just changing the first two bits of SPCR 
    SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((clockDiv >> 2) & SPI_2XCLOCK_MASK); // this is just chaning the 2X bit 
}


//ISR(SPI_STC_vect)