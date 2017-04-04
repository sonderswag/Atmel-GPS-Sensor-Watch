//
//  main.c
//  parse_gps
//
//  Created by Christian Wagner on 2/28/17.
//  Copyright © 2017 Christian Wagner. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
};


void parseLocation(const char* latitude, const char* dirNS,
                   const char* longitude, const char* dirEW,
                   struct GPS* gps)
{
    char lat[7];
    strncpy(lat, latitude, 2);
    gps->latitude = atof(lat);
    
    strncpy(lat, &(latitude[2]),7);
    gps->latitudeDegrees = atof(lat);
    
    // checking to see if it is south. if so then should be -
    if (strcmp(dirNS,"S") == 0)
    {
        gps->latitude *= -1.0;
    }
    
    char lon[9];
    strncpy(lon, longitude, 3);
    gps->longitude = atof(lon);
    
    strncpy(lon, &(longitude[3]),7);
    gps->longitudeDegrees = atof(lon);
    
    if (strcmp(dirEW,"W") == 0)
    {
        gps->longitude *= -1.0;
    }
}

char parse( char* input, struct GPS* gps)
{
    
    if (strncmp(input, "$GPRMC",6) == 0)
    {
        char* split;
        char* splitString[14];
        char i = 0 ;
        split = strtok(input,",");
        while (split != NULL)
        {
            splitString[i++] = split;
            split = strtok(NULL, ",");
        }
        
        //Check to see if we are getting valid data
        if (splitString[2][1] != "A")
        {
            return 1;
        }
        
        /* ------------------- Parse the time ---------------------- */
        // splitString[1] == time data
        
        int time = atoi(splitString[1]);
        gps->hour = time / 10000;
        gps->minute = (time % 10000) / 100;
        gps->seconds = (time % 100);
        
        
        /* ------------------- Parse Location ---------------------- */
        
        
        parseLocation(splitString[3], splitString[4], splitString[5], splitString[6],gps);
        
        /* ------------------- Parse Date ---------------------- */
        int date = atoi(splitString[9]);
        gps->day = date/10000;
        gps->month = (date%10000)/100;
        gps->year = (date%100) ;
    }
    else if (strncmp(input, "$GPRMC",6) == 0)
    {
        char* split;
        char* splitString[14];
        char i = 0 ;
        split = strtok(input,",");
        while (split != NULL)
        {
            splitString[i++] = split;
            split = strtok(NULL, ",");
        }
        
        parseLocation(splitString[2], splitString[3], splitString[4], splitString[5], gps);
        
        // checking if it is working
        if (atoi(splitString[6]) == 0 )
        {
            return 1;
        }
        
        gps->satellites = atoi(splitString[7]);
        
        gps->altitude = atof(splitString[8]);
        
        // has other information that we don't need
        
    }
    

    
    
    return 0;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    struct GPS gps;
    parse(argv[1],&gps);
    
    printf("time %d: %d: %d \n", gps.hour, gps.minute, gps.seconds);
    printf("latitude %f \n", gps.latitude);
    printf("latitude degrees %f \n", gps.latitudeDegrees);
    printf("longitude %f \n", gps.longitude);
    printf("longitude degrees %f \n", gps.longitudeDegrees);
    
    printf("date %d / %d / %d \n", gps.month, gps.day, gps.year);
    
    return 0;
}
