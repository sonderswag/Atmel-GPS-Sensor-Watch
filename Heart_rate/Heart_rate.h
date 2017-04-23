#ifndef HEART_RATE_H
#define HEART_RATE_H
#include <stdint.h>

volatile struct HR_data
{
	
	char take_data; 
	uint8_t state; // 0 not a new heart rate , 1 currently high 

	uint8_t BPM; 
	uint8_t heart_count;
	uint8_t last_count; 
	// uint8_t long_count; 
	// uint8_t calibrate_count; 
	uint16_t max; 
	uint16_t min;
	uint16_t count; 
	
	// uint16_t lower_threshold; 

	uint16_t reading;
	
	uint16_t threshold; 

	
};


void HR_start(volatile struct HR_data* HR);
void HR_stop(volatile struct HR_data* HR);
void HR_init(); 
void HR_read(volatile struct HR_data* HR); 
void HR_calc_BPM(volatile struct HR_data* HR);


#endif 