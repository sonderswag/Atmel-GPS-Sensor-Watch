
#include "Screen.h"
#include "../I2C/I2C.h"
#include <stdio.h>
#include <stdint.h>

void screen_init()
{
	i2c_init(); 
	screen_sendCommand(SSD1306_DISPLAYOFF);

	screen_sendCommand(SSD1306_SETDISPLAYCLOCKDIV);
    screen_sendCommand(0x80);

    screen_sendCommand(SSD1306_SETMULTIPLEX);
    screen_sendCommand(0x3F);
    
    screen_sendCommand(SSD1306_SETDISPLAYOFFSET);
    screen_sendCommand(0x00);
    
    screen_sendCommand(SSD1306_SETSTARTLINE | 0x00);
    
    // We use internal charge pump
    screen_sendCommand(SSD1306_CHARGEPUMP);
    screen_sendCommand(0x14);
    
    // Horizontal memory mode
    screen_sendCommand(SSD1306_MEMORYMODE);
    screen_sendCommand(0x00);
    
    screen_sendCommand(SSD1306_SEGREMAP | 0x1);

    screen_sendCommand(SSD1306_COMSCANDEC);

    screen_sendCommand(SSD1306_SETCOMPINS);
    screen_sendCommand(0x12);

    // Max contrast
    screen_sendCommand(SSD1306_SETCONTRAST);
    screen_sendCommand(0xCF);

    screen_sendCommand(SSD1306_SETPRECHARGE);
    screen_sendCommand(0xF1);

    screen_sendCommand(SSD1306_SETVCOMDETECT);
    screen_sendCommand(0x40);

    screen_sendCommand(SSD1306_DISPLAYALLON_RESUME);

    // Non-inverted display
    screen_sendCommand(SSD1306_NORMALDISPLAY);

    // Turn display back on
    screen_sendCommand(SSD1306_DISPLAYON);
}

void screen_sendCommand(uint8_t command)
{
	i2c_start(SSD1306_DEFAULT_ADDRESS);
	i2c_write(0x00);
	i2c_write(command);
	i2c_stop(); 
}