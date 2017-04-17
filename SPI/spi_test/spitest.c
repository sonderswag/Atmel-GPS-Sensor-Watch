#include <avr/io.h>
#include <util/delay.h>

int main(void) {

    DDRB |= (1 << PB3);         // set MOSI for output
    DDRB |= (1 << PB5);         // set SCLK for output
    DDRB |= (1 << PB2);
    DDRC |= (1 << PC1); 



    SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR0);

    while (1) {
    PORTC &= ~(1<<PC1); 

	SPDR = 'a';

	
	while (!(SPSR & (1 << SPIF))) ;
	PORTC |= (1<<PC1); 
	_delay_ms(10);


    }

}
