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


#include "GPS.h"
#include "../Serial/serial.h"


void GPS_parseLocation(const char* latitude, const char* dirNS,
                   const char* longitude, const char* dirEW,
                   struct GPS* gps)
{
   
    // strncpy(lat, &(latitude[2]),7);
    // gps->latitudeDegrees = atof(lat);
    
    // // checking to see if it is south. if so then should be -
    // if (strcmp(dirNS,"S") == 0)
    // {
    //     gps->latitude *= -1.0;
    // }
    
    // char lon[9];
    // strncpy(lon, longitude, 3);
    // gps->longitude = atof(lon);
    
    // strncpy(lon, &(longitude[3]),7);
    // gps->longitudeDegrees = atof(lon);
    
    // if (strcmp(dirEW,"W") == 0)
    // {
    //     gps->longitude *= -1.0;
    // }
}

char GPS_parse(struct GPS* gps)
{



    char* split;
    char* splitString[14];
    char i = 0 ;
    split = strtok(gps->buffer,",");
    while (split != NULL)
    {
        splitString[i++] = split;
        split = strtok(NULL, ",");
    }
    
    // 0 == time 
    // 1 == lat 
    // 2 == N / S 
    // 3 == log 
    // 4 == E / W
    // 5 == 0 = no sat , 1 / 2 = good 
    // 6 == number of sat 
    // 7 == Horizontal dilution
    // 8 == Antenna altitude
   
    //Check to see if we are getting valid data
    if (splitString[5] == 0x30) //0 
    {
        return 1;
    }
     // serial_outputString(splitString[6]);
    // serial_outputString('active');
    
    /* ------------------- Parse the time ---------------------- */
    // // splitString[0] == time data

    char hour[3] = {splitString[0][0],splitString[0][1]};
    char min[3]  = {splitString[0][2],splitString[0][3]};
    char sec[3]  = {splitString[0][4],splitString[0][5]}; 

    gps->hour    = atoi(hour);
    if (gps->hour < 9)
    {
        gps->hour += 17; 

    }
    else 
    {
        gps->hour -= 7; 
    }
    gps->minute  = atoi(min);
    gps->seconds = atoi(sec); 

    //  ------------------- Parse Location ---------------------- 
    char lat[3] = {splitString[1][0], splitString[1][1]};
    char lat_deg_front[3] = {splitString[1][2],splitString[1][3]};
    char lat_deg_back[5] = {splitString[1][5],splitString[1][6],splitString[1][7],splitString[1][8]}; 


    gps->latitudeDegrees_back = atoi(lat_deg_back); 
    gps->latitudeDegrees_front = atoi(lat_deg_front); // note need to divide by .0001 for the agnel 
    gps->latitude = atoi(lat);


    if (strcmp(splitString[2],"S") == 0) // S
    {
        gps->latitude *= -1; 
    }


    char log[4] = {splitString[3][0], splitString[3][1], splitString[3][2]};
    char log_deg_front[3] = {splitString[3][3],splitString[3][4]};
    char log_deg_back[5]  = {splitString[3][6],splitString[3][7],splitString[3][8], splitString[3][9]}; 

    
    gps->longitudeDegrees_front = atoi(log_deg_front); 
    gps->longitudeDegrees_back = atoi(log_deg_back);
    gps->longitude = atoi(log);

    // serial_outputString(log_deg);
    // serial_outputString(splitString[4]);

    if (strcmp(splitString[4],"W") == 0) //W 
    {
        gps->longitude *= -1 ; 
    }


    gps->altitude_front = atoi(strtok(splitString[8],"."));
    gps->altitude_back = atoi(strtok(NULL,".")) ;

    // GPS_printInfo(gps); 
    return 0;
}

void GPS_readSerialInput(struct GPS* gps)
{
    
    char input; 
    while (1)
        {
            input = serial_in(); 
            if (gps->state == 0)
            {
                // serial_outputString("0");
                if (input == 0x24) gps->state = 1 ; //$
            }
            else if (gps->state == 1)
            {
                if (input == 0x47) gps->state = 2 ; //G
                else gps->state = 0; 
            }
            else if (gps->state == 2)
            {
                if (input == 0x50) gps->state = 3; //P
                else gps->state = 0; 
            }
            else if (gps->state == 3)
            {

                if (input == 0x47) gps->state = 4; //G
                else gps->state = 0; 
            }
            else if (gps->state == 4)
            {
                if (input == 0x47) gps->state = 5; //G
                else gps->state = 0; 
            }
            else if (gps->state == 5)
            {
                if (input == 0x41) gps->state = 6; //A
                else gps->state = 0; 
            }
            else if (gps->state == 6)
            {

                if (input == 0x0D)
                {
                    break; 
                }
                gps->buffer[gps->sizeInputString++] = input; 
                
                
            }
        }
        serial_outputString(gps->buffer); 

        GPS_parse(gps); 
   
        gps->state = 0; 
        gps->sizeInputString = 0; 

}



void GPS_printInfo(struct GPS* gps)
{
    char buffer[50]; 
    sprintf(buffer, "Log %d log_deg %d.%d", gps->longitude, gps->longitudeDegrees_front, gps->longitudeDegrees_back);
    serial_outputString(buffer); 
    sprintf(buffer, "Lat %d lat_deg %d.%d", gps->latitude, gps->latitudeDegrees_front, gps->latitudeDegrees_back); 
    serial_outputString(buffer); 
    sprintf(buffer, "hour %d, min %d, sec %d", gps->hour, gps->minute, gps->seconds);
    serial_outputString(buffer); 
    sprintf(buffer, "Altitude %d.%d", gps->altitude_front, gps->altitude_back);
    serial_outputString(buffer);

}

