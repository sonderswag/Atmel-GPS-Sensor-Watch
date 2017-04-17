#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

#define SSD1306_DEFAULT_ADDRESS 0x78
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SWITCHCAPVCC 0x2
#define SSD1306_NOP 0xE3

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_BUFFERSIZE (SSD1306_WIDTH*SSD1306_HEIGHT)/8

struct Screen
{
	char buffer[1024]; 
};

void screen_sendCommand(uint8_t command); 
void screen_clear(char* buffer);
void screen_init(); 
void screen_drawPixel(char pos_x, char pos_y, char status, char* buffer); 
void screen_drawHLine(char x, char y, char length, char* buffer); 
void screen_invert(char inverted);
void screen_sendBuffer(char* buffer);
void screen_drawVLine(char x, char y, char length, char* buffer);
void screen_drawRectangle(char x1, char y1, char x2, char y2, char* buffer);
void screen_drawFillRectangle(char x1, char y1, char x2, char y2, char status, char* buffer); 
void screen_drawCircle(char x0, char y0, char r, char status, char* buffer);
void screen_drawFillCircle(char x0, char y0, char r, char status, char* buffer);
void screen_drawChar(unsigned char pos_x, unsigned char pos_y, unsigned char letter, uint8_t *buff);
void screen_drawString(unsigned char pos_x, unsigned char pos_y, char *string, uint8_t *buff); 
#endif 