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

#endif /* SPI_control_h */


void spi_init_master (void);
unsigned char spi_tranceiver (unsigned char data);
void pin_state(char port, char state);
void enable_SPI_device(port);
void disable_SPI_device(port);

