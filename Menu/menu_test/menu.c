#include <avr/io.h>
#include <util/delay.h>
#include <string.h> 

#include "menu.h"

void press_button()	{
	char press1 = (PIND & (1<<PD6));
	if (press1 == 0)	{
		_delay_ms(5);
		while ((PIND & (1<<PD6))==0)	{}
		_delay_ms(5);
		return 1;
	}	
	else	{
		return 0;
	}
		//if ((PIND & (1<<PD6))==0)	{
}