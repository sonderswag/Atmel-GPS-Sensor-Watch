//
//  main.c
//  transReciever:: SEND 
//
//  Created by Christian Wagner on 3/9/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>


#include "../Heart_rate.h"
#include "../../Digital_IO/DigitalIo.h"
#include "../../Serial/Serial.h"


volatile struct HR_data HR = {0,0,0,0,0,0,0,0,0,0};

int main(int argc, const char * argv[]) 
{
	serial_init(47);
	HR_init();
	HR_start(&HR);
	// LSM_getTemp(&temp);
	serial_outputString("start");


	while (1)
	{

	}	 

    return 0;
}


// when the adc is done 
ISR(ADC_vect)
{
	HR_read(&HR); 

}

ISR(TIMER1_COMPA_vect)
{
	serial_outputString("time");
	HR_calc_BPM(&HR);
}

