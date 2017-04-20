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
#include <math.h>	//test this
#include <util/delay.h>
#include <avr/interrupt.h>

#include "GPS.h"
#include "../Serial/serial.h"


char GPS_parse(struct GPS* gps)
{
    char* split;
    char* splitString[9];
    char i = 0 ;
    cli();
    split = strtok(gps->buffer,",");
    split = strtok(NULL,",");
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
   
    // gps->fixquality = atoi(splitString[5]); 

    //Check to see if we are getting valid data
    // zero is bad
    if (strcmp(splitString[2],"0") == 0) //0 
    {
        gps->satellites = 0; 
        return 1;
    }
    
    /* ------------------- Parse the time ---------------------- */
    // // splitString[0] == time data

    char buf[5] = {splitString[0][0],splitString[0][1]};




    gps->hour    = atoi(buf);
    if (gps->hour < 9)
    {
        gps->hour += 17; 
    }
    else 
    {
        gps->hour -= 7; 
    }

    memset(buf,0,sizeof(buf)); 
    strncpy(buf,&(splitString[0][2]),2); 
    gps->minute  = atoi(buf);

    memset(buf,0,sizeof(buf)); 
    strncpy(buf,&(splitString[0][4]),2); 
    gps->seconds = atoi(buf); 

    //  ------------------- Parse Location ---------------------- 
    // latitude 
    memset(buf,0,sizeof(buf)); 
    strncpy(buf,splitString[1],2); 
    gps->latitude = atoi(buf) +atof(&(splitString[1][2]))/60.0;


    if (strcmp(splitString[2],"S") == 0) // S
    {
        gps->latitude *= -1; 
    }


    memset(buf,0,sizeof(buf)); 
    // longitude 
    strncpy(buf,splitString[3],3); 

    gps->longitude = atoi(buf)+atof(&(splitString[3][3]))/60.0; 

    if (strcmp(splitString[4],"W") == 0) //W 
    {
        gps->longitude *= -1 ; 
    }


    gps->altitude = atof(splitString[8]); 

    gps->satellites = atoi(splitString[6]);


    // GPS_printInfo(gps); 
    sei();
    return 0;
}

void GPS_readSerialInput(struct GPS* gps)
{
    
    char input; 
    char state =0; 
    char place = 0; 
    cli();
    memset(gps->buffer,0,sizeof(gps->buffer));

    while(1)
    {
        input = serial_in(); 
        // serial_out(input);
        if (input == 0x47 && state == 0) // $
        {
            state = 1; 
        }

        if ((input == 0x0D || place == 69) && state == 1) // newline 
        {
            state = 0; 
            place = 0;

            if (strncmp(gps->buffer,"GPGGA",5) == 0)
            {
                break; 
            } 
        }

        else if (state == 1) //message 
        {
            // serial_out(input);
            gps->buffer[place++] = input; 
        }
    }
    // serial_outputString(gps->buffer); 
    GPS_parse(gps); 

    sei();

}



void GPS_printInfo(struct GPS* gps)
{
    cli(); 
    char buffer[20]; 

    memset(buffer,0,sizeof(buffer));
    FloatToStringNew(buffer,gps->longitude , 6); 
    serial_outputString("lon ");
    serial_outputString(buffer);

    memset(buffer,0,sizeof(buffer));
    FloatToStringNew(buffer,gps->latitude , 6); 
    serial_outputString("lat ");
    serial_outputString(buffer);

    memset(buffer,0,sizeof(buffer));
    FloatToStringNew(buffer,gps->altitude , 1); 
    serial_outputString("Alt ");
    serial_outputString(buffer);

    memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "%d:%d:%d", gps->hour, gps->minute, gps->seconds);
    serial_outputString(buffer); 

    memset(buffer,0,sizeof(buffer));
    sprintf(buffer, "satellites %d",gps->satellites); 
    serial_outputString(buffer); 
    sei();

}


float GPS_calculate(struct GPS* gps1, float lat_2, float log_2)
{
	float R = 6371000;	//meters
	float phi1 = (gps1->latitude)*M_PI/180;
	float phi2 = (lat_2)*M_PI/180;
	float lambda1 = (gps1->longitude)*M_PI/180;
	float lambda2 = (log_2)*M_PI/180;
	
	float a = sin((phi1-phi2)/2)*sin((phi1-phi2)/2) + cos(phi1)*cos(phi2)*sin((lambda1-lambda2)/2)*sin((lambda1-lambda2)/2);
	float c = 2*atan2(sqrt(a),sqrt(1-a));
	float d = R*c;
	
	return(d/1000);	//return kilometers
}
