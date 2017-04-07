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

#include "gpsParse.h"





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
        if (splitString[2][1] == 'A')
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

void readSerialInput(const char input, struct GPS* gps)
{
    char buffer[100]; 
    if (input == "/n" || input == "/r")
    {
        gps->sizeInputString = 0; 
        parse(buffer, gps);
    }
    else 
    {
        gps->sizeInputString++ ;
        buffer[gps->sizeInputString] = input; 

    }
}


