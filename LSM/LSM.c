#include <avr/io.h>
#include <util/twi.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/interrupt.h>
#include <math.h> 
#include "LSM.h"
#include "../I2C/I2C.h"
#include "../Serial/Serial.h"

void LSM_init()
{
	//enabling the hardware interrupt -------------------------------
	DDRD &= ~(1 <<DDD3) ; 
	PORTD |= (1<<PORTD3); 
	EICRA |= (1<<ISC10) | (1<<ISC11); // set it for rising edge 
	EIMSK |= (1 << INT1); //turn on int1 hardware interrupt 

	//------------------------------------------------------
	
	cli(); 
	i2c_init(); // initiate i2c ;

	LSM_writeReg(LSM9DS1_ADDRESS_ACCELGYRO_WRITE,CTRL_REG5_XL,0x38); 
	/* setting for acce 
	001 == 10 hz sample
	or 
	101 == 476 hz 
	or 
	110 == 952 hz 
	00 == +- 2g range 
	0 == bandwidth = sample 
	00 == anti_alising 408hz 
	*/ 
	
	LSM_writeReg(LSM9DS1_ADDRESS_ACCELGYRO_WRITE,CTRL_REG6_XL,0xC0);

	//set interrupts for x and y events 
	LSM_writeReg(LSM9DS1_ADDRESS_ACCELGYRO_WRITE,0x06,0x0A); 
 
	//Enable on interrupts events 
	LSM_writeReg(LSM9DS1_ADDRESS_ACCELGYRO_WRITE,0x0C,0x40); 

	//set interrupt thresholds 
	LSM_writeReg(LSM9DS1_ADDRESS_ACCELGYRO_WRITE,0x07,0x0F); // 20
	LSM_writeReg(LSM9DS1_ADDRESS_ACCELGYRO_WRITE,0x08,0x0F); // 20


	//set accelerometer wait 
	LSM_writeReg(LSM9DS1_ADDRESS_ACCELGYRO_WRITE,0x0A,0xB2); // 100


	// set wait to be 
	LSM_writeReg(LSM9DS1_ADDRESS_MAG_WRITE,CTRL_REG3_M,0x00);
	// LSM_writeReg(LSM9DS1_ADDRESS_MAG_WRITE,CTRL_REG1_M,0x3D); // contorl reg 1 



	// no temp adjust 
	// x-y high performence 
	// 10hz sampling 
	// no self test 

	// turn on the device chage 0x03 -> 0x00 
	
	// LSM_writeReg(LSM9DS1_ADDRESS_MAG_WRITE,CTRL_REG3_M,0x03);
	// MAG_calibrate();  
	sei(); 
}

void LSM_writeReg(char devAdd, char reg, char data)
{
	i2c_start(devAdd);
	i2c_write(reg);
	i2c_write(data);
	i2c_stop(); 
}

void MAG_calibrate()
{
	short int x,y; 

	int16_t magMax[2] = {0,0};
	int16_t magMin[2] = {0,0}; 
	

	uint8_t i; 
	for (i = 0 ; i < 50 ; i++)
	{
		i2c_start(LSM9DS1_ADDRESS_MAG_WRITE);
		i2c_write(0x28); //28 is for starting with x
		i2c_start(LSM9DS1_ADDRESS_MAG_READ);
		char lower = i2c_read_ack(); 
		char upper = i2c_read_ack(); 
	
		x = lower | (upper << 8);
	
		lower = i2c_read_ack(); 
		upper = i2c_read_nack(); 
	
		y = lower | (upper << 8); 
	
	    i2c_stop(); 

	    if (x > magMax[0]) { magMax[0] = x; }
	    else if (x < magMin[0]) { magMin[0] = x; }


	    if (y > magMax[1]) { magMax[1] = y; } 
	    else if (y < magMin[1]) { magMin[1] = y; }
	}

	int16_t bias_x = (magMax[0] + magMin[0]) / 2; // x offset
	int16_t bias_y = (magMax[1] + magMin[1]) / 2; // y offset 

	int8_t bias_x_l =  (bias_x & 0xFF00) >> 8; //lower
	int8_t bias_x_u =  bias_x & 0x00FF; //upper 

	int8_t bias_y_l =  (bias_y & 0xFF00) >> 8; // lower
	int8_t bias_y_u =  bias_y & 0x00FF;  // upper 



	LSM_writeReg(LSM9DS1_ADDRESS_MAG_WRITE,0x05,bias_x_l); //lower bias 
	LSM_writeReg(LSM9DS1_ADDRESS_MAG_WRITE,0x06,bias_x_u); //lower bias 

	LSM_writeReg(LSM9DS1_ADDRESS_MAG_WRITE,0x07,bias_y_l); //lower bias 
	LSM_writeReg(LSM9DS1_ADDRESS_MAG_WRITE,0x08,bias_y_u); //lower bias 

	float X = ((bias_x*0.14)/1000);  
	float Y = ((bias_y*0.14)/1000);  
	// char buf[20];
	// FloatToStringNew(buf, X, 6);
	// serial_outputString(buf); 
	// FloatToStringNew(buf, Y, 6);
	// serial_outputString(buf); 

}

