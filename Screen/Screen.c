
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
	i2c_write(0x00); //setting it to write mode 
	i2c_write(command);
	i2c_stop(); 
}

void screen_drawPixel(char pos_x, char pos_y, char status, char* buffer)
{
    if (pos_x >= SSD1306_WIDTH || pos_y >= SSD1306_HEIGHT) {
        return;
    }
    //have to div by 8 to seperate into pages 
    if (status)
    {
        buffer[pos_x+(pos_y/8)*SSD1306_WIDTH] |= (1 << (pos_y&7)); 
    }
    else 
    {
        buffer[pos_x+(pos_y/8)*SSD1306_WIDTH] &= ~(1 << (pos_y&7));
    }
}

void screen_drawHLine(char x, char y, char length, char* buffer)
{
    uint8_t i = 0; 
    for (i = 0 ; i < length; i++)
    {
        screen_drawPixel(i+x,y,1,buffer); 
    }
}

void screen_invert(char inverted)
{
    if (inverted)
    {
        screen_sendCommand(SSD1306_INVERTDISPLAY);
    }
    else 
    {
        screen_sendCommand(SSD1306_NORMALDISPLAY);
    }
}
void screen_sendBuffer(char* buffer)
{
    screen_sendCommand(SSD1306_COLUMNADDR); //set the coloumb current address to 00 
    screen_sendCommand(0x00); //saying this is a command 
    screen_sendCommand(0x7F); //reset contrast value 

    screen_sendCommand(SSD1306_PAGEADDR); //this set's the page address t0 00
    screen_sendCommand(0x00);
    screen_sendCommand(0x07);

    // We have to send the buffer as 16 bytes packets
    // Our buffer is 1024 bytes long, 1024/16 = 64
    // We have to send 64 packets
    uint8_t packet ; 
    uint8_t packet_byte ;
    for (packet = 0; packet < 64; packet++) {
        i2c_start(SSD1306_DEFAULT_ADDRESS); //start i2c communication 
        i2c_write(0x40); //this is saying that it is not a command  
        for (packet_byte = 0; packet_byte < 16; ++packet_byte) {
            i2c_write(buffer[packet*16+packet_byte]);
        }
        i2c_stop();
    }
}