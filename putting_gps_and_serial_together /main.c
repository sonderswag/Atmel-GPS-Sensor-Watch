/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <string.h> 
#include <stdlib.h>

#include "serial.h" 
#include "gpsParse.h" 




int main(void)
{
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output

    /* calculate from clock freq and buad see handout 
 	UBRR = [f_osc / (16*BUAD)] - 1 
 	UBRR = 47 
 	*/
	serial_init(47);

	struct GPS gps; 
	gps.sizeInputString = 0; 

    while (1) 
    {
		PORTC |= 1 << PC0;      // Set PC0 to a 1
		PORTC &= ~(1 << PC0);   // Set PC0 to a 0
		readSerialInput(serial_in, &gps); 
		serial_out((char)gps.latitude); 



    }

	

    return 0;   /* never reached */
}
