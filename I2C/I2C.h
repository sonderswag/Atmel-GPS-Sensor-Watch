#ifndef I2C_h
#define I2C_h

 

void i2c_init(); 

uint8_t i2c_start(uint8_t address); // have to send it to every device 

uint8_t i2c_write(uint8_t data); 

void i2c_stop(void); 

uint8_t i2c_read_ack();

uint8_t i2c_read_nack(); 

#endif 