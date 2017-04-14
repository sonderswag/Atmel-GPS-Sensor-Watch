
#ifndef GPS_h
#define GPS_h 
#endif 


struct GPS
{
    char buffer[120]; 
    char state ; 
    uint8_t hour, minute, seconds;

   
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
float GPS_calculate(struct GPS* gps1, struct GPS* gps2);