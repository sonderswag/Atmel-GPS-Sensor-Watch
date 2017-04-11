
#ifndef GPS_h
#define GPS_h 
#endif 


struct GPS
{
    char buffer[120]; 
    char state ; 
    uint8_t hour, minute, seconds, year, month, day;
    uint16_t milliseconds;
    // Floating point latitude and longitude value in degrees.
    // int latitude, longitude;
    // Fixed point latitude and longitude value with degrees stored in units of 1/100000 degrees,
    // and minutes stored in units of 1/100000 degrees.  See pull #13 for more details:
    //   https://github.com/adafruit/Adafruit-GPS-Library/pull/13
    int32_t latitude_fixed, longitude_fixed;
    // int latitudeDegrees_front, longitudeDegrees_front;
    // int latitudeDegrees_back, longitudeDegrees_back; // note need to divide by .0001 for the agnel 
    // int altitude_front, altitude_back;
    float speed, angle, magvariation, HDOP;
   
    uint8_t fixquality, satellites;

    char sizeInputString; 
    char message_count  ;

    float latitude, longitude, altitude; 
};


void GPS_parseLocation(const char* latitude, const char* dirNS,
                   const char* longitude, const char* dirEW,
                   struct GPS* gps); 

char GPS_parse(struct GPS* gps); 
void GPS_readSerialInput(struct GPS* gps); 