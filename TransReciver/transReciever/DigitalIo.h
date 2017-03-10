
#ifndef DigitalIo_h
#define DigitalIo_h

#endif 

#include <avr/io.h>

#define HIGH 1 
#define LOW 0 
#define OUTPUT 1 
#define INPUT 0 


void digitalWrite(char port, char state);
void pinMode(char port, char mode);