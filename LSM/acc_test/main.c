//
//  main.c
//  transReciever:: SEND 
//
//  Created by Christian Wagner on 3/9/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h> 
#include <avr/interrupt.h>


#include "../LSM.h"
#include "../../Digital_IO/DigitalIo.h"
#include "../../Serial/Serial.h"

int steps = 0; 


int main(int argc, const char * argv[]) {
	serial_init(47);
	
	DDRD  &= ~(1 << DDD3); 
	PORTD |= (1<<PORTD3);
	EICRA |= (1<<ISC10) | (1 << ISC11);
	EIMSK |= (1<< INT1);  
	float x,y,z,temp; 
	float x_avg, y_avg, z_avg; 
	char buf[20];
	
	LSM_init();
	_delay_ms(10000);

	// LSM_getTemp(&temp);
	
	while (1)
	{
		Acc_readXYZ(&x,&y,&z);

		// FloatToStringNew(buf, x_avg, 6);
		// serial_out('X');
		// serial_out(' '); 
		// serial_outputString(buf); 

		// FloatToStringNew(buf, y_avg, 6);
		// serial_out('Y');
		// serial_out(' '); 
		// serial_outputString(buf); 

		// FloatToStringNew(buf, z, 6);
		// serial_out('Z');
		// serial_out(' '); 
		// serial_outputString(buf); 

		_delay_ms(5);
	}

    return 0;
}


//Hardware interrupt
ISR(INT1_vect)
{
	steps++ ; 
	char buf[20] ; 
	sprintf(buf,"count : %d",steps);
	serial_outputString(buf);
}

