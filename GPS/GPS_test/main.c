
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Serial/serial.h"
#include "../GPS.h"

// convert float to string one decimal digit at a time
// assumes float is < 65536 and ARRAYSIZE is big enough
// problem: it truncates numbers at size without rounding
// str is a char array to hold the result, float is the number to convert
// size is the number of decimal digits you want

struct GPS gps;


int main(void)
{
    // DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output

    /* calculate from clock freq and buad see handout
 	UBRR = [f_osc / (16*BUAD)] - 1
 	UBRR = 47
 	*/
	serial_init(47);
	// gps.sizeInputString = 0;
	// gps.state = 0;

    while (1)
    {
    	// serial_out(serial_in());

    	GPS_readSerialInput(&gps);
        GPS_printInfo(&gps);
    	// _delay_ms(2000);


 	}



    return 0;   /* never reached */
}
