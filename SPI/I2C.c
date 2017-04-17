

#include <stdio.h>
#include <stdint.h>I
#include <avr/io.h>
#include <util/twi.h>

#define F_CPU 7372800
#define F_SCL 100000UL // SCL frequency
#define Prescaler 1
#define TWBR_val ((((F_CPU / F_SCL) / Prescaler) - 16 ) / 2)

// void i2c_sendCommand(uint8_t address, uint8_t command)
// {
// 	i2c_start(address);
// 	i2c_write(0x00); 
// 	i2c_write(command); 
// 	i2c_stop(); 
// }

void i2c_init()
{
	TWBR = (uint8_t)TWBR_val; 

}

uint8_t i2c_start(uint8_t address)
{
	// reset TWI control register
	TWCR = 0;

	// transmit START condition 
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );

	// check if the start condition was successfully transmitted
	if((TWSR & 0xF8) != TW_START){ return 1; }

	// load slave address into data register
	TWDR = address;

	// start transmission of address
	TWCR = (1<<TWINT) | (1<<TWEN);

	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );

	// check if the device has acknowledged the READ / WRITE mode
	uint8_t twst = TW_STATUS & 0xF8;
	if ( (twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK) ) return 1;
	
	return 0;
}

uint8_t i2c_write(uint8_t data)
{
	// load data into data register
	TWDR = data;
	// start transmission of data
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );
	
	if( (TWSR & 0xF8) != TW_MT_DATA_ACK ){ return 1; }
	
	return 0;
}

void i2c_stop(void)
{
	// transmit STOP condition
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}
