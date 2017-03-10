
#include "DigitalIo.h"
// assumes ddr register is already enable for output and not input
void digitalWrite(char port, char state)
{
    if (port == 1 || (port >= 23 && port <= 28 )) // c
    {
        if (port == 1)
        {
            if (state == 1) PORTC |= (1<<6);

            else PORTC &= ~(1<<6);

        }
        else
        {
            port -= 23;
            if (state == 1) PORTC |= (1<<port);

            else PORTC &= ~(1<<port);

        }
    }
    else if ((port >= 2 && port <= 6) || ( port >= 11 && port <= 13)) // d
    {
        if (port >= 2 && port <= 6)  port -= 2; 
        
        else  port -= 6;
        
        if (state == 1) PORTD |= (1<<port);

        else PORTD &= ~(1<<port);; 

    }
    else if ((port >= 9 && port <= 10) || (port >= 14 && port <= 19)) //b
    {
        if (port >= 9 && port <= 10) port -= 3;

        else port -= 14;

        
        if (state == 1) PORTB |= (1<<port);

        else PORTB &= ~(1<<port);
    }
}

void pinMode(char pin, char mode)
{
    if (port == 1 || (port >= 23 && port <= 28 )) // c
    {
        if (port == 1)
        {
            if (mode == 1) DDRC |= (1<<6);

            else DDRC &= ~(1<<6);

        }
        else
        {
            port -= 23;
            if (mode == 1) DDRC |= (1<<port);

            else DDRC &= ~(1<<port);

        }
    }
    else if ((port >= 2 && port <= 6) || ( port >= 11 && port <= 13)) // d
    {
        if (port >= 2 && port <= 6)  port -= 2; 
        
        else  port -= 6;
        
        if (mode == 1) DDRD |= (1<<port);

        else DDRD &= ~(1<<port);; 

    }
    else if ((port >= 9 && port <= 10) || (port >= 14 && port <= 19)) //b
    {
        if (port >= 9 && port <= 10) port -= 3;

        else port -= 14;

        
        if (mode == 1) DDRB |= (1<<port);

        else DDRB &= ~(1<<port);
    }
}