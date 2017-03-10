//
//  SPI_control.c
//  SPI
//
//  Created by Christian Wagner on 3/8/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include "SPI_control.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

// Initialize SPI Master Device (with SPI interrupt)
void spi_init_master (void)
{
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
  uint8_t transfer(uint8_t data) 
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
void SPI_tranceiver(void *data, size_t count)
{
    if (count == 0) return;
    
    unit8_t *buf = (unit8_t *)data; // loading in the buffer
    SPDR = *buf; // SPDR is the registered that the data will be transfered from
    while (--count > 0)
    {
        unit8_t out = *(buf+1);
        while(!(SPSR & (1<<SPIF) )); // where the actual transfer happens
        unit8_t in = SPDR; // getting the input
        SPDR = out; // loading the next byte to pushed out
        *buf+ = in; // loading into the buffer the recieved data
        
    }
    while(!(SPSR & (1<<SPIF) )); ; // get the last bit 
    *buf = SPDR;
}




void SPI_enableDevice(port)
{
    digitalWrite(port, 0);
}

void SPI_disableDevice(port)
{
    digitalWrite(port, 1);
}




//ISR(SPI_STC_vect)