#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H


#define LSM9DS1_ADDRESS_ACCELGYRO_READ     0xD7
#define LSM9DS1_ADDRESS_ACCELGYRO_WRITE    0xD6
#define CTRL_REG6_XL					   0x20 
#define CTRL_REG5_XL					   0x1F
#define CTRL_REG1_M						   0x20
#define CTRL_REG2_M						   0x21
#define CTRL_REG3_M						   0x22

#define LSM9DS1_ADDRESS_MAG_READ           0x3D
#define LSM9DS1_ADDRESS_MAG_WRITE          0x3C
#define LSM9DS1_XG_ID                      0b01101000
// #define LSM9DS1_MAG_ID                     (0b00111101)


/*

CTRL_REG6_XL (20h) write to this to turn on accelerometer and off gyo 

 to control to modes of mag: 
 default is power down 

 CTRL_REG3_M (22h), setting values in the MD[1:0] bits. 
 For the output of the magnetic data compensated by temperature, 
 the TEMP_COMP bit in CTRL_REG1_M (20h) must be set to ‘1’.


When only accelerometer is activated and the gyroscope is in power down, 
starting from OUT_X_XL (28h - 29h) multiple reads can be performed. 
Once OUT_Z_XL (2Ch - 2Dh) is read, the system automatically restarts 
from OUT_X_XL (28h - 29h) (see Figure 6).

for accelerometer: 
This buffer can work accordingly to five different modes: 
Bypass mode, FIFO-mode, Continuous mode, Continuous-to-FIFO mode and Bypass-to-Continuous. 
Each mode is selected by the FMODE [2:0] bits in the FIFO_CTRL (2Eh) register.
*/ 
void LSM_init(); 

void LSM_writeReg(char devAdd, char reg, char data);
void Acc_readXYZ(float* X, float* Y, float* Z);
void Mag_readXY(float* X, float* Y);
void LSM_getTemp(float* temp); 
void LSM_getHeading(float* heading);

#endif 