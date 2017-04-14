
#include "Screen.h"
#include "../I2C/I2C.h"
#include "../Digital_IO/DigitalIo.h"
#include "fonts.h"
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

void screen_drawVLine(char x, char y, char length, char* buffer)
{
    uint8_t i = 0; 
    for (i = 0 ; i < length; i++)
    {
        screen_drawPixel(x,y+i,1,buffer); 
    }
}

void screen_drawRectangle(char x1, char y1, char x2, char y2, char* buffer)
{
    char length = x2-x1 + 1;
    char height = y2-y1; 
    screen_drawHLine(x1,y1,length,buffer);
    screen_drawHLine(x1,y2,length,buffer);
    screen_drawVLine(x1,y1,height,buffer);
    screen_drawVLine(x2,y1,height,buffer); 
}

void screen_drawFillRectangle(char x1, char y1, char x2, char y2, char status, char* buffer)
{
    char length = x2-x1 + 1;
    char height = y2-y1; 

    char x,y; 
    for (x = 0 ; x < length; ++x)
    {
        for (y = 0 ; y < height ; ++y)
        {
            screen_drawPixel(x1+x,y1+y,status,buffer); 
        }
    }
}

void screen_drawCircle(char x0, char y0, char r, char status, char* buffer)
{
    char x = r ;
    char y = 0; 
    char err = 0; 
    while (x >= y)
    {
        screen_drawPixel(x0+x,y0+y,status,buffer); 
        screen_drawPixel(x0+y,y0+x,status,buffer);
        screen_drawPixel(x0-y,y0+x,status,buffer);  
        screen_drawPixel(x0-x,y0+y,status,buffer); 
        screen_drawPixel(x0-x,y0-y,status,buffer); 
        screen_drawPixel(x0-y,y0-x,status,buffer); 
        screen_drawPixel(x0+y,y0-x,status,buffer);
        screen_drawPixel(x0+x,y0-y,status,buffer); 

        if (err <= 0)
        {
            y += 1;
            err += 2*y +1;
        } 
        if (err > 0)
        {
            x -= 1; 
            err -=2*x+1;
        }
    }
}

void screen_drawFillCircle(char x0, char y0, char r, char status, char* buffer)
{
    char radius = r ; 
    while (radius > 0)
    {
        screen_drawCircle(x0,y0,radius,status,buffer); 
        radius--;
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

void screen_clear(char* buffer)
{
    memset(buffer,0,1024);
}

void screen_drawChar(unsigned char pos_x, unsigned char pos_y, 
    unsigned char letter, uint8_t *buff)
{
    char page = (pos_y/8); 

    uint8_t ascii_offset = 32;
    char i ;
    for(i = 0; i < 5; i++)
    {
        buff[i+((page*128)+pos_x)] = Ascii_1[letter-ascii_offset][i];
    }
}

void screen_drawString(unsigned char pos_x, unsigned char pos_y, char *string, uint8_t *buff)
{
    uint8_t i = 0;
    while(string[0] != 0){
        screen_drawChar(pos_x+(5*i), pos_y, (string[0]), buff);
        string++;
        i++;
    }
}

void screen_sendBuffer(char* buffer)
{
    screen_sendCommand(SSD1306_COLUMNADDR); //set the coloumb current address to 00 
    screen_sendCommand(0x00); //saying this is a command 
    screen_sendCommand(0x7F); //line end address

    screen_sendCommand(SSD1306_PAGEADDR); //this set's the page address t0 00
    screen_sendCommand(0x00); //page start address
    screen_sendCommand(0x07); //page end address

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