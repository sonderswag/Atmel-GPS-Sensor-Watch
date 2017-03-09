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

//Function to send and receive data for both master and slave
unsigned char spi_tranceiver(unsigned char data)
{
    // Load data into the buffer
    SPDR = data;
    
    //Wait until transmission complete
    while(!(SPSR & (1<<SPIF) ));
    
    // Return received data
    return(SPDR);
}


// assumes ddr register is already enable for output and not input
void digitalWrite(char port, char state)
{
    if (port == 1 || (port >= 23 && port <= 28 )) // c
    {
        if (port == 1)
        {
            if (state == 1)
            {
                PORTC |= (1<<6);
            }
            else
            {
                PORTC &= ~(1<<6);
            }
        }
        else
        {
            port -= 23;
            if (state == 1)
            {
                PORTC |= (1<<port);
            }
            else
            {
                PORTC &= ~(1<<port);
            }
        }
    }
    else if ((port >= 2 && port <= 6) || ( port >= 11 && port <= 13)) // d
    {
        if (port >= 2 && port <= 6)
        {
            port -= 2;
        }
        else
        {
            port -= 6;
        }
        
        if (state == 1)
        {
            PORTD |= (1<<port);
        }
        else
        {
            PORTD &= ~(1<<port);
        }
        
    }
    else if ((port >= 9 && port <= 10) || (port >= 14 && port <= 19)) //b
    {
        if (port >= 9 && port <= 10)
        {
            port -= 3;
        }
        else
        {
            port -= 14;
        }
        
        if (state == 1)
        {
            PORTB |= (1<<port);
        }
        else
        {
            PORTB &= ~(1<<port);
        }
    }
}


void enable_SPI_device(port)
{
    digitalWrite(port, 0);
}

void disable_SPI_device(port)
{
    digitalWrite(port, 1);
}




//ISR(SPI_STC_vect)