void LSM_getHeading(float* heading)
{
	float x,y;
	Mag_readXY(&x,&y);

	*heading = atan2(y,x);
	*heading *= 180/3.14159;
	*heading += (12.0 + (3.0 / 60.0)) ;

	if (*heading < 0)
	{
		*heading += 360; 
	}
	else if (*heading > 360)
	{
		*heading -= 360; 
	}


}


void Mag_readXY(float* X, float* Y)
{
	cli(); 
	i2c_start(LSM9DS1_ADDRESS_MAG_WRITE);
	i2c_write(0x28); //28 is for starting with x
	i2c_start(LSM9DS1_ADDRESS_MAG_READ);
	int16_t x,y;

	uint8_t lower = i2c_read_ack(); 
	uint8_t upper = i2c_read_ack(); 

	x = lower | (upper << 8);

	lower = i2c_read_ack(); 
	upper = i2c_read_nack(); 

	y = lower | (upper << 8); 

	i2c_stop(); 

	//convert via the scale 

	*X = (((float)x*0.153)/1000);  
	*Y = (((float)y*0.153)/1000); 
	sei();

}	



void Acc_readXYZ(float* X, float* Y, float* Z)
{
	cli(); 
	i2c_start(LSM9DS1_ADDRESS_ACCELGYRO_WRITE);
	i2c_write(0x28); //this is starting x_lower 
	i2c_start(LSM9DS1_ADDRESS_ACCELGYRO_READ);
	uint16_t x,y,z;

	uint8_t lower = i2c_read_ack(); 
	uint8_t upper = i2c_read_ack(); 

	x = lower | (upper << 8);
	x += 400 ; //this is the offset for the small board 
	lower = i2c_read_ack(); 
	upper = i2c_read_ack(); 

	y = lower | (upper << 8); 
	y += 20 ; // for small board 

	lower = i2c_read_ack(); 
	upper = i2c_read_nack(); 

	z = lower | (upper << 8); 

	i2c_stop(); 

	//convert via the scale finding threshold
	// if (x > 2000 || y > 2000 || x < -2000 || y < -2000)
	// {
	// 	char buf[20]; 
	// 	sprintf(buf,"x: %d y: %d",x,y);  
	// 	serial_outputString(buf); 
	// }

	*X = ((x*0.061)/1000);  
	*Y = (y*0.061)/1000; 
	*Z = (z*0.061)/1000; 

	sei(); 
}



void LSM_getTemp(float* temp)
{
	cli(); 
	short int tempData; 
	i2c_start(LSM9DS1_ADDRESS_ACCELGYRO_WRITE);
	i2c_write(0x15);
	i2c_start(LSM9DS1_ADDRESS_ACCELGYRO_READ);
	char lower = i2c_read_nack(); 
	char upper = i2c_read_nack(); 
	tempData = lower | (upper << 8); 

	i2c_stop();

	*temp = (28+(float)tempData/8)*(9/5.0)+32;
	sei(); 

}
