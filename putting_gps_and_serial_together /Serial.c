//initialize the USART port
#include <avr/io.h>
#include <util/delay.h>
#include <string.h> 

#include "serial.h"

void serial_init(unsigned short ubrr)	{
	UBRR0 = ubrr;
	UCSR0B |= (1 << TXEN0);
	UCSR0B |= (1 << RXEN0);
	UCSR0C = (3 << UCSZ00);
}
//output a byte to the USART0 port
void serial_out(char ch)	{
	while ((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = ch;
}
//read a byte from the USART0 and return it
char serial_in()	{
	while (!(UCSR0A & (1<<RXC0)));
	return UDR0;

}

void outputString (char* in)
{
	char i = 0; 
	for (i = 0; i <= strlen(in) ; i ++)
	{
		serial_out(in[i]);
	}
}



// int main ()	{

// 	DDRC |= 1 << DDC0; 

// 	/* calculate from clock freq and buad see handout 
// 	UBRR = [f_osc / (16*BUAD)] - 1 
// 	UBRR = 47 
// 	*/
// 	serial_init(47);

// 	char temp;
// 	char str[80]; 
// 	while(1)	{
// 		temp = 0; 
// 		temp = serial_in();
		
// 		if (temp != 0 )
// 		{
// 			sprintf(str, "You entered = %c \r\n", temp);
// 			outputString(str);
			
// 		}
// 	}
// 	return 0;
// }