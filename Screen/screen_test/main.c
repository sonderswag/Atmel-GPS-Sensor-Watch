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


#include "../Screen.h"

	
int main(int argc, const char * argv[]) {
	
	
	screen_init(); 

	while (1)
	{
		// serial_outputString(radio.buffer);

		_delay_ms(1);


	}

    return 0;
}
