//
//  main.c
//  SPI
//
//  Created by Christian Wagner on 3/8/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include <stdio.h>
#include <avr/io.h>
#include "SPI_control.h" 
#include "serial.h"
#include <avr/interrupt.h>

int main(int argc, const char * argv[]) {
    
    DDRC |= (1<<DD1); // enabling output for cs
    spi_init_master();
    
    enable_SPI_device(24);
    
    char data_in;
    char data_out = 0b00000000;
    while(1)
    {
        data_in = spi_tranceiver(data_out);
        serial_out(data_in); 
    }
    
    
    
    
    return 0;
}
