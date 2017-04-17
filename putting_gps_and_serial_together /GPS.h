
#ifndef GPS_h
#define GPS_h 
#endif 


struct GPS
{
    uint8_t hour, minute, seconds, year, month, day;
    uint16_t milliseconds;
    // Floating point latitude and longitude value in degrees.
    float latitude, longitude;
    // Fixed point latitude and longitude value with degrees stored in units of 1/100000 degrees,
    // and minutes stored in units of 1/100000 degrees.  See pull #13 for more details:
    //   https://github.com/adafruit/Adafruit-GPS-Library/pull/13
    int32_t latitude_fixed, longitude_fixed;
    float latitudeDegrees, longitudeDegrees;
    float geoidheight, altitude;
    float speed, angle, magvariation, HDOP;
    char lat, lon, mag;
   
    uint8_t fixquality, satellites;

    int sizeInputString; 
};


void parseLocation(const char* latitude, const char* dirNS,
                   const char* longitude, const char* dirEW,
                   struct GPS* gps); 

char parse( char* input, struct GPS* gps); 
void readSerialInput(const char input, struct GPS* gps); 