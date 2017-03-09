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
#include <Arduino.h>

int main(void)
{
    init(); 

    DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output
    DDRD |= 1 << DDD0; 
    DDRB |= 1 << DDB0; 



    while (1) {

    // port B 
    PORTB |= 1 << PB0; 
    _delay_ms(5);
    PORTB &= ~(1 << PB0); 

    PORTB |= 1 << PB1; 
    _delay_ms(5);
    PORTB &= ~(1 << PB1); 

    PORTB |= 1 << PB2; 
    _delay_ms(5);
    PORTB &= ~(1 << PB2); 

    PORTB |= 1 << PB3; 
    _delay_ms(5);
    PORTB &= ~(1 << PB3); 

    PORTB |= 1 << PB4; 
    _delay_ms(5);
    PORTB &= ~(1 << PB4); 

    PORTB |= 1 << PB5; 
    _delay_ms(5);
    PORTB &= ~(1 << PB5); 

    PORTB |= 1 << PB7; 
    _delay_ms(5);
    PORTB &= ~(1 << PB7); 

    // Port
     PORTC |= 1 << PC0; 
    
    _delay_ms(5);
    PORTC &= ~(1 << PC0); 

    PORTC |= 1 << PC1; 
    _delay_ms(5);
    PORTC &= ~(1 << PC1); 

    PORTC |= 1 << PC2; 
    _delay_ms(5);
    PORTC &= ~(1 << PC2); 

    PORTC |= 1 << PC3; 
    _delay_ms(5);
    PORTC &= ~(1 << PC3); 

    PORTC |= 1 << PC4; 
    _delay_ms(5);
    PORTC &= ~(1 << PC4); 

    PORTC |= 1 << PC5; 
    _delay_ms(5);
    PORTC &= ~(1 << PC5); 

    //Port D

    PORTD |= 1 << PD0; 
   
    _delay_ms(5);
    PORTD &= ~(1 << PD0); 

    PORTD |= 1 << PD1; 
    _delay_ms(5);
    PORTD &= ~(1 << PD1); 

    PORTD |= 1 << PD2; 
    _delay_ms(5);
    PORTD &= ~(1 << PD2); 

    PORTD |= 1 << PD3; 
    _delay_ms(5);
    PORTD &= ~(1 << PD3); 

    PORTD |= 1 << PD4; 
    _delay_ms(5);
    PORTD &= ~(1 << PD4); 

    PORTD |= 1 << PD5; 
    _delay_ms(5);
    PORTD &= ~(1 << PD5);

    PORTD |= 1 << PD6; 
    _delay_ms(5);
    PORTD &= ~(1 << PD6);

    PORTD |= 1 << PD7; 
    _delay_ms(5);
    PORTD &= ~(1 << PD7); 


    }

    return 0;   /* never reached */
}
