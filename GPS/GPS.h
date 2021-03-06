
#ifndef GPS_h
#define GPS_h 

struct GPS
{
    char buffer[70];
    uint8_t hour, minute, seconds;

   
    uint8_t satellites;

    float latitude, longitude, altitude; 
};


void GPS_parseLocation(const char* latitude, const char* dirNS,
                   const char* longitude, const char* dirEW,
                   struct GPS* gps); 

char GPS_parse(struct GPS* gps); 
void GPS_readSerialInput(struct GPS* gps); 
void GPS_printInfo(struct GPS* gps);
float GPS_calculate(struct GPS* gps1, float lat_2, float log_2);

#endif 