
//  Created by Christian Wagner on 3/9/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include "Heart_rate.h"

#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>


//need to initalize the adc conversion
void HR_init()
{
	// this is for ADC stuff
	ADMUX |= (1 << REFS0); // set to use AVCC for high refernce
	ADMUX |= (1 << MUX0) | ( 1 << MUX1); // setting the mux to adc3
	ADCSRA |= 0x06 ; // set the prescale to 64 don't want to eat up too much time
	ADCSRA |= (1 << ADEN) ; //enable
	ADCSRA |= (1 << ADIE) ; // enabling interrupts for completed conversion

	// ADCSRA |= (1 << ADSC) ; //start conversion

	//this is for the timer
	//this sets up the timer to clear on compare
	TCCR1B |= (1 << WGM12);
	TIMSK1 |= (1 <<OCIE1A) ;
	OCR1A = 36000; // max count for 5 seconds

	TCCR1B |= (1 << CS12) | (1 << CS10); //this starts the timer; and sets the prescale to 1024
	sei(); // enable global interrupts

	DDRB |= (1 << 1); // 15


}

//starts running the heart rate monitor
//sets up all of the pins and setting values
void HR_start(volatile struct HR_data* HR)
{
	// HR->upper_threshold = 900;
	// HR->lower_threshold = 300;
	HR->threshold = 550;
	HR->min = 1000;
	HR->heart_count = 0;
	HR->last_count = 0;
	HR->take_data = 1;
	HR->BPM = 0;

	ADCSRA |= (1 << ADSC) ; // start the first conversion
	// TCCR1B |= (1 << CS12) | (1 << CS10); //this starts the timer; and sets the prescale to 1024
}

void HR_stop(volatile struct HR_data* HR)
{
	HR->take_data = 0;
	// TCCR1B &= ~((1 <<CS11) | (1 << CS12) | (1 << CS10)); //stopping the timer by setting the prescale to 0
}

void HR_read(volatile struct HR_data* HR)
{
	HR->reading = ADCL | (ADCH << 8);
	// char buf[20];
	// sprintf(buf,"hr : %d",HR->reading);
	// serial_outputString(buf);

	// measuring count and handling the state machine
	if ((HR->reading > HR->threshold) && (HR->state == 0) && HR->count > 3200) //600 is the current threshold
	{
		PORTB |= (1 << 1);
		HR->state = 1;
		HR->heart_count++;


	// char buf[5];
	// sprintf(buf,"%d",HR->count);
	// serial_outputString(buf);
		HR->count = 0;
	}
	else if ((HR->state == 1) && (HR->reading < HR->threshold) && HR->count > 1500)
	{
		HR->state = 0;
		PORTB &= ~(1 << 1);
	}
	else
	{
		HR->count++;
	}

	// this is calibrate the range of the heart rate to allow for dynamic measuring

	if (HR->reading > HR->max)
	{
		HR->max = (HR->max + HR->reading)/2;
	}
	else if (HR->reading < HR->min)
	{
		HR->min = (HR->min + HR->reading)/2;
	}


	if (HR->take_data)
	{
		ADCSRA |= (1 << ADSC) ; //continue to convert
	}

}

void HR_calc_BPM(volatile struct HR_data* HR)
{

	if (HR->last_count != 0)
	{
		HR->BPM = (HR->last_count+HR->heart_count)*6;
	}
	else
	{
		HR->BPM = 12*HR->heart_count;
	}
	// char buf[20];
	// sprintf(buf,"\tbpm : %d",HR->BPM);
	// serial_outputString(buf);

	if (HR->heart_count > 4)
	{
		HR->last_count = HR->heart_count;
		// HR->long_count = HR->last_count;
	}
	else
	{
		HR->last_count = 0;
	}

	HR->heart_count = 0;


		if (HR->max == 0 || HR->min == 0)
		{
// 			HR->threshold = 550;

		}
		else
		{
			HR->threshold = ((HR->max+HR->min) / 2)+120 ;
		}

		// sprintf(buf,"uppser : %d, lower: %d",HR->upper_threshold, HR->lower_threshold);
		// sprintf(buf,"threshold %d",HR->threshold);
		// serial_outputString(buf);
		HR->max = 0;
		HR->min = 1000;


}